#pragma once
#include "Object.h"
class Asteroid : public Object {
public:
	Asteroid() {}
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

	void EachOtherCollision(std::vector<Asteroid*> asteroids) {
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

