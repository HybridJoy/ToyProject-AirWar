#pragma once

#ifndef  _APATHY_ITEMS_H_
#define  _APATHY_ITEMS_H_

#include "CommonData.h"

typedef enum{
	UNKNOWN_ITEM,
	BULLET1,
	BULLET2,
	UFO1,
	UFO2
}ItemType;

class Apathy_Items : public Sprite
{
private:
	ItemType I_type;
	int I_Speed;
public:
	Apathy_Items();
	~Apathy_Items();

	static Apathy_Items * Create(ItemType);
	bool Init(ItemType);
	void Item_animation();

	ItemType GetType();
	int GetSpeed();
	void SetSpeed(int);
};

#endif