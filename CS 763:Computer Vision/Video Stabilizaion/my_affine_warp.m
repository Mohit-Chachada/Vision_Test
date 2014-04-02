function im2 = my_affine_warp(im1,A)

[H,W] = size(im1);
yc = round(H/2); xc = round(W/2);
im2 = zeros(H,W);
%invA = inv(A);
for i=1:H
    for j=1:W
        xy = A\[j-xc i-yc]';
        
        y = round(xy(2) + yc);
        x = round(xy(1) + xc);
        
        if x > 0 && y > 0 && x <= W && y <= H,
            im2(i,j) = im1(y,x);
        end
    end
%     im2 = im2./255;
%     im2 = im2uint8(im2);
   % imshow(im2);
end