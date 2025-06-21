#include <SFML/Graphics.hpp>

#include <memory>
#include <iostream>

#include "Snake.h"
#include "Game.h"
#include "Fruit.h"
#include "SnakeNode.h"
#include "GameOverScreen.h"

using namespace sfSnake;

const int Snake::InitialSize = 5;

Snake::Snake() : direction_(Direction::Up), hitSelf_(false), pendingGrowth_(0), hasTargetVector_(false),new_direction_(Direction::Left)
{
	initNodes();
	pickupBuffer_.loadFromFile("Sounds/pickup_1.wav");

	// pickupBuffer_.loadFromFile("Sounds/pickup1.wav");
	pickupSound_.setBuffer(pickupBuffer_);
	pickupSound_.setVolume(30);

	dieBuffer_.loadFromFile("Sounds/die.wav");
	dieSound_.setBuffer(dieBuffer_);
	dieSound_.setVolume(50);

	// 初始化移动计时器
	moveTimer_ = sf::Time::Zero;
	moveInterval_ = sf::seconds(1.f / 15.f);  // 每秒移动15次，是原来的1.5倍
}

void Snake::initNodes()
{
	for (int i = 0; i < Snake::InitialSize; ++i)
	{
		nodes_.push_back(SnakeNode(sf::Vector2f(
			Game::Width / 2 - SnakeNode::Width / 2,
			Game::Height / 2 - (SnakeNode::Height / 2) + (SnakeNode::Height * i))));
	}
}

void Snake::handleInput()
{
	// 如果有键盘输入，清除目标向量
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && direction_!=Direction::Down)
	{
		hasTargetVector_ = false;
		new_direction_ = Direction::Up;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && direction_!=Direction::Up)
	{
		new_direction_ = Direction::Down;
		hasTargetVector_ = false;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && direction_!=Direction::Right)
	{
		new_direction_ = Direction::Left;
		hasTargetVector_ = false;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && direction_!=Direction::Left)
	{
		new_direction_ = Direction::Right;
		hasTargetVector_ = false;
	}
}

void Snake::update(sf::Time delta)
{
	// 处理待增长的部分
	static sf::Clock growClock;
	static sf::Time growDelay = sf::milliseconds(200);
	
	if (pendingGrowth_ > 0 && growClock.getElapsedTime() >= growDelay)
	{
		grow();
		pendingGrowth_--;
		growClock.restart();
	}
    Direction direction;

	// 持续更新方向
	if (hasTargetVector_)
	{
		sf::Vector2f headPos = nodes_[0].getPosition();
        auto direction_vector = targetVector_ - headPos;
		float dx = std::abs(direction_vector.x);
		float dy = std::abs(direction_vector.y);
		if(dx <= SnakeNode::Width && dy <= SnakeNode::Height)
		{
			hasTargetVector_ = false;
		}
		else{
			if (dx > dy)
			{
				if (direction_vector.x > 0 && direction_ != Direction::Left)
					new_direction_ = Direction::Right;
				else if (direction_vector.x < 0 && direction_ != Direction::Right)
					new_direction_ = Direction::Left;
				else{
					if (direction_vector.y > 0 && direction_ != Direction::Up)
						new_direction_ = Direction::Down;
					else if (direction_vector.y < 0 && direction_ != Direction::Down)
						new_direction_ = Direction::Up;
				}
			}
			else
			{
				if (direction_vector.y > 0 && direction_ != Direction::Up)
					new_direction_ = Direction::Down;
				else if (direction_vector.y < 0 && direction_ != Direction::Down)
					new_direction_ = Direction::Up;
				else{
					if (direction_vector.x > 0 && direction_ != Direction::Left)
						new_direction_ = Direction::Right;
					else if (direction_vector.x < 0 && direction_ != Direction::Right)
						new_direction_ = Direction::Left;
				}
			}
		}
		
	}

	// 处理移动
	moveTimer_ += delta;
	if (moveTimer_ >= moveInterval_)
	{
        direction_ = new_direction_;
		move();
		checkEdgeCollisions();
		checkSelfCollisions();
		moveTimer_ = sf::Time::Zero;
	}
}

void Snake::checkFruitCollisions(std::vector<Fruit>& fruits)
{
	decltype(fruits.begin()) toRemove = fruits.end();

	for (auto it = fruits.begin(); it != fruits.end(); ++it)
	{
		if (it->getBounds().intersects(nodes_[0].getBounds()))
		{
			toRemove = it;
			int score = it->getScore();
			
			if (score > 0)
			{
				// 立即增长一个节点
				grow();
				pickupSound_.play();
				
				// 如果分数大于1，将剩余的分数存储起来，在后续的update中逐步增长
				if (score > 1)
				{
					pendingGrowth_ = score - 1;
				}
			}
			break;
		}
	}

	if (toRemove != fruits.end())
	{
		fruits.erase(toRemove);
	}
}

void Snake::grow()
{
	switch (direction_)
	{
	case Direction::Up:
		nodes_.push_back(SnakeNode(sf::Vector2f(nodes_[nodes_.size() - 1].getPosition().x,
			nodes_[nodes_.size() - 1].getPosition().y + SnakeNode::Height)));
		break;
	case Direction::Down:
		nodes_.push_back(SnakeNode(sf::Vector2f(nodes_[nodes_.size() - 1].getPosition().x,
			nodes_[nodes_.size() - 1].getPosition().y - SnakeNode::Height)));
		break;
	case Direction::Left:
		nodes_.push_back(SnakeNode(sf::Vector2f(nodes_[nodes_.size() - 1].getPosition().x + SnakeNode::Width,
			nodes_[nodes_.size() - 1].getPosition().y)));
		break;
	case Direction::Right:
		nodes_.push_back(SnakeNode(sf::Vector2f(nodes_[nodes_.size() - 1].getPosition().x - SnakeNode::Width,
			nodes_[nodes_.size() - 1].getPosition().y)));
		break;
	}
}

unsigned Snake::getSize() const
{
	return nodes_.size();
}

bool Snake::hitSelf() const
{
	return hitSelf_;
}

void Snake::checkSelfCollisions()
{
	SnakeNode& headNode = nodes_[0];

	for (decltype(nodes_.size()) i = 1; i < nodes_.size(); ++i)
	{
		if (headNode.getBounds().intersects(nodes_[i].getBounds()))
		{
			dieSound_.play();
			sf::sleep(sf::seconds(dieBuffer_.getDuration().asSeconds()));
			hitSelf_ = true;
		}
	}
}

void Snake::checkEdgeCollisions()
{
	SnakeNode& headNode = nodes_[0];

	if (headNode.getPosition().x <= 0)
		headNode.setPosition(Game::Width, headNode.getPosition().y);
	else if (headNode.getPosition().x >= Game::Width)
		headNode.setPosition(0, headNode.getPosition().y);
	else if (headNode.getPosition().y <= 0)
		headNode.setPosition(headNode.getPosition().x, Game::Height);
	else if (headNode.getPosition().y >= Game::Height)
		headNode.setPosition(headNode.getPosition().x, 0);
}

void Snake::move()
{
	for (decltype(nodes_.size()) i = nodes_.size() - 1; i > 0; --i)
	{
		nodes_[i].setPosition(nodes_.at(i - 1).getPosition());
	}

	switch (direction_)
	{
	case Direction::Up:
		nodes_[0].move(0, -SnakeNode::Height);
		break;
	case Direction::Down:
		nodes_[0].move(0, SnakeNode::Height);
		break;
	case Direction::Left:
		nodes_[0].move(-SnakeNode::Width, 0);
		break;
	case Direction::Right:
		nodes_[0].move(SnakeNode::Width, 0);
		break;
	}
}

void Snake::render(sf::RenderWindow& window)
{
	for (auto& node : nodes_)
		node.render(window);
}

sf::Vector2f Snake::getHeadPosition() const
{
	return nodes_[0].getPosition();
}

void Snake::setTargetVector(sf::Vector2f targetPoint)
{
	// sf::Vector2f headPos = nodes_[0].getPosition();
	targetVector_ = targetPoint;
	hasTargetVector_ = true;
}

bool Snake::hitOtherSnake(const EnemySnake& other) const
{
    // 检查蛇头是否与敌蛇的任何部分相撞
    sf::FloatRect headBounds = nodes_[0].getBounds();
    
    // 检查与敌蛇头的碰撞
    if (headBounds.intersects(other.getBounds()))
        return true;
        
    // 检查与敌蛇身体的碰撞
    for (size_t i = 1; i < other.getSize(); ++i)
    {
        if (headBounds.intersects(other.getNodeBounds(i)))
            return true;
    }
    
    return false;
}
