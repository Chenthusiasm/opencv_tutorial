// https://docs.opencv.org/4.5.5/d4/d70/tutorial_hough_circle.html

#include "Definitions.h"
#include "DisplayImage.h"

using namespace std;

static string const detectedCirclesTitle = "detected circles";
static string const grayscaleTitle = "grayscale";
static string const blurredTitle = "blurred (median)";
static string const redChannelTitle = "red channel";
static string const subImageTitle = "sub image";

// note that the K size must be an odd number
static int const BlurKSize = 11;
static int const SubImageBorder = 0;
static double const ActivePercentThreshold = 0.25;

enum ProgramState {
    running,
    paused,
    quit
};

enum DetectState {
    search,
    track
};

static ProgramState processKey(ProgramState currentState) {
    ProgramState newState = currentState;
    int key = cv::waitKey(1);
    switch (key) {
        case ' ':
            if (currentState == running) {
                return paused;
            } else if (currentState == paused) {
                return running;
            }
            break;
        case 27:
            return quit;
        default:
            ; // do nothing
    }
    return newState;
}

static void findRedCircles(cv::Mat const& src, vector<cv::Vec3f>& circles) {
    cv::Mat gray;
    //Mat redChannel;
    cv::Mat blurred;
    #if 1
    vector<cv::Mat> channels(NumberOfColors);
    split(src, channels);
    //cv::imshow("red", channels[red]);
    cv::medianBlur(channels[red], blurred, BlurKSize);
    #else
    cv::cvtColor(frame, gray, COLOR_BGR2GRAY);
    //cv::imshow(grayscaleTitle, gray);
    cv::medianBlur(gray, blurred, blurKSize);
    #endif
    //cv::imshow(blurredTitle, blurred);

    cv::HoughCircles(blurred, circles, cv::HOUGH_GRADIENT, 1,
        blurred.rows/10,
        90, 90/3, 10, 300
    );
}

static int findBestMatch(cv::Mat const& src, vector<cv::Vec3f> const& circles) {
    int circleIndex = -1;
    if (circles.size() <= 0) {
        return circleIndex;
    }
    double maxActivePercent = 0.0;
    for(size_t i = 0; i < circles.size(); ++i) {
        cv::Vec3i c = circles[i];
        
        // verify that each circle falls within the color thresholds
        int width = (c[2] + SubImageBorder) * 2;
        int height = (c[2] + SubImageBorder) * 2;
        int x = max(c[0] - (c[2] + SubImageBorder), 0);
        int y = max(c[1] - (c[2] + SubImageBorder), 0);
        if ((x + width) >= src.cols) {
            width = max(src.cols - x - 1, 0);
        }
        if ((y + height) >= src.rows) {
            height = max(src.rows - y - 1, 0);
        }
        // create a cropped sub-image
        cv::Mat subSrc = src(cv::Rect(x, y, width, height));
        // invert the image
        cv::Mat inverseSrc = ~subSrc;
        // convert image to HSV color
        cv::Mat inverseHSV;
        cv::cvtColor(inverseSrc, inverseHSV, cv::COLOR_BGR2HSV);
        cv::Mat mask;
        inRange(inverseHSV, cv::Scalar(90 - 10, 64, 32), cv::Scalar(90 + 10, 255, 255), mask);
        int count = countNonZero(mask);
        size_t pixels = inverseHSV.total();
        double activePercent = (double)count / (double)pixels;
        if ((activePercent > ActivePercentThreshold) && (activePercent > maxActivePercent)) {
            maxActivePercent = activePercent;
            circleIndex = i;
        }
    }
    return circleIndex;
}

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

    ProgramState state = running;
    
    for ( ; ; ) {
        state = processKey(state);
        if (state == quit) {
            break;
        }
        if (state == paused) {
            // show the image
            cv::imshow(detectedCirclesTitle, frame);
            continue;
        }

        cap >> frame;

        // stop the program if there are no more images
        if ((frame.rows == 0) || (frame.cols == 0)) {
            break;
        }

        vector<cv::Vec3f> circles;
        findRedCircles(frame, circles);

        int circleIndex = findBestMatch(frame, circles);

        if (circleIndex >= 0) {
            cv::Vec3i c = circles[circleIndex];
            // circle center
            cv::Point center = cv::Point(c[0], c[1]);
            cv::circle(frame, center, 1, cv::Scalar(0,255,0), 3, cv::LINE_AA);
            // circle outline
            int radius = c[2];
            cv::circle(frame, center, radius, cv::Scalar(255,255,0), 3, cv::LINE_AA);
        }

        // show the image
        cv::imshow(detectedCirclesTitle, frame);
    }
    return EXIT_SUCCESS;
}