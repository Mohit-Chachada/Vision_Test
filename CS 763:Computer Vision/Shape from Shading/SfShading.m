clear;
clc;

ps=-0.5;
qs=0.5;
Nt=1;
eps=1;
%I = imread('images/beethoven3.pgm');
I = imread('images/sphere2.pgm');
%I = imread('images/mozart1.pgm');
I = im2double(I);
%imshow(I);
Nc = size(I,2);
Nr = size(I,1);
Z = zeros(size(I));
for t=1:Nt
for x=2:Nc
    for y=2:Nr
    p = Z(y,x)-Z(y,x-1);
    q = Z(y,x)-Z(y-1,x);
    R = (1+p*ps+q*qs)/(sqrt(1+ps*ps+qs*qs)*sqrt(1+p*p+q*q));
    if(R<0)
    R=0;
    end
    ff = I(y,x)-R;
    grad_f = -((ps+qs)/(sqrt(1+ps*ps+qs*qs)*sqrt(1+p*p+q*q))) + (((p+q)*(1+p*ps+q*qs))/(((p*p+q*q+1)^(1.5))*sqrt(1+ps*ps+qs*qs)));

    Z(y,x) = Z(y,x) - (ff/eps+grad_f);
    end
end
medfilt2(Z);
end
maxZ=max(Z(:));
Z=Z./maxZ;
imshow(Z);
imwrite(Z,'Dsphere2_1.png');