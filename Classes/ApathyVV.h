#pragma once

#ifndef  _APATHY_VV_H_
#define  _APATHY_VV_H_


#include "Apathy_Enemy.h"
#include "Apathy_Hero.h"
#include "Apathy_Items.h"

class ApathyVV : public Layer 
{
public:
	static Scene* CreateScene();

	virtual bool init();  

	void update(float);
	void UpdateBomb();
	void PauseAndResume(Ref *);
	void GameOver(Apathy_Hero *);

	CREATE_FUNC(ApathyVV);

	void SetBackground();

	void CreateProp(float);
	void CreateBullet(float);
	void CreateEnemyBullet(Apathy_Enemy *);
	void CreateHero(HeroType);

	void CreateEnemy(EnemyType);
	void CreateSmallEnemy(float);
	void CreateMiddleEnemy(float);
	void CreateBigEnemy(float);

	void DisplayDamage(Apathy_Enemy *);
private:
	Vector<Apathy_Items *> M_Props;
	Vector<Apathy_Items *> M_Bullets;
	Vector<Apathy_Items *> M_EnemyBullets;
	Vector<Apathy_Enemy *> M_Enemies;
	Vector<Apathy_Hero *> M_Heroes;

	int Total_Score;
	int M_DobuleBullet;
	int M_Bombs;
	int Hero_Inition_HP;
};

#endif