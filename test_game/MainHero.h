#pragma once
#include "Object.h"
#include "Asteroids.h"


enum class Ability {
	SHIELD,
	ROCKET,
	AUTO_SHOOT
};

class Icon : public Object
{
public:
	Icon(bool is_static = true, double x_new = 30, double y_new = 30) {
		Init(x_new, y_new);
	}
	void Init(double x_new, double y_new) {
		x = x_new;
		y = y_new;
	}
	Ability GetAbility() {
		return ability;
	}
protected:
	Ability ability;
};

class Shield : public Icon
{
public:
	Shield(bool is_static = true, double x_new = 30, double y_new = 30);
};

class Rocket : public Icon
{
public:
	Rocket(bool is_static = true, double x_new = 30, double y_new = 30) {
		if (is_static)
		{
			object = createSprite("data/rocket_icon.png");
		}
		else {
			object = createSprite("data/rocket_icon_small.png");
		}
		width = 63;
		height = 63;
		ability = Ability::ROCKET;
		Init(x_new, y_new);
	}
};

class AutoShoot : public Icon
{
public:
	AutoShoot(bool is_static = true, double x_new = 30, double y_new = 30) {
		if (is_static)
		{
			object = createSprite("data/autoshoot_icon.png");
		}
		else {
			object = createSprite("data/autoshoot_icon_small.png");
		}
		width = 63;
		height = 63;
		ability = Ability::AUTO_SHOOT;
		Init(x_new, y_new);
	}
};

class Character : public Object
{
public:
	Character(double speed = 2, double shield_duration_in = 80)
		: shield_duration(shield_duration_in) {
		object = createSprite("data/spaceship.png");
		self_speed = speed;
		width = 48;
		height = 48;
		y = WINDOW_HEIGHT / 2 - height / 2;
		x = WINDOW_WIDTH / 2 - width / 2;
		shield = createSprite("data/shield.png");
		shield_02 = createSprite("data/shield_02.png");
		shield_03 = createSprite("data/shield_03.png");
		power = createSprite("data/spaceship_power.png");
	}
	void AbilityInit() {
		if (abitity)
		{
			shield_time = shield_duration;
			abitity = NULL;
		}
	}

	void SetRot(int x_mouse, int y_mouse) {
		if (x_mouse <= WINDOW_WIDTH / 2)
		{
			if (abs(x_mouse- WINDOW_WIDTH / 2) > abs(y_mouse- WINDOW_WIDTH / 2))
			{
				rotation = 2;
			}
			else
			{
				if (y_mouse <= WINDOW_HEIGHT / 2)
				{
					rotation = 1;
				}
				else
				{
					rotation = 3;
				}
			}
		}
		else
		{
			if (abs(x_mouse- WINDOW_WIDTH / 2) > abs(y_mouse- WINDOW_WIDTH / 2))
			{
				rotation = 4;
			}
			else
			{
				if (y_mouse <= WINDOW_HEIGHT / 2)
				{
					rotation = 1;
				}
				else
				{
					rotation = 3;
				}
			}
		}
	}

	void SetRotSprite() {
		switch (rotation)
		{
		case 1:
		object = createSprite("data/spaceship.png");
		power = createSprite("data/spaceship_power.png");

			break;
		case 2:
			object = createSprite("data/spaceship_l.png");
			power = createSprite("data/spaceship_power_l.png");

			break;
		case 3:
			object = createSprite("data/spaceship_b.png");
			power = createSprite("data/spaceship_power_b.png");

			break;
		case 4:
			object = createSprite("data/spaceship_r.png");
			power = createSprite("data/spaceship_power_r.png");
			break;
		default:
			break;
		}
	}

	void Draw() {
		SetRotSprite();
		if (abs(impulse.first) > 0.2 || abs(impulse.second) > 0.2)
		{
			drawSprite(power, x-29, y-29);
		}
		drawSprite(object, x, y);
		if (shield_time > 0.1)
		{
			if (shield_time < shield_duration / 4)
			{
				drawSprite(shield_03, x + width / 2.0 - shield_size / 2 - 1.3, y + height / 2.0 - shield_size / 2);
			}
			else if (shield_time < shield_duration / 2)
			{
				drawSprite(shield_02, x + width / 2.0 - shield_size / 2 - 1.3, y + height / 2.0 - shield_size / 2);
			}
			else
			{
				drawSprite(shield, x + width / 2.0 - shield_size / 2 - 1.3, y + height / 2.0 - shield_size / 2);
			}
			shield_time -= 0.1;
		}
		MoveCheck();
		if (abitity)
		{
			abitity->Draw();
		}
	}
	bool CheckCollision(Asteroid* asteroid) {

		if (shield_time < 0.1)
		{
			if (GetRadius() + asteroid->GetRadius()
				>= sqrt(pow(GetCenter().first - asteroid->GetCenter().first, 2)
					+ pow(GetCenter().second - asteroid->GetCenter().second, 2)))
			{
				return true;
			}
		}
		else if (shield_time > 0.1) {
			if (shield_size / 2 + asteroid->GetRadius()
				>= sqrt(pow(GetCenter().first - asteroid->GetCenter().first, 2)
					+ pow(GetCenter().second - asteroid->GetCenter().second, 2))) {
				double a_speed_x = asteroid->GetSpeed().first * -1;
				double a_speed_y = asteroid->GetSpeed().second * -1;

				asteroid->SetSpeed(std::make_pair(a_speed_x, a_speed_y));
			}
		}
		return false;
	}

	bool CheckCollisionIcon(Icon* icon) {
		if (GetRadius() + 20
			>= sqrt(pow(GetCenter().first - icon->GetCenter().first, 2)
				+ pow(GetCenter().second - icon->GetCenter().second, 2))) {
			abitity = new Shield();
			return true;
		}
		return false;
	}
	void MoveCheck() {
		impulse.first /= 1.01;
		impulse.second /= 1.01;
	}
private:
	Sprite* power;
	Sprite* shield;
	Sprite* shield_02;
	Sprite* shield_03;
	double shield_duration;
	double shield_time;
	double mass = 100;
	double shield_size = 81;
	Icon* abitity;
	int rotation;
};

class Bullet : public Object
{
public:
	Bullet(int x_start, int y_start, int x_aim, int y_aim, double bullet_speed = 4) {
		object = createSprite("data/bullet.png");
		width = 13;
		height = 13;
		y = y_start - (height / 2);
		x = x_start - (width / 2);
		y_speed = y_aim - y_start;
		x_speed = x_aim - x_start;
		double param = abs(y_speed) + abs(x_speed);
		param = bullet_speed / param;
		y_speed *= param;
		x_speed *= param;
	}
	bool Border() {
		if (x + width / 2 >= MAP_WIDTH)
		{
			return true;
		}
		else if (x < (width / 2) * -1)
		{
			return true;

		}
		if (y + height / 2 >= MAP_HEIGHT)
		{
			return true;

		}
		else if (y < (height / 2) * -1)
		{
			return true;

		}
		return false;
	};
private:
	double bullet_speed;
};

class Reticle : public Object
{
public:
	Reticle(double bullet_reload_duration = 30) : reload_duration(bullet_reload_duration) {
		object = createSprite("data/reticle2.png");
		width = 65;
		height = 64;
		y = 0;
		x = 0;
		ammo_count = NUM_AMMO;
		time = 0;
	}
	void SetPos(int x_mouse, int y_mouse) {
		y = y_mouse - height / 2;
		x = x_mouse - width / 2;
	}
	void Shoot(int in_x_ship, int in_y_ship) {
		if (reload_time < 0.1)
		{
			x_ship = in_x_ship;
			y_ship = in_y_ship;
			bullets.push_back(new Bullet(x_ship, y_ship, GetCenter().first, GetCenter().second));
			x_center_temp = GetCenter().first;
			y_center_temp = GetCenter().second;
			time = getTickCount();
			reload_time = reload_duration;
			ammo_count = 0;
		}
	}
	bool Disappear(Bullet* b) {
		return b->Border();
	}
	void Draw() {
		drawSprite(object, x, y);
		for (auto bullet : bullets)
		{
			bullet->MoveCheck();
			bullet->Draw();
			bullet->Move();
		}
		std::erase_if(bullets, [this](Bullet* b) -> bool {
			return Disappear(b); });
		reload_time -= 0.2;
		if (ammo_count >= 0 && ammo_count < NUM_AMMO - 1)
		{
			Ammo();
		}
	}
	void Ammo() {
		if (getTickCount() - time > 50)
		{
			std::cout << "!";
			bullets.push_back(new Bullet(x_ship, y_ship, GetCenter().first, GetCenter().second));
			time = getTickCount();
			ammo_count++;
		}

	}

	bool CheckShoot(Asteroid* asteroid, bool is_delete = true) {
		std::vector<Bullet*>::iterator it_b;
		it_b = bullets.begin();
		while (it_b != bullets.end())
		{
			if ((*it_b)->GetRadius() + asteroid->GetRadius()
				>= sqrt(pow((*it_b)->GetCenter().first - asteroid->GetCenter().first, 2)
					+ pow((*it_b)->GetCenter().second - asteroid->GetCenter().second, 2)))
			{
				if (is_delete)
				{
					bullets.erase(it_b);
				}
				return true;
			}
			it_b++;
		}
		return false;
	}

private:
	std::vector<Bullet*> bullets;
	double bullet_speed;
	const char* bullet_path;
	double reload_time;
	double reload_duration;
	int ammo_count;
	double x_ship;
	double y_ship;
	double x_center_temp;
	double y_center_temp;
	int time;
};