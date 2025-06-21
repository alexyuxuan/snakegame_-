#include "Game.h"
#include <iostream>
#include <filesystem>


using namespace sfSnake;

int main()
{
	std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;
	Game game;
	game.run();

	return 0;
}