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
	bool hitOtherSnake(const EnemySnake& other) const;  // 新增：检查与其他蛇的碰撞

	unsigned getSize() const;

	void setDirectionToPoint(sf::Vector2f targetPoint);  // 新增：设置朝向目标点的方向
	void setTargetVector(sf::Vector2f targetPoint);  // 新增：设置目标向量
	void updateDirection();  // 根据目标向量更新方向
	sf::Vector2f getHeadPosition() const;  // 新增：获取蛇头位置

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

	int pendingGrowth_;       // 待增长的长度

	sf::Time moveTimer_;      // 移动计时器
	sf::Time moveInterval_;   // 移动间隔时间

	sf::Vector2f targetVector_;  // 新增：目标向量
	bool hasTargetVector_;       // 新增：是否有目标向量
	float lastMoveTime_;         // 上次移动时间
};
}

#endif