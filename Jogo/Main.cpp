#include <iostream>
#include <Windows.h> //Função de tocar os sons no Windows
#include <stdlib.h>
#include <time.h>
#include <ctime> //Clock
#include "player.hpp" //Classe Player

#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2\opencv.hpp>

#pragma comment(lib, "Winmm.lib") //Linkar Winmm.lib para tocar os soms

using namespace std;
using namespace cv;

double scale = 3.0;
int vx1 = 0;
int vx2 = 0;
int vy1 = 0;
int vy2 = 0;
int dist = 160 / scale; //Tamanho dos quadrados
double Twidth = 638.0 / scale; //Valor total do comprimento da tela
double Theight = 479.0 / scale; //Valor total da altura da tela
double Dwidth = Twidth / 6.0;
//double Dheight = Theight / 6.0;
string cascadeName;

void detectAndDraw(Mat& img, player& cascade,
	double scale, bool& foi);



int main() {

    VideoCapture capture;
    Mat frame, image;
    string inputName;
    //CascadeClassifier cascade, nestedCascade;

    player cascade = player("Alguém"); //Objeto de Player que herda de CascadeClassifier
    clock_t relogio_init, relogio_end;

    srand(time(NULL));
    bool flag = true;


    string folder = "C:\\opencv\\build\\install\\etc\\haarcascades\\";
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
        PlaySound(TEXT("Efeitos\\sons_arcade.wav"), NULL, SND_FILENAME | SND_ASYNC); //Som para o início do jogo

        for (;;)
        {
            capture >> frame;
            if (frame.empty())
                break;

            detectAndDraw(frame, cascade, scale, flag);
            relogio_end = clock();
      
            if ((double)(relogio_end - relogio_init) / 1000.0 >= 20) //Windows - dividir por 1000.0, Linux - dividir por 1000000.0
            {
                cv::putText(frame, //target image
                    "Fim de Jogo! Pressione 'q' para sair...", //text
                    cv::Point(270, 25), //top-left position
                    cv::FONT_HERSHEY_DUPLEX,
                    0.5,
                    CV_RGB(255, 0, 0), //font color
                    2);
                imshow("result", frame);

                PlaySound(TEXT("Efeitos\\sons_endgame.wav"), NULL, SND_FILENAME | SND_ASYNC); //Som após fim de jogo

                cout << "Score: " << cascade.getScore() << endl;

                char k = (char)waitKey(0);
                if (k == 27 || k == 'q' || k == 'Q')
                    break;
            }

            char c = (char)waitKey(10);
            if (c == 27 || c == 'q' || c == 'Q')
                break;
        }
    }



	PlaySound(TEXT("Efeitos\\sons_gameover1.wav"), NULL, SND_FILENAME | SND_SYNC); //Som ao fechar o programa

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

    //frames++;
    //if (frames % 30 == 0)
        //system("mplayer /usr/lib/libreoffice/share/gallery/sounds/kling.wav &");

    t = (double)getTickCount() - t;
    //    printf( "detection time = %g ms\n", t*1000/getTickFrequency());

    Scalar cor = colors[1];

    if (foi == true) { //Achar uma posição aleatória para os quadrados

        vx1 = rand() % (((int)Twidth - (int)Dwidth - dist) - (int)Dwidth) + (int)Dwidth;
        vx2 = vx1 + dist;
        vy1 = rand() % ((int)(Theight - dist)) + 0;
        vy2 = vy1 + dist;

    }
    rectangle(img, Point((vx1 * scale), vy1 * scale), //Desenhar os quadrados
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


        if (r.x >= vx1 && (r.x + r.width - 1) <= vx2) { //Detectar dentro do quadrado desenhado
            

            //printf("[%3d, %3d]  -  [%3d, %3d]\n", r.x, r.y, r.x + r.width - 1, r.y + r.height - 1);
            cout << "Conseguiu!" << endl;

            PlaySound(TEXT("Efeitos\\sons_bubble.wav"), NULL, SND_FILENAME | SND_ASYNC); //Efeito sonoro ao acertar o quadrado
  
            cascade.incrementaScore();

            foi = true;
        }
        else {

            foi = false;
        }


        
    }

    score = "Score: " + to_string(cascade.getScore());

    cv::putText(img, //target image
        score, //text
        cv::Point(25, 25), //top-left position
        cv::FONT_HERSHEY_DUPLEX,
        1.0,
        CV_RGB(255, 0, 0), //font color
        2);

    imshow("result", img);

}

