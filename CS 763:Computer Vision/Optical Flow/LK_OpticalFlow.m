clear;
clc;
load woman;
%I = ind2gray(X,map);
I = imread('other-data-gray/Grove2/frame10.png');
I1 = imread('other-data-gray/Grove2/frame11.png');
I = im2double(I);
I1 = im2double(I1);
Nx = size(I,2);
Ny = size(I,1);

%tform2 = maketform('affine',[1 0 0;0 1 0;1 0 1]);
%I2 = imtransform(I,tform2,'size',size(I),'FillValues',0); %figure(1);imshow(I2);

%tform3 = maketform('affine',[1 0 0;0 1 0;0 1 1]);
%I3 = imtransform(I,tform3,'size',size(I),'FillValues',0); %figure(2);imshow(I3);

I2 = zeros(size(I));
for c=2:size(I,2);
    I2(:,c) = I(:,c-1);
end
%figure(1);imshow(I2);

I3 = zeros(size(I));
for r=2:size(I,1);
    I3(r,:) = I(r-1,:);
end
%figure(2);imshow(I3);

tform4 = maketform('affine',[cosd(-1.5) -sind(-1.5) 0;sind(-1.5) cosd(-1.5) 0;0 0 1]);
I4 = imtransform(I,tform4,'size',size(I),'FillValues',0); %figure(3);imshow(I4);

% % image data corruption
% index_c = randperm(Nx);
% index_c = index_c(1:round(0.15*Nx));
% index_r = randperm(Ny);
% index_r = index_r(1:round(0.15*Ny));
% % I2 / I3 / I4
% for r=1:size(index_r,2)
%     for c=1:size(index_c,2)
%     I3(index_r(r),index_c(c)) = 100/255;
%     end
% end
% %figure(9);imshow(I2); %corrupt image
% imwrite(I3,'I2_corrupt.png');

%image gradients
Ix = zeros(size(I));
Iy = zeros(size(I));
for c=2:size(I,2)
    Ix(:,c) = I(:,c)-I(:,c-1);
end
for r=2:size(I,1)
    Iy(r,:) = I(r,:)-I(r-1,:);
end
It = I1-I;

% initialise u & v
u = zeros(size(I));
v = zeros(size(I));

% Optical Flow: Lucas-Kanade
Nw = 3; % window size [NwxNw]
A = zeros(Nw*Nw,2);
b = zeros(Nw*Nw,1);
w = zeros(2,1);
% sliding window [overlaping]
for sr=1:Ny-(Nw-1)
    for sc=1:Nx-(Nw-1)
        for i=0:(Nw*Nw-1)
            b(i+1) = -It(sr+floor(i/Nw),sc+mod(i,Nw));
            A(i+1,1) = Ix(sr+floor(i/Nw),sc+mod(i,Nw));
            A(i+1,2) = Iy(sr+floor(i/Nw),sc+mod(i,Nw));
        end
        if(cond(A'*A)<50)
            w = (A'*A)\(A'*b);
            for i=0:(Nw*Nw-1)
                u(sr+floor(i/Nw),sc+mod(i,Nw)) = w(1);
                v(sr+floor(i/Nw),sc+mod(i,Nw)) = w(2);
            end
        end
    end
end

% interpolate u & v in between
for r=1:Ny
    for c=1:Nx
        if (~(r==1||r==Ny||c==1||c==Nx))
            if(u(r,c)==0 && v(r,c)==0)
                u(r,c) = (u(r,c-1)+u(r,c+1)+u(r-1,c)+u(r+1,c))/4;
                v(r,c) = (v(r,c-1)+v(r,c+1)+v(r-1,c)+v(r+1,c))/4;
            end
        end
    end
end
figure(3);quiver(u,v);axis ([0 256 0 256]);

% True velocities
%I3 
%utru = zeros(size(I));  vtru = ones(size(I));
%I2 
%utru = ones(size(I));  vtru = zeros(size(I));
% %I4 
% for r=1:Ny
%     for c=1:Nx
%         utru(r,c) = (c-Nx/2)*cosd(-1.5)+(r-Ny/2)*sind(-1.5)-(c-Nx/2);
%         vtru(r,c) = (c-Nx/2)*sind(-1.5)-(r-Ny/2)*cosd(-1.5)+(r-Ny/2);
%     end
% end

% Ground Truth
gtru = readFlowFile('other-gt-flow/Grove2/flow10.flo');
utru = gtru(:,:,1);
vtru = gtru(:,:,2);
figure(4);quiver(utru,vtru);axis ([0 256 0 256]);

% Comparison measures : MSE
SSE = 0;
for r=1:Ny
    for c=1:Nx
        uerr = u(r,c)-utru(r,c); 
        verr = v(r,c)-vtru(r,c);
        err = uerr*uerr+verr*verr;
        SSE = SSE + err;
    end
end
MSE = SSE/(Nx*Ny);

% Comparison measures : MAE
SAE = 0;
for r=1:Ny
    for c=1:Nx
        if((u(r,c)*utru(r,c)+v(r,c)*vtru(r,c))==0 && (sqrt(u(r,c)*u(r,c)+v(r,c)*v(r,c))*sqrt(utru(r,c)*utru(r,c)+vtru(r,c)*vtru(r,c)))==0)
            tmp = 0;
        else
            tmp = ((u(r,c)*utru(r,c)+v(r,c)*vtru(r,c))/(sqrt(u(r,c)*u(r,c)+v(r,c)*v(r,c))*sqrt(utru(r,c)*utru(r,c)+vtru(r,c)*vtru(r,c))));
        end
        aerr = acos(tmp);
        SAE = SAE + aerr;
    end
end
MAE = SAE/(Nx*Ny);

% warping
warp = zeros(size(I));
for r=1:Ny
    for c=1:Nx
        r_new = round(r+v(r,c));
        c_new = round(c+u(r,c));
        if(r_new>0 && r_new<=Ny && c_new>0 && c_new<=Nx)
            warp(r_new,c_new) = I(r,c);
        end
    end
end
figure(11);imshow(warp);

% warping improved
for r=1:Ny
    for c=1:Nx
        if (~(r==1||r==Ny||c==1||c==Nx))
            if(warp(r,c)==0)
                warp(r,c) = (warp(r,c-1)+warp(r,c+1)+warp(r-1,c)+warp(r+1,c))/4;
            end
        end
    end
end
figure(12);imshow(warp);