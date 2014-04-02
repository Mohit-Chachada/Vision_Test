function [H,e] = Homography_RANSAC(I1,I2,numiters,threshold)
% H is 3x3 homography matrix from Image1 to Image2:=> I2=H*I1;
% numiters is the termination criteria for RANSAC
% threshold decides whether a point is inlier or not

% Find matching SIFT keypoints in the two images
[N,pts1,pts2] = match(I1,I2);

% For Homography min no of points to find H is 4;
K = 4;

% RANSAC params
%P = 0.99;
lam = threshold; % square error threshold for a point to be an inlier
S = numiters;

tf = zeros(3,3,S);
nrInliers = zeros(S,1);
maxInliers = 0;
set_index = 1;

for s=1:S
    % select K out of N points
    index = randperm(N);
    index = index(1:K);
    pts1_k = zeros(size(pts1,1),K);
    pts2_k = zeros(size(pts2,1),K);
    for k=1:K
        pts1_k(:,k) = pts1(:,index(k));
        pts2_k(:,k) = pts2(:,index(k));
    end
    
    % find svd based Homography using these K points
    tf(:,:,s) = findHomography(pts1_k,pts2_k);
    
    % find remaining (N-K) points
    R = N-K;
    pts1_r = zeros(size(pts1,1),R);
    pts2_r = zeros(size(pts2,1),R);
    r = 1;
    for run_id=1:N
        flag=false;
        for m=1:K
            if(index(m)==run_id)
                flag=true;
                break;
            end
        end
        if(flag==false)
            pts1_r(:,r) = pts1(:,run_id);
            pts2_r(:,r) = pts2(:,run_id);
            r=r+1;
        end
    end
    
    % find squared error for all the remaining (N-K) points
    % and find the number of inliers
    for r=1:R
        P1 = ones(3,1);  P2 = ones(3,1);
        P1(1:2) = pts1_r(1:2,r);  P2(1:2) = pts2_r(1:2,r);
        err = (P2-tf(:,:,s)*P1);
        err = err.*err;
        e = sum(err(:));
        if(e<lam)
            nrInliers(s)=nrInliers(s)+1;
        end
    end
    if(nrInliers(s)>maxInliers)
        maxInliers = nrInliers(s);
        set_index = s;
    end
end
H = tf(:,:,set_index);
P1 = ones(3,size(pts1,2));  P2 = ones(3,size(pts2,2));
P1(1:2,:) = pts1(1:2,:);  P2(1:2,:) = pts2(1:2,:);
err = (P2-H*P1);
err = err.*err;
e = sum(sum(err))/(size(P1,1)*size(P1,2));
fprintf('RANSAC error %d  \n',e);
% fprintf('maxInliers %d  \n',maxInliers);
% fprintf('total pts %d \n',N);
end