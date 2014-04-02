function [tx,ty] = estimate2dTranslationLS(pts1,pts2)
tr = pts1-pts2;
tx=sum(tr(1,:))/size(tr,2);
ty=sum(tr(2,:))/size(tr,2);
end