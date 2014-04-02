function frame_wp = warpRotationFrame(frame,theta)
frame_wp = imrotate(frame,theta,'bilinear','crop');
end