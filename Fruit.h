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

	// ��������ȡˮ�����ͺͷ����ķ���
	Type getType() const { return type_; }
	int getScore() const;
	sf::Vector2f getposition() const;

private:
	sf::CircleShape shape_;
	Type type_;  // �������洢ˮ������
	static const float Radius;
};
}

#endif