#include <iostream>
#include <stdio.h>
//#include <stdlib.h>
#include "include/io_png/io_png.h"
#include "include/colorspace/colorspace.h"
#include "include/opencv2/highgui/highgui.hpp"

/** @brief Min of A, B, and C */
#define MIN3(A,B,C)	(((A) <= (B)) ? MIN(A,C) : MIN(B,C))

/** @brief Max of A, B, and C */
#define MAX3(A,B,C)	(((A) >= (B)) ? MAX(A,C) : MAX(B,C))

using namespace cv;
using namespace std;

unsigned char ScaleAndRound(float X) 
{
    if (X<=0) {
        return 0;
    } else if (X>=1) {
        return 255;
    } else
        return (unsigned char)(255.0f*X+0.5f);
}

void Rgb2Ypbpr(num *Y, num *Pb, num *Pr, num R, num G, num B)
{
	*Y  = (num)( 0.299    *R + 0.587   *G + 0.114   *B);
	*Pb = (num)(-0.1687367*R - 0.331264*G + 0.5     *B);
	*Pr = (num)( 0.5      *R - 0.418688*G - 0.081312*B);
}

void Rgb2Hsl(num *H, num *S, num *L, num R, num G, num B)
{
	num Max = MAX3(R, G, B);
	num Min = MIN3(R, G, B);
	num C = Max - Min;
	
	
	*L = (Max + Min)/2;
	
	if(C > 0)
	{
		if(Max == R)
		{
			*H = (G - B) / C;
			
			if(G < B)
				*H += 6;
		}
		else if(Max == G)
			*H = 2 + (B - R) / C;
		else
			*H = 4 + (R - G) / C;
		
		*H *= 60;
		*S = (*L <= 0.5) ? (C/(2*(*L))) : (C/(2 - 2*(*L)));
	}
	else
		*H = *S = 0;
}

void Ypbpr2Rgb(num *R, num *G, num *B, num Y, num Pb, num Pr)
{
	*R = (num)(0.99999999999914679361*Y - 1.2188941887145875e-06*Pb + 1.4019995886561440468*Pr);
	*G = (num)(0.99999975910502514331*Y - 0.34413567816504303521*Pb - 0.71413649331646789076*Pr);
	*B = (num)(1.00000124040004623180*Y + 1.77200006607230409200*Pb + 2.1453384174593273e-06*Pr);
}

void Hsl2Rgb(num *R, num *G, num *B, num H, num S, num L)
{
	num C = (L <= 0.5) ? (2*L*S) : ((2 - 2*L)*S);
	num Min = L - 0.5*C;
	num X;
	
	
	H -= 360*floor(H/360);
	H /= 60;
	X = C*(1 - fabs(H - 2*floor(H/2) - 1));
	
	switch((int)H)
	{
	case 0:
		*R = Min + C;
		*G = Min + X;
		*B = Min;
		break;
	case 1:
		*R = Min + X;
		*G = Min + C;
		*B = Min;
		break;
	case 2:
		*R = Min;
		*G = Min + C;
		*B = Min + X;
		break;
	case 3:
		*R = Min;
		*G = Min + X;
		*B = Min + C;
		break;
	case 4:
		*R = Min + X;
		*G = Min;
		*B = Min + C;
		break;
	case 5:
		*R = Min + C;
		*G = Min;
		*B = Min + X;
		break;
	default:
		*R = *G = *B = 0;
	}
}

void input2BGR(unsigned char *input,int Dx, int Dy,int channels, float **BB, float **GG, float **RR) 
{
    float *R, *G, *B;
    int temp,pos;
	int size=Dx*Dy;

	B=new float[Dx*Dy];
	G=new float[Dx*Dy];
	R=new float[Dx*Dy];
for(int i = 0;i < Dy ;i++){
        for(int j = 0;j < Dx ;j++){
			temp=Dx*i+j;
			pos = i*Dx*channels + j*channels;
			B[temp]=(float)input[pos]/255.0f;
			G[temp]=(float)input[pos+1]/255.0f;
			R[temp]=(float)input[pos+2]/255.0f;
        }
    }

    *RR= R;
    *GG= G;
    *BB= B;
}

void applyLCC(int radius, int option, int Dx, int Dy, float **RR, float **GG, float **BB, float **RRout, float **GGout, float **BBout)
{
	float *I, *Iout, *Mask, *Mask2, *Pb, *Pr, *H, *S, *Kernel;
	num ii, ppb, ppr, hh, ss, ll, rr, gg, bb;
	float sum, r;
	int n, nx, ny, t;
	float *R, *G, *B;
	float *Rout, *Gout, *Bout;

	R=new float[Dx*Dy];
	G=new float[Dx*Dy];
	B=new float[Dx*Dy];
	R=*RR;
	G=*GG;
	B=*BB;
	Rout=new float[Dx*Dy];
	Gout=new float[Dx*Dy];
	Bout=new float[Dx*Dy];
    /*option=1: process each color channel separately
     option=2: process intensity channel and add original color information
     option=3: process the Luma channel in the YPbPr color space
     option=4: process the Luminance channel in the HSL color space*/
  
	I= (float*) malloc(Dx*Dy*sizeof(float));
    Mask= (float*) malloc(Dx*Dy*sizeof(float));
    Mask2= (float*) malloc(Dx*Dy*sizeof(float));

	Pb=NULL;
    Pr=NULL;
    H=NULL;
    S=NULL;
    Iout=NULL;

    switch (option) {
    case 3:
        Pb= (float*) malloc(Dx*Dy*sizeof(float));
        Pr= (float*) malloc(Dx*Dy*sizeof(float));
        /*I is in this case the Y component*/
        for (n=0; n < Dx*Dy; n++) {
            Rgb2Ypbpr(&ii, &ppb, &ppr, R[n], G[n], B[n]);
            I[n]=ii;
            Pb[n]=ppb;
            Pr[n]=ppr;
        }
        break;
    case 4:
        H= (float*) malloc(Dx*Dy*sizeof(float));
        S= (float*) malloc(Dx*Dy*sizeof(float));
        /*I is in this case the L component*/
        for (n=0; n < Dx*Dy; n++) {
            Rgb2Hsl(&hh, &ss, &ll, R[n], G[n], B[n]);
            H[n]=hh;
            S[n]=ss;
            I[n]=ll;
        }
        break;

    default:
        for (n=0; n < Dx*Dy; n++) {
            I[n]= (R[n]+ G[n]+B[n])/3.0f;
        }
        break;
    }

    for (n=0; n < Dx*Dy; n++) {
        Mask[n] = I[n];
        Mask2[n] =Mask[n];
    }

    /*Create convolution kernel*/
    Kernel =(float*) malloc((2*radius+1)*sizeof(float));

    if (radius > 0) {
        for (n=0; n< 2*radius+1; n++)
            Kernel[n] =
                (float) exp((double)(-2.0f*(-radius+n)*(-radius+n)/(radius*radius)));

        sum=0.0f;
        for (n=0; n< 2*radius+1; n++)
            sum = sum+Kernel[n];
        for (n=0; n< 2*radius+1; n++)
            Kernel[n]=(float) (Kernel[n]/sum);

        /*If the radius of the mask is less than the mid-size of the image,
          we perform the desired algorithm*/
        if (radius<Dx/2&&radius<Dy/2) {
            /*Convolve in the x direction*/
            for (nx=0; nx<Dx; nx++) {
                for (ny=0; ny<Dy; ny++) {
                    sum=0.0f;
                    for (t=-radius; t<=radius; t++) {
                        if (nx+t>=Dx)
                            sum = sum+Mask[2*Dx-2-nx-t+ny*Dx]*Kernel[t+radius];
                        else
                            sum = sum+Mask[abs(nx+t)+ny*Dx]*Kernel[t+radius];
                    }
                    Mask2[nx+ny*Dx]=(float) sum;
                }
            }

            /*Convolve in the y direction*/
            for (ny=0; ny<Dy; ny++)
                for (nx=0; nx<Dx; nx++) {
                    sum=0.0f;
                    for (t=-radius; t<=radius; t++) {
                        if (ny+t>=Dy)
                            sum = sum+Mask2[nx+(2*Dy-2-ny-t)*Dx]*
                                      Kernel[t+radius];
                        else
                            sum = sum+Mask2[nx+abs(ny+t)*Dx]*
                                      Kernel[t+radius];
                    }
                    Mask[nx+ny*Dx]=(float) sum;
                }
        } else {
            /*Otherwise, perform a simple gamma correction*/
            for (ny=0; ny<Dy; ny++) {
                for (nx=0; nx<Dx; nx++) {
                    sum = sum+Mask[nx+ny*Dx];
                }
            }
            sum=sum/(Dx*Dy);
            for (ny=0; ny<Dy; ny++) {
                for (nx=0; nx<Dx; nx++) {
                    Mask[nx+ny*Dx] = sum;
                }
            }
        }
    }

	    if (option!=1)
        Iout=(float*) malloc(Dx*Dy*sizeof(float));

    for (n=0; n < Dx*Dy; n++) {
        Mask[n]=(float) (2*Mask[n]-1);
        Mask[n]=(float) pow(2.0,(double)Mask[n]);

        if (option==1) {
            Rout[n]=(float) pow((double)R[n],(double)Mask[n]);
            Gout[n]=(float) pow((double)G[n],(double)Mask[n]);
            Bout[n]=(float) pow((double)B[n],(double)Mask[n]);
			} else {
            Iout[n]=(float) pow((double)I[n],(double)Mask[n]);
            switch (option) {
            case 2:
                Iout[n]=(float) pow((double)I[n],(double)Mask[n]);
                if (I[n]==0)
                    r=0;
                else
                    r=Iout[n]/I[n];

                rr=r*(float) R[n];
                gg=r*(float) G[n];
                bb=r*(float) B[n];
                if ((rr >= 1) || (gg >= 1) || (bb >= 1)) {
                    if ((R[n] >= G[n]) && (R[n] >= B[n])) {
                        r=1/(float) R[n];
                    }
                    if ((G[n] >= R[n]) && (G[n] >= B[n])) {
                        r=1/(float) G[n];
                    }
                    if ((B[n] >= R[n]) && (B[n] >= G[n])) {
                        r=1/(float) B[n];
                    }
                    rr=r*(float) R[n];
                    gg=r*(float) G[n];
                    bb=r*(float) B[n];
                }
                Rout[n]=rr;
                Gout[n]=gg;
                Bout[n]=bb;
                break;

            case 3:
                Ypbpr2Rgb(&rr, &gg, &bb, Iout[n], Pb[n], Pr[n]);
                Rout[n]=rr;
                Gout[n]=gg;
                Bout[n]=bb;
                break;

            case 4:
                Hsl2Rgb(&rr, &gg, &bb, H[n], S[n], Iout[n]);
                Rout[n]=rr;
                Gout[n]=gg;
                Bout[n]=bb;
                break;
            }
        }
    }

    *RRout= Rout;
    *GGout= Gout;
    *BBout= Bout;
}

void RGB2output(unsigned char *output,int Dx, int Dy,int channels, float **RRout, float **GGout, float **BBout) 
{
    float *Rout, *Gout, *Bout;
    int temp,pos;
	
	Rout=new float[Dx*Dy];
	Gout=new float[Dx*Dy];
	Bout=new float[Dx*Dy];
	Rout=*RRout;
	Gout=*GGout;
	Bout=*BBout;

for(int i = 0;i < Dy ;i++){
        for(int j = 0;j < Dx ;j++){
			temp=Dx*i+j;
			pos = i*Dx*channels + j*channels;
			output[pos]=ScaleAndRound(Bout[temp]);
			output[pos+1]=ScaleAndRound(Gout[temp]);
			output[pos+2]=ScaleAndRound(Rout[temp]);
        }
    }
}

int main( int argc, const char** argv )
{
	int radius, option, channels,pos, Dx,Dy,i, j, n,temp;
	float *R, *G, *B, *Rout, *Gout, *Bout;

// read the image using opencv
 Mat img = imread("MyPic.jpg", CV_LOAD_IMAGE_UNCHANGED);// stores as BGR1BGR2BGR3...
  
     if (img.empty()) //check whether the image is loaded or not
     {    cout << "Error : Image cannot be loaded..!!" << endl;
          return -1;
     }
	 
unsigned char *input = (unsigned char*)(img.data);
 channels=img.channels();//channels=3;
 Dy=img.rows;
 Dx=img.cols;

// take input of radius and option
radius=40;
option=2;
    if (argc > 1) sscanf(argv[1], "%i", &radius);
    if (argc > 2) sscanf(argv[2], "%i", &option);
	if (option!=1&&option!=2&&option!=3&&option!=4){        
		cout<<"Option should be {1,2,3,4}"<<endl;
		return EXIT_FAILURE;
    }

// convert Mat [OpenCV] stored as BGR1BGR2BGR3.. to independent B,G,R channels
input2BGR(input,Dx,Dy,channels,&B,&G,&R);

// apply LCC algo with 'radius' and 'option' parameters
applyLCC(radius, option, Dx, Dy, &R, &G, &B, &Rout, &Gout, &Bout);

// create empty output image
Mat img_out(Dy,Dx,CV_8UC3);
unsigned char *output = (unsigned char*)(img_out.data);

// obtain color corrected image using RGB_out
RGB2output(output,Dx,Dy,channels,&Rout,&Gout,&Bout);

//cout<<img_out<<endl;

// display input image
 namedWindow("MyWindow", CV_WINDOW_AUTOSIZE); //create a window with the name "MyWindow"
 imshow("MyWindow", img); //display the image which is stored in the 'img' in the "MyWindow" window
 waitKey(10);  //wait some time (ms) 
// destroyWindow("MyWindow"); //destroy the window with the name, "MyWindow"

// display output image
 namedWindow("MyWindow_OUT", CV_WINDOW_AUTOSIZE); //create a window with the name "MyWindow"
 imshow("MyWindow_OUT", img_out); //display the image which is stored in the 'img' in the "MyWindow" window
 waitKey(0);  //wait infinite time for a keypress
// destroyWindow("MyWindow_OUT"); //destroy the window with the name, "MyWindow"

 return 0;
}
