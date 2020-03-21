#include "player.hpp"

player::player()
{
    nome = "";
    score = 0;
}
player::player(string nome)
{
    this->nome = nome;
    this->score = 0;
}
string player::getNome()
{
    return nome;
}
void player::incrementaScore()
{
    score++;
}
int player::getScore()
{
    return score;
}
void player::setNome(string nome)
{
    this->nome = nome;
}
void player::setScore(int score)
{
    this->score = score;
}
