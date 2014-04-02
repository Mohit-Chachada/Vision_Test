function [H,e] = Homography_LeastSquares(I1,I2)
% H is 3x3 homography matrix from Image1 to Image2:=> I2=H*I1;

% Find matching SIFT keypoints in the two images
[~,pts1,pts2] = match(I1,I2);

% find svd based Homography
H = findHomography(pts1,pts2);
P1 = ones(3,size(pts1,2));  P2 = ones(3,size(pts2,2));
P1(1:2,:) = pts1(1:2,:);  P2(1:2,:) = pts2(1:2,:);
err = (P2-H*P1);
err = err.*err;
e = sum(sum(err))/(size(P1,1)*size(P1,2));
fprintf('LS error %d  \n',e);
end