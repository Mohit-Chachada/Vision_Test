clear;
clc;
method = 2; % method=1 for LS & method=2 for RANSAC

frames = getVideoFrames('gbus.avi');
%displayvideo(frames,0.05);
frames_sh = generate_shaky_video_Rigid('gbus.avi');
%displayvideo(frames_sh,0.05);

nFrames = size(frames_sh,3);
tr = zeros(3,nFrames); % motion struct {tx,ty,theta}'
for k=2:nFrames
    [num,pts1,pts2] = match(frames_sh(:,:,k-1),frames_sh(:,:,k));
    if(method==1)
    [tx,ty,theta] = estimate2dRotation_TranslationLS(pts1,pts2);
    elseif(method==2)
        tf = estimate2dMotionRANSAC(pts1,pts2,2);
        tx = tf(1,3); ty = tf(2,3); theta = acos(tf(1,1));
    end
    tr(1,k) = tx;
    tr(2,k) = ty;
    tr(3,k) = theta;
end

% smooth the obtained motion
win_size = 7;
tr_sm = smoothMotion(tr,win_size);
%figure(1);plot(tr(1,:));hold on;plot(tr_sm(1,:),'color','g');
%figure(2);plot(tr(2,:));hold on;plot(tr_sm(2,:),'color','g');
%figure(3);plot(tr(3,:));hold on;plot(tr_sm(3,:),'color','g');

% warp shaky video to make smooth video
dtr = zeros(size(tr,1),1);
frames_wp = frames_sh;
for n=2:nFrames
    dtr = dtr + (tr(:,n)-tr_sm(:,n));
    dtr(1) = 0; dtr(2) = 0;
    frame_wp = warpRotationFrame(frames_sh(:,:,n),theta);
    frame_wp = warpTranslationFrame(frame_wp,dtr);
    frames_wp(:,:,n) = frame_wp(:,:);
% tx = tr_sm(1,n);
% ty = tr_sm(2,n);
% theta  = tr_sm(3,n);
% A = [cos(theta),-sin(theta);
%      sin(theta),cos(theta)];
% frame_wp = my_affine_warp(frames_sh(:,:,n),A);
% frame_wp = warpTranslationFrame(frame_wp,[tx,ty,0]');
% frames_wp(:,:,n) = frame_wp(:,:);
end

% combine frames_sh and frames_wp
[H,W,~] = size(frames_sh);
combine(1:H,:,:) = frames_sh(:,:,:);
combine(H+1:2*H,:,:) = frames_wp(:,:,:);

% store stabilized video
storeVideo(combine,'save.avi');
