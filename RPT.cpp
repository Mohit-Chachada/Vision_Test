/*
 * Rotation Invariant Feature Descriptor
 * RPT: Ring Projection Transform
 * Author: Mohit Chachada
 */

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

#define PI 3.14

vector<float> calcRPTFeature(Mat img)
{
	//Params
	int N = 20;											// size of feature vector
	
	Mat I_gray = Mat(Size(img.cols,img.rows),CV_8UC1);
	cvtColor(img,I_gray,CV_BGR2GRAY);
	int width = img.cols;
	int height = img.rows;
	Point center(width/2,height/2);
	float max_r = min(width/2,height/2);		//max radius
	vector<float> radius;							// all possible radii
	radius.resize(N);
	vector<int> NrPoints;							//no of points on each circle
	NrPoints.resize(N);
	vector<vector<Point> > pt;						//point locations on each circle
	pt.resize(N);
	vector<float>I_sum,I_avg,I_norm;
	I_sum.resize(N);
	I_avg.resize(N);
	I_norm.resize(N);			

	//since avging over pixels on the circle at radius r, the no of points are taken=floor(2*pi*r) => del_theta=1/r
	// x=xc+r*cos(theta); theta=k/r; k=0,1,2,3...NrPoints-1

	for(int i=0;i<N;i++)
	{
		radius[i] = (i+1)*max_r/N;
		NrPoints[i] = floor(2*PI*radius[i]);
		pt[i].resize(NrPoints[i]);
		I_sum[i] = 0;
		I_avg[i] = 0;
		int Imax = 0;
		for(int j=0; j<NrPoints[i];j++)
		{
			pt[i][j].x = floor( center.x + radius[i]*cos(j/radius[i]) );
			pt[i][j].y = floor( center.y + radius[i]*sin(j/radius[i]) );			
//			cout<< pt[i][j]<<"\t";
			I_sum[i] += I_gray.at<uchar>(pt[i][j].x,pt[i][j].y);
			int temp = I_gray.at<uchar>(pt[i][j].x,pt[i][j].y);			
			if(temp>Imax) Imax=temp;
		}
		I_avg[i] = I_sum[i]/NrPoints[i];
		I_norm[i] = I_avg[i]/Imax;
	}
	
	// forming the feature vector
	vector<float> feature;
	int feature_size = N;
	feature.resize(feature_size);
	for(int i=0; i<feature_size; i++)	
	{
//		feature[i] = I_avg[i];
		feature[i] = I_norm[i];
		//To avoid variations at vary small and very large radii, dont consider those feature elements
		if(radius[i]<0.30*max_r || radius[i]>0.90*max_r)
		{
			feature[i] = 0;
		}		
	}
	
	return feature;
}

int main(int argc, char** argv)
{
	Mat img=imread(argv[1]);
	// NOTE: Not Illumination Invariant
	vector<float> feature=calcRPTFeature(img);
	
	cout<<"input feature: "<<"\n";
	for(int i=0;i<feature.size();i++)
	{
		cout<<feature[i]<<"\t";
	}
	cout<<"\n";
	
	// test on rotated input image
	Mat test;
	Point center(img.cols/2,img.rows/2);
	float angle = 50; //[anticlk +ve]
	float scale = 1;
	Mat M=getRotationMatrix2D(center,angle,scale);
	warpAffine(img,test,M,img.size(),INTER_LINEAR,BORDER_CONSTANT,0);

	// calc feature on test rotated image
	vector<float> feature_test=calcRPTFeature(test);	
	
	cout<<"test feature: "<<"\n";
	for(int i=0;i<feature_test.size();i++)
	{
		cout<<feature_test[i]<<"\t";
	}
	cout<<"\n";
		
	imshow("input image",img);
	imshow("rotated image",test);
	waitKey(0);
	return 0;
}	
