function [tx,ty,theta] = estimate2dRotation_TranslationLS(pts1,pts2)
N = size(pts1,2);
pts1_a = zeros(2,1);
pts2_a = zeros(2,1);
pts1_a(1) = sum(pts1(1,:))/N;
pts1_a(2) = sum(pts1(2,:))/N;
pts2_a(1) = sum(pts2(1,:))/N;
pts2_a(2) = sum(pts2(2,:))/N;

dpts1 = zeros(size(pts1));
dpts2 = zeros(size(pts2));
for n=1:N
    dpts1(:,n) = pts1(:,n)-pts1_a;
    dpts2(:,n) = pts2(:,n)-pts2_a;
end

A = dpts2*dpts1';
[U,~,V] = svd(A);
R = V*U';
if(det(R)==-1) 
    disp('det(R)= -1');
    J = [1,0;
         0,-1];
     R = V*J*U';
end
theta = (acos(R(1,1))+asin(-R(1,2))+asin(R(2,1))+acos(R(2,2)))/4;
theta = theta*180/3.14;
tt = pts1_a-R*pts2_a;
tx = tt(1,1);
ty = tt(2,1);
end