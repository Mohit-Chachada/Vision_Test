function storeVideo(frames,filename)
[H,W,nFrames] = size(frames);
% Preallocate movie structure.
mov(1:nFrames) = struct('cdata', zeros(H,W,1,'uint8'),'colormap', []);
% Read one frame at a time.
for k = 1 : nFrames
    [X,map] = gray2ind(frames(:,:,k),256);
    mov(k).cdata = X;
    mov(k).colormap = map;
end
movie2avi(mov,filename,'fps',15);
end