function im12 = warpHomography(img1,H12)
[W1,W2] = size(img1);
newX = zeros(W1,W2); newY = newX;
% warp im1 to get a new image
for i=1:W1
     for j=1:W2
        v = H12*[j i 1]';
        v(1:2) = v(1:2)/v(3);
        newX(i,j) = v(1);
        newY(i,j) = v(2);
    end
end

im12= zeros(W1,W2);
for i=1:W1
     for j=1:W2
        v = H12*[j i 1]';
        v(1:2) = v(1:2)/v(3);
        if (v(2) >= 1 && v(1) >= 1&& v(2)<=W1 && v(1)<=W2), im12(floor(v(2)),floor(v(1))) = img1(i,j); end;
     end
 end
im12 = im12./255;
end