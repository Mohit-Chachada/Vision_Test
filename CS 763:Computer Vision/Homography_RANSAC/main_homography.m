clear;
clc;
img1 = imread('test_images\goi1_downsampled.jpg');
img2 = imread('test_images\goi2_downsampled.jpg');

% Homography using RANSAC
numiters = 500;
threshold = 0.05;
[H12,e_RN] = Homography_RANSAC(img1,img2,numiters,threshold);
disp('H12_RANSAC: I2=H*I1');
disp(H12);

% warp image
im12 = warpHomography(img1,H12);
figure(1); imshow(im12);title('im12');
% imwrite(im12,'warp12_RANSAC.png');
% warp using perspective transformation method
warp12 = warpHomography2(img1,H12);
figure(2); imshow(warp12);title('warp12');
% imwrite(warp12,'warp12_RANSAC_P.png');

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Homography using Least Squares
[H12LS,e_LS] = Homography_LeastSquares(img1,img2);
disp('H12_LeastSquares: I2=H*I1');
disp(H12LS);
% warp image
im12LS = warpHomography(img1,H12LS);
figure(3); imshow(im12LS);title('im12LS');
% imwrite(im12LS,'warp12_LS.png');
% warp using perspective transformation method
warp12LS = warpHomography2(img1,H12LS);
% imwrite(warp12LS,'warp12_LS_P.png');
figure(4); imshow(warp12LS);title('warp12LS');