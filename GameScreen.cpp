#include <SFML/Graphics.hpp>

#include <random>
#include <memory>

#include "GameScreen.h"
#include "GameOverScreen.h"
#include "Game.h"
#include "EnemySnake.h"

using namespace sfSnake;

GameScreen::GameScreen() : snake_(),enemySnake_()
{

}

void GameScreen::handleInput(sf::RenderWindow& window)
{
	// 处理键盘输入
	snake_.handleInput();

	// 处理鼠标点击
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		// 获取鼠标点击位置
		sf::Vector2i mousePos = sf::Mouse::getPosition(window);
		// 转换为世界坐标
		sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
		// 设置目标向量
		snake_.setTargetVector(worldPos);
	}
}

void GameScreen::update(sf::Time delta)
{
	if (fruit_.size() == 0)
		generateFruit();
		// generateFruit();

	snake_.update(delta);
	snake_.checkFruitCollisions(fruit_);
	
	// 更新敌蛇
	enemySnake_.update(delta, fruit_, snake_);
	enemySnake_.checkFruitCollisions(fruit_);

	// 只检查玩家蛇的碰撞
	if (snake_.hitSelf() || snake_.hitOtherSnake(enemySnake_))
		Game::Screen = std::make_shared<GameOverScreen>(snake_.getSize());
}

void GameScreen::render(sf::RenderWindow& window)
{
	snake_.render(window);
	enemySnake_.render(window);  // 渲染敌蛇

	for (auto fruit : fruit_)
		fruit.render(window);
}

void GameScreen::generateFruit()
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<int> xDistribution(2*SnakeNode::Width, Game::Width - 2*SnakeNode::Width);
	static std::uniform_int_distribution<int> yDistribution(2*SnakeNode::Height, Game::Height - 2*SnakeNode::Height);

	fruit_.push_back(Fruit(sf::Vector2f(xDistribution(gen), yDistribution(gen))));
	fruit_.push_back(Fruit(sf::Vector2f(xDistribution(gen), yDistribution(gen))));

}

