// https://docs.opencv.org/4.5.5/d4/d70/tutorial_hough_circle.html

#include "DisplayImage.h"

using namespace std;

static string const detectedCirclesTitle = "detected circles";
static string const grayscaleTitle = "grayscale";
static string const blurredTitle = "blurred (median)";
static string const redChannelTitle = "red channel";
static string const subImageTitle = "sub image";

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
	cv::Mat frame;

	// set input video
	std::string video = argv[1];
	cv::VideoCapture cap(video);
    
    for (size_t i = 0; ; ++i) {
        cap >> frame;

        // stop the program if there are no more images
        if ((frame.rows == 0) || (frame.cols == 0)) {
            break;
        }

        cv::Mat gray;
        //Mat redChannel;
        cv::Mat blurred;
        #if 1
        vector<cv::Mat> channels(3);
        split(frame, channels);
        //cv::imshow("red", channels[2]);
        cv::medianBlur(channels[2], blurred, blurKSize);
        #else
        cv::cvtColor(frame, gray, COLOR_BGR2GRAY);
        //cv::imshow(grayscaleTitle, gray);
        cv::medianBlur(gray, blurred, blurKSize);
        #endif
        //cv::imshow(blurredTitle, blurred);

        vector<cv::Vec3f> circles;
        cv::HoughCircles(blurred, circles, cv::HOUGH_GRADIENT, 1,
            blurred.rows/10,
            90, 90/3, 10, 300
        );

        for(size_t j = 0; j < circles.size(); ++j) {
            cv::Vec3i c = circles[j];
            
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
            cv::Mat subImage = frame(cv::Rect(x, y, width, height));
            // invert the image
            cv::Mat inverseImage = ~subImage;
            // convert image to HSV color
            cv::Mat inverseHSV;
            cv::cvtColor(inverseImage, inverseHSV, cv::COLOR_BGR2HSV);
            cv::Mat mask;
            inRange(inverseHSV, cv::Scalar(90 - 10, 64, 32), cv::Scalar(90 + 10, 255, 255), mask);
            int count = countNonZero(mask);
            size_t pixels = inverseHSV.total();
            double activePercent = (double)count / (double)pixels;
            //printf("[%lu-%lu]: %d / %ld = %0.3lf%%\n", i, j, count, pixels, activePercent);
            if (activePercent < 0.25) {
                continue;
            }

            // imshow(subImageTitle, subImage);
            // if ((activePercent > 0.10) && (activePercent < 0.50)) {
            //     cv::waitKey(0);
            // }

            // circle center
            cv::Point center = cv::Point(c[0], c[1]);
            cv::circle(frame, center, 1, cv::Scalar(0,255,0), 3, cv::LINE_AA);
            // circle outline
            int radius = c[2];
            cv::circle(frame, center, radius, cv::Scalar(255,255,0), 3, cv::LINE_AA);
            // print circle data to terminal
            //printf("[%lu-%lu]: (%d, %d) r=%d\n", i, j, c[0], c[1], c[2]);
        }

        // show the image
        cv::imshow(detectedCirclesTitle, frame);
        // quit on ESC button
        if (cv::waitKey(1) == 27) {
            break;
        }
    }
    return EXIT_SUCCESS;
}