#pragma once
#include <iostream>
#include <vector>
#include "Framework.h"
#include "GlobalVariables.h"
class Object {
public:
	Object() {}
	Object(const char* new_path, double new_width, double new_height) : width(new_width), height(new_height) {
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

		if (y + height / 2 >= MAP_HEIGHT)
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
	double self_speed = 2;
	std::pair<double, double> impulse;
};