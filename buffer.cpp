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

Mat B[5];
B[0]=imread("10.png");
B[1]=imread("16.png");
B[2]=imread("37.png");
B[3]=imread("98.png");
B[4]=imread("all_4.png");

Mat newImage=imread("1.png");
int No=5;

for (int i=0;i<No-1;i++){
	B[i]=B[i+1];
}
B[No-1]=newImage;

imshow("B[0]",B[0]);
waitKey(0);
imshow("B[1]",B[1]);
waitKey(0);
imshow("B[2]",B[2]);
waitKey(0);
imshow("B[3]",B[3]);
waitKey(0);
imshow("B[4]",B[4]);
waitKey(0);

return 0;
}
