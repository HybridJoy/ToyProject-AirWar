#pragma once

#ifndef  _APATHY_GAMEOVER_SCENE_H_
#define  _APATHY_GAMEOVER_SCENE_H_


#include "CommonData.h"

class Apathy_GameOverScene : public Layer
{
public:
	virtual bool init(int);	
	static Scene * CreateScene(int);
	static Apathy_GameOverScene * create(int);
};
#endif