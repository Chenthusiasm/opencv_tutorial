// https://docs.opencv.org/4.5.5/d4/d70/tutorial_hough_circle.html

#include "DisplayImage.h"

using namespace cv;
using namespace std;

String detectedCirclesTitle = "detected circles";
String grayscaleTitle = "grayscale";
String blurredTitle = "blurred (median)";
String redChannelTitle = "red channel";

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
    	return 0;
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
        Mat redChannel;
        Mat blurred;
        //vector<Mat> channels(3);
        //split(frame, channels);
        // zero out blue channel
        //channels[0]=Mat::zeros(Size(frame.rows, frame.cols), CV_8UC1);
        // zero out green channel
        //channels[1]=Mat::zeros(Size(frame.rows, frame.cols), CV_8UC1);
        //imshow("blue", channels[0]);
        //imshow("green", channels[1]);
        //imshow("red", channels[2]);
        //merge(channelVector, redChannel);
        //imshow(redChannelTitle, redChannel);

        cvtColor(frame, gray, COLOR_BGR2GRAY);
        imshow(grayscaleTitle, gray);
        medianBlur(gray, blurred, 25);
        imshow(blurredTitle, blurred);

        vector<Vec3f> circles;
        HoughCircles(blurred, circles, HOUGH_GRADIENT, 1,
            blurred.rows/10,
            90, 90/3, 5, 100
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

        imshow(detectedCirclesTitle, frame);
        // quit on ESC button
        if (waitKey(1) == 27) {
            break;
        }
    }
    return EXIT_SUCCESS;
    Mat gray;
}