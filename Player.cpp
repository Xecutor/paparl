#include "Player.hpp"
#include "GameLoop.hpp"

Player::Player()
{
  sym = "@";
  fg = Color::white / 1.5f;
}

double Player::makeTurn(GameLoop* game)
{
  game->playerTurn();
  return -1.0;
}
