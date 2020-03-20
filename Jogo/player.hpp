#ifndef PLAYER_HPP
#define PLAYER_HPP

#include<iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/objdetect.hpp"

using namespace std;
using namespace cv;

class player : public CascadeClassifier
{
private:
    int score;
    string nome;
public:
    player(string nome);
    string getNome();
    void incrementaScore();
    int getScore();
};
#endif