function warp12 = warpHomography2(img1,H12)
% Warping using perspective transform
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
ch2(:,1) = (P2(1,:)./P2(3,:))';
ch2(:,2) = (P2(2,:)./P2(3,:))';

tform12 = maketform('projective',ch1,ch2);
warp12 = imtransform(img1,tform12,'XData',[1 size(img1,2)],'YData',[1 size(img1,1)]);
end