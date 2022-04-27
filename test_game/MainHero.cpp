#pragma once
#include "MainHero.h"
Shield::Shield(bool is_static, double x_new, double y_new) {
	if (is_static)
	{
		object = createSprite("data/shield_icon.png");
	}
	else {
		object = createSprite("data/shield_icon_small.png");
	}
	width = 63;
	height = 63;
	ability = Ability::SHIELD;
	Init(x_new, y_new);
}