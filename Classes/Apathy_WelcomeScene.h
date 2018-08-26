#pragma once

#ifndef  _APATHY_WELCOME_SCENE_H_
#define  _APATHY_WELCOME_SCENE_H_


#include "CommonData.h"

class Apathy_WelcomeScene : public Layer
{
public:
	virtual bool init();	
	static Scene * CreateScene();

	CREATE_FUNC(Apathy_WelcomeScene);

};

#endif