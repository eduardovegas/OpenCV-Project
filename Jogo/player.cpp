#include "player.hpp"

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