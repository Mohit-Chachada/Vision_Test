clear;
clc;
vidObj = aviread('gbus.avi',(1:20));
Is1 = vidObj(1);
I1 = ind2gray(Is1.cdata,Is1.colormap);
imshow(I1);
% 
% vid = mmreader('gbus.avi');%,'NumberOfFrames',20,'Path','./sample_videos');
% nFrames = 20;%vid.NumberOfFrames;
% vidHeight = vid.Height;
% vidWidth = vid.Width;
% 
% % Preallocate movie structure.
% mov(1:nFrames) = struct('cdata', zeros(vidHeight, vidWidth, 3, 'uint8'),'colormap', []);
% 
% % Read one frame at a time.
% for k = 1 : nFrames
%     mov(k).cdata = read(vid, k);
%     %frames(k) = ind2gray(mov(k).cdata,mov(k).colormap);
% end
% frame1=ind2gray(mov(1).cdata,mov(1).colormap);

% 
% images = read(vid,[1 20]);
% images_s(1:20) = 
% %im1=images(:,:,:,2);
% %I1 = ind2gray(im1.cdata,im1.colormap);
% %imshow(im1,'frame1');