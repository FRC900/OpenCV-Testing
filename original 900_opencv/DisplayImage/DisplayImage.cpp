#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

int main(int argc, char** argv )
{
    if ( argc != 2 )
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }

    //VideoCapture stream(0);
    Mat image;
		Mat image2;
    //image = stream.read(image);
    image = imread( argv[1], 1 );

    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }
    namedWindow("Display Image", WINDOW_AUTOSIZE );
    //pyrDown(image,image2);
		Canny( image, image2, 10, 100, 3, true);
		imshow("Display Image", image2);

    waitKey(0);

    return 0;
}
