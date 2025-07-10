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
static int const ROIFrame = 6;
static double const ActivePercentThreshold = 0.25;
static int DetectIntervalFrames = 10;

enum ProgramState {
    running,
    paused,
    quit
};

enum VisionState {
    detecting,
    tracking
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
    cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    //cv::imshow(grayscaleTitle, gray);
    cv::medianBlur(gray, blurred, BlurKSize);
    #endif
    //cv::imshow(blurredTitle, blurred);

    cv::HoughCircles(blurred, circles, cv::HOUGH_GRADIENT, 1,
        blurred.rows/10,
        90, 90/3, 10, 300
    );
}

static bool makeBoundedRectangle(cv::Mat const& src, int x, int y, int r, int border, cv::Rect& rectangle) {
    int srcWidth = src.cols;
    int srcHeight = src.rows;
    if (x < 0) {
        return false;
    }
    if (y < 0) {
        return false;
    }
    if (x >= srcWidth) {
        return false;
    }
    if (y >= srcHeight) {
        return false;
    }
    if (r <= 0) {
        return false;
    }
    if (border < 0) {
        return false;
    }
    int rectWidth = (r + border) * 2;
    int rectHeight = (r + border) * 2;
    int rectX = max(x - (r + border), 0);
    int rectY = max(y - (r + border), 0);
    if ((rectX + rectWidth) >= srcWidth) {
        rectWidth = max(srcWidth - rectX - 1, 0);
    }
    if ((rectY + rectHeight) >= srcHeight) {
        rectHeight = max(srcHeight - rectY - 1, 0);
    }
    rectangle = cv::Rect(rectX, rectY, rectWidth, rectHeight);
    return true;
}

static int __attribute__((unused)) findBestMatch(cv::Mat const& src, vector<cv::Vec3f> const& circles) {
    int circleIndex = -1;
    if (circles.size() <= 0) {
        return circleIndex;
    }
    double maxActivePercent = 0.0;
    for(size_t i = 0; i < circles.size(); ++i) {
        cv::Vec3i c = circles[i];
        
        // verify that each circle falls within the color thresholds
        cv::Rect rectangle;
        bool result = makeBoundedRectangle(src, c[0], c[1], c[2], SubImageBorder, rectangle);
        if (result == false) {
            continue;
        }
        // create a cropped sub-image
        cv::Mat subSrc = src(rectangle);
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

static void drawTrackingBox(const cv::Rect& roi, cv::Mat& frame) {
    cv::rectangle(frame, roi, cv::Scalar(255, 255, 0), 2, 1);
}

int main(int argc, char** argv) {
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

    

	// set input video
	std::string video = argv[1];
	cv::VideoCapture cap(video);

    // declare variables
	cv::Mat frame;
    cv::Rect roi;
    cv::Ptr<cv::Tracker> trackerPtr;
    ProgramState programState = running;
    VisionState visionState = detecting;
    int trackingFrame = 0;
    
    for ( ; ; ) {
        programState = processKey(programState);
        if (programState == quit) {
            break;
        }
        if (programState == paused) {
            // show the image
            cv::imshow(detectedCirclesTitle, frame);
            continue;
        }

        cap >> frame;

        // stop the program if there are no more images
        if ((frame.rows == 0) || (frame.cols == 0)) {
            break;
        }

        #if 1
        switch (visionState) {
            case detecting: {
                vector<cv::Vec3f> circles;
                findRedCircles(frame, circles);

                int circleIndex = findBestMatch(frame, circles);

                if (circleIndex >= 0) {
                    cv::Vec3i c = circles[circleIndex];
                    #if 0
                    // circle center
                    cv::Point center = cv::Point(c[0], c[1]);
                    cv::circle(frame, center, 1, cv::Scalar(0,255,0), 3, cv::LINE_AA);
                    // circle outline
                    int radius = c[2];
                    cv::circle(frame, center, radius, cv::Scalar(255,255,0), 3, cv::LINE_AA);
                    #endif

                    bool result = makeBoundedRectangle(frame, c[0], c[1], c[2], ROIFrame, roi);
                    if (result == false) {
                        break;
                    }
                    trackerPtr = cv::TrackerKCF::create();
                    trackerPtr->init(frame, roi);
                    drawTrackingBox(roi, frame);
                    ++trackingFrame;

                    // update ROI for tracking
                    visionState = tracking;
                }
            }
            break;
            case tracking: {
                bool result = trackerPtr->update(frame, roi);
                if (result == false) {
                    visionState = detecting;
                    trackingFrame = 0;
                    break;
                }
                drawTrackingBox(roi, frame);
                ++trackingFrame;
                if (trackingFrame >= DetectIntervalFrames) {
                    visionState = detecting;
                    trackingFrame = 0;
                }
            }
            break;
        }
        #else
        vector<cv::Vec3f> circles;
        findRedCircles(frame, circles);

        #if 1
        int circleIndex = findBestMatch(frame, circles);

        if (circleIndex >= 0) {
        #else
        for (size_t circleIndex = 0; circleIndex < circles.size(); ++circleIndex) {
        #endif
            cv::Vec3i c = circles[circleIndex];
            // circle center
            cv::Point center = cv::Point(c[0], c[1]);
            cv::circle(frame, center, 1, cv::Scalar(0,255,0), 3, cv::LINE_AA);
            // circle outline
            int radius = c[2];
            cv::circle(frame, center, radius, cv::Scalar(255,255,0), 3, cv::LINE_AA);
        }
        #endif

        // show the image
        cv::imshow(detectedCirclesTitle, frame);
    }
    return EXIT_SUCCESS;
}