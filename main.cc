
#include <iostream>
#include <unistd.h>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#define PROCESS_IMAGE
#define WINDOW_NAME "CVUI Hello World!"

#define CVUI_IMPLEMENTATION
#include "cvui.h"

#include "arfilter.h"

double randd()
{
    return (double)rand() / (double)RAND_MAX;
}

std::vector< Point2d > generatePointsInShape( const std::vector< cv::Point2d >& shape, double density )
{
    std::vector< Point2d > ret;

    // Compute bounding box.
    cv::Rect bbox = boundingRect( shape );

    // Compute number of points
    int npts = bbox.area() * density;

    // Generate npts points in the AABB and test for inclusion in shape
    for( int k = 0; k < npts; ++k )
    {
        double randx = randd() * bbox.width + bbox.x;
        double randy = randd() * bbox.height + bbox.y;
        cv::Point2d randp( randx, randy );
        if( pointPolygonTest( shape, randp, false ) >= 0.0 )
        {
            ret.push_back( randp );
        }
    }

    return ret;
}

std::vector< Point2d > generateShapeFromPoints( const std::vector< cv::Point2d >& pts )
{
    std::vector< cv::Point2d > ret;
    convexHull( pts, ret, true );
    return ret;
}

std::vector< std::vector< Point2d > > divideShape( const std::vector< cv::Point2d >& shape )
{
    std::vector< std::vector< Point2d > > ret;

    // Generate points in shape
    std::vector< cv::Point2d > shapePoints = generatePointsInShape( shape, 0.01 );

    // Cluster points
    const int clusterCount = 10;
    Mat labels;
    std::vector<Point2d> centers;
    double compactness = kmeans(shapePoints, clusterCount, labels,
                TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 10, 1.0),
                   3, KMEANS_PP_CENTERS, centers);

    // Generate new shapes from clusters.
    std::vector< std::vector< cv::Point2d > > clusters( clusterCount );
    for( int k = 0; k < shapePoints.size(); ++k )
    {
        int idx = labels.at<int>( k );
        clusters[ idx ].push_back( shapePoints[k] );
    }

    for( std::vector<cv::Point2d>& c : clusters )
    {
        ret.push_back( c );
    }

    // Profit.
    return ret;
}

int main( int argc, char** argv )
{

    srand(time(NULL));

#ifndef PROCESS_IMAGE
    namedWindow( "frame" );
    cv::VideoCapture cap( argv[1] );
    bool isPlaying = true;

    cv::Mat frame;

    for(;;)
    {
        if( isPlaying )
        {
            cap >> frame;
            if( frame.cols <= 0 || frame.rows <= 0 )
                continue;

        }

        imshow("frame", frame);

        char c = cv::waitKey(5);
        if( c == ' ' )
        {
            isPlaying = !isPlaying;
        }
        else if( c == 'q' )
        {
            break;
        }

    }
#else

    cv::Mat input = cv::imread( argv[1] );

    // Create a frame where components will be rendered to.
    cv::Mat frame = cv::Mat(200, 500, CV_8UC3);

    // Init cvui and tell it to create a OpenCV window, i.e. cv::namedWindow(WINDOW_NAME).
    cvui::init(WINDOW_NAME);

    bool needUpdate = false;

    while(true)
    {

        // Fill the frame with a nice color
        frame = cv::Scalar(49, 52, 49);

        /*
        // Render UI components to the frame
        if( cvui::trackbar(frame, 5, 10, 240, &stepx, (int)1., (int)100.) )
        {
            needUpdate = true;
        }

        if( cvui::trackbar(frame, 5, 60, 240, &pointSpeed, 0., 100.) )
        {
            needUpdate = true;
        }

        if( cvui::trackbar(frame, 5, 110, 240, &propSpeed, 0., 1.) )
        {
            needUpdate = true;
        }

        if( cvui::trackbar(frame, 5, 160, 240, &alphaFilter, 0., 1.) )
        {
            needUpdate = true;
        }

        if (cvui::button(frame, 300, 80, "&Quit")) {
            break;
        }
        */

        if( needUpdate )
        {

        }

        // Update cvui stuff and show everything on the screen
        cvui::imshow(WINDOW_NAME, frame);

        cv::waitKey(20);

    }

#endif

    return 0;

}

