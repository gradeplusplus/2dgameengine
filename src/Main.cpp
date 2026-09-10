#include "./Game/game.h"
#include <iostream>
int main() {
  Game game;

  game.Init();
  game.Run();
  game.Destory();

  return 0;
}
