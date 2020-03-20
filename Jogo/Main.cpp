#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/opencv.hpp>
#include "player.hpp"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <ctime>

using namespace std;
using namespace cv;

double scale = 3.0;
int vx1 = 0;
int vx2 = 0;
int vy1 = 0;
int vy2 = 0;
int dist = 160 / scale; //TAMANHO DO QUADRADO
double Twidth = 638.0 / scale; //VALOR TOTAL DO COMPRIMENTO DA TELA
double Theight = 479.0 / scale; //VALOR TOTAL DA ALTURA DA TELA
double Dwidth = Twidth / 6.0;

void detectAndDraw(Mat& img, player& cascade, double scale, bool& foi);

string cascadeName;
string nestedCascadeName;
Mat fruta;

/**
 * @brief Draws a transparent image over a frame Mat.
 *
 * @param frame the frame where the transparent image will be drawn
 * @param transp the Mat image with transparency, read from a PNG image, with the IMREAD_UNCHANGED flag
 * @param xPos x position of the frame image where the image will start.
 * @param yPos y position of the frame image where the image will start.
 */
void drawTransparency(Mat frame, Mat transp, int xPos, int yPos) {
    Mat mask;
    vector<Mat> layers;

    split(transp, layers); // seperate channels
    Mat rgb[3] = { layers[0],layers[1],layers[2] };
    mask = layers[3]; // png's alpha channel used as mask
    merge(rgb, 3, transp);  // put together the RGB channels, now transp insn't transparent 
    transp.copyTo(frame.rowRange(yPos, yPos + transp.rows).colRange(xPos, xPos + transp.cols), mask);
}

void drawTransparency2(Mat frame, Mat transp, int xPos, int yPos) {
    Mat mask;
    vector<Mat> layers;

    split(transp, layers); // seperate channels
    Mat rgb[3] = { layers[0],layers[1],layers[2] };
    mask = layers[3]; // png's alpha channel used as mask
    merge(rgb, 3, transp);  // put together the RGB channels, now transp insn't transparent 
    Mat roi1 = frame(Rect(xPos, yPos, transp.cols, transp.rows));
    Mat roi2 = roi1.clone();
    transp.copyTo(roi2.rowRange(0, transp.rows).colRange(0, transp.cols), mask);
    printf("%p, %p\n", roi1.data, roi2.data);
    double alpha = 0.9;
    addWeighted(roi2, alpha, roi1, 1.0 - alpha, 0.0, roi1);
}


int main(int argc, const char** argv)
{
    VideoCapture capture;
    Mat frame, image;
    string inputName;
    player cascade = player("Alguém");
    clock_t relogio_init, relogio_end;
    
    srand(time(NULL));
    bool flag = true;
   
    string folder = "/home/andre/Downloads/opencv-4.1.2/data/haarcascades/";
    cascadeName = folder + "haarcascade_frontalface_alt.xml";

    if (!cascade.load(samples::findFile(cascadeName)))
    {
        cerr << "ERROR: Could not load classifier cascade" << endl;
        return -1;
    }

    if (!capture.open(0))
    {
        cout << "Capture from camera #" << 0 << " didn't work" << endl;
        return 1;
    }

    if (capture.isOpened())
    {
        cout << "Video capturing has been started ..." << endl;

        relogio_init = clock();
        for (;;)
        {
            capture >> frame;
            if (frame.empty())
                break;
            detectAndDraw(frame, cascade, scale, flag);
            relogio_end = clock();
            if ((double)(relogio_end - relogio_init)/1000000.0 >= 120)
            {
                cv::putText(frame, //target image
                "Fim de Jogo! Pressione 'q' para sair...", //text
                cv::Point(270, 25), //top-left position
                cv::FONT_HERSHEY_DUPLEX,
                0.5,
                CV_RGB(255, 0, 0), //font color
                2);
                imshow("result", frame);

                cout << "Score: " << cascade.getScore() << endl;
                char k = (char)waitKey(0);
                if(k == 27 || k == 'q' || k == 'Q')
                    break;
            }
            
            char c = (char)waitKey(10);
            if (c == 27 || c == 'q' || c == 'Q')
                break;
        }
    }

    return 0;
}

void detectAndDraw(Mat& img, player& cascade, double scale, bool& foi)
{
    static int frames = 0;
    double t = 0;
    vector<Rect> faces, faces2;
    const static Scalar colors[] =
    {
        Scalar(255,0,0),
        Scalar(255,128,0),
        Scalar(255,255,0),
        Scalar(0,255,0),
        Scalar(0,128,255),
        Scalar(0,255,255),
        Scalar(0,0,255),
        Scalar(255,0,255)
    };
    Mat gray, smallImg;
    string score;

    cvtColor(img, gray, COLOR_BGR2GRAY);
    double fx = 1 / scale;
    resize(gray, smallImg, Size(), fx, fx, INTER_LINEAR_EXACT);
    equalizeHist(smallImg, smallImg);

    t = (double)getTickCount();
    cascade.detectMultiScale(smallImg, faces,
        1.2, 2, 0
        //|CASCADE_FIND_BIGGEST_OBJECT
        //|CASCADE_DO_ROUGH_SEARCH
        | CASCADE_SCALE_IMAGE,
        Size(30, 30));

    
    t = (double)getTickCount() - t;

    Scalar cor = colors[6];

    if (foi == true) {

        vx1 = rand() % (((int)Twidth - (int)Dwidth - dist) - (int)Dwidth) + (int)Dwidth;
        vx2 = vx1 + dist;
        vy1 = rand() % ((int)(Theight - dist)) + 0;
        vy2 = vy1 + dist;

    }
    rectangle(img, Point((vx1 * scale), vy1 * scale),
        Point((vx2 - 1) * scale, (vy2 - 1) * scale),
        cor, 3, 8, 0);
    
    foi = false;

    for (size_t i = 0; i < faces.size(); i++)
    {
        Rect r = faces[i];
        Mat smallImgROI;
        Point center;
        Scalar color = colors[i % 8];
        int radius;


        if (r.x >= vx1 && (r.x + r.width - 1) <= vx2) { //DENTRO DO QUADRADO DESENHADO
            cout << "Conseguiu!" <<endl;

            foi = true;
            cascade.incrementaScore();
        }
        else {

            foi = false;
        }
        
        
    }
    score = "Score: "+to_string(cascade.getScore());
    cv::putText(img, //target image
        score, //text
        cv::Point(25, 25), //top-left position
        cv::FONT_HERSHEY_DUPLEX,
        1.0,
        CV_RGB(255, 0, 0), //font color
        2);

    imshow("result", img);

}