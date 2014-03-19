#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <math.h>
#include "svm.h"
#include "svm-scale.h"
#include <fstream>

using namespace std;
using namespace cv;

void SlidingWindow::inputParams(SlidingWindow::Params params)
{
	// Input parameters
	min_width = params.min_width;
	min_height = params.min_height;
	max_width = params.max_width;
	max_height = params.min_height;	
	scale_factor = params.scale_factor;
	aspect_ratio = params.aspect_ratio;
}

int SlidingWindow::main(Mat img)
{
	Mat img=imread(argv[1]);
	Rect window;	// sliding window
	
	// Sliding Window parameters
	int delta_x = 25;
	int delta_y = 25;	
	Scalar color(255,0,0);
	
	// Sliding window variables
	int img_width = img.cols;
	int img_height = img.rows;	
	int start_width, start_height;	// starting window size
	int scale_number;
	
	if(aspect_ratio >= 1)
	{
		start_width = min(min_width,min_height);
		start_height = aspect_ratio*start_width;
	}
	else
	{
		start_height = min(min_width,min_height);
		start_width = start_height/aspect_ratio;		
	}

	scale_number = ceil(log(max(max_width,max_height)/max(min_width,min_height))/log(scale_factor));
	
	// Starting window
	window.x = 0;
	window.y = 0;
	window.width = start_width;
	window.height = start_height;
	cout<<"scale no "<<scale_number<<"\n";
	
	for (int i=0; i< scale_number; i++)
	{
		color= Scalar(i*255/scale_number,0,0);
		int sliding_num_x = ceil((img_width-window.width)/delta_x);
		int sliding_num_y = ceil((img_height-window.height)/delta_y);
	
		for(int j=0; j<sliding_num_y; j++)
		{
			for(int k=0; k<sliding_num_x; k++)
			{
				//rectangle(img,window,color);
				Mat win_img = img(window).clone();
				imshow ("window",win_img);
				waitKey(0);
								
				float prob; int OP_class;
				test_object_detection(win_img, OP_class, prob);
				if(OP_class==1)
				{
					//cout<<"buoy: prob "<<prob<<"\n";
				}
				else
				{
					//cout<<"not buoy: prob "<<prob<<"\n";					
				}
				window.x = window.x + delta_x;				
			}
			window.x = 0;
			window.y = window.y + delta_y;			
		}
		window.y = 0;
		window.width = window.width*scale_factor;
		window.height = window.height*scale_factor;		
	}
		
	return 0;
}
