function TF = estimate2dMotionRANSAC(pts1,pts2,model)
% TF is 3x3 matrix with last row [0,0,1] always (since affine)
if(model==1)        % translation only
    K = 1;
elseif(model==2)    % rotation+translation
    K = 2;
elseif(model==3)    % affine+translation
    K = 3;
end

N = size(pts1,2);

% RANSAC params
%P = 0.99;
lam = 0.05; % square error threshold for a point to be an inlier
S = 100;
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
    
    % find Least Squares estimated motion using these K points
    if (model==1)
        [tx,ty] = estimate2dTranslationLS(pts1_k,pts2_k);
        tf(:,:,s) = [1,0,tx;0,1,ty;0,0,1];
    elseif (model==2)
        [tx,ty,theta] = estimate2dRotation_TranslationLS(pts1_k,pts2_k);
        tf(:,:,s) = [cos(theta),-sin(theta),tx;sin(theta),cos(theta),ty;0,0,1];
    elseif (model==3)
        [tx,ty,A] = estimate2dAffine_TranslationLS(pts1_k,pts2_k);
        tf(1:2,1:2,s) = A(1:2,1:2); tf(1,3,s) = tx; tf(2,3,s) = ty;
        tf(3,3,s) = 1;
    end
    
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
        if(model==1)
            tr(1:2) = tf(1:2,3);
            err = (pts2_r(:,r)+tr(:)-pts1_r(:,r));
        elseif(model==2)
            R(1:2,1:2) = tf(1:2,1:2,s);
            tr(1:2) = tf(1:2,3);
            err = (R*pts2_r(:,r)+tr(:)-pts1_r(:,r));
        elseif(model==3)
            A(1:2,1:2) = tf(1:2,1:2,s);
            tr(1:2) = tf(1:2,3);
            err = (A*pts2_r(:,r)+tr(:)-pts1_r(:,r));
        end
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
TF = tf(:,:,set_index);
end