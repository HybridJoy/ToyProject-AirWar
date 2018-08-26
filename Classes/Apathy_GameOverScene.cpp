#include "Apathy_GameOverScene.h"
#include "ApathyVV.h"

Scene* Apathy_GameOverScene ::CreateScene(int score)
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = Apathy_GameOverScene ::create(score);

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool Apathy_GameOverScene ::init(int score)
{
	// super init first
	if ( !Layer::init() )
	{
		return false;
	}
	
	auto Game_Over_Bg = Sprite::createWithSpriteFrameName("gameover.png");
	Game_Over_Bg->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
	Game_Over_Bg->setPosition(0, visibleSize.height);
	this->addChild(Game_Over_Bg,BackGround_Layer,Game_Over_Bg_Tag);

	auto Highest_Score = UserDefault::getInstance()->getIntegerForKey("HIGHSCORE");
	auto Higner_Score = MAX(Highest_Score,score);
	UserDefault::getInstance()->setIntegerForKey("HIGHSCORE", Higner_Score);

	//添加历史最高分标签
	auto Highest_Score_Lable_Txt = StringUtils::format("%d", Higner_Score);
	auto Highest_Score_Lable = Label::create(Highest_Score_Lable_Txt,"Buxton Sketch",75);
	Highest_Score_Lable->setPosition(Point(visibleSize)/2 + Point(0, 200));
	this->addChild(Highest_Score_Lable, Label_Layer, Highest_Score_Lable_Tag);
	Highest_Score_Lable->setColor(Color3B(150,200,50));

	if(Highest_Score < Higner_Score)
	{
		Audio->playEffect("achievement.mp3");
		auto scaleIn = ScaleTo::create(0.5f, 1.5);
		auto scaleOut = ScaleTo::create(0.5f, 1);
		Highest_Score_Lable->runAction(Sequence::create(scaleIn, scaleOut, NULL));

	}

	//添加总分数标签
	auto Score_Lable_Txt = StringUtils::format("%d", score);
	auto Score_Label = Label::create(Score_Lable_Txt,"Buxton Sketch",60);
	Score_Label->setPosition(visibleSize.width / 2, visibleSize.height / 3);
	this->addChild(Score_Label, Label_Layer, Score_Label_Tag);
	Score_Label->setColor(Color3B::BLACK);

	auto GameRestart_Lable = Sprite::createWithSpriteFrameName("btn_finish.png");
	//当触摸重新开始游戏图标时，重新开始游戏
	auto menuItemRestart = MenuItemSprite::create(GameRestart_Lable, GameRestart_Lable, [=](Ref *)
	{
		Highest_Score_Lable->setVisible(false);
		Score_Label->setVisible(false);
		auto Game_Restart_Bg = Sprite::createWithSpriteFrameName("background.png");
		//Game_Restart_Bg->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
		Game_Restart_Bg->setPosition(visibleSize.width / 2, visibleSize.height / 2);
		this->addChild(Game_Restart_Bg,BackGround_Layer,Game_Restart_Bg_Tag);
		auto aniLoading = Animate::create(AnimationCache::getInstance()->getAnimation("aniLoading"));
		auto callFunc = CallFunc::create([=]()
		{
			//跳转场景的代码
			auto scene = ApathyVV :: CreateScene();
			Director::getInstance()->replaceScene(scene);
		});
		GameRestart_Lable->stopAllActions();
		GameRestart_Lable->runAction(Sequence::create(aniLoading,callFunc, NULL));
	});

	menuItemRestart->setPosition(visibleSize.width - GameRestart_Lable->getContentSize().width / 2 - 20, GameRestart_Lable->getContentSize().height / 2 + 20);
	auto menu = Menu::create();
	menu->addChild(menuItemRestart,Menu_Layer,menuItemRestart_Tag);
	//默认菜单的坐标中中央，这里不方便定位，所以置零，让菜单项自己定位
	menu->setPosition(Point::ZERO);
	this->addChild(menu,Button_Layer,Game_Over_Menu_Tag);
	return true;
}

Apathy_GameOverScene * Apathy_GameOverScene ::create(int score)
{
	auto *pEn = new Apathy_GameOverScene();
	if (pEn && pEn->init(score))
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