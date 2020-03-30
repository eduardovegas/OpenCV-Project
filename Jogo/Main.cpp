#include <iostream>
#include <fstream>
#include <Windows.h>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <ctime> //Clock
#include "player.hpp" //Classe Player

#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2\opencv.hpp>

#if defined(_WIN32) || defined(_WIN64)
    string folder = "C:\\opencv\\build\\install\\etc\\haarcascades\\";
    double divisao = 1000.0;
#else defined(__linux__) || defined(__unix__)
    #include <opencv2/opencv.hpp>
    string folder = "/home/andre/Downloads/opencv-4.1.2/data/haarcascades/";
    double divisao = 1000000.0;
#endif

#pragma comment(lib, "Winmm.lib") //Linkar Winmm.lib para tocar os soms

using namespace std;
using namespace cv;

double scale = 3.0;
int vx1 = 0;
int vx2 = 0;
int vy1 = 0;
int vy2 = 0;
int dist = 170 / scale; //Tamanho dos quadrados
double Twidth = 638.0 / scale; //Valor total do comprimento da tela dependendo da escala
double Theight = 479.0 / scale; //Valor total da altura da tela dependendo da escala
double Dwidth = Twidth / 6.0;
double Dheight = Theight / 6.0;
string cascadeName;

void limpa_tela()
{
    #if defined(_WIN32) || defined(_WIN64)
        system("cls");
    #else defined(__linux__) || defined(__unix__)
        system("clear");
    #endif
}

void menu_inicial(Mat frame, double scale);
void pegar_sigla(Mat frame, char sigla[3], string nome[3]);
void exibir_fotos(vector<player>& dados);
void exibir_placar(Mat frame, vector<player>& dados);
void exibir_creditos(Mat frame);
void lerArquivo(vector<player>& dados);
void adicionarPlacar(vector<player>& dados, player& jogador);
void salvarArquivo(vector<player>& dados);
void detectAndDraw(Mat& img, player& cascade, double scale, bool& foi);


int main() {

    VideoCapture capture;
    Mat frame, image;
    string salvar;
    string inputName;
    string nome[3];
    char sigla[3];
    char init;

    std::vector<player> dados;
    player cascade = player("jaozin"); //Objeto de Player que herda de CascadeClassifier
    clock_t relogio_init, relogio_end;

    srand(time(NULL));
    bool flag = true;
    bool menu = true;

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

        limpa_tela();

        lerArquivo(dados);

        PlaySound(TEXT("Efeitos\\sons_arcade.wav"), NULL, SND_FILENAME | SND_ASYNC); //Som ao iniciar o programa ASSINCRONO

        while (menu) {

            capture >> frame;
            if (frame.empty())
                break;

            menu_inicial(frame, scale);

            char opcao = (char)waitKey(10);

            switch (opcao)
            {
            case 'j':

                PlaySound(TEXT("Efeitos\\sons_endgame.wav"), NULL, SND_FILENAME | SND_ASYNC);

                capture >> frame;
                if (frame.empty())
                    break;

                pegar_sigla(frame, sigla, nome);

                cascade.setNome(nome[0] + nome[1] + nome[2]);


                init = (char)waitKey(0);

                PlaySound(TEXT("Efeitos\\sons_arcade.wav"), NULL, SND_FILENAME | SND_ASYNC); //Som ao iniciar a partida ASSINCRONO

                relogio_init = clock();

                while (true)
                {
                    capture >> frame;
                    if (frame.empty())
                        break;

                    detectAndDraw(frame, cascade, scale, flag);

                    relogio_end = clock();

                    //cout << (double)(relogio_end - relogio_init) / divisao << endl;

                    if ((double)(relogio_end - relogio_init) / divisao >= 120) //Windows - dividir por 1000.0, Linux - dividir por 1000000.0
                    {
                        cv::putText(frame, 
                            "Fim de Jogo!", 
                            cv::Point(222, 405), 
                            cv::FONT_HERSHEY_DUPLEX,
                            0.9,
                            CV_RGB(255, 255, 0), 
                            2);
                        imshow("Put Your Face!", frame);

                        salvar = "Results/"+cascade.getNome()+".jpg";
                        imwrite(salvar, frame); //Salvar a foto de fim de jogo

                        cv::putText(frame, 
                            "Pressione 'q' para sair e salvar o placar...", 
                            cv::Point(36, 447), 
                            cv::FONT_HERSHEY_DUPLEX,
                            0.8,
                            CV_RGB(255, 128, 0),
                            2);
                        imshow("Put Your Face!", frame);

                        PlaySound(TEXT("Efeitos\\sons_gameover1.wav"), NULL, SND_FILENAME | SND_ASYNC); // Som ao terminar a partida ASSINCRONO

                        cout << "Jogador: " << cascade.getNome() << endl;
                        cout << "Score: " << cascade.getScore() << endl;

                        char k = (char)waitKey(0);
                        if (k == 27 || k == 'q' || k == 'Q') {
                            adicionarPlacar(dados, cascade);
                            cascade.setScore(0);
                            break;
                        }
                    }

                    char c = (char)waitKey(5);

                }

                break;

            case 'r':

                PlaySound(TEXT("Efeitos\\sons_endgame.wav"), NULL, SND_FILENAME | SND_ASYNC);

                capture >> frame;
                if (frame.empty())
                    break;

                exibir_placar(frame, dados);

                exibir_fotos(dados);

                while (true)
                {
                    capture >> frame;
                    if (frame.empty())
                        break;

                    exibir_placar(frame, dados);

                    char n = (char)waitKey(5);
                    if (n == 27 || n == 'q' || n == 'Q')
                        break;

                }


                break;

            case 'c':

                PlaySound(TEXT("Efeitos\\sons_endgame.wav"), NULL, SND_FILENAME | SND_ASYNC);

                while (true)
                {
                    capture >> frame;
                    if (frame.empty())
                        break;

                    exibir_creditos(frame);

                    char n = (char)waitKey(5);
                    if (n == 27 || n == 'q' || n == 'Q')
                        break;

                }
                break;

            case 's':

                PlaySound(TEXT("Efeitos\\sons_endgame.wav"), NULL, SND_FILENAME | SND_SYNC);
                salvarArquivo(dados);
                menu = false;
                break;

            default:

                break;
            }
        }
    }



    return 0;
}

void menu_inicial(Mat frame, double scale)
{
    Mat gray, smallImg;

    cvtColor(frame, gray, COLOR_BGR2GRAY);
    double fx = 1 / scale;
    resize(gray, smallImg, Size(), fx, fx, INTER_LINEAR_EXACT);
    equalizeHist(smallImg, smallImg);

    cv::putText(frame, //target image
        "Put Your Face!", //text
        cv::Point(80, 75), //top-left position
        cv::FONT_HERSHEY_DUPLEX,
        2.0,
        CV_RGB(255, 0, 0), //font color
        2);


    string menuzim[4];
    menuzim[0] = "j - Jogar";
    menuzim[1] = "r - Recordes";
    menuzim[2] = "c - Criadores";
    menuzim[3] = "s - Sair";
    for (int i = 0; i < 4; i++)
    {
        cv::putText(frame, //target image
            menuzim[i], //text
            cv::Point(230, 160 + 65 * i), //top-left position
            cv::FONT_HERSHEY_DUPLEX,
            1.0,
            CV_RGB(255, 128, 0), //font color
            2);
    }
    imshow("Put Your Face!", frame);
    /*Scalar(255,0,0),
        Scalar(255,128,0),--
        Scalar(255,255,0),-
        Scalar(0,255,0), 0
        Scalar(0,128,255), 0
        Scalar(0,255,255), 0
        Scalar(0,0,255), 0
        Scalar(255,0,255)*/
}

void pegar_sigla(Mat frame, char sigla[3], string nome[3])
{

    cv::putText(frame, 
        "Put Your Face!", 
        cv::Point(80, 75), 
        cv::FONT_HERSHEY_DUPLEX,
        2.0,
        CV_RGB(255, 0, 0), 
        2);
    cv::putText(frame, 
        "Digite sua sigla:", 
        cv::Point(145, 170), 
        cv::FONT_HERSHEY_DUPLEX,
        1.3,
        CV_RGB(255, 128, 0), 
        2);
    imshow("Put Your Face!", frame);

    sigla[0] = (char)waitKey(0);
    nome[0] = sigla[0];
    cv::putText(frame, 
        nome[0], 
        cv::Point(260, 300), 
        cv::FONT_HERSHEY_DUPLEX,
        1.8,
        CV_RGB(255, 255, 0), 
        2);
    imshow("Put Your Face!", frame);
    sigla[1] = (char)waitKey(0);
    nome[1] = sigla[1];
    cv::putText(frame, 
        nome[1], 
        cv::Point(300, 300), 
        cv::FONT_HERSHEY_DUPLEX,
        1.8,
        CV_RGB(255, 255, 0), 
        2);
    imshow("Put Your Face!", frame);
    sigla[2] = (char)waitKey(0);
    nome[2] = sigla[2];
    cv::putText(frame, 
        nome[2], 
        cv::Point(340, 300), 
        cv::FONT_HERSHEY_DUPLEX,
        1.8,
        CV_RGB(255, 255, 0), 
        2);
    imshow("Put Your Face!", frame);
    
    cv::putText(frame, 
        "Pressione 'j' para iniciar...", 
        cv::Point(100, 450), 
        cv::FONT_HERSHEY_DUPLEX,
        1.0,
        CV_RGB(255, 128, 0), 
        2);
    imshow("Put Your Face!", frame);
}

void exibir_fotos(vector<player>& dados) {

    if (dados.size() < 3)
        return;

    string photo;
    int wait_time = 1000;
    const static Scalar colors[] =
    {
        Scalar(0,255,0), //VERDE EM SCALAR
        Scalar(255, 128, 0), //AZUL EM SCALAR
        Scalar(255,255,0), //AZUL CLARO EM SCALAR
    };


    for (int i = 0; i < 3; i++) {

        string nome = dados[i].getNome();

        photo = "Results/" + nome + ".jpg";

        Mat foto = imread(photo, -1); //FLAG UNCHANGED

        try {

            imshow(nome, foto); //Tentar abrir imagem para ver se existe. Se não existe, lança exception 
            cv::destroyWindow(nome); //Se existe, irá abrir, então é necessário destruir ela para abrir abaixo editada


            cv::namedWindow(nome, WINDOW_FREERATIO);

            cv::putText(foto,
                nome,
                cv::Point(550, 40),
                cv::FONT_HERSHEY_DUPLEX,
                1.2,
                colors[i],
                2);

            imshow(nome, foto);

            resizeWindow(nome, foto.cols / 1.2, foto.rows / 1.2);

            while (cv::getWindowProperty(nome, WND_PROP_VISIBLE) >= 1) { //Verificar se a janela foi fechada ou nao

                char n = (char)waitKey(wait_time);

                if (n == 27 || n == 'q' || n == 'Q') {

                    cv::destroyWindow(nome);

                    break;
                }

            }

            //cout << "leu" << endl;

        }
        catch (const cv::Exception& exec) {

            cout << "Arquivo '" + nome + ".jpg' nao existe na pasta." << endl;

        }



    }


    return;
}

void exibir_placar(Mat frame, vector<player>& dados) {

    if (dados.size() < 3)
        return;

    string score;

    cv::putText(frame,
        "Put Your Face!",
        cv::Point(80, 75),
        cv::FONT_HERSHEY_DUPLEX,
        2.0,
        CV_RGB(255, 0, 0),
        2);
    cv::putText(frame,
        "*TOP 3*",
        cv::Point(242, 164),
        cv::FONT_HERSHEY_DUPLEX,
        1.2,
        CV_RGB(255, 255, 0),
        2);
    
    score = to_string(dados[0].getScore());
    cv::putText(frame,
        "1 Lugar: " + dados[0].getNome() + " / " + score + " pts",
        cv::Point(125, 240),
        cv::FONT_HERSHEY_DUPLEX,
        1.0,
        CV_RGB(0, 255, 0),
        2);

    score = to_string(dados[1].getScore());
    cv::putText(frame,
        "2 Lugar: " + dados[1].getNome() + " / " + score + " pts",
        cv::Point(125, 290),
        cv::FONT_HERSHEY_DUPLEX,
        1.0,
        CV_RGB(0, 128, 255),
        2);

    score = to_string(dados[2].getScore());
    cv::putText(frame,
        "3 Lugar: " + dados[2].getNome() + " / " + score + " pts",
        cv::Point(125, 340),
        cv::FONT_HERSHEY_DUPLEX,
        1.0,
        CV_RGB(0, 255, 255),
        2);

    cv::putText(frame, 
        "Pressione 'q' para voltar ao menu...", 
        cv::Point(80, 447), 
        cv::FONT_HERSHEY_DUPLEX,
        0.8,
        CV_RGB(255, 128, 0),
        2);
    
    imshow("Put Your Face!", frame);

    /*Scalar(255,0,0), VERMELHO EM RGB
        Scalar(255,128,0), LARANJA EM RGB
        Scalar(255,255,0), AMARELO EM RGB
        Scalar(0,255,0), VERDE EM RGB
        Scalar(0,128,255), AZUL EM RGB
        Scalar(0,255,255), AZUL CLARO EM RGB
        Scalar(0,0,255), 0
        Scalar(255,0,255)*/


    return;
}

void exibir_creditos(Mat frame)
{
    cv::putText(frame, 
        "Put Your Face!", 
        cv::Point(80, 75), 
        cv::FONT_HERSHEY_DUPLEX,
        2.0,
        CV_RGB(255, 0, 0), 
        2);
    cv::putText(frame, 
        "Criado por:", 
        cv::Point(200, 164), 
        cv::FONT_HERSHEY_DUPLEX,
        1.3,
        CV_RGB(255, 0, 0), 
        2);
    cv::putText(frame, 
        "-> Andre Hugo (P2)", 
        cv::Point(125, 290), 
        cv::FONT_HERSHEY_DUPLEX,
        1.0,
        CV_RGB(255, 128, 0), 
        2);
    cv::putText(frame,
        "-> Eduardo Luiz (P2)",
        cv::Point(125, 340),
        cv::FONT_HERSHEY_DUPLEX,
        1.0,
        CV_RGB(0, 255, 255),
        2);

    cv::putText(frame, 
        "Pressione 'q' para voltar ao menu...", 
        cv::Point(80, 447), 
        cv::FONT_HERSHEY_DUPLEX,
        0.8,
        CV_RGB(255, 128, 0),
        2);

    imshow("Put Your Face!", frame);
}

void lerArquivo(vector<player>& dados) {

    std::ifstream file1; //Abrir arquivo para leitura
    std::string name;
    int i = 0;
    int in = 0;
    int placar = 0;

    file1.open("Results/Rank.txt");
    if (!file1.is_open()) {
        std::cout << "Nao foi possivel abrir o arquivo para leitura" << std::endl;
        return;
    }

    while (1) { //Ler a lista completa dos jogadores do sistema

        file1 >> in;
        if (file1.eof() || file1.bad() || file1.fail())
            break;

        file1.ignore();

        std::getline(file1, name);
        dados.push_back(player(name));

        file1 >> placar;
        dados[i].setScore(placar);

        file1.ignore();
        file1.ignore(256, '\n'); //Ignorar a linha de separacao no arquivo


        i++;

    }

    file1.close();

    return;


}

void adicionarPlacar(vector<player>& dados, player& jogador) {

    int placar = 0;
    int maior_j = -1;


    for (int j = 0; j < dados.size(); j++) {

        if (jogador.getScore() > dados[j].getScore()) {

            maior_j = j;
            break;

        }

    }

    if(maior_j == -1)
        dados.push_back(jogador); //Adicionar o jogador da vez
    else
        dados.insert(dados.begin() + maior_j, jogador); //Adicionar o jogador da vez

    return;

}

void salvarArquivo(vector<player>& dados) {

    std::ofstream file2; //Abrir arquivo para escrita

    file2.open("Results/Rank.txt");
    if (!file2.is_open()) {
        std::cout << "Nao foi possivel abrir o arquivo para escrita" << std::endl;
        return;
    }

    for (int i = 0; i < dados.size(); i++) {

        file2 << i + 1 << std::endl;
        file2 << dados[i].getNome() << std::endl;
        file2 << dados[i].getScore() << std::endl;
        file2 << "--------------------" << std::endl;

    }

    file2.close();

    return;

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

    if (foi == true) { //Achar uma posição aleatória para os quadrados dentro de uma certa área

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


        if ((r.x >= vx1 && (r.x + r.width - 1) <= vx2) && (r.y >= vy1 && (r.y + r.height - 1) <= vy2)) { //Detectar dentro do quadrado desenhado


            cout << "Conseguiu!" << endl;

            PlaySound(TEXT("Efeitos\\sons_bubble.wav"), NULL, SND_FILENAME | SND_SYNC);

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
        cv::Point(18, 35), //top-left position
        cv::FONT_HERSHEY_DUPLEX,
        1.0,
        CV_RGB(255, 0, 0), //font color
        2);

    imshow("Put Your Face!", img);

}
