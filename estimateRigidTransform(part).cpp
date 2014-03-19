#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/video/tracking.hpp"

using namespace std;
using namespace cv;

int main(int argc, char** argv){

// estimateRigidTransform method to find rotation
std::vector<Point2f> src;
std::vector<Point2f> dst;
Point2f point_1,point_2;
Point2f centre(uo,vo);

/*
Mat sr= Mat (N,2,CV_8UC1);
Mat ds= Mat (N,2,CV_8UC1);
unsigned char *src = (unsigned char*)(sr.data);
unsigned char *dst = (unsigned char*)(ds.data);
*/
for(size_t i = 0; i < 15; i++)
{
    point_1 = keypoints1[matches[i].queryIdx].pt;
    point_2 = keypoints2[matches[i].trainIdx].pt;  
    point_1 = point_1-centre;
    point_2 = point_2-centre;
   src.push_back(point_1);
   dst.push_back(point_2);    
  
/*
src[2*i]=point_1.x;
src[2*i+1]=point_1.y;
dst[2*i]=point_2.x;
dst[2*i+1]=point_2.y;
*/
}
Mat rot=estimateRigidTransform(src,dst,false);
cout<<rot<<"\n";


