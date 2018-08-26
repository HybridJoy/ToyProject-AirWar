#pragma once

#ifndef  _APATHY_HERO_H_
#define  _APATHY_HERO_H_


#include "CommonData.h"

typedef enum{
	UNKNOWN_HERO,
	HERO1
}HeroType;

class Apathy_Hero : public Sprite
{
private:
	HeroType Current_Type;
	int H_Speed;
	int H_HP;
	Point Vec;

public:
	Apathy_Hero();
	~Apathy_Hero();

	static Apathy_Hero * Create(HeroType);
	bool Init(HeroType);

	void Move();
	bool Hit();
	void Hit_Down();

	int GetSpeed();
	int GetHP();
};

#endif