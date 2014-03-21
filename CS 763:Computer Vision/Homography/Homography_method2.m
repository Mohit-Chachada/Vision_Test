clear;
clc;
pts1 = dlmread('pts2_homography.txt');
pts2 = dlmread('pts1_homography.txt');

for count=1:2
    if (count==2)
        tmp1 = pts2;
        pts2 = pts1;
        pts1 = tmp1;
    end
    Y1 = pts1(:,1);
    X1 = pts1(:,2);
    Y2 = pts2(:,1);
    X2 = pts2(:,2);
    
    [N Ny] = size(X1);
    
    A = zeros(2*N,8);
    b = zeros(2*N,1);
    
    for i=1:2:N
        A(i,:) = [X1(i) Y1(i) 1 0 0 0 -X2(i)*X1(i) -X2(i)*Y1(i)];
        A(i+1,:) = [0 0 0 X1(i) Y1(i) 1 -Y2(i)*X1(i) -Y2(i)*Y1(i)];
        b(i) = X2(i);
        b(i+1) = Y2(i);
    end
    
    [U,S,V] = svd(A);
    
    P = zeros(8,2*N);
    
    for k=1:8
        if S(k,k) ~= 0;
            P(k,k) = 1/S(k,k);
        else
            P(k,k) = 0;
        end
    end
    t = V*P*U'*b;
    %t=(A'*A)\(A'*b); %same result
    H12 = [t(1) t(2) t(3); t(4) t(5) t(6); t(7) t(8) 1];
    %scaling factor   
    gamma = 0;
    for i=1:N
        gamma = gamma + 1/(H12(3,1)*X1(i)+H12(3,2)*Y1(i)+1);
    end
    gamma = gamma/N;
    H12 = gamma*H12;
    if(count==1)
        tmp_H = H12;
    elseif (count==2)
        H21 = H12;
        H12 = tmp_H;
    end
end
disp(H12);
disp(H21);
I1=H21*H12;
I2=H12*H21;
disp(I1);
disp(I2);

img1 = imread('im1_homography.png');
img2 = imread('im2_homography.png');
% Warping
img_height = size(img1,1);
img_width = size(img1,2);
% img corners
ch1 = [1 1;img_width 1; img_height img_width; 1 img_height];

% img corners
P1 = ones(3,4);
P1(1,:) = ch1(:,1)';
P1(2,:) = ch1(:,2)';
P2 = H12*P1; 

% warp img corners
ch2(:,1) = P2(1,:)';
ch2(:,2) = P2(2,:)';

tform12 = maketform('projective',ch1,ch2);
warp12 = imtransform(img2,tform12);
tform21 = maketform('projective',ch2,ch1);
warp21 = imtransform(img1,tform21);
figure(1);imshow(img1);title('img1');
figure(2);imshow(warp12);title('warp12');
figure(3);imshow(warp21);title('warp21');
figure(4);imshow(img2);title('img2');
%imwrite(warp12,'warp12.png');
%imwrite(warp21,'warp21.png');