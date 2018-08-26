#pragma once

#ifndef  _APATHY_ENEMY_H_
#define  _APATHY_ENEMY_H_

#include "CommonData.h"

typedef enum{
	UNKNOWN_ENEMY,
	SMALL_ENEMY,
	MIDDLE_ENEMY,
	BIG_ENEMY
}EnemyType;

class Apathy_Enemy : public Sprite
{
public:

private:
	EnemyType Current_Type;
	int M_Speed;
	int M_HP;
	int M_Score;

public:
	Apathy_Enemy();
	~Apathy_Enemy();

	static Apathy_Enemy * Create(EnemyType);
	bool Init(EnemyType);

	bool Hit();
	void Hit_Down();

	int GetSpeed();
	int GetHP();
	int GetScore();
};

#endif