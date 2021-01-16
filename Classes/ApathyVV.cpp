#include "ApathyVV.h"
#include "Apathy_GameOverScene.h"

Scene* ApathyVV::CreateScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = ApathyVV::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

bool ApathyVV::init()
{
    
    // super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
	//初始化成员变量
	this->Total_Score = 0;
	this->M_DobuleBullet = 0;
	this->M_Bombs = 0;
	this->Hero_Inition_HP = 0;

	//导入plist
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("shoot_background.plist");//背景图片
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("shoot.plist");//飞机
	
	//创造英雄
	this->CreateHero(HERO1);

	//导入背景和分数、血量标签
	this->SetBackground();

	

	//固定频率产生子弹、道具和敌机
	this->schedule(schedule_selector(ApathyVV::CreateBullet), 0.5f);
	this->schedule(schedule_selector(ApathyVV::CreateProp), 10.0f);

	this->schedule(schedule_selector(ApathyVV::CreateSmallEnemy), 2.0f);
	this->schedule(schedule_selector(ApathyVV::CreateMiddleEnemy), 4.0f);
	this->schedule(schedule_selector(ApathyVV::CreateBigEnemy), 10.0f);

	//保证每一帧图片调用update
	this->scheduleUpdate();
    return true;
}

void ApathyVV::update(float dt)
{
	//找到背景图片
	auto bg1 = this->getChildByTag(BackGround1_Tag);
	auto bg2 = this->getChildByTag(BackGround2_Tag);

	//设置图片移动
	bg1->setPosition(bg1->getPosition().x,bg1->getPosition().y - 2);
	bg2->setPositionY(bg1->getPositionY() + bg1->getContentSize().height);
	if(bg2->getPositionY() < 0)
	{
		bg1->setPosition(0,0);
	}

	//设置英雄子弹移动
	Vector<Apathy_Items *> V_RemoveBullets;//溢出屏幕外的子弹集合
	for(auto bullet : this->M_Bullets)
	{
		bullet->setPositionY(bullet->getPositionY() + 3);
		if(bullet->getPositionY() > visibleSize.height + bullet->getContentSize().height / 2)
		{
			V_RemoveBullets.pushBack(bullet);
		}
	}

	//设置敌机移动
	Vector<Apathy_Enemy *> V_RemoveE;//溢出屏幕和被击毁的敌机集合

	for(auto currEnemy : this->M_Enemies)
	{
		currEnemy->setPositionY(currEnemy->getPositionY() - currEnemy->GetSpeed());
		if(currEnemy->getPositionY() < 0 - currEnemy->getContentSize().height / 2)
		{
			V_RemoveE.pushBack(currEnemy);
			this->removeChild(currEnemy);//从屏幕上移除
		}	
	}

	//设置敌机子弹移动
	Vector<Apathy_Items *> V_RemoveEnemyBullets;//溢出屏幕外的子弹集合
	for(auto EnemyBullet : this->M_EnemyBullets)
	{
		EnemyBullet->setPositionY(EnemyBullet->getPositionY() - EnemyBullet->GetSpeed());
		if(EnemyBullet->getPositionY() <= 0 - EnemyBullet->getContentSize().height / 2)
		{
			V_RemoveEnemyBullets.pushBack(EnemyBullet);
		}
	}

	//碰撞检测
		//敌机与英雄
	for(auto Hero : this->M_Heroes)
	{
		for(auto currEnemy : this->M_Enemies)
		{
			if(Hero->getBoundingBox().intersectsRect((currEnemy->getBoundingBox())))
			{
				V_RemoveE.pushBack(currEnemy);
				this->removeChild(currEnemy);
				this->GameOver(Hero);
			}
		}
	}
		//敌机子弹与英雄
	for(auto Hero : this->M_Heroes)
	{
		for(auto EnemyBullet : this->M_EnemyBullets)
		{
			if(Hero->getBoundingBox().intersectsRect((EnemyBullet->getBoundingBox())))
			{
				V_RemoveEnemyBullets.pushBack(EnemyBullet);
				this->removeChild(EnemyBullet);
				if(Hero->Hit())
				{
					this->GameOver(Hero);
				}
				//更新血量
				auto HP_Label = (Label *)this->getChildByTag(HP_Label_Tag);
				auto strHP = StringUtils::format("HP: %d", MAX(Hero->GetHP(),0));
				HP_Label ->setString(strHP);
			}
		}
	}
		//英雄子弹与敌机
	for(auto currEnemy : this->M_Enemies)
		for(auto bullet : this->M_Bullets)
		{
			if(currEnemy->getBoundingBox().intersectsRect(bullet->getBoundingBox()))
			{
				V_RemoveBullets.pushBack(bullet);
				this->removeChild(bullet);
				this->DisplayDamage(currEnemy);
				if(currEnemy->Hit())
				{
					//更新分数
					auto Score_Label = (Label *)this->getChildByTag(Score_Label_Tag);
					this->Total_Score += currEnemy->GetScore();
					auto strScore = StringUtils::format("Total_Score: %d", Total_Score);
					Score_Label ->setString(strScore);

					V_RemoveE.pushBack(currEnemy);
				}
				
			}
		}
		//英雄与道具
	Vector<Apathy_Items *> V_RemoveProps;
	for(auto Hero : this->M_Heroes)
	{
		for(auto Prop : this->M_Props)
		{
			if(Hero->getBoundingBox().intersectsRect((Prop->getBoundingBox())))
			{
				//Prop->removeFromParentAndCleanup(true);
				V_RemoveProps.pushBack(Prop);
				this->removeChild(Prop);
				switch(Prop->GetType())
				{
					case UFO1:
						Audio->playEffect("get_double_laser.mp3");
						this->M_DobuleBullet =  5;
						break;
					case UFO2:
						{
							Audio->playEffect("get_bomb.mp3");
							this->M_Bombs++;
							//更新炸弹数量
							auto Bomb_Label = (Label *)this->getChildByTag(Bomb_Count_Tag);
							auto strBomb = StringUtils::format("X%d", this->M_Bombs);
							Bomb_Label ->setString(strBomb);
							break;
						}
					default:
						break;
				}
				
			}
		}
	}

	//清除溢出屏幕外的子弹
	for(auto bullet : V_RemoveBullets)
	{
		this->M_Bullets.eraseObject(bullet);
		this->removeChild(bullet);
	}
	V_RemoveBullets.clear();

	//敌机子弹
	for(auto EnemyBullet : V_RemoveEnemyBullets)
	{
		this->M_EnemyBullets.eraseObject(EnemyBullet);
		this->removeChild(EnemyBullet);
	}
	V_RemoveEnemyBullets.clear();

	//处理溢出屏幕外的道具
	for(auto Prop : M_Props)
	{
		if(Prop->getPositionY() <= 0 - Prop->getContentSize().height / 2)
		{
			V_RemoveProps.pushBack(Prop);
			this->removeChild(Prop);
		}
	}
	for(auto Prop : V_RemoveProps)
	{
		M_Props.eraseObject(Prop);
	} 
	V_RemoveProps.clear();

	//清除溢出屏幕和被击毁的敌机
	for(auto Enemy : V_RemoveE)
	{
		this->M_Enemies.eraseObject(Enemy);
	}
	V_RemoveE.clear();
	this->UpdateBomb();
}

void ApathyVV::UpdateBomb()
{
	auto menu = this->getChildByTag(Menu_Tag);
	auto item = menu->getChildByTag(MenuItemBomb_Tag);
	auto Bomb__Count_Lable = (Label *)this->getChildByTag(Bomb_Count_Tag);
	if (this->M_Bombs <= 0)
	{
		item->setVisible(false);
		Bomb__Count_Lable->setVisible(false);
	} 
	else if (this->M_Bombs == 1)
	{
		item->setVisible(true);
		Bomb__Count_Lable->setVisible(false);
	} 
	else 
	{
		item->setVisible(true);
		Bomb__Count_Lable->setVisible(true);
		//更新炸弹数
		Bomb__Count_Lable->setString(StringUtils::format("X%d", this->M_Bombs));
	}
}

void ApathyVV::PauseAndResume(Ref * ref)
{
	Audio->playEffect("button.mp3");
	auto toggle = (MenuItemToggle *) ref;
	//通过Toggle菜单项的SelectedIndex可以判断现在切换到第几项，他的值从0开始
	if (toggle->getSelectedIndex() == 0) 
	{
		Director::getInstance()->resume();
	}
	else 
	{
		Director::getInstance()->pause();
	}

}

void ApathyVV::GameOver(Apathy_Hero * Hero)
{
	//切换音乐
	Audio->stopBackgroundMusic(true);
	Audio->stopAllEffects();
	Audio->playEffect("game_over.mp3");

	auto aniHeroDown = Animate::create(AnimationCache::getInstance()->getAnimation("aniHeroDown"));
	auto callFunc = CallFunc::create([=](){
		auto scene = Apathy_GameOverScene::CreateScene(this->Total_Score);
		Director::getInstance()->replaceScene(scene);
	});
	Hero->stopAllActions();
	Hero->runAction(Sequence::create(aniHeroDown,callFunc, NULL));
}

void ApathyVV::SetBackground()
{
	//导入背景音乐
	Audio->setBackgroundMusicVolume(0.5f);
	Audio->playBackgroundMusic("game_music.mp3", true);

	//创建精灵
	auto bg1 = Sprite::createWithSpriteFrameName("background.png");//背景图片1
	auto bg2 = Sprite::createWithSpriteFrameName("background.png");//背景图片2

	//设置锚点
	bg1->setAnchorPoint(Point::ZERO);
	bg2->setAnchorPoint(Point::ZERO);

	//两张背景图片位置
	bg1->setPosition(0,0);
	bg2->setPositionY(bg1->getContentSize().height);

	//抗锯齿
	bg1->getTexture()->setAliasTexParameters();
	bg2->getTexture()->setAliasTexParameters();


	// add the sprite as a child to this layer
	this->addChild(bg1, BackGround_Layer, BackGround1_Tag);
	this->addChild(bg2, BackGround_Layer, BackGround2_Tag);

	//添加分数标签
	auto Lable_Txt = StringUtils::format("Total_Score: %d", 0);
	auto Score_Label = Label::create(Lable_Txt,"Buxton Sketch",45);
	//auto Score_Label = Label::createWithBMFont("font.fnt",Lable_Txt);
	Score_Label->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
	Score_Label->setPosition(20, visibleSize.height-25);
	this->addChild(Score_Label,Label_Layer,Score_Label_Tag);
	//改变字体颜色
	Score_Label->setColor(Color3B::BLACK);

	//添加血量标签
	if(!(this->getChildByTag(Hero_Tag) == nullptr))
	{
		auto Lable_Txt1 = StringUtils::format("HP: %d",MAX(this->Hero_Inition_HP,0));
		auto HP_Label = Label::create(Lable_Txt1,"Buxton Sketch",45);
		//auto Score_Label = Label::createWithBMFont("font.fnt",Lable_Txt);
		HP_Label->setAnchorPoint(Point(1,0));
		HP_Label->setPosition(visibleSize.width-40,20);
		this->addChild(HP_Label,Label_Layer, HP_Label_Tag);
		//改变字体颜色
		HP_Label->setColor(Color3B::RED);
	}
			
	//添加炸弹标签和菜单
		//菜单
	auto Bomb_Lable = Sprite::createWithSpriteFrameName("bomb.png");
	//当触摸炸弹图标时，触发全屏飞机的爆炸并记分
	auto menuItemBomb = MenuItemSprite::create(Bomb_Lable, Bomb_Lable, [=](Ref *)
	{
		Audio->playEffect("use_bomb.mp3");
		this->M_Bombs--;
		this->UpdateBomb();
		//清空敌机集合
		for(auto Enemy : M_Enemies)
		{
			Enemy->Hit_Down();
			this->Total_Score = this->Total_Score + Enemy->GetScore();
		}
		this->M_Enemies.clear();
		//清空敌机子弹集合
		for(auto EnemyBullet : M_EnemyBullets)
		{
			this->removeChild(EnemyBullet);
		}
		this->M_EnemyBullets.clear();
		//更新分数标签
		auto strScore = StringUtils::format("Total_Score: %d", Total_Score);
		Score_Label ->setString(strScore);
	});
	menuItemBomb->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
	menuItemBomb->setPosition(40,40);
	auto menu = Menu::create();
	menu->addChild(menuItemBomb,UI_Layer,MenuItemBomb_Tag);
	//默认菜单的坐标中中央，这里不方便定位，所以置零，让菜单项自己定位
	menu->setPosition(Point::ZERO);
	this->addChild(menu,Menu_Layer,Menu_Tag);
		//标签
	auto Bomb_Count = Label::createWithBMFont("font.fnt", "X0");
	Bomb_Count->setPosition(menuItemBomb->getContentSize().width+45, 40);
	Bomb_Count->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
	Bomb_Count->setColor(Color3B::BLACK);
	this->addChild(Bomb_Count,Label_Layer,Bomb_Count_Tag);
	//设置为不可见
	menuItemBomb->setVisible(false);
	Bomb_Count->setVisible(false);

	//暂停标签
	auto Pause_Normal_Label = Sprite::createWithSpriteFrameName("game_pause_nor.png");
	auto Pause_Pressed_Label = Sprite::createWithSpriteFrameName("game_pause_pressed.png");
	auto menuItemPause = MenuItemSprite::create(Pause_Normal_Label, Pause_Pressed_Label);

	//继续
	auto Resume_Normal_Label = Sprite::createWithSpriteFrameName("game_resume_nor.png");
	auto Resume_Pressed_Label = Sprite::createWithSpriteFrameName("game_resume_pressed.png");
	auto menuItemResume = MenuItemSprite::create(Resume_Normal_Label, Resume_Pressed_Label);

	auto menuItemToggle = MenuItemToggle::createWithCallback(CC_CALLBACK_1(ApathyVV::PauseAndResume, this), menuItemPause, menuItemResume, nullptr);
	menuItemToggle->setPosition(visibleSize.width - menuItemPause->getContentSize().width,visibleSize.height - menuItemPause->getContentSize().height);

	menu->addChild(menuItemToggle,UI_Layer);
	//menu->setPosition(Point(;	//菜单默认定位在屏幕中央，这里定位在左下角，方便菜单项定位
}

void ApathyVV::CreateProp(float dt)
{
	Apathy_Items * Prop;
	int Type =  rand() % 2;
	if(Type == 0)
	{
		Prop = Apathy_Items :: Create(UFO1);
	}
	else
	{
		Prop = Apathy_Items :: Create(UFO2);
	}
	float minX = Prop->getContentSize().width / 2;
	float maxX = visibleSize.width - minX;
	float x = rand()%(int)((maxX - minX)) + minX;
	Prop->setPosition(x,visibleSize.height + Prop->getContentSize().height / 2);
	this->addChild(Prop,Item_Layer);
	this->M_Props.pushBack(Prop);
	Audio->playEffect("out_porp.mp3");
	Prop->Item_animation();
}

void ApathyVV::CreateBullet(float dt)
{
	//获得英雄
	auto Air = this->getChildByTag(Hero_Tag);
	//播放音效
	Audio->playEffect("bullet.mp3");
	//双子弹
	if(this->M_DobuleBullet > 0)
	{
		auto bullet1 = Apathy_Items :: Create(BULLET2);
		auto bullet2 = Apathy_Items :: Create(BULLET2);
		bullet1->setPosition(Air->getPositionX() - Air->getContentSize().width / 3,Air->getPositionY() + Air->getContentSize().height / 2);
		bullet2->setPosition(Air->getPositionX() + Air->getContentSize().width / 3,Air->getPositionY() + Air->getContentSize().height / 2);
		this->addChild(bullet1,Item_Layer);
		this->addChild(bullet2,Item_Layer);
		this->M_Bullets.pushBack(bullet1);
		this->M_Bullets.pushBack(bullet2);
		this->M_DobuleBullet--;
	}
	//单子弹
	else
	{
		auto bullet = Apathy_Items :: Create(BULLET1);
		bullet->setPosition(Air->getPositionX(),Air->getPositionY() + Air->getContentSize().height / 2);
		this->addChild(bullet,Item_Layer);
		this->M_Bullets.pushBack(bullet);
	}
	//敌机子弹
	for(auto currEnemy : M_Enemies)
	{
		this->CreateEnemyBullet(currEnemy);
	}
}

void ApathyVV::CreateEnemyBullet(Apathy_Enemy * currEnemy)
{
	auto bullet = Apathy_Items :: Create(BULLET1);
	bullet->setPosition(currEnemy->getPositionX(),currEnemy->getPositionY() - currEnemy->getContentSize().height / 2);
	bullet->SetSpeed(currEnemy->GetSpeed() + 2);
	this->addChild(bullet,Item_Layer);
	this->M_EnemyBullets.pushBack(bullet);
}

void ApathyVV::CreateHero(HeroType)
{
	auto Hero = Apathy_Hero :: Create(HERO1);
	Hero->setPosition(visibleSize.width/2,Hero->getContentSize().height);
	this->addChild(Hero,Airplane_Layer,Hero_Tag);
	M_Heroes.pushBack(Hero);
	this->Hero_Inition_HP = Hero->GetHP();
	Hero->Move();
}

void ApathyVV::CreateEnemy(EnemyType type)
{
	//创建敌机
	auto currEnemy = Apathy_Enemy :: Create(type);
	float minX = currEnemy->getContentSize().width / 2;
	float maxX = visibleSize.width - minX;
	float x = rand()%(int)((maxX - minX)) + minX;
	currEnemy->setPosition(x,visibleSize.height + currEnemy->getContentSize().height / 2);
	this->addChild(currEnemy,Airplane_Layer);
	this->M_Enemies.pushBack(currEnemy);
}

void ApathyVV::CreateSmallEnemy(float dt)
{
	this->CreateEnemy(SMALL_ENEMY);
}

void ApathyVV::CreateMiddleEnemy(float dt)
{
	this->CreateEnemy(MIDDLE_ENEMY);
}

void ApathyVV::CreateBigEnemy(float dt)
{
	this->CreateEnemy(BIG_ENEMY);
}

void ApathyVV :: DisplayDamage(Apathy_Enemy * currEnemy)
{
	auto Damage_Label = Label :: create("-1","Microsoft YaHei UI",35);
	Damage_Label->setColor(Color3B::RED);
	Damage_Label->setPosition(Point(currEnemy->getPosition()));
	this->addChild(Damage_Label,Label_Layer + 1 );
	//auto move1 = MoveBy :: create(0.5f,Point(currEnemy->getPosition()));
	auto move2 = MoveTo :: create(1,Point(Damage_Label->getPositionX(),currEnemy->getPositionY() - currEnemy->getContentSize().height / 2 - currEnemy->GetSpeed()));
	auto sequence = Sequence::create(move2, RemoveSelf::create(), nullptr);
	Damage_Label->runAction(sequence);
}