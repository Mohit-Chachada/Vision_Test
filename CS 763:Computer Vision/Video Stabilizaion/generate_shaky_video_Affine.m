function vid = generate_shaky_video_Affine(filename)
% a = mmread('E:\VideoStab\Stable\coastguard.avi');
% framerate = a.rate;
% vid = zeros(a.height,a.width,a.nrFramesTotal);

frames = getVideoFrames(filename);
nFrames = size(frames,3);
for i=1:nFrames
    %b = rgb2gray(a.frames(i).cdata); 
    b = frames(:,:,i);
    [H,W] = size(b);
    if i > 1, tx = round(rand(1)*3); else tx = 0; end;
    if i > 1, ty = round(rand(1)*3); else ty = 0; end;
    if i > 1,
        A = [1 0; 0 1] + rand(2,2)*0.1;
    else
        A = [1 0; 0 1];
    end;
           
    c = my_affine_warp(b,A);    
    d = c; d(:,:) = 0;
    d(ty+1:H,tx+1:W) = c(1:H-ty,1:W-tx);
    
    vid(:,:,i) = d;
end
% filename = 'shaky_affine_coastguard2.avi';
% writevideo(filename,vid/max(vid(:)),framerate);
end    