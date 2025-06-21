#ifndef ENEMY_SNAKE_H
#define ENEMY_SNAKE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "SnakeNode.h"
#include "Fruit.h"
#include "Game.h"

namespace sfSnake
{
class Snake;  // 前向声明
enum class Direction;  // 前向声明

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
    // 移动相关
    void move();
    void grow();
    void checkEdgeCollisions();
    void checkSelfCollisions();
    void initNodes();

    // AI决策相关
    void makeDecision(const std::vector<Fruit>& fruits, const Snake& playerSnake);
    bool isPathSafe(sf::Vector2f target) const;
    float calculateDistance(sf::Vector2f pos1, sf::Vector2f pos2) const;
    
    // AI状态
    enum class AIState
    {
        ChaseFruit,    // 追逐水果
        ChasePlayer,   // 追逐玩家
        Escape,        // 逃跑
        Wander,        // 随机游走
        Rest          // 休息状态
    };
    
    // 基本属性
    std::vector<SnakeNode> nodes_;
    bool hitSelf_;
    sfSnake::Direction direction_;
    int pendingGrowth_;
    
    // 移动控制
    sf::Time moveTimer_;
    sf::Time moveInterval_;
    
    // AI相关
    AIState currentState_;
    sf::Time decisionTimer_;
    const sf::Time decisionInterval_ = sf::seconds(0.5f);
    
    // 追逐控制
    int chaseCycleCount_;        // 当前追逐周期计数
    const int maxChaseCycles_ = 6;  // 最大追逐周期数
    int restCycleCount_;         // 当前休息周期计数
    const int maxRestCycles_ = 10;   // 最大休息周期数
    bool canChasePlayer_ = true;        // 是否可以追逐玩家
    
    // 音效
    sf::SoundBuffer pickupBuffer_;
    sf::Sound pickupSound_;
    sf::SoundBuffer dieBuffer_;
    sf::Sound dieSound_;
    
    static const int InitialSize = 5;
};

}

#endif