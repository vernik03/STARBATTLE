#include "Framework.h"
#include <random>
#include <iostream>
#include <vector>
#include <iterator>
#include <string>
#include <iterator>
#include <regex>
#include <sstream>

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;
int MAP_WIDTH = 1000;
int MAP_HEIGHT = 1000;
int NUM_ASTEROIDS = 10;
int NUM_AMMO = 3;
double ABILITY_PROBABILITY = 0.3;

enum class Ability {
	SHIELD,
	ROCKET,
	AUTO_SHOOT
};

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

	void MoveManual(FRKey k) {
		switch (k)
		{
		case FRKey::RIGHT:
			impulse.first = -1 * self_speed;
			break;
		case FRKey::LEFT:
			impulse.first = self_speed;
			break;
		case FRKey::DOWN:
			impulse.second = -1 * self_speed;
			break;
		case FRKey::UP:
			impulse.second = self_speed;
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
		if (abs(impulse.first) < control_num)
		{
			impulse.first = 0;
		}
		if (abs(impulse.second) < control_num)
		{
			impulse.second = 0;
		}
		Border();
	}

	void Border() {
		
		if (x + width / 2 >= MAP_WIDTH)
		{
			x = -1 * (width / 2);
		}
		else if (x < (width / 2) * -1)
		{
			x = MAP_WIDTH - width / 2;
		}

		if (y + height/2 >= MAP_HEIGHT)
		{			
			y = -1 * (height / 2);
		}
		else if (y < (height / 2) * -1)
		{
			y = MAP_HEIGHT - height / 2;
		}

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
	double self_speed=2;
	std::pair<double, double> impulse;
};

class Asteroid : public Object{
public:
	Asteroid(){}
	void AsteroidSingleInit(std::vector<Asteroid*> asteroids, int speed, const char* new_path, bool is_rand, int x_speed_new = 0) {
		object = createSprite(new_path);
		if (is_rand)
		{
			x = WINDOW_WIDTH / 2;
			y = WINDOW_HEIGHT / 2;
			while (x > WINDOW_WIDTH / 2 - 150 && x < WINDOW_WIDTH / 2 + 150 
				&& y > WINDOW_HEIGHT / 2 - 150 && y < WINDOW_HEIGHT / 2 + 150)
			{
				x = rand() % int((WINDOW_WIDTH - width));
				y = rand() % int((WINDOW_HEIGHT - height));
			}
			y_speed = ((rand() % speed + 2) * pow(-1, rand() % 3)) / 10;
			x_speed = ((rand() % speed + 2) * pow(-1, rand() % 3)) / 10;
			for (auto asteroids_elem : asteroids)
			{
				while ((GetRadius() + asteroids_elem->GetRadius()
					>= sqrt(pow(GetCenter().first - asteroids_elem->GetCenter().first, 2)
						+ pow(GetCenter().second - asteroids_elem->GetCenter().second, 2)))
					|| (x > WINDOW_WIDTH / 2 - 150 && x < WINDOW_WIDTH / 2 + 150
							&& y > WINDOW_HEIGHT / 2 - 150 && y < WINDOW_HEIGHT / 2 + 150))
				{
					x = rand() % int((WINDOW_WIDTH - width));
					y = rand() % int((WINDOW_HEIGHT - height));
				}
			}
		}	
		else
		{
			y_speed = x_speed_new;
			x_speed = 0;
		}
		
	}
	void Move() {
		y += y_speed;
		x += x_speed;

		if (x_speed > max_speed)
		{
			x_speed = max_speed - 1;

		}
		if (y_speed > max_speed)
		{
			y_speed = max_speed - 1;

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
	std::string GetType() {
		return type;
	}
protected:
	int mass;
	std::string type;
	double max_speed;
};

class BigAsteroid : public Asteroid
{
public:
	BigAsteroid(std::vector<Asteroid*> asteroids, int speed, bool is_rand = true, int x_new = 0, int y_new = 0) {
		width = 68;
		height = 60;		
		mass = 10;
		x = x_new;
		y = y_new;
		type = "big";
		max_speed = 3;
		AsteroidSingleInit(asteroids, speed, "data/big_asteroid.png", is_rand);
	}
};

struct SmallAsteroid : public Asteroid
{
public:
	SmallAsteroid(std::vector<Asteroid*> asteroids, int speed, int x_speed_new = 0, bool is_rand = true, int x_new = 0, int y_new = 0) {
		width = 44;
		height = 36;
		mass = 6;
		x = x_new;
		y = y_new;
		type = "small";
		max_speed = 4;
		AsteroidSingleInit(asteroids, speed, "data/small_asteroid.png", is_rand, x_speed_new);
	}
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
	Shield(bool is_static = true, double x_new = 30, double y_new = 30) {
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
			: shield_duration(shield_duration_in){
		object = createSprite("data/spaceship.png");
		self_speed = speed;
		width = 48;
		height = 45;
		y = WINDOW_HEIGHT / 2 - height /2;
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
	void Draw() {
		if (abs(impulse.first) > 0.2 || abs(impulse.second) > 0.2)
		{
			drawSprite(power, x, y);
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
	Reticle(double bullet_reload_duration = 30) : reload_duration(bullet_reload_duration){
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
		if (ammo_count >= 0 && ammo_count < NUM_AMMO-1)
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
		background = createSprite("data/background_2.png");

		for (int i = 0; i < NUM_ASTEROIDS; i++)
		{
			if (i < NUM_ASTEROIDS / 2)
			{
				asteroids.push_back(new BigAsteroid(asteroids, (rand() % 5 + 1)));
			}
			else
			{
				asteroids.push_back(new SmallAsteroid(asteroids, (rand() % 5 + 1)));
			}
		}
		battleship = new Character();
		reticle = new Reticle();
		return true;
	}

	virtual void Close() {

	}

	virtual bool Tick() {
		showCursor(false);
		for (int i = 0; i < WINDOW_WIDTH; i += 1000)
		{
			for (int j = 0; j < WINDOW_HEIGHT; j += 1000)
			{
				drawSprite(background, i-100, j-100);
			}
		}

		std::vector<Asteroid*> asteroids_temp;
		for (auto asteroid : asteroids)
		{
			asteroid->EachOtherCollision(asteroids);
			asteroid->Move();	
			asteroid->Draw();
			if (battleship->CheckCollision(asteroid))
			{
				std::cout << "Game over!" << "\n";
				return 1;
			}
			if (reticle->CheckShoot(asteroid, false) && asteroid->GetType() == "big")
			{
				asteroids_temp.push_back(new SmallAsteroid(asteroids, 0, -1.5, false, asteroid->GetCoords().first + 12, asteroid->GetCoords().second - 10));
				asteroids_temp.push_back(new SmallAsteroid(asteroids, 0, 1.5, false, asteroid->GetCoords().first + 12, asteroid->GetCoords().second + 35));
			}
			if (reticle->CheckShoot(asteroid, false) && asteroid->GetType() == "small")
			{
				if (static_cast<double>(rand()) / RAND_MAX >= 1 - ABILITY_PROBABILITY)
				{
					icons.push_back(new Shield(false, asteroid->GetCoords().first, asteroid->GetCoords().second));					
				}				
			}
		}
		std::erase_if(asteroids, [this](Asteroid* a) -> bool {
			return reticle->CheckShoot(a); });
		for (auto asteroid : asteroids_temp)
		{			
			asteroids.push_back(asteroid);
		}
		for (auto icon : icons)
		{
			icon->Draw();
			icon->MoveCheck();
		}
		std::erase_if(icons, [this](Icon* i) -> bool {
			return battleship->CheckCollisionIcon(i); });
		for (auto asteroid : asteroids)
		{
			asteroid->MoveCheck();
		}	
		
		battleship->Draw();
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
			battleship->AbilityInit();
			break;
		case FRMouseButton::COUNT:
			break;
		default:
			break;
		}
	}

	virtual void onKeyPressed(FRKey k) {
		for (auto asteroid : asteroids)
		{
			asteroid->MoveManual(k);
		}
		for (auto icon : icons)
		{
			icon->MoveManual(k);
		}
		battleship->MoveManual(k);
	}

	virtual void onKeyReleased(FRKey k) {
	}

	virtual const char* GetTitle() override
	{
		return "asteroids";
	}
private:
	//Asteroid* a1;
	Sprite* background;
	Character* battleship;
	std::vector<Asteroid*> asteroids;
	std::vector<Icon*> icons;

	Reticle* reticle;

	 void asteroidsSpawn(int count, const char* path) {
		for (int i = 0; i < count; i++)
		{
			drawSprite(createSprite(path), rand() % WINDOW_WIDTH, rand() % WINDOW_HEIGHT);
		}
	}
};

int main(int argc, char** argv)
{
	for (int i = 0; i < argc-1; i++)
	{
		if (strcmp(argv[i], "-window") == 0)
		{
			std::string str = std::string(argv[i+1]);
			std::istringstream iss(argv[i]);
			static const std::regex rdelim{ "x" };
			std::vector<std::string> strPairs{
					std::sregex_token_iterator(str.begin(), str.end(), rdelim, -1),
					std::sregex_token_iterator()
			};
			WINDOW_WIDTH = stoi(strPairs[0]);
			WINDOW_HEIGHT = stoi(strPairs[1]);
		}
		else if (strcmp(argv[i], "-map") == 0)
		{
			std::string str = std::string(argv[i + 1]);
			std::istringstream iss(argv[i]);
			static const std::regex rdelim{ "x" };
			std::vector<std::string> strPairs{
					std::sregex_token_iterator(str.begin(), str.end(), rdelim, -1),
					std::sregex_token_iterator()
			};
			MAP_WIDTH = stoi(strPairs[0]);
			MAP_HEIGHT = stoi(strPairs[1]);
		}
		else if (strcmp(argv[i], "-num_asteroids") == 0)
		{
			NUM_ASTEROIDS = atoi(argv[i + 1]);
		}
		else if (strcmp(argv[i], "-num_ammo") == 0)
		{
			NUM_AMMO = atoi(argv[i + 1]);
		}
		else if (strcmp(argv[i], "-ability_probability") == 0)
		{
			ABILITY_PROBABILITY = std::stod(argv[i + 1]);
		}
	}
	
	srand(time(NULL));
	while (true)
	{
		run(new MyFramework);
	}
	return 0;
}

//game.exe -window 800x600 -map 1000x1000 -num_asteroids 10 -num_ammo 3 -ability_probability 0.3
