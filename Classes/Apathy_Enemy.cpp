#include "Apathy_Enemy.h"


Apathy_Enemy :: Apathy_Enemy()
{
	Current_Type = UNKNOWN_ENEMY;
	M_Speed	     = 0;
	M_HP         = 0;
	M_Score      = 0;
}

Apathy_Enemy :: ~Apathy_Enemy()
{
	Current_Type = UNKNOWN_ENEMY;
	M_Speed	     = 0;
	M_HP         = 0;
	M_Score      = 0;
}

Apathy_Enemy * Apathy_Enemy :: Create(EnemyType type)
{
	auto *pEn = new Apathy_Enemy();
	if (pEn && pEn->Init(type))
    {
        pEn->autorelease();
        return pEn;
    }
    else
    {
        delete pEn;
        pEn = NULL;
        return NULL;
    }
}

bool Apathy_Enemy :: Init(EnemyType type)
{
	this->Current_Type = type;
	switch(type)
	{
		case SMALL_ENEMY:
			if (!Sprite::initWithSpriteFrameName("enemy1.png")) 
			{
				return false;
			}
			this->M_HP = 1;
			this->M_Speed = 5;
			this->M_Score = 1;
			break;
		case MIDDLE_ENEMY:
			if (!Sprite::initWithSpriteFrameName("enemy2.png")) 
			{
				return false;
			}
			this->M_HP = 3;
			this->M_Speed = 3;
			this->M_Score = 3;
			break;
		case BIG_ENEMY:
			{
				if (!Sprite::initWithSpriteFrameName("enemy3_n1.png")) 
				{
					return false;
				}
				//大型敌机的飞行动画
				auto aniBigEnemeyFly = AnimationCache::getInstance()->getAnimation("aniBigEnemeyFly");
				this->runAction(Animate::create(aniBigEnemeyFly));
				//播放音效
				Audio->playEffect("big_spaceship_flying.mp3",true);
				this->M_Speed = 1;
				this->M_HP = 10;
				this->M_Score = 10;
				break;
			}
		default:
			break;
	}
	return true;
}

bool Apathy_Enemy :: Hit()
{
	this->M_HP--;
	if(M_HP <= 0)
	{
		this->Hit_Down();
		return true;
	}
	auto aniEnemyHit = Animation::create();
	switch (this->Current_Type)
	{
		case SMALL_ENEMY:
			return false;	//小敌机没有受伤动画，提前退出
		case MIDDLE_ENEMY:
			aniEnemyHit = AnimationCache::getInstance()->getAnimation("aniMiddleEnemyHit");
			break;
		case BIG_ENEMY:
			aniEnemyHit = AnimationCache::getInstance()->getAnimation("aniBigEnemyHit");
			break;
	}
	aniEnemyHit->setDelayPerUnit(0.2f);
	this->runAction(Animate::create(aniEnemyHit));

	return false;
}

void Apathy_Enemy :: Hit_Down()
{
	auto aniEnemyDown = Animation::create();
	switch (this->Current_Type)
	{
		case SMALL_ENEMY:
			aniEnemyDown = AnimationCache::getInstance()->getAnimation("aniSmallEnemyDown");
			Audio->playEffect("enemy1_down.mp3");
			break;
		case MIDDLE_ENEMY:
			aniEnemyDown = AnimationCache::getInstance()->getAnimation("aniMiddleEnemyDown");
			Audio->playEffect("enemy2_down.mp3");
			break;
		case BIG_ENEMY:
			aniEnemyDown = AnimationCache::getInstance()->getAnimation("aniBigEnemyDown");
			Audio->stopEffect(Audio->playEffect("big_spaceship_flying.mp3"));
			Audio->playEffect("enemy3_down.mp3");
			break;
		default:
			break;
	}

	//aniEnemyDown->setLoops(1);
	aniEnemyDown->setDelayPerUnit(0.1f);
	
	auto animate = Animate::create(aniEnemyDown);

	//CallFunc是动作，允许我们的代码作为动作执行
	//Sequence是动作，作用是将多个动作按顺序依次执行
	auto callFuncN = CallFuncN::create([](Node* node) 
	{
		//Node参数是这个动作的执行者
		//将自己从父容器删除并清除(定时器，动作)
		node->removeFromParentAndCleanup(true);
	});

	//this->stopAllActions();
	this->runAction(Sequence::create(animate, callFuncN, NULL));

	//this->runAction(Sequence :: create(Animate :: create(aniEnemyDown),RemoveSelf :: create(),nullptr));
}

int Apathy_Enemy :: GetSpeed()
{
	return this->M_Speed;
}

int Apathy_Enemy :: GetHP()
{
	return this->M_HP;
}

int Apathy_Enemy :: GetScore()
{
	return this->M_Score;
}