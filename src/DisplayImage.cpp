// https://docs.opencv.org/4.5.5/d4/d70/tutorial_hough_circle.html

#include "DisplayImage.h"

using namespace cv;
using namespace std;

static String detectedCirclesTitle = "detected circles";
static String grayscaleTitle = "grayscale";
static String blurredTitle = "blurred (median)";
static String redChannelTitle = "red channel";
static int blurKSize = 11;

int main(int argc, char** argv)
{
  	// show help
  	if(argc<2){
    	cout<<
      		" Usage: DisplayImage <video_name>\n"
			" examples:\n"
			" DisplayImage Bolt/img/%04d.jpg\n"
			" DisplayImage faceocc2.webm\n"
			<< endl;
    	return EXIT_FAILURE;
	}

    // declare variables
	Mat frame;

	// set input video
	std::string video = argv[1];
	VideoCapture cap(video);
    
    for ( size_t i; ; ++i ) {
        cap >> frame;

        // stop the program if there are no more images
        if ((frame.rows == 0) || (frame.cols == 0)) {
            break;
        }

        Mat gray;
        //Mat redChannel;
        Mat blurred;
        #if 1
        vector<Mat> channels(3);
        split(frame, channels);
        imshow("red", channels[2]);
        medianBlur(channels[2], blurred, blurKSize);
        #else
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        imshow(grayscaleTitle, gray);
        medianBlur(gray, blurred, blurKSize);
        #endif
        imshow(blurredTitle, blurred);

        vector<Vec3f> circles;
        HoughCircles(blurred, circles, HOUGH_GRADIENT, 1,
            blurred.rows/10,
            90, 90/3, 10, 50
        );

        for(size_t j = 0; j < circles.size(); ++j) {
            Vec3i c = circles[j];
            Point center = Point(c[0], c[1]);
            // circle center
            circle(frame, center, 1, Scalar(0,255,0), 3, LINE_AA);
            // circle outline
            int radius = c[2];
            circle(frame, center, radius, Scalar(0,0,255), 3, LINE_AA);
            // print circle data to terminal
            //printf("[%lu-%lu]: (%d, %d) r=%d\n", i, j, c[0], c[1], c[2]);
        }

        // show the image
        imshow(detectedCirclesTitle, frame);
        // quit on ESC button
        if (waitKey(1) == 27) {
            break;
        }
    }
    return EXIT_SUCCESS;
    Mat gray;
}