function frame_wp = warpTranslationFrame(frame,tr)
% tr is [tx,ty,0]'
tx = round(tr(1));
ty = round(tr(2));

frame_wp = zeros(size(frame));
H = size(frame_wp,1);
W = size(frame_wp,2);

b = frame;
if(tx>=0 && ty>=0)
    frame_wp(ty+1:H,tx+1:W) = b(1:H-ty,1:W-tx);
elseif(tx<=0 && ty>=0)
    tx = abs(tx);
    frame_wp(ty+1:H,1:W-tx) = b(1:H-ty,tx+1:W);
elseif(tx>=0 && ty<=0)
    ty = abs(ty);
    frame_wp(1:H-ty,tx+1:W) = b(ty+1:H,1:W-tx);
elseif(tx<=0 && ty<=0)
    tx = abs(tx);    ty = abs(ty);
    frame_wp(1:H-ty,1:W-tx) = b(ty+1:H,tx+1:W);
end
end