#ifndef SNAKE_H
#define SNAKE_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <vector>

#include "SnakeNode.h"
#include "Fruit.h"
#include "EnemySnake.h"

namespace sfSnake
{
	enum class Direction
	{
		Left, Right, Up, Down
	};

class Snake
{
public:
	Snake();
	void handleInput();// set direction same as input
	void update(sf::Time delta);
	void render(sf::RenderWindow& window);

	void checkFruitCollisions(std::vector<Fruit>& fruits);

	bool hitSelf() const;
	bool hitOtherSnake(const EnemySnake& other) const;  // ����������������ߵ���ײ

	unsigned getSize() const;

	void setDirectionToPoint(sf::Vector2f targetPoint);  // ���������ó���Ŀ���ķ���
	void setTargetVector(sf::Vector2f targetPoint);  // ����������Ŀ������
	void updateDirection();  // ����Ŀ���������·���
	sf::Vector2f getHeadPosition() const;  // ��������ȡ��ͷλ��

private:

	void move();
	void grow();
	void checkEdgeCollisions();
	void checkSelfCollisions();
	void initNodes();

	bool hitSelf_;

	sf::Vector2f position_;
	Direction direction_;
	Direction new_direction_;

	sf::SoundBuffer pickupBuffer_;
	sf::Sound pickupSound_;

	sf::SoundBuffer dieBuffer_;
	sf::Sound dieSound_;

	std::vector<SnakeNode> nodes_;

	static const int InitialSize;

	int pendingGrowth_;       // �������ĳ���

	sf::Time moveTimer_;      // �ƶ���ʱ��
	sf::Time moveInterval_;   // �ƶ����ʱ��

	sf::Vector2f targetVector_;  // ������Ŀ������
	bool hasTargetVector_;       // �������Ƿ���Ŀ������
	float lastMoveTime_;         // �ϴ��ƶ�ʱ��
};
}

#endif