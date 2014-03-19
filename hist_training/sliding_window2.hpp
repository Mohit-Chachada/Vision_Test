#ifndef SLIDING_WINDOW_H
#define SLIDING_WINDOW_H

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <math.h>

class SlidingWindow
{
	public:
	Class Params
	{
		int min_width;
		int min_height;
		int max_width;
		int max_height;	
		int scale_factor;
		float aspect_ratio;	//(height/width)
		Params ()
		{
			min_width = 20;
			min_height = 20;
			max_width = 150;
			max_height = 150;	
			scale_factor = 2;
			aspect_ratio = 1;	
		}
	};		
	protected:
		int min_width;
		int min_height;
		int max_width;
		int max_height;	
		int scale_factor;
		float aspect_ratio;
		
};


#endif //SLIDING_WINDOW_H
