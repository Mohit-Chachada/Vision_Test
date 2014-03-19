#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/video/tracking.hpp"

using namespace std;
using namespace cv;

int main(int argc, char** argv){
	Mat src, dst;
	float uo,vo;
	uo=src.cols/2;
	vo=src.rows/2;	
	src = imread(argv[1]);
	dst = imread(argv[2]);
	Mat A = estimateRigidTransform(src,dst,true); 
	//cout<<A<<"\n";
	
std::vector< Point2f > obj1;
std::vector< Point2f > scene1;
for (int i=0;i<4;i++)
{
    obj1.push_back(Point2f(i,i+3));
    scene1.push_back(Point2f(i+1,i+20));
}
Mat H1 = estimateRigidTransform(obj1,scene1,true);
cout << H1 << endl;

	return 0;
}
