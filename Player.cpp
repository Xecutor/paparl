#include "Player.hpp"
#include "GameLoop.hpp"

Player::Player()
{
  name="Player";
  sym = "@";
  fg = Color::white / 1.5f;
  lightSource=true;
  lightStrength=300;
  lightTint=Color::white;
}

double Player::makeTurn(GameLoop* game)
{
  game->playerTurn();
  return -1.0;
}
