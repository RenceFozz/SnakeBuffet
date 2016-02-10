#include "GraphicsScene.h"

#define LEFT    EventKeyboard::KeyCode::KEY_LEFT_ARROW
#define RIGHT   EventKeyboard::KeyCode::KEY_RIGHT_ARROW
#define UP      EventKeyboard::KeyCode::KEY_UP_ARROW
#define DOWN    EventKeyboard::KeyCode::KEY_DOWN_ARROW
#define SPACE   EventKeyboard::KeyCode::KEY_SPACE

USING_NS_CC;

enum Category   {S_HEAD, S_BODY, FOOD, WALL};
enum Directions {L,R,U,D};

Scene* GraphicsScene::createScene(){
    auto scene = Scene::createWithPhysics();
    scene->getPhysicsWorld()->setDebugDrawMask( PhysicsWorld::DEBUGDRAW_ALL );
    scene->getPhysicsWorld()->setGravity(Vec2(0,0));
    
    auto layer = GraphicsScene::create();
    layer->SetPhysicsWorld( scene->getPhysicsWorld() );
    
    scene->addChild(layer);
    
    return scene;
}

bool GraphicsScene::init()
{
    if ( !Layer::init() ){
        return false;
    }
    
    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
    foodSprites = {"Assets/bananas_t.png", "Assets/cheese_t.png", "Assets/cheeseburger_t.png", "Assets/cheesecake_t.png", "Assets/chicken_t.png", "Assets/dimsum_set_t.png", "Assets/turkey_t.png"};
    play = true;
    
    GraphicsScene::initSound();
    GraphicsScene::initBG();
    GraphicsScene::respawnFood();
    GraphicsScene::initSnake();
    GraphicsScene::initBorder();
    GraphicsScene::initScore();
    
    //LISTENERS
    auto listener = EventListenerKeyboard::create();
    listener->onKeyPressed = CC_CALLBACK_2(GraphicsScene::onKeyPressed, this);
    //listener->onKeyReleased = CC_CALLBACK_2(KeyboardTest::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(GraphicsScene::onContactBegin, this);
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);
    
    if(play){
        this->scheduleUpdate();
    }
    
    return true;
}

void GraphicsScene::update(float delta){
    auto position = snakeSprite[0]->getPosition();
    position.y += coorY;
    position.x += coorX;
    
    //if (position.x  < 0 - (snake->getBoundingBox().size.width / 2))
    //    position.x = this->getBoundingBox().getMaxX() + snake->getBoundingBox().size.width/2;
    /*for(int iter=0;i<snakeSprite.size();i++){
     snakeSprite[iter]->setPosition(position.x * (iter+1), position.y * (iter+1));
     }*/
    /*for(int iter=snakeSprite.size()-1;iter>1;i--){
     auto prevPosition =snakeSprite[iter-1]->getPosition();
     snakeSprite[iter]->setPosition(prevPosition.x, prevPosition.y);
     }*/
    if(snakeSprite.size() > 1){
        //cout << "oldX: " << oldX << endl;
        //cout << "oldY: " << oldY << endl;
        for(int x=snakeSprite.size()-1;x>0 ;x--){
            int y=x-1;
            auto oldX = snakeSprite[y]->getPositionX();
            auto oldY = snakeSprite[y]->getPositionY();
            //cout << "oldX: " << oldX << endl;
            //cout << "oldY: " << oldY << endl;
            snakeSprite[x]->setPosition(oldX,oldY);
        }
    }
    
    snakeSprite[0]->setPosition(position);
}

void GraphicsScene::initBG(){
    bg = Sprite::create("Assets/grass_bg.png");
    bg->setScale(visibleSize.width / bg->getContentSize().width, visibleSize.height / bg->getContentSize().height);
    bg->setPosition(this->getContentSize().width / 2, this->getContentSize().height / 2);
    
    this->addChild(bg);
}

void GraphicsScene::initSound(){
    audio->preloadBackgroundMusic("Assets/Song_A.wav");
    audio->playBackgroundMusic("Assets/Song_A.wav", true);
}

void GraphicsScene::initSnake(){
    coorX = 0; coorY = 0;
    snake = Sprite::create("Assets/snake_head_t.png");
    auto spriteBody = PhysicsBody::createBox(snake->getContentSize(), PhysicsMaterial(0.1, 1.0, 0.0));
    snake->setPosition(this->getContentSize().width / 2, this->getContentSize().height / 2);
    spriteBody->setCollisionBitmask(Category::S_HEAD);
    spriteBody->setContactTestBitmask(true);
    snake->setPhysicsBody(spriteBody);
    snakeSprite.push_back(snake);
    
    this->addChild(snakeSprite[0]);
}

void GraphicsScene::initBorder(){
    auto edgeBody = PhysicsBody::createEdgeBox(visibleSize, PhysicsMaterial(0.1, 1.0, 0.0));
    //edgeBody->setDynamic(false);
    edgeBody->setCollisionBitmask(Category::WALL);
    edgeBody->setContactTestBitmask(true);
    
    auto edgeNode = Node::create();
    edgeNode->setPosition( Point( visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y ) );
    edgeNode->setPhysicsBody(edgeBody);
    
    this->addChild(edgeNode);
}

void GraphicsScene::initScore(){
    scoreLabel = Label::createWithTTF("Score: "+to_string(score),"fonts/Marker Felt.ttf",12);
    scoreLabel->setPosition(Vec2(420,310));
    this->addChild(scoreLabel,4);
}

void GraphicsScene::respawnFood(){
    foodX = (arc4random() % 8) + 1;
    foodY = (arc4random() % 6) + 1;
    i = arc4random() % foodSprites.size();
    
    food = Sprite::create(foodSprites[i]);
    auto spriteBody = PhysicsBody::createCircle(food->getContentSize().width / 2, PhysicsMaterial(0,1,0) );
    food->setPosition(foodX*50, foodY*50);
    spriteBody->setCollisionBitmask(Category::FOOD);
    spriteBody->setContactTestBitmask(true);
    food->setPhysicsBody(spriteBody);
    
    this->addChild(food);
    
    cout << foodSprites[i] << endl;
    cout << foodX*50 << endl;
    cout << foodY*50 << endl;
}

void GraphicsScene::addSnake(){
    auto body = Sprite::create("Assets/snake_ball_t.png");
    auto spriteBody = PhysicsBody::createBox(body->getContentSize(), PhysicsMaterial(0.1, 1.0, 0.0));
    auto prevPosition = snakeSprite.back()->getPosition();
    
    body->setPosition(prevPosition.x, prevPosition.y);
    spriteBody->setCollisionBitmask(Category::S_BODY);
    spriteBody->setContactTestBitmask(true);
    body->setPhysicsBody(spriteBody);
    snakeSprite.push_back(body);
    
    this->addChild(body);
}

void GraphicsScene::updateScore(){
    score+=10;
    scoreLabel->setString("Score: "+to_string(score));
}

void GraphicsScene::gameOver(){
    gameOverLabel = Label::createWithTTF("GAME OVER!","fonts/Marker Felt.ttf",64);
    gameOverLabel->setPosition(this->getContentSize().width / 2, this->getContentSize().height / 2);
    gameOverLabel->setTextColor(Color4B::RED);
    this->addChild(gameOverLabel,5);
}

bool GraphicsScene::onContactBegin(PhysicsContact &contact){
    int a = contact.getShapeA()->getBody()->getCollisionBitmask();
    int b = contact.getShapeB()->getBody()->getCollisionBitmask();
    auto nodeA = contact.getShapeA()->getBody()->getNode();
    auto nodeB = contact.getShapeB()->getBody()->getNode();
    
    if ( (a==Category::S_HEAD && b==Category::WALL)     ||
        (b==Category::S_HEAD && a==Category::WALL)     ||
        (a==Category::S_HEAD && b==Category::S_BODY)   ||
        (b==Category::S_HEAD && a==Category::S_BODY)){
        CCLOG("COLLISION IS HAPPENING");
        play = false;
        GraphicsScene::gameOver();
        Director::getInstance()->pause();
    }
    else if (a==Category::S_HEAD && b==Category::FOOD){
        nodeB->removeFromParent();
        GraphicsScene::respawnFood();
        GraphicsScene::addSnake();
        cout << "Size: " << snakeSprite.size() << endl;
        GraphicsScene::updateScore();
    }
    else if (b==Category::S_HEAD && a==Category::FOOD){
        nodeA->removeFromParent();
        GraphicsScene::respawnFood();
        GraphicsScene::addSnake();
        cout << "Size: " << snakeSprite.size() << endl;
        GraphicsScene::updateScore();
    }
    return true;
}

void GraphicsScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event){
    if(play){
        Vec2 loc = snake->getPosition();
        
        int direc=-1;
        switch(keyCode){
            case EventKeyboard::KeyCode::KEY_R:
                //Director::getInstance()->restart();
                cout << "Supposed to be Restart" << endl;
                break;
            case SPACE:
                if(!Director::getInstance()->isPaused()){
                    Director::getInstance()->pause();
                }
                else{
                    Director::getInstance()->resume();
                }
                break;
            case LEFT:
                if(currDir!=Directions::R){
                    coorX = -25; coorY = 0;
                    direc=0;
                    cout << "LEFT" << endl;
                    currDir = Directions::L;
                }
                break;
            case UP:
                if(currDir!=Directions::D){
                    coorY = 25; coorX = 0;
                    direc=1;
                    cout << "UP" << endl;
                    currDir = Directions::U;
                }
                break;
            case RIGHT:
                if(currDir!=Directions::L){
                    coorX = 25; coorY = 0;
                    direc=2;
                    cout << "RIGHT" << endl;
                    currDir = Directions::R;
                }
                break;
            case DOWN:
                if(currDir!=Directions::U){
                    coorY = -25; coorX = 0;
                    direc=3;
                    cout << "DOWN" << endl;
                    currDir = Directions::D;
                }
                break;
        }
        if(direc>=0){
            auto rotateTo = RotateTo::create(0.00001,90*direc);
            snake->runAction(rotateTo);
        }
    }
}

