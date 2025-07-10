// https://docs.opencv.org/4.5.5/d2/d0a/tutorial_introduction_to_tracker.html

#include "DisplayImage.h"

using namespace std;

static string const trackerTitle = "tracker";

enum ProgramState {
    running,
    paused,
    quit
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

int main( int argc, char** argv ){
  	// show help
  	if(argc<2){
    	cout<<
      		" Usage: DisplayImage <video_name>\n"
			" examples:\n"
			" example_tracking_kcf Bolt/img/%04d.jpg\n"
			" example_tracking_kcf faceocc2.webm\n"
			<< endl;
    	return 0;
	}
	// declares all required variables
	cv::Rect roi;
	cv::Mat frame;
	// create a tracker object
	cv::Ptr<cv::Tracker> tracker = cv::TrackerKCF::create();
	// set input video
	string video = argv[1];
	cv::VideoCapture cap(video);
	// get bounding box
	cap >> frame;
	roi = cv::selectROI(trackerTitle,frame);
	//quit if ROI was not selected
	if(roi.width==0 || roi.height==0)
		return 0;
	// initialize the tracker
	tracker->init(frame,roi);
	// perform the tracking process
	cout << "Start the tracking process, press ESC to quit.\n";
    ProgramState programState = running;
	for ( ;; ){
        programState = processKey(programState);
        if (programState == quit) {
            break;
        }
        if (programState == paused) {
            // show the image
            cv::imshow(trackerTitle, frame);
            continue;
        }
		// get frame from the video
		cap >> frame;
		// stop the program if no more images
		if(frame.rows==0 || frame.cols==0)
		break;
		// update the tracking result
		tracker->update(frame,roi);
		// draw the tracked object
		cv::rectangle( frame, roi, cv::Scalar( 255, 0, 0 ), 2, 1 );
        cv::imshow(trackerTitle, frame);
	}
	return 0;
}