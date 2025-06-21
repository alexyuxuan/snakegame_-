#ifndef ENEMY_SNAKE_H
#define ENEMY_SNAKE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "SnakeNode.h"
#include "Fruit.h"
#include "Game.h"

namespace sfSnake
{
class Snake;  // ǰ������
enum class Direction;  // ǰ������

class EnemySnake
{
public:
    EnemySnake();
    void update(sf::Time delta, const std::vector<Fruit>& fruits, const Snake& playerSnake);
    void render(sf::RenderWindow& window);
    void checkFruitCollisions(std::vector<Fruit>& fruits);
    bool hitSelf() const { return hitSelf_; }
    bool hitOtherSnake(const Snake& other) const;
    unsigned getSize() const { return nodes_.size(); }
    sf::Vector2f getHeadPosition() const { return nodes_[0].getPosition(); }
    sf::FloatRect getNodeBounds(size_t index) const { return nodes_[index].getBounds(); }
    sf::FloatRect getBounds() const { return nodes_[0].getBounds(); }

private:
    // �ƶ����
    void move();
    void grow();
    void checkEdgeCollisions();
    void checkSelfCollisions();
    void initNodes();

    // AI�������
    void makeDecision(const std::vector<Fruit>& fruits, const Snake& playerSnake);
    bool isPathSafe(sf::Vector2f target) const;
    float calculateDistance(sf::Vector2f pos1, sf::Vector2f pos2) const;
    
    // AI״̬
    enum class AIState
    {
        ChaseFruit,    // ׷��ˮ��
        ChasePlayer,   // ׷�����
        Escape,        // ����
        Wander,        // �������
        Rest          // ��Ϣ״̬
    };
    
    // ��������
    std::vector<SnakeNode> nodes_;
    bool hitSelf_;
    sfSnake::Direction direction_;
    int pendingGrowth_;
    
    // �ƶ�����
    sf::Time moveTimer_;
    sf::Time moveInterval_;
    
    // AI���
    AIState currentState_;
    sf::Time decisionTimer_;
    const sf::Time decisionInterval_ = sf::seconds(0.5f);
    
    // ׷�����
    int chaseCycleCount_;        // ��ǰ׷�����ڼ���
    const int maxChaseCycles_ = 6;  // ���׷��������
    int restCycleCount_;         // ��ǰ��Ϣ���ڼ���
    const int maxRestCycles_ = 10;   // �����Ϣ������
    bool canChasePlayer_ = true;        // �Ƿ����׷�����
    
    // ��Ч
    sf::SoundBuffer pickupBuffer_;
    sf::Sound pickupSound_;
    sf::SoundBuffer dieBuffer_;
    sf::Sound dieSound_;
    
    static const int InitialSize = 5;
};

}

#endif