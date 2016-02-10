#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include "cocos2d.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace std;

class GraphicsScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(GraphicsScene);
    
    void update(float) override;
    
private:
    PhysicsWorld *sceneWorld;
    void SetPhysicsWorld(PhysicsWorld *world ) { sceneWorld = world; };
    bool onContactBegin(PhysicsContact &contact);
    void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
    void respawnFood();
    void moveSnake(int coordX, int coordY);
    void initBG();
    void initSound();
    void initSnake();
    void initBorder();
    void initScore();
    void addSnake();
    void updateScore();
    void gameOver();
    
    CocosDenshion::SimpleAudioEngine* audio = CocosDenshion::SimpleAudioEngine::getInstance();
    
    int foodX, foodY, i, coorX, coorY;
    vector<string> foodSprites;
    bool play;
    Size visibleSize;
    Vec2 origin;
    
    int currDir = -1;
    int score = 0;
    
    Sprite* food;
    Sprite* bg;
    Sprite* snake;
    Label* scoreLabel;
    Label* gameOverLabel;
    vector<Sprite*> snakeSprite;
};