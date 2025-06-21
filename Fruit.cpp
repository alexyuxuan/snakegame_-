#include <SFML/Graphics.hpp>
#include <random>

#include "Fruit.h"

using namespace sfSnake;

const float Fruit::Radius = 5.f;

Fruit::Fruit(sf::Vector2f position)
{
	// 随机生成水果类型
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> dis(0, 4);  // 0-4对应5种类型
	
	type_ = static_cast<Type>(dis(gen));
	
	shape_.setPosition(position);
	shape_.setRadius(Fruit::Radius);
	
	// 根据类型设置颜色
	switch(type_)
	{
		case Type::Black:
			shape_.setFillColor(sf::Color(139, 69, 19));
			break;
		case Type::Brown:
			shape_.setFillColor(sf::Color(139, 69, 19));  // 棕色
			break;
		case Type::Red:
			shape_.setFillColor(sf::Color::Red);
			break;
		case Type::Blue:
			shape_.setFillColor(sf::Color::Blue);
			break;
		case Type::Green:
			shape_.setFillColor(sf::Color::Green);
			break;
	}
}

void Fruit::render(sf::RenderWindow& window)
{
	window.draw(shape_);
}

sf::FloatRect Fruit::getBounds() const
{
	return shape_.getGlobalBounds();
}

int Fruit::getScore() const
{
	switch(type_)
	{
		case Type::Black:
		case Type::Brown:
			return 0;
		case Type::Red:
			return 1;
		case Type::Blue:
			return 2;
		case Type::Green:
			return 3;
		default:
			return 0;
	}
}

sf::Vector2f Fruit::getposition() const
{
	return shape_.getPosition();
}
