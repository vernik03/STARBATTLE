﻿#include "Framework.h"
#include <random>
#include <iostream>
#include <vector>
#include <iterator>

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;

class Object {
public:
	Object(){}
	Object(const char* new_path, double new_width, double new_height) : width(new_width), height(new_height){
		object = createSprite(new_path);
		y = 0;
		x = 0;
		y_speed = 0;
		x_speed = 0;
	}
	void Draw() {
		drawSprite(object, x, y);
	}
	void Move() {
		y += y_speed;
		x += x_speed;
	}
	std::pair<double, double> GetCoords() {
		return std::make_pair(x, y);
	}
	std::pair<double, double> GetSize() {
		return std::make_pair(width, height);
	}
	double GetRadius() {
		return (height + width) / 4.0;
	}
	std::pair<double, double> GetCenter() {
		return std::make_pair(x + width / 2, y + height / 2);
	}
	std::pair<double, double> GetSpeed() {
		return std::make_pair(y_speed, x_speed);
	}
	void SetSpeed(std::pair<double, double> speed) {
		x_speed = speed.first;
		y_speed = speed.second;
	}
protected:
	const char* path;
	Sprite* object;
	double y;
	double x;
	double width;
	double height;
	double y_speed;
	double x_speed;	
};

class Asteroid : public Object{
public:
	Asteroid(){}
	void AsteroidSingleInit(int speed, const char* new_path) {
		object = createSprite(new_path);
		x = rand() % WINDOW_WIDTH;
		y = rand() % WINDOW_HEIGHT;
		y_speed = ((rand() % speed + 2) * pow(-1, rand() % 3)) / 10;
		x_speed = ((rand() % speed + 2) * pow(-1, rand() % 3)) / 10;		
	}
	void Move() {
		Collision();
		y += y_speed;
		x += x_speed;
	}
	void Collision() {
		if (x + width >= WINDOW_WIDTH || x <= 0)
		{
			x_speed *= -1;
		}
		if (y + height >= WINDOW_HEIGHT || y <= 0)
		{
			y_speed *= -1;
		}
	}
	void EachOtherCollision(std::vector<Asteroid*> asteroids){
		for (auto asteroids_elem : asteroids)
		{
			if (asteroids_elem == this)
			{				
				continue;
			}
			else if (GetRadius() + asteroids_elem->GetRadius()
				>= sqrt(pow(GetCenter().first - asteroids_elem->GetCenter().first, 2)
					  + pow(GetCenter().second - asteroids_elem->GetCenter().second, 2)))
			{	
			
				double d = sqrt(pow(GetCenter().first - asteroids_elem->GetCenter().first, 2)
					          + pow(GetCenter().second - asteroids_elem->GetCenter().second, 2));
				double ny = (asteroids_elem->GetCenter().second - GetCenter().second) / d;
				double nx = (asteroids_elem->GetCenter().first - GetCenter().first) / d;
				double p = 2 * (y_speed * ny + x_speed * nx - asteroids_elem->GetSpeed().second * ny - asteroids_elem->GetSpeed().first * nx) /
					(mass + asteroids_elem->GetMass());
				y_speed = y_speed - p * mass * ny;
				x_speed = x_speed - p * mass * nx;
				double vy2 = asteroids_elem->GetSpeed().second + p * asteroids_elem->GetMass() * ny;
				double vx2 = asteroids_elem->GetSpeed().first + p * asteroids_elem->GetMass() * nx;
				asteroids_elem->SetSpeed(std::make_pair(vx2, vy2));
			}
		}
	}
	double GetMass() {
		return mass;
	}
protected:
	int mass;
};

class BigAsteroid : public Asteroid
{
public:
	BigAsteroid(int speed, const char* new_path = "data/big_asteroid.png") {
		width = 68;
		height = 60;		
		mass = 10;
		AsteroidSingleInit(speed, new_path);
	}
};

struct SmallAsteroid : public Asteroid
{
public:
	SmallAsteroid(int speed, const char* new_path = "data/big_asteroid.png") {
		width = 44;
		height = 36;
		mass = 6;
		AsteroidSingleInit(speed, new_path);
	}
	
};

class Character : public Object
{
public:
	Character(const char* new_path = "data/spaceship.png", double speed = 2, double shield_duration_in = 30, double new_width = 48, double new_height = 45)
			: shield_duration(shield_duration_in), self_speed(speed) {
		object = createSprite(new_path);
		width = new_width;
		height = new_height;
		y = WINDOW_HEIGHT / 2 - height /2;
		x = WINDOW_WIDTH / 2 - width / 2;
		shield = createSprite("data/shield.png");
		shield_02 = createSprite("data/shield_02.png");
		shield_03 = createSprite("data/shield_03.png");
	}
	void Move(FRKey k) {
		switch (k)
		{
		case FRKey::RIGHT:
			impulse.first = self_speed;
			break;
		case FRKey::LEFT:
			impulse.first = -1*self_speed;
			break;
		case FRKey::DOWN:
			impulse.second = self_speed;
			break;
		case FRKey::UP:
			impulse.second = -1*self_speed;
			break;
		default:
			break;
		}
	}
	void MoveCheck() {
		double control_num = 0.01;
		x += impulse.first;
		y += impulse.second;
		impulse.first /= 1.01;
		impulse.second /= 1.01;
		if (abs(impulse.first)< control_num)
		{
			impulse.first = 0;
		}
		if (abs(impulse.second) < control_num)
		{
			impulse.second = 0;
		}

	}
	void Shield() {
		shield_time = shield_duration;
		std::cout << shield_time;
	}
	void Draw() {
		drawSprite(object, x, y);
		if (shield_time > 0.1)
		{
			if (shield_time < shield_duration / 4)
			{
				drawSprite(shield_03, x + width / 2.0 - 81.0 / 2 - 1.3, y + height / 2.0 - 81.0 / 2);
			}
			else if (shield_time < shield_duration / 2)
			{
				drawSprite(shield_02, x + width / 2.0 - 81.0 / 2 - 1.3, y + height / 2.0 - 81.0 / 2);
			}
			else
			{
				drawSprite(shield, x + width / 2.0 - 81.0 / 2 - 1.3, y + height / 2.0 - 81.0 / 2);	
			}
			shield_time -= 0.1;
		}

	}
private:
	Sprite* shield;
	Sprite* shield_02;
	Sprite* shield_03;
	double shield_duration;
	double shield_time;
	double self_speed;
	std::pair<double, double> impulse;
};


class Bullet : public Object
{
public:
	Bullet(int x_start, int y_start, int x_aim, int y_aim, const char* new_path = "data/bullet.png", double bullet_speed = 5, double new_width = 13, double new_height = 12) {
		object = createSprite(new_path);
		width = new_width;
		height = new_height;
		y = y_start - (height / 2);								
		x = x_start - (width / 2);
		y_speed = (y_aim + (height / 2)) - y_start;
		x_speed = (x_aim + (width / 2)) - x_start;
		double param = abs(y_speed) + abs(x_speed);
		param = bullet_speed / param;
		y_speed *= param;
		x_speed *= param;
	}
private:
	double bullet_speed;
};

class Reticle : public Object
{
public:
	Reticle(const char* new_path = "data/reticle.png", double bullet_reload_duration = 5, double new_width = 35, double new_height = 34) : reload_duration(bullet_reload_duration){
		object = createSprite(new_path);
		width = new_width;
		height = new_height;
		y = 0;
		x = 0;
	}
	void SetPos(int x_mouse, int y_mouse) {
		y = y_mouse - height / 2;
		x = x_mouse - width / 2;
	}
	void Shoot(int x_ship, int y_ship) {
		if (reload_time < 0.1)
		{
			bullets.push_back(new Bullet(x_ship, y_ship, x, y));
			reload_time = reload_duration;
		}				
	}
	void Draw() {
		drawSprite(object, x, y);
		for (auto bullet : bullets)
		{
			bullet->Draw();
			bullet->Move();
		}
		reload_time -= 0.2;		
	}

	bool CheckShoot(Asteroid* asteroid) {
		std::vector<Bullet*>::iterator it_b;
		it_b = bullets.begin();		
		while (it_b != bullets.end()) 
		{
			if ((*it_b)->GetRadius() + asteroid->GetRadius()
				>= sqrt(pow((*it_b)->GetCenter().first - asteroid->GetCenter().first, 2)
					+ pow((*it_b)->GetCenter().second - asteroid->GetCenter().second, 2)))
			{	
				bullets.erase(it_b);
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
};



/* Test Framework realization */
class MyFramework : public Framework {

public:

	virtual void PreInit(int& width, int& height, bool& fullscreen)
	{
		width = WINDOW_WIDTH;
		height = WINDOW_HEIGHT;
		fullscreen = false;
	}

	virtual bool Init() {
		for (int i = 0; i < 5; i++)
		{
			asteroids.push_back(new BigAsteroid((rand()%20+10)));
		}
		battleship = new Character();
		reticle = new Reticle();
		//a1 = new BigAsteroid(WINDOW_WIDTH, WINDOW_HEIGHT, 20);
		return true;
	}

	virtual void Close() {

	}

	virtual bool Tick() {
		drawTestBackground();
		for (auto asteroid : asteroids)
		{
			asteroid->EachOtherCollision(asteroids);
			asteroid->Move();
			asteroid->Draw();
			/*if (reticle->CheckShoot(*it_a))
			{
				asteroids.erase(it_a);
			}*/
			
			//it_a++;		
		}
		/*remove_if(asteroids.begin(), asteroids.end(), [](Asteroid* a, Reticle* reticle) {
			return reticle->CheckShoot(a); });*/
		
		/*asteroids.erase(remove_if(asteroids.begin(), asteroids.end(), [this](Asteroid* a) -> bool {
			return reticle->CheckShoot(a); }));*/
		/*remove_if(asteroids.begin(), asteroids.end(), [this](Asteroid* a) -> bool {
			return reticle->CheckShoot(a); });*/
		std::erase_if(asteroids, [this](Asteroid* a) -> bool {
			return reticle->CheckShoot(a); });
		battleship->Draw();
		battleship->MoveCheck();
		reticle->Draw();
		return false;
	}

	virtual void onMouseMove(int x, int y, int xrelative, int yrelative) {
		reticle->SetPos(x,y);
	}

	virtual void onMouseButtonClick(FRMouseButton button, bool isReleased) {
		switch (button)
		{
		case FRMouseButton::LEFT:
			reticle->Shoot(battleship->GetCoords().first+battleship->GetSize().second/2, battleship->GetCoords().second + battleship->GetSize().first/2);
			break;
		case FRMouseButton::MIDDLE:
			break;
		case FRMouseButton::RIGHT:
			battleship->Shield();
			break;
		case FRMouseButton::COUNT:
			break;
		default:
			break;
		}
	}

	virtual void onKeyPressed(FRKey k) {
		battleship->Move(k);
	}

	virtual void onKeyReleased(FRKey k) {
	}

	virtual const char* GetTitle() override
	{
		return "asteroids";
	}
private:
	//Asteroid* a1;
	Character* battleship;
	std::vector<Asteroid*> asteroids;
	Reticle* reticle;
	int WINDOW_WIDTH = 800;
	int WINDOW_HEIGHT = 600;

	 void asteroidsSpawn(int count, const char* path) {
		for (int i = 0; i < count; i++)
		{
			drawSprite(createSprite(path), rand() % WINDOW_WIDTH, rand() % WINDOW_HEIGHT);
		}
	}
};

int main(int argc, char* argv[])
{
	return run(new MyFramework);
}