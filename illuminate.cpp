#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;


int main( int argc, char** argv ){
/*
Mat im,im2;
im = imread(argv[1]);
cvtColor(im,im2,CV_BGR2GRAY);


              float alpha = 0.1;

              float tau = 10.0;

              float gamma = 0.2;

              int sigma0 = 1;

              int sigma1 = 2;

              im2.convertTo(im2, CV_32FC1);

              Mat I;

              pow(im2, gamma, I);

              Mat gaussian0, gaussian1;

              int kernel_sz0 = (1*sigma0);

              int kernel_sz1 = (3*sigma1);

               kernel_sz0 += ((kernel_sz0 % 2) == 0) ? 1 : 0;

                kernel_sz1 += ((kernel_sz1 % 2) == 0) ? 1 : 0;

            GaussianBlur(I, gaussian0, Size(kernel_sz0,kernel_sz0), sigma0, sigma0, BORDER_CONSTANT);

            GaussianBlur(I, gaussian1, Size(kernel_sz1,kernel_sz1), sigma1, sigma1, BORDER_CONSTANT);

   subtract(gaussian0, gaussian1, I);

   {

           double meanI = 0.0;

           {

               Mat tmp;

               pow(min(abs(I), tau), alpha, tmp);

               meanI = mean(tmp).val[0];

           }

           I = I / pow(meanI, 1.0/alpha);

       }


       // Squash into the tanh:

       {

           for(int r = 0; r < I.rows; r++) {

               for(int c = 0; c < I.cols; c++) {

                   I.at<float>(r,c) = tanh(I.at<float>(r,c) / tau);

               }

           }

           I = tau * I;

       }
normalize(I, I, 0, 255, NORM_MINMAX, CV_8UC1);
            imshow("normalised",I);
		waitKey(1);

            I.convertTo(I,CV_8UC1,1.0,0);
imshow("original",im);
		waitKey(1);

            imshow("nomal",I);
		waitKey(0);
*/



    int size,h,w;
    IplImage* src,ycrcb,yval,cr,cb,dctout;
    CvMat* y_mat,dct_mat;
    CvScalar s;
    float sum,avg_dct;
    int i,j;
    
    size = 40;
    
    src = cvLoadImage("16.png");
    
    h = src->height;
    w = src->width;
    
    // code for DCT
    {
        ycrcb = cvCreateImage (cvGetSize (src), 8, 3);
        yval = cvCreateImage (cvGetSize (ycrcb),8,1);
        cr = cvCreateImage (cvGetSize (ycrcb),8,1);
        cb = cvCreateImage (cvGetSize (ycrcb),8,1);
        dctout = cvCreateImage (cvGetSize (ycrcb),8,3);
          
        cvCvtColor (src, ycrcb, CV_BGR2YCrCb);
        cvSplit (ycrcb, yval, cr, cb, 0);
            
        y_mat = cvCreateMat (size,size,CV_32FC1);
        dct_mat = cvCreateMat (size,size,CV_32FC1);
        
        // 'w' and 'h' are dimensions of image
        // 'size' indiciates the block size in which DCT is applied  
        // apply DCT only on y channel of the ycrcb image
        sum = 0;
        for (i=0; i<w/size; i++)
        {
            for (j=0; j<h/size; j++)
            {
                cvSetImageROI (yval,cvRect(i*size,j*size,size,size));
                cvConvert (yval, y_mat);
                cvDCT (y_mat, dct_mat, CV_DXT_FORWARD);
                s = cvGet2D (dct_mat, 0,0);
                dctarray[i][j] = s.val[0];   // storing the DCT value of every block in an array
                sum = sum + dctarray[i][j];
                cvResetImageROI (yval);
            }
        }        
        avg_dct = sum/(w*h/(size*size));  // average DCT value of the image
        
        for (i=0; i<w/size; i++)
        {
            for (j=0; j<h/size; j++)
            {
                cvSetImageROI (yval,cvRect(i*size,j*size,size,size));
                cvConvert (yval, y_mat);
                cvDCT (y_mat, dct_mat, CV_DXT_FORWARD);
                s = cvGet2D (dct_mat, 0,0);
                if (s.val[0] > dct_param*avg_dct/100) // pulling down acording to the 'dct_param'
                {
                    s.val[0] = avg_dct * dct_param / 100;
                }
                cvSet2D (dct_mat, 0,0, s);
                cvDCT (dct_mat, y_mat, CV_DXT_INVERSE);
                cvConvert (y_mat, yval);    
                cvResetImageROI (yval);
            }
        }
          
        cvMerge (yval,cr,cb,0,ycrcb);
        cvCvtColor (ycrcb, dctout, CV_YCrCb2BGR); // final output image dctout
    } 




return 0;
}
