#include <iostream>
#include <cmath>
#include <time.h>
#include <stdio.h>
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/nonfree.hpp"

#include "opencv2/video/tracking.hpp"


using namespace std;
using namespace cv;


int main( int argc, char** argv )
{
   //vector<Point2f> 
   vector<KeyPoint> _keypoints1,keypoints1,keypoints2;
    vector<Point2f> keypoints1_2f,keypoints2_2f;
   
	Mat img1=imread(argv[1]);   
	Mat img2=imread(argv[2]);
	cvtColor(img1,img1,CV_BGR2GRAY);
	cvtColor(img2,img2,CV_BGR2GRAY);
      int maxCorners=120;
      GoodFeaturesToTrackDetector detector(maxCorners);
      detector.detect(img1, _keypoints1);
	cout<<_keypoints1.size()<<endl;
   
           // convert KeyPoint to Point2f
       for (int i=0;i<_keypoints1.size(); i++)
       {
        float x= _keypoints1[i].pt.x;
        float y= _keypoints1[i].pt.y;
        keypoints1_2f.push_back(cv::Point2f(x,y));
       }

        // subpixel corner refinement
    Size SPwinSize = Size(3,3);
    Size zeroZone = Size(1,1);
    TermCriteria SPcriteria=TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 30, 0.01);
    cornerSubPix(img1, keypoints1_2f, SPwinSize, zeroZone, SPcriteria);

      // LK OpticalFlow
      vector<uchar> status; //	(made global for class)
      vector<float> err;
      Size winSize=Size(21,21);
      int maxLevel=3;
      TermCriteria criteria=TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 30, 0.01);
      int flags=0;
      double minEigThreshold=1e-4;
      calcOpticalFlowPyrLK(img1, img2, keypoints1_2f, keypoints2_2f, status, err, winSize, maxLevel, criteria, flags, minEigThreshold);

	//cout<<keypoints2_2f[90]<<endl;
    
    int nMatches=0;
        for (int i=0;i<keypoints2_2f.size(); i++)
       {
        if(status[i]==1){
	nMatches++;
        }
       }
    
    cout<<nMatches<<"\n";
    int *matches;
    matches=new int [nMatches];
    int k=0;
    
    // convert Point2fs to KeyPoints
    for (int i=0;i<keypoints2_2f.size(); i++)
       {
        if(status[i]==1){
        float x= keypoints2_2f[i].x;
        float y= keypoints2_2f[i].y;
        KeyPoint kp2(x,y,1.0,-1.0,0.0,0,-1);
        keypoints2.push_back(kp2);

        x= keypoints1_2f[i].x;
        y= keypoints1_2f[i].y;
        KeyPoint kp1(x,y,1.0,-1.0,0.0,0,-1);
        keypoints1.push_back(kp1);        
	matches[k]=i;
	k++;
        }
       }
       	cout<<keypoints1.size()<<endl;
	cout<<keypoints1[50].pt<<endl;         
     //  cout<<keypoints1_2f[matches[50]]<<endl;       
       cout<<keypoints1_2f[matches[50]]<<endl;
	//cout<<keypoints1[matches[50]].pt<<endl;  
	     

   /*     keypoints2[2].pt.x= keypoints2_2f[2].x;
        keypoints2[2].pt.y= keypoints2_2f[2].y;
        cout<<keypoints2[2].pt<<endl;
        
       // convert Point2f to KeyPoint
       for (int i=0;i<keypoints2_2f.size(); i++)
       {
        keypoints2[i].pt.x= keypoints2_2f[i].x;
        keypoints2[i].pt.y= keypoints2_2f[i].y;
       // keypoints2[i].pt=cv::Point2f(x,y);
//        keypoints2.push_back(cv::Point2f(x,y));
       }      
*/

   return 0;
}
