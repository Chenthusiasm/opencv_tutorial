// https://docs.opencv.org/4.5.5/d4/d70/tutorial_hough_circle.html

#include "DisplayImage.h"

using namespace cv;
using namespace std;
int main(int argc, char** argv)
{
    const char* filename = argc >=2 ? argv[1] : "smarties.png";
    // Loads an image
    Mat src = imread( samples::findFile( filename ), IMREAD_COLOR );
    // Check if image is loaded fine
    if(src.empty()){
        printf(" Error opening image\n");
        printf(" Program Arguments: [image_name -- default %s] \n", filename);
        return EXIT_FAILURE;
    }
    
    int rows = src.rows;
    int cols = src.cols;
    printf("%s: rows=%d, cols=%d\n", filename, rows, cols);
    // Size srcSize = src.size();
    // rows = srcSize.height;
    // cols = srcSize.width;
    // printf("%s: height=%d, width=%d\n", filename, rows, cols);
    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);
    imshow("grayscale", gray);
    Mat blurred;
    medianBlur(gray, blurred, 5);
    imshow("median blurred", blurred);
    vector<Vec3f> circles;
    HoughCircles(blurred, circles, HOUGH_GRADIENT, 1,
                 blurred.rows/16,  // change this value to detect circles with different distances to each other
                 100, 30, 1, 30 // change the last two parameters
            // (min_radius & max_radius) to detect larger circles
    );
    for( size_t i = 0; i < circles.size(); i++ )
    {
        Vec3i c = circles[i];
        Point center = Point(c[0], c[1]);
        // circle center
        circle( src, center, 1, Scalar(0,255,0), 3, LINE_AA);
        // circle outline
        int radius = c[2];
        circle( src, center, radius, Scalar(0,0,255), 3, LINE_AA);
		// print circle data to terminal
		printf("[%lu]: (%d, %d) r=%d\n", i, c[0], c[1], c[2]);
    }
    
    imshow("detected circles", src);
    waitKey();
    return EXIT_SUCCESS;
}