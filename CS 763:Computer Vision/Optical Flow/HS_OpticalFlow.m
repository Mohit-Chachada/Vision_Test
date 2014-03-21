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

% tform2 = maketform('affine',[1 0 0;0 1 0;1 0 1]);
% I2 = imtransform(I,tform2,'size',size(I),'FillValues',0); figure(1);imshow(I2);

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

% Optical Flow: Horne-Shunk
ubar = zeros(size(I));
vbar = zeros(size(I));
lm = 10;

% Iterative loop for u & v
for T=1:50
   for r=1:Ny
    for c=1:Nx
      if(r==1 && c~=1 && c~=Nx) 
       ubar(r,c) = (u(r,c)+u(r+1,c)+u(r,c-1)+u(r,c+1))/4;
       vbar(r,c) = (v(r,c)+v(r+1,c)+v(r,c-1)+v(r,c+1))/4;
      elseif(r==Ny && c~=1 && c~=Nx)
       ubar(r,c) = (u(r-1,c)+u(r,c)+u(r,c-1)+u(r,c+1))/4;
       vbar(r,c) = (v(r-1,c)+v(r,c)+v(r,c-1)+v(r,c+1))/4;
      elseif(c==1 && r~=1 && r~=Ny)
       ubar(r,c) = (u(r-1,c)+u(r+1,c)+u(r,c)+u(r,c+1))/4;
       vbar(r,c) = (v(r-1,c)+v(r+1,c)+v(r,c)+v(r,c+1))/4; 
      elseif(c==Nx && r~=1 && r~=Ny)
       ubar(r,c) = (u(r-1,c)+u(r+1,c)+u(r,c-1)+u(r,c))/4;
       vbar(r,c) = (v(r-1,c)+v(r+1,c)+v(r,c-1)+v(r,c))/4;  
      elseif(r==1 && c==1)
       ubar(r,c) = (u(r,c)+u(r+1,c)+u(r,c)+u(r,c+1))/4;
       vbar(r,c) = (v(r,c)+v(r+1,c)+v(r,c)+v(r,c+1))/4; 
      elseif(r==1 && c==Nx)
       ubar(r,c) = (u(r,c)+u(r+1,c)+u(r,c-1)+u(r,c))/4;
       vbar(r,c) = (v(r,c)+v(r+1,c)+v(r,c-1)+v(r,c))/4;  
      elseif(r==Ny && c==1)
       ubar(r,c) = (u(r-1,c)+u(r,c)+u(r,c)+u(r,c+1))/4;
       vbar(r,c) = (v(r-1,c)+v(r,c)+v(r,c)+v(r,c+1))/4; 
      elseif(r==Ny && c==Nx)
       ubar(r,c) = (u(r-1,c)+u(r,c)+u(r,c-1)+u(r,c))/4;
       vbar(r,c) = (v(r-1,c)+v(r,c)+v(r,c-1)+v(r,c))/4;        
      else
       ubar(r,c) = (u(r-1,c)+u(r+1,c)+u(r,c-1)+u(r,c+1))/4;
       vbar(r,c) = (v(r-1,c)+v(r+1,c)+v(r,c-1)+v(r,c+1))/4;
      end
      
      u(r,c) = ubar(r,c) - lm*Ix(r,c)*((Ix(r,c)*ubar(r,c)+Iy(r,c)*vbar(r,c)+It(r,c))/(1+lm*Ix(r,c)*Ix(r,c)+lm*Iy(r,c)*Iy(r,c)));
      v(r,c) = vbar(r,c) - lm*Iy(r,c)*((Ix(r,c)*ubar(r,c)+Iy(r,c)*vbar(r,c)+It(r,c))/(1+lm*Ix(r,c)*Ix(r,c)+lm*Iy(r,c)*Iy(r,c)));
    end
   end
end
figure(3);quiver(u,v);axis ([0 256 0 256]);

% % True u & v
% %I3 
%utru = zeros(size(I));  vtru = ones(size(I));
% %I2 
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
figure(4);quiver(utru,vtru);
axis ([0 256 0 256]);

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