#include <cmath>
#include <iostream>
#include <time.h>
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"

using namespace std;
using namespace cv;

int main(int argc, char** argv){

Mat img1=imread("pic3.png");
Mat mask=imread("mask.png");
Mat img,test,test_r;
//Mat img2=imread(argv[2]);

Point2f centre(160,120);
Mat M=getRotationMatrix2D(centre,-1,1); // 2nd arg angle degree (anticlk +ve); 1xscale
//cout<<M<<"\n";

//img1.copyTo(img,mask); //apply mask
img1.copyTo(img);
imshow("original_img",img1);
waitKey(10);
imshow("masked_img",img);
waitKey(10);
//imwrite("test.png",img);

Size dsize=img1.size();
int flags=INTER_LINEAR;
 const Scalar borderValue=Scalar();

Size S = Size(320,240);
VideoWriter outputVideo;
const string NAME="10.avi";
outputVideo.open(NAME,CV_FOURCC('M','J','P','G'),10, S, true); // 10 FPS

for(int i=0;i<360;i++){
M=getRotationMatrix2D(centre,0,1);
warpAffine(img1,img,M,dsize,INTER_LINEAR,BORDER_CONSTANT,0);
outputVideo.write(img);
}
 waitKey(0);
/*
warpAffine(img,img,M,dsize,INTER_LINEAR,BORDER_CONSTANT,0);
imwrite("tx6_r.png",img); //rotated image
namedWindow("rotated_img",1);
imshow("rotated_img",img);
waitKey(0);
*/

}
