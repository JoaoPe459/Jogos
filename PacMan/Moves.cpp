#include "Moves.h"
#include <cmath>

void Moves::Stop()
{
	this->velX = 0;
	this->velY = 0;
}

void Moves::applyGravity(float dt)
{
	if (this->ghostMode)
		return;
	const float g = 800.0f;

	this->velY += g * dt;
}

void Moves::Up()
{
	this->velY = -this->getSpeed()*3;
}

void Moves::Down()
{
	this->velY =  this->getSpeed();
}

void Moves::Left()
{
	this->velX = -this->getSpeed();
}

void Moves::Right()
{
	velX = this->getSpeed();
}

float Moves::getVelX()
{
	return this->velX;
}

float Moves::getVelY()
{
	return this->velY;
}

float Moves::getSpeed()
{
	return this->speed;
}

bool Moves::getGhostMode()
{
	return this->ghostMode;
}

bool Moves::getOnGround()
{
	return this->onGround;
}

void Moves::setOnGround(bool g)
{
	this->onGround = g;
	if (g) {
		this->velY = 0.0f;
	}
}

void Moves::setSpeed(float s)
{
	if (s <= 0.0f) {
		return;
	}

	if (this->speed == 0.0f) {
		this->speed = s;
		return;
	}

	float vx = this->velX;
	float vy = this->velY;

	float mag = sqrtf(vx * vx + vy * vy);

	if (mag == 0.0f) {
		this->speed = s;
		return;
	}

	float nx = vx / mag;
	float ny = vy / mag;

	this->speed = s;
	this->velX = nx * this->speed;
	this->velY = ny * this->speed;
}
