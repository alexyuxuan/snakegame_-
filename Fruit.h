#ifndef FRUIT_H
#define FRUIT_H

#include <SFML/Graphics.hpp>

enum class Type {Black, Brown, Red, Blue, Green};

namespace sfSnake
{
class Fruit
{
public:
	Fruit(sf::Vector2f position = sf::Vector2f(0, 0));

	void render(sf::RenderWindow& window);

	sf::FloatRect getBounds() const;

	// 新增：获取水果类型和分数的方法
	Type getType() const { return type_; }
	int getScore() const;
	sf::Vector2f getposition() const;

private:
	sf::CircleShape shape_;
	Type type_;  // 新增：存储水果类型
	static const float Radius;
};
}

#endif