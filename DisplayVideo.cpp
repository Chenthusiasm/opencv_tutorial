// reference (in Python)
// https://docs.opencv.org/4.11.0/dd/d43/tutorial_py_video_display.html
// https://learnopencv.com/read-write-and-display-a-video-using-opencv-cpp-python/

#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
 
using namespace cv;
 
int main(int argc, char** argv )
{
    if ( argc != 2 )
    {
        printf("usage: DisplayVideo.out <Video_Path>\n");
        return -1;
    }

    // construct VideoCapture using the arg
    VideoCapture cap(argv[1]);

    if (!cap.isOpened()) {
        printf("Error opening video stream or file %s\n", argv[1]);
        return -1;
    }
    
    while (true) {
        // handle frame-by-frame
        Mat frame;
        cap >> frame;

        // if the frame is empty, break and end
        if (frame.empty()) {
            break;
        }

        imshow("Frame", frame);

        // press [esc] on keyboard to exit
        char c = (char) waitKey(25);
        if (c == 27) {
            break;
        }
    }

    // release the video capture object
    cap.release();

    // close all frames
    destroyAllWindows();
 
    return 0;
}