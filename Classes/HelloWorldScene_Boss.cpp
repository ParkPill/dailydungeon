#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "LegendDaryButton.h"
#include "HudLayer.h"
#include "GameManager.h"
#include "MyMessageBox.h"
#include "NativeInterface.h"
#include "Buddies.h"
#include "AwesomeDialogBox.h"
#include "LanguageManager.h"
using namespace cocos2d;
using namespace CocosDenshion;

void HelloWorld::initBoss(int boss)
{
    CCLOG("init boss");
    Droppable* door = Droppable::createDroppable(UNIT_DOOR, 1, 0, "bigDoor.png");
    door->centerPosition = Point(0, -12);

    theDoor = door;
    this->setScale(4);
    bossScheduleTime = 0;
    bossStep = 0;
    bossInnerStep = 0;
    float energyRate = 1;
    int clearCount = UDGetInt(KEY_CLEAR_COUNT, 0);
    if (clearCount == 0) {
        energyRate = 0.25f;
    }else if (clearCount == 1) {
        energyRate = 0.5f;
    }else if (clearCount == 2) {
        energyRate = 0.75f;
    }
    
    if(boss == BOSS_BAZOOKA){
        GameManager::getInstance()->getHudLayer()->displayBossEnergy("BAZOOKA MAN");
        GameManager::getInstance()->getHudLayer()->setBossEnergy(100);
        theBoss = Bazooka::create(UNIT_PIERROT_NORMAL, 1500*energyRate, 0, "pierrotBoss1_0.png", 50, 3, ENEMY_MOVE_WANDER_CROSS_PLATFORM, ENEMY_MOVE_WANDER_CROSS_PLATFORM, 1000);
        theBoss->setTarget(player);
        spriteBatch->addChild(theBoss);
        theBoss->setPosition(Point(26*TILE_SIZE, 11*TILE_SIZE));
        
        droppableArray.pushBack(theBoss);
        fireableArray.pushBack(theBoss);
        theBoss->setFlippedX(true);
        enemyArray.pushBack(theBoss);
        
        door->setPosition(Point(430, 320));
    }else if(boss == BOSS_MACHINE_GUN){
        GameManager::getInstance()->getHudLayer()->displayBossEnergy("MACHINE GAN");
        GameManager::getInstance()->getHudLayer()->setBossEnergy(100);
        theBoss = Machinegun::create(UNIT_PIERROT_NORMAL, 2300*energyRate, 0, "pierrotBoss1_0.png", 50, 3, ENEMY_MOVE_WANDER_CROSS_PLATFORM, ENEMY_MOVE_WANDER_CROSS_PLATFORM, 1000);
        theBoss->setTarget(player);
        spriteBatch->addChild(theBoss);
        theBoss->setPosition(Point(26*TILE_SIZE, 11*TILE_SIZE));
        
        droppableArray.pushBack(theBoss);
        fireableArray.pushBack(theBoss);
        theBoss->setFlippedX(true);
        enemyArray.pushBack(theBoss);
        
        door->setPosition(Point(430, 320));
    }else if(boss == BOSS_GIANT){
        GameManager::getInstance()->getHudLayer()->displayBossEnergy("GIANT PIERROT");
        GameManager::getInstance()->getHudLayer()->setBossEnergy(100);
        theBoss = Giant::create(UNIT_PIERROT_NORMAL, 3000*energyRate, 0, "pierrotBoss1_0.png", 50, 3, ENEMY_MOVE_WANDER_CROSS_PLATFORM, ENEMY_MOVE_WANDER_CROSS_PLATFORM, 1000);
        theBoss->setTarget(player);
        spriteBatch->addChild(theBoss);
        theBoss->setPosition(Point(26*TILE_SIZE, 11*TILE_SIZE));
        
        fireableArray.pushBack(theBoss);
        theBoss->setFlippedX(true);
        enemyArray.pushBack(theBoss);
        
        door->setPosition(Point(430, 320));
    }else if(boss == BOSS_GIANT_HOOD){
        GameManager::getInstance()->getHudLayer()->displayBossEnergy("MYSTeRY HOOD");
        GameManager::getInstance()->getHudLayer()->setBossEnergy(100);
        theBoss = GiantHood::create(UNIT_PIERROT_NORMAL, 5000*energyRate, 0, "pierrotBoss1_0.png", 50, 3, ENEMY_MOVE_WANDER_CROSS_PLATFORM, ENEMY_MOVE_WANDER_CROSS_PLATFORM, 1000);
        theBoss->setTarget(player);
        spriteBatch->addChild(theBoss);
        theBoss->setPosition(Point(26*TILE_SIZE, 11*TILE_SIZE));
        
        fireableArray.pushBack(theBoss);
        theBoss->setFlippedX(true);
        enemyArray.pushBack(theBoss);
        
        door->setPosition(Point(430, 320));
    }

    enemyMaxCount++;
}
void HelloWorld::bossScheduleBazooka(float dt){
    bossScheduleTime += dt;
    if(bossStep == -1){
        if(bossScheduleTime > 6) return;
        else bossStep++;
    }else if(bossStep == 0){
        if(bossScheduleTime < 3) return;
        Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 50, 0, 130, "pierrotBoss0_missile_small.png");
        missile->isFregile = true;
        missile->setPosition(theBoss->getPosition() + Point(-50, 50));
        missile->movingAngle = 100 + bossInnerStep*5;
        spriteBatch->addChild(missile);
        straightMovingArray.pushBack(missile);
        enemyMissileArray.pushBack(missile);
        bossInnerStep++;
        if(bossInnerStep == 30){
            bossStep++;
            bossInnerStep = 0;
            bossScheduleTime = 0;
        }
    }else if(bossStep == 1){
        if(bossScheduleTime < 3) return;
        Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 100, 0, 100, "pierrotBoss0_missile_big.png");
        missile->isFregile = true;
        missile->setPosition(theBoss->getPosition() + Point(-50, 50));
        missile->weight = 1;
        missile->movingAngle = GameManager::getInstance()->getAngle(player->getPosition() + Point(0, 15), missile->getPosition());
        spriteBatch->addChild(missile);
        straightMovingArray.pushBack(missile);
        enemyMissileArray.pushBack(missile);
        bossInnerStep++;
        if(bossInnerStep == 30){
            bossStep++;
            bossInnerStep = 0;
            bossScheduleTime = 0;
        }
    }else if(bossStep == 2){
        if(bossScheduleTime < 3) return;
        
        if(bossScheduleTime > 4){
            bossInnerStep++;
            bossScheduleTime = 3;
        }
        
        if(bossInnerStep == 30){
            bossStep++;
            bossInnerStep = 0;
            bossScheduleTime = 0;
        }
        if(bossInnerStep == 7){
            bossStep = -1;
            bossInnerStep = 0;
            bossScheduleTime = 0;
        }
    }
}
void HelloWorld::bossScheduleGiantPenguin(float dt){
    bossScheduleTime += dt;
    
    if(theBoss->energy < theBoss->maxEnergy/2 && theBoss->getTag() == UNIT_FLYING_PENGUIN){
        theBoss->setTag(UNIT_WALKING_PENGUIN);
        theBoss->runAnimation("walkingPenguin", true);
        float y = 104;
        float leftX = 150;
        float rightX = 500;
        theBoss->runAction(MoveTo::create(1, Point(theBoss->getPosition().x, y)));
        theBoss->runAction(RepeatForever::create(Sequence::create(DelayTime::create(1), ScaleTo::create(0.0001, -4, 4), MoveTo::create(5, Point(leftX, y)), DelayTime::create(1), ScaleTo::create(0.0001, 4, 4), MoveTo::create(5, Point(rightX, y)), NULL)));
    }
    
    if(bossStep == -1){
        if(bossScheduleTime > 6) return;
        else bossStep++;
    }else if(bossStep == 0){
        if(bossScheduleTime < 3) return;
        Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 1, 0, 100, "msHotDog.png");
        missile->setScale(2);
        missile->isFregile = true;
        missile->setPosition(theBoss->getPosition());
        missile->weight = 1;
        missile->movingAngle = GameManager::getInstance()->getAngle(player->getPosition() + Point(0, 15), missile->getPosition());
        missile->setRotation(-missile->movingAngle);
        spriteBatch->addChild(missile);
        straightMovingArray.pushBack(missile);
        enemyMissileArray.pushBack(missile);
        
        bossInnerStep++;
        if(bossInnerStep == 10){
            bossStep = 1;
            bossInnerStep = 0;
            bossScheduleTime = 0;
        }
    }else if(bossStep == 1){
        if(bossScheduleTime < 3) return;
        if(bossInnerStep%5==0){
            for(int i = rand()%15;i < 360;i += 15){
                Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 1, 0, 100, "msHotDog.png");
                missile->setScale(2);
                missile->isFregile = true;
                missile->setPosition(theBoss->getPosition());
                missile->weight = 1;
                missile->movingAngle = i;
                missile->setRotation(-i);
                spriteBatch->addChild(missile);
                straightMovingArray.pushBack(missile);
                enemyMissileArray.pushBack(missile);
            }
        }
        
        bossInnerStep++;
        
        if(bossInnerStep == 30){
            bossStep = 2;
            bossInnerStep = 0;
            bossScheduleTime = 0;
            
        }
    }else if(bossStep == 2){
        if(bossScheduleTime < 3) return;
        
        if(bossScheduleTime > 4){
            CCLOG("bat!!!");
            Bat* enemy = Bat::create(UNIT_BAT, 40, -12, "bat1.png", 0, 10, ENEMY_MOVE_HANG, ENEMY_MOVE_FLY_TO_HERO_FOLLOW, 400);
            enemy->setTarget(player);
            spriteBatch->addChild(enemy);
            enemy->setPosition(Point(100 + rand()%400, 360));
            enemyArray.pushBack(enemy);
            flyingSwimingEnemyArray.pushBack(enemy);
        }
        
        bossInnerStep++;
        
        if(bossInnerStep == 6){
            if(theBoss->getTag() == UNIT_FLYING_PENGUIN){
                bossStep = 3;
            }else{
                bossStep = 0;
            }
            bossInnerStep = 0;
            bossScheduleTime = 0;
        }
    }else if(bossStep == 3){
        CCLOG("move to");
//        bossStep = rand()%4;
//        bossInnerStep = 0;
//        bossScheduleTime = 0;
        if(bossScheduleTime < 3) return;

        if(bossInnerStep == 0){
            float gapX = theBoss->getPositionX() - player->getPositionX();
            float gapY = theBoss->getPositionY() - player->getPositionY();
            float distance = sqrtf(gapX*gapX + gapY*gapY);
            float time = distance/20;
            theBoss->runAction(MoveTo::create(time, player->getPosition()));//Point(theBoss->getPosition().x, 120)));
//            theBoss->runAnimation("walkingPenguin", true);
        }
        
        bossInnerStep++;
        
        if(bossInnerStep == 10){
            bossStep = 0;
            bossInnerStep = 0;
            bossScheduleTime = 0;
        }
    }
}
void HelloWorld::bossScheduleGiantWood(float dt){
    bossScheduleTime += dt;
    float bossX = 660;
    float bossYTop = 1000;
    float bossYBottom = 150;
    if(bossStep == -1){
        for (int i = 0;i<enemyArray.size();i++) {
            EnemyBase* enemy = enemyArray.at(i);
            if(enemy->getTag() == UNIT_WOOD && enemy != theBoss){
                i--;
                enemyArray.eraseObject(enemy);
                enemy->removeFromParentAndCleanup(true);
            }
        }
        
        if(bossScheduleTime > 6) return;
        else bossStep++;
    }else if(bossStep == 0){
        if(bossScheduleTime < 3) return;
        Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 1, 0, 130, "msCloud.png");
        missile->isFregile = true;
        missile->setPosition(theBoss->getPosition() + Point(-(rand()%100), rand()%100));
        missile->movingAngle = 100 + bossInnerStep*5;
        missile->setScale(2);
        spriteBatch->addChild(missile);
        straightMovingArray.pushBack(missile);
        enemyMissileArray.pushBack(missile);
        bossInnerStep++;
        if(bossInnerStep >= 80){
            bossStep = rand()%4;
            bossInnerStep = 0;
            bossScheduleTime = 0;
        }
        GameManager::getInstance()->getAngle(player->getPosition() + Point(0, 15), missile->getPosition());
    }else if(bossStep == 1){
        if(bossScheduleTime < 3) return;
        Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 1, 0, 130, "msCloud.png");
        missile->isFregile = true;
        missile->setPosition(theBoss->getPosition() + Point(-(rand()%100), rand()%100));
        missile->movingAngle = GameManager::getInstance()->getAngle(player->getPosition() + Point(0, 15), missile->getPosition());
        missile->setScale(2);
        spriteBatch->addChild(missile);
        straightMovingArray.pushBack(missile);
        enemyMissileArray.pushBack(missile);
        bossInnerStep++;
        if(bossInnerStep >= 50){
            bossStep = rand()%4;
            bossInnerStep = 0;
            bossScheduleTime = 0;
        }
    }else if(bossStep == 2){
        if(bossScheduleTime < 3) return;
        if(bossInnerStep%5==0){
            for(int i = rand()%15;i < 360;i += 15){
                Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 1, 0, 100, "msCloud.png");
                missile->setScale(2);
                missile->isFregile = true;
                missile->setPosition(theBoss->getPosition());
                missile->weight = 1;
                missile->movingAngle = i;
                missile->setRotation(-i);
                spriteBatch->addChild(missile);
                straightMovingArray.pushBack(missile);
                enemyMissileArray.pushBack(missile);
            }
        }
        
        bossInnerStep++;
        
        if(bossInnerStep == 30){
            this->runEffect(EFFECT_TWINKLE, theBoss->getPosition());
            bossStep = rand()%4;
            bossInnerStep = 0;
            bossScheduleTime = 0;
        }
    }else if(bossStep == 3){
        if(bossScheduleTime < 1) return;
        
        if(bossInnerStep%3==0){
            theBoss->setPosition(Point(bossX, bossYTop));
            bossScheduleTime = 0;
        }else if(bossInnerStep%3==1){
            if (rand()%10 < 3) {
                theBoss->setPosition(Point(player->getPosition().x, bossYTop));
            }else{
                theBoss->setPosition(Point(100 + rand()%400, bossYTop));
            }
            this->runEffect(EFFECT_TWINKLE, Point(theBoss->getPosition().x, bossYBottom));
            bossScheduleTime = 0;
        }else if(bossInnerStep%3==2){
            theBoss->setPosition(Point(theBoss->getPosition().x, bossYBottom));
            bossScheduleTime = 0;
        }
        
        bossInnerStep++;
        
        if(bossInnerStep == 30){
            this->runEffect(EFFECT_TWINKLE, theBoss->getPosition());
            theBoss->setPosition(Point(bossX, bossYTop));
            bossStep++;
            bossInnerStep = 0;
            bossScheduleTime = 0;
        }
    }else if(bossStep == 4){
        if(bossScheduleTime < 3) return;
        
        if(bossScheduleTime > 4){
            if (bossInnerStep < 8) {
                Wood* enemy = Wood::create(UNIT_WOOD, 300, 0, "wood0.png", 4, 10, ENEMY_MOVE_STAND, ENEMY_MOVE_STAND, 500);
                enemy->setTarget(player);
                spriteBatch->addChild(enemy);
                if (bossInnerStep == 0) {
                    enemy->setPosition(Point(16*TILE_SIZE, 22*TILE_SIZE));
                }else if(bossInnerStep == 1) {
                    enemy->setPosition(Point(16*TILE_SIZE, 12*TILE_SIZE));
                }else if(bossInnerStep == 2) {
                    enemy->setPosition(Point(28*TILE_SIZE, 9*TILE_SIZE));
                }else if(bossInnerStep == 3) {
                    enemy->setPosition(Point(28*TILE_SIZE, 19*TILE_SIZE));
                }else if(bossInnerStep == 4) {
                    enemy->setPosition(Point(48*TILE_SIZE, 17*TILE_SIZE));
                }else if(bossInnerStep == 5) {
                    enemy->setPosition(Point(48*TILE_SIZE, 7*TILE_SIZE));
                }else if(bossInnerStep == 6) {
                    enemy->setPosition(Point(60*TILE_SIZE, 24*TILE_SIZE));
                }else{
                    enemy->setPosition(Point(60*TILE_SIZE, 14*TILE_SIZE));
                }
                enemyArray.pushBack(enemy);
                droppableArray.pushBack(enemy);
                fireableArray.pushBack(enemy);
            }
            bossInnerStep++;
            
            bossScheduleTime = 3;
        }
        
        if(bossInnerStep == 25){
            theBoss->setPosition(Point(bossX, bossYBottom));
            bossStep = -1;
            bossInnerStep = 0;
            bossScheduleTime = 0;
        }
    }
}
void HelloWorld::bossScheduleGiantFish(float dt){
    bossScheduleTime += dt;

    if (player->getPositionY() < 32*TILE_SIZE && !theBoss->isTargetFound) {
        GameManager::getInstance()->getHudLayer()->displayBossEnergy("MEGI");
        theBoss->isTargetFound = true;
        this->setScale(2);
    }
    if(bossStep == -1){
        
        if(bossScheduleTime > 6) return;
        else bossStep++;
    }else if(bossStep == 0){
        if(bossScheduleTime < 3) return;
        Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 1, 0, 130, "msGhost.png");
        missile->isFregile = true;
        missile->setPosition(theBoss->getPosition() + Point(-(rand()%100), rand()%100));
        missile->movingAngle = 100 + bossInnerStep*5;
        missile->setScale(2);
        missile->setRotation(-missile->movingAngle);
        spriteBatch->addChild(missile);
        straightMovingArray.pushBack(missile);
        enemyMissileArray.pushBack(missile);
        bossInnerStep++;
        if(bossInnerStep >= 80){
            bossStep = rand()%4;
            bossInnerStep = 0;
            bossScheduleTime = 0;
        }
        GameManager::getInstance()->getAngle(player->getPosition() + Point(0, 15), missile->getPosition());
    }else if(bossStep == 1){
        if(bossScheduleTime < 3) return;
        Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 1, 0, 130, "msOwl.png");
        missile->runAction(RepeatForever::create(RotateBy::create(1, 460)));
        missile->isFregile = true;
        missile->setPosition(theBoss->getPosition() + Point(-(rand()%100), rand()%100));
        missile->movingAngle = GameManager::getInstance()->getAngle(player->getPosition() + Point(0, 15), missile->getPosition());
        missile->setScale(2);
        spriteBatch->addChild(missile);
        straightMovingArray.pushBack(missile);
        enemyMissileArray.pushBack(missile);
        bossInnerStep++;
        if(bossInnerStep >= 50){
            bossStep = rand()%4;
            bossInnerStep = 0;
            bossScheduleTime = 0;
        }
    }else if(bossStep == 2){
        if(bossScheduleTime < 3) return;
        if(bossInnerStep%5==0){
            for(int i = rand()%15;i < 360;i += 15){
                Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 1, 0, 100, "msThreeEyes.png");
                missile->setScale(2);
                missile->isFregile = true;
                missile->setPosition(theBoss->getPosition());
                missile->weight = 1;
                missile->movingAngle = i;
                missile->setRotation(-i);
                spriteBatch->addChild(missile);
                straightMovingArray.pushBack(missile);
                enemyMissileArray.pushBack(missile);
            }
        }
        
        bossInnerStep++;
        
        if(bossInnerStep == 30){
            this->runEffect(EFFECT_TWINKLE, theBoss->getPosition());
            bossStep = rand()%4;
            bossInnerStep = 0;
            bossScheduleTime = 0;
        }
    }else if(bossStep == 3){
        if(bossScheduleTime < 1) return;
        
        
        bossInnerStep++;
        
        if(bossInnerStep == 30){
            bossStep = -1;
            bossInnerStep = 0;
            bossScheduleTime = 0;
        }
    }
}

void HelloWorld::createBossRewards()
{
    if(GameManager::getInstance()->theme == 0)
        this->unschedule(schedule_selector(HelloWorld::bossScheduleBazooka));
    else if(GameManager::getInstance()->theme == 1)
        this->unschedule(schedule_selector(HelloWorld::bossScheduleGiantPenguin));
    else if(GameManager::getInstance()->theme == 2)
        this->unschedule(schedule_selector(HelloWorld::bossScheduleGiantWood));
    else if(GameManager::getInstance()->theme == 3)
        this->unschedule(schedule_selector(HelloWorld::bossScheduleGiantFish));
    
    Droppable* coin;
    for (int i = 0; i < 0; i++) {
        coin = Droppable::createDroppable(UNIT_STAR, 1, 0, "star.png");
        
        coin->velocity = Point(500 - rand()%1000, 50 + rand()%200);
        coin->setPosition(theBoss->getPosition());
        coinArray.pushBack(coin);
        starArray.pushBack(coin);
        spriteBatch->addChild(coin);
        droppableCoinArray.pushBack(coin);
        GameManager::getInstance()->makeItSiluk(coin);
    }
    
    //this->scheduleOnce(schedule_selector(HelloWorld::showSuccessLayer), 6);
    if (GameManager::getInstance()->getHudLayer()->bossName) {
        GameManager::getInstance()->getHudLayer()->bossName->removeFromParentAndCleanup(true);
        GameManager::getInstance()->getHudLayer()->bossName = NULL;
    }
}

void HelloWorld::bossClear()
{
    isGameOver = true;
}
