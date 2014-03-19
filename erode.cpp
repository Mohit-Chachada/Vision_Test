#include <cmath>
#include <time.h>
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;

int main(int argc, char** argv){
Mat mask=imread("mask.png");
int erosion_size = 50;
Mat element = getStructuringElement( MORPH_ELLIPSE,
                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                       Point( erosion_size, erosion_size ) );

    namedWindow("original", 1);
    imshow("original", mask);
    waitKey(10);
erode(mask,mask,element);
    namedWindow("erosion", 1);
    imshow("erosion", mask);
    waitKey(0);
    imwrite("mask_e.png",mask);
    return 0;
}
