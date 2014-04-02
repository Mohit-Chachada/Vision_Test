function frames = getVideoFrames(filename)
nFrames = 30;
vidObj = aviread(filename,(1:nFrames));
% OR
% vidObj = aviread(filename);
% nFrames = size(vidObj,2);

%frames = zeros(height,width,nFrames);
fr = cell(nFrames,1);
for k=1:nFrames
    tmp = vidObj(k);
    frames(:,:,k) = ind2gray(tmp.cdata,tmp.colormap);
    fr{k} = frames(:,:,k);
end
end