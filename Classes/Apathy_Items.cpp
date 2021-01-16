#include "Apathy_Items.h"

Apathy_Items :: Apathy_Items()
{
	I_type = UNKNOWN_ITEM;
	I_Speed  = 0;
}

Apathy_Items :: ~Apathy_Items()
{
	I_type = UNKNOWN_ITEM;
	I_Speed  = 0;
}

Apathy_Items * Apathy_Items :: Create(ItemType type)
{
	auto *pEn = new Apathy_Items();

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

bool Apathy_Items :: Init(ItemType type)
{
	this->I_type = type;
	switch (type)
	{
		case BULLET1:
			if(!Sprite::initWithSpriteFrameName("bullet1.png"))
			{
				return false;
			}
			this->I_Speed = 3;
			break;
		case BULLET2:
			if(!Sprite::initWithSpriteFrameName("bullet2.png"))
			{
				return false;
			}
			this->I_Speed = 4;
			break;
		case UFO1:
			if(!Sprite::initWithSpriteFrameName("ufo1.png"))
			{
				return false;
			}
			this->I_Speed = 4;
			break;
		case UFO2:
			if(!Sprite::initWithSpriteFrameName("ufo2.png"))
			{
				return false;
			}
			this->I_Speed = 4;
			break;
		default:
			break;
	}
	return true;
}

void Apathy_Items :: Item_animation()
{
	switch (this->I_type)
	{
		case BULLET1:
		case BULLET2:
			break;
		case UFO1:
		case UFO2:
			auto move1 = MoveBy::create(0.5f, Point(0, -300));	//第一次进入屏幕
			auto move2 = MoveTo::create(1, Point(this->getPositionX(), -this->getContentSize().height / 2));	//第二次移出屏幕下边界
			auto sequence = Sequence::create(move1, move1->reverse(), move2, RemoveSelf::create(), nullptr);
			this->runAction(sequence);
			break;
	}
}

ItemType Apathy_Items :: GetType()
{
	return this->I_type;
}

int Apathy_Items :: GetSpeed()
{
	return this->I_Speed;
}

void Apathy_Items :: SetSpeed(int speed)
{
	this->I_Speed = speed;
}