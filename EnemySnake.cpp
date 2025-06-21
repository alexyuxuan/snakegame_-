#include "EnemySnake.h"
#include "Snake.h"
#include <cmath>
#include <limits>
#include <random>

namespace sfSnake
{

EnemySnake::EnemySnake() 
    : hitSelf_(false), direction_(Direction::Right), pendingGrowth_(0),
      currentState_(AIState::Wander), decisionTimer_(sf::Time::Zero),
      chaseCycleCount_(0), restCycleCount_(0), canChasePlayer_(true)
{
    initNodes();
    
    // 加载音效
    pickupBuffer_.loadFromFile("Sounds/pickup_1.wav");
    pickupSound_.setBuffer(pickupBuffer_);
    pickupSound_.setVolume(30);

    dieBuffer_.loadFromFile("Sounds/die.wav");
    dieSound_.setBuffer(dieBuffer_);
    dieSound_.setVolume(50);

    // 设置移动间隔
    moveTimer_ = sf::Time::Zero;
    moveInterval_ = sf::seconds(1.f / 17.f);
}

void EnemySnake::initNodes()
{
    // 创建随机数生成器
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> xDistribution(0, Game::Width - SnakeNode::Width);
    static std::uniform_int_distribution<int> yDistribution(0, Game::Height - SnakeNode::Height);

    // 生成随机起始位置
    float startX = xDistribution(gen);
    float startY = yDistribution(gen);

    // 确保不会生成在屏幕正中间（玩家蛇的位置）
    while (std::abs(startX - Game::Width/2) < 100 && std::abs(startY - Game::Height/2) < 100)
    {
        startX = xDistribution(gen);
        startY = yDistribution(gen);
    }

    // 创建蛇的节点
    for (int i = 0; i < InitialSize; ++i)
    {
        nodes_.push_back(SnakeNode(sf::Vector2f(
            startX,
            startY + (SnakeNode::Height * i)), true));
    }
}

void EnemySnake::update(sf::Time delta, const std::vector<Fruit>& fruits, const Snake& playerSnake)
{
    // 更新决策计时器
    decisionTimer_ += delta;
    
    // 定期做决策
    if (decisionTimer_ >= decisionInterval_)
    {
        // 更新追逐状态
        if (currentState_ == AIState::ChasePlayer)
        {
            chaseCycleCount_++;
            if (chaseCycleCount_ >= maxChaseCycles_)
            {
                currentState_ = AIState::Rest;
                restCycleCount_ = 0;
                canChasePlayer_ = false;
            }
        }
        else if (currentState_ == AIState::ChaseFruit)
        {
            restCycleCount_++;
            if (restCycleCount_ >= maxRestCycles_)
            {
                currentState_ = AIState::ChasePlayer;
                restCycleCount_ = 0;
            }
        }
        else if (currentState_ == AIState::Rest)
        {
            restCycleCount_++;
            if (restCycleCount_ >= maxRestCycles_)
            {
                chaseCycleCount_ = 0;
                canChasePlayer_ = true;
                currentState_ = AIState::Wander;
            }
        }
        
        makeDecision(fruits, playerSnake);
        decisionTimer_ = sf::Time::Zero;
    }
    
    // 处理待增长的部分
    static sf::Clock growClock;
    static sf::Time growDelay = sf::milliseconds(200);
    
    if (pendingGrowth_ > 0 && growClock.getElapsedTime() >= growDelay)
    {
        grow();
        pendingGrowth_--;
        growClock.restart();
    }

    // 处理移动
    moveTimer_ += delta;
    if (moveTimer_ >= moveInterval_)
    {
        move();
        checkEdgeCollisions();
        checkSelfCollisions();
        moveTimer_ = sf::Time::Zero;
    }
}

void EnemySnake::move()
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

void EnemySnake::grow()
{
    switch (direction_)
    {
    case Direction::Up:
        nodes_.push_back(SnakeNode(sf::Vector2f(nodes_[nodes_.size() - 1].getPosition().x,
            nodes_[nodes_.size() - 1].getPosition().y + SnakeNode::Height),1));
        break;
    case Direction::Down:
        nodes_.push_back(SnakeNode(sf::Vector2f(nodes_[nodes_.size() - 1].getPosition().x,
            nodes_[nodes_.size() - 1].getPosition().y - SnakeNode::Height),1));
        break;
    case Direction::Left:
        nodes_.push_back(SnakeNode(sf::Vector2f(nodes_[nodes_.size() - 1].getPosition().x + SnakeNode::Width,
            nodes_[nodes_.size() - 1].getPosition().y),1));
        break;
    case Direction::Right:
        nodes_.push_back(SnakeNode(sf::Vector2f(nodes_[nodes_.size() - 1].getPosition().x - SnakeNode::Width,
            nodes_[nodes_.size() - 1].getPosition().y),1));
        break;
    }
}

void EnemySnake::checkEdgeCollisions()
{
    SnakeNode& headNode = nodes_[0];

    // 保持边界穿越功能
    if (headNode.getPosition().x <= 0)
        headNode.setPosition(Game::Width, headNode.getPosition().y);
    else if (headNode.getPosition().x >= Game::Width)
        headNode.setPosition(0, headNode.getPosition().y);
    else if (headNode.getPosition().y <= 0)
        headNode.setPosition(headNode.getPosition().x, Game::Height);
    else if (headNode.getPosition().y >= Game::Height)
        headNode.setPosition(headNode.getPosition().x, 0);
}

void EnemySnake::checkSelfCollisions()
{
    // 移除敌蛇的自身碰撞检测，使其不会因为碰到自己而死亡
    hitSelf_ = false;
}

void EnemySnake::checkFruitCollisions(std::vector<Fruit>& fruits)
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
                grow();
                pickupSound_.play();
                
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

void EnemySnake::render(sf::RenderWindow& window)
{
    for (auto& node : nodes_)
        node.render(window);
}

void EnemySnake::makeDecision(const std::vector<Fruit>& fruits, const Snake& playerSnake)
{
    sf::Vector2f headPos = getHeadPosition();
    sf::Vector2f playerHeadPos = playerSnake.getHeadPosition();
    
    // 计算到最近水果的距离
    float minFruitDistance = std::numeric_limits<float>::max();
    sf::Vector2f nearestFruitPos;
    
    for (const auto& fruit : fruits)
    {
        float distance = calculateDistance(headPos, fruit.getposition());
        if (distance < minFruitDistance)
        {
            minFruitDistance = distance;
            nearestFruitPos = fruit.getposition();
        }
    }

    float playerDistance = calculateDistance(headPos, playerHeadPos);

    // 如果玩家在附近且可以追逐
    if (playerDistance < 100.f && canChasePlayer_)
    {
        currentState_ = AIState::ChasePlayer;
        
        // 设置方向
        float dx = playerHeadPos.x - headPos.x;
        float dy = playerHeadPos.y - headPos.y;
        
        if (std::abs(dx) > std::abs(dy))
        {
            if (dx > 0 && direction_ != Direction::Left)
                direction_ = Direction::Right;
            else if (dx < 0 && direction_ != Direction::Right)
                direction_ = Direction::Left;
        }
        else
        {
            if (dy > 0 && direction_ != Direction::Up)
                direction_ = Direction::Down;
            else if (dy < 0 && direction_ != Direction::Down)
                direction_ = Direction::Up;
        }
    }
    // 如果周围有水果，就去吃水果
    else if (minFruitDistance < 100.f)
    {
        currentState_ = AIState::ChaseFruit;
        
        // 计算到水果的精确方向
        float dx = nearestFruitPos.x - headPos.x;
        float dy = nearestFruitPos.y - headPos.y;
        
        // 如果距离水果很近（比如蛇身宽度的3倍），进行精确对齐
        if (minFruitDistance < SnakeNode::Width * 3)
        {
            // 先对齐横向位置
            if (std::abs(dx) > SnakeNode::Width / 2)
            {
                if (dx > 0 && direction_ != Direction::Left)
                    direction_ = Direction::Right;
                else if (dx < 0 && direction_ != Direction::Right)
                    direction_ = Direction::Left;
            }
            // 横向对齐后再对齐纵向位置
            else if (std::abs(dy) > SnakeNode::Height / 2)
            {
                if (dy > 0 && direction_ != Direction::Up)
                    direction_ = Direction::Down;
                else if (dy < 0 && direction_ != Direction::Down)
                    direction_ = Direction::Up;
            }
        }
        // 如果距离较远，使用普通的追踪逻辑
        else
        {
            if (std::abs(dx) > std::abs(dy))
            {
                if (dx > 0 && direction_ != Direction::Left)
                    direction_ = Direction::Right;
                else if (dx < 0 && direction_ != Direction::Right)
                    direction_ = Direction::Left;
            }
            else
            {
                if (dy > 0 && direction_ != Direction::Up)
                    direction_ = Direction::Down;
                else if (dy < 0 && direction_ != Direction::Down)
                    direction_ = Direction::Up;
            }
        }
    }
    // 如果周围没有水果，就随机游走
    else 
    {
        currentState_ = AIState::Wander;
        // 随机选择一个方向
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<float> dis(0, 2*std::acos(-1));
        
        float randomAngle = dis(gen);
        float dx = cos(randomAngle);
        float dy = sin(randomAngle);
        
        if (std::abs(dx) > std::abs(dy))
        {
            if (dx > 0 && direction_ != Direction::Left)
                direction_ = Direction::Right;
            else if (dx < 0 && direction_ != Direction::Right)
                direction_ = Direction::Left;
        }
        else
        {
            if (dy > 0 && direction_ != Direction::Up)
                direction_ = Direction::Down;
            else if (dy < 0 && direction_ != Direction::Down)
                direction_ = Direction::Up;
        }
    }
}

float EnemySnake::calculateDistance(sf::Vector2f pos1, sf::Vector2f pos2) const
{
    return std::sqrt(
        std::pow(pos2.x - pos1.x, 2) + 
        std::pow(pos2.y - pos1.y, 2)
    );
}

bool EnemySnake::isPathSafe(sf::Vector2f target) const
{
    // 检查路径是否安全（不会撞到自己或边界）
    // 这里可以添加更复杂的路径检查逻辑
    return true;
}

// bool EnemySnake::hitOtherSnake(const Snake& other) const
// {
//     // 检查蛇头是否与玩家蛇的任何部分相撞
//     sf::FloatRect headBounds = nodes_[0].getBounds();
    
//     // 检查与玩家蛇头的碰撞
//     if (headBounds.intersects(other.getBounds()))
//         return true;
        
//     // 检查与玩家蛇身体的碰撞
//     for (size_t i = 1; i < other.getSize(); ++i)
//     {
//         if (headBounds.intersects(other.getNodeBounds(i)))
//             return true;
//     }
    
//     return false;
// }

}