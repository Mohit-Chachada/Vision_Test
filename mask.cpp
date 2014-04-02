#include <iostream>
#include <stdio.h>
#include <fstream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

int main(int argc, char** argv){
int height=512;
int width=512;
Mat mask(height,width,CV_8U);





for(int v=0;v<512;v++){
  for(int u=0;u<512;u++){
    //for(int v=512*330;v<512*512;v++){
     if ( (u<156||u>356) || (v<156||v>356)){
     mask.data[width*v+u]=0;
     }  
     else mask.data[width*v+u]=255;
  }
}
imshow("mask",mask);
imwrite("mask.jpg",mask);
waitKey(0);
return 0;
}
