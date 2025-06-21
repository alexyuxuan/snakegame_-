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
	// �����������
	snake_.handleInput();

	// ���������
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		// ��ȡ�����λ��
		sf::Vector2i mousePos = sf::Mouse::getPosition(window);
		// ת��Ϊ��������
		sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
		// ����Ŀ������
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
	
	// ���µ���
	enemySnake_.update(delta, fruit_, snake_);
	enemySnake_.checkFruitCollisions(fruit_);

	// ֻ�������ߵ���ײ
	if (snake_.hitSelf() || snake_.hitOtherSnake(enemySnake_))
		Game::Screen = std::make_shared<GameOverScreen>(snake_.getSize());
}

void GameScreen::render(sf::RenderWindow& window)
{
	snake_.render(window);
	enemySnake_.render(window);  // ��Ⱦ����

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

