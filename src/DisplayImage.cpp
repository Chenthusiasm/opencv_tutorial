// https://docs.opencv.org/4.5.5/d4/d70/tutorial_hough_circle.html

#include "DisplayImage.h"

using namespace cv;
using namespace std;

static String const detectedCirclesTitle = "detected circles";
static String const grayscaleTitle = "grayscale";
static String const blurredTitle = "blurred (median)";
static String const redChannelTitle = "red channel";
static String const subImageTitle = "sub image";

// note that the K size must be an odd number
static int const blurKSize = 11;
static int const subImageBorder = 0;

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
    
    for (size_t i = 0; ; ++i) {
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
        //imshow("red", channels[2]);
        medianBlur(channels[2], blurred, blurKSize);
        #else
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        //imshow(grayscaleTitle, gray);
        medianBlur(gray, blurred, blurKSize);
        #endif
        //imshow(blurredTitle, blurred);

        vector<Vec3f> circles;
        HoughCircles(blurred, circles, HOUGH_GRADIENT, 1,
            blurred.rows/10,
            90, 90/3, 10, 300
        );

        for(size_t j = 0; j < circles.size(); ++j) {
            Vec3i c = circles[j];
            
            // verify that each circle falls within the color thresholds
            int width = (c[2] + subImageBorder) * 2;
            int height = (c[2] + subImageBorder) * 2;
            int x = max(c[0] - (c[2] + subImageBorder), 0);
            int y = max(c[1] - (c[2] + subImageBorder), 0);
            if ((x + width) >= frame.cols) {
                width = max(frame.cols - x - 1, 0);
            }
            if ((y + height) >= frame.rows) {
                height = max(frame.rows - y - 1, 0);
            }
            if ((width <= 0) || (height <= 0)) {
                printf("ERROR: (x, y) = (%d, %d) width = %d, height = %d\n", x, y, width, height);
            }
            // create a cropped sub-image
            Mat subImage = frame(Rect(x, y, width, height));
            // invert the image
            Mat inverseImage = ~subImage;
            // convert image to HSV color
            Mat inverseHSV;
            cvtColor(inverseImage, inverseHSV, COLOR_BGR2HSV);
            Mat mask;
            inRange(inverseHSV, Scalar(90 - 10, 64, 32), Scalar(90 + 10, 255, 255), mask);
            int count = countNonZero(mask);
            size_t pixels = inverseHSV.total();
            double activePercent = (double)count / (double)pixels;
            //printf("[%lu-%lu]: %d / %ld = %0.3lf%%\n", i, j, count, pixels, activePercent);
            if (activePercent < 0.25) {
                continue;
            }

            imshow(subImageTitle, subImage);
            // if ((activePercent > 0.10) && (activePercent < 0.50)) {
            //     waitKey(0);
            // }

            // circle center
            Point center = Point(c[0], c[1]);
            circle(frame, center, 1, Scalar(0,255,0), 3, LINE_AA);
            // circle outline
            int radius = c[2];
            circle(frame, center, radius, Scalar(255,255,0), 3, LINE_AA);
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