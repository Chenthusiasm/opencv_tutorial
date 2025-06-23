// https://docs.opencv.org/4.11.0/db/df5/tutorial_linux_gcc_cmake.html

#include <stdio.h>
#include <opencv2/opencv.hpp>
 
using namespace cv;
 
int main(int argc, char** argv )
{
    if ( argc != 2 )
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }
 
    Mat image;
    image = imread( argv[1], IMREAD_COLOR );
 
    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }
    namedWindow("Display Image", WINDOW_AUTOSIZE );
    imshow("Display Image", image);
 
    waitKey(0);
 
    return 0;
}