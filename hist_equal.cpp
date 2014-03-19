#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

/**  @function main */
int main( int argc, char** argv )
{
  Mat src, dst;

 const char* source_window = "Source image";
 const char* equalized_window = "Equalized Image";

  /// Load image
  src = imread( argv[1], 1 );

  if( !src.data )
    { cout<<"Usage: ./Histogram_Demo <path_to_image>"<<endl;
      return -1;}
      
vector<Mat> bgr;
Mat outfile = Mat::zeros( src.size(), CV_8UC3 );
split(src, bgr);

  /// Convert to grayscale
//  cvtColor( src, src, CV_BGR2GRAY );

  /// Apply Histogram Equalization
//  equalizeHist( src, dst );

/*
  //equalizeHist( bgr[0], bgr[0] );
  equalizeHist( bgr[1], bgr[1] );
  equalizeHist( bgr[2], bgr[2] );
*/
//normalize(bgr[0], bgr[0], 0, 255, NORM_MINMAX, CV_8UC1);
normalize(bgr[1], bgr[1], 0, 255, NORM_MINMAX, CV_8UC1);
normalize(bgr[2], bgr[2], 0, 255, NORM_MINMAX, CV_8UC1);

  
      for (int i=0; i<outfile.rows; i++){
        for (int j=0; j< outfile.cols; j++){
            //Vec3b data = outfile.at<Vec3b>(i,j);
            outfile.at<Vec3b>(i,j)[0] = bgr[0].at<uchar>(i,j);// blue pixel
            outfile.at<Vec3b>(i,j)[1] = bgr[1].at<uchar>(i,j);// green pixel
            outfile.at<Vec3b>(i,j)[2] = bgr[2].at<uchar>(i,j);// red pixel
        }
    }

cvtColor(outfile,outfile,CV_BGR2HSV);
vector<Mat> hsv;
split(outfile,hsv);
//equalizeHist(outfile,outfile);
//normalize(outfile,outfile, 0, 255, NORM_MINMAX, CV_8UC1);
//equalizeHist(hsv[0],hsv[0]);
//normalize(hsv[0],hsv[0], 0, 255, NORM_MINMAX, CV_8UC1);

  /// Display results
  namedWindow( source_window, CV_WINDOW_AUTOSIZE );
  namedWindow( equalized_window, CV_WINDOW_AUTOSIZE );

  imshow( source_window, src );
  //imshow( equalized_window, dst );
  imshow( equalized_window, hsv[0] );
  /// Wait until user exits the program
    imshow( "hsv1", hsv[1] );
      imshow( "hsv2", hsv[2] );
      
  
  
  waitKey(0);

  return 0;
}
