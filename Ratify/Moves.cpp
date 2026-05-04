#include "Moves.h"
#include "Physics.h" // Incluindo a classe global de física
#include <cmath>

void Moves::Stop()
{
    this->velX = 0;
    this->velY = 0;
}

void Moves::applyGravity(float gameTime) {
    if (this->ghostMode || this->onGround) return; // Se está no chão, a gravidade não acumula velY

    float gravityStep = Physics::GravityValue * Physics::Direction * gameTime;
    velY += gravityStep;
}

void Moves::invertGravity() {
    Physics::Invert();
}

void Moves::Up()
{
    // O pulo deve ser sempre oposto à direção atual da gravidade.
    // Physics::Direction é 1.0f para baixo e -1.0f para cima.
    /*float jumpForce = this->getSpeed() * 3.0f;

    if (Physics::Direction > 0) {
        // Gravidade normal: pula para cima (Y negativo)
        this->velY = -jumpForce;
    }
    else {
        // Gravidade invertida: pula para baixo (Y positivo)
        this->velY = jumpForce;
    }*/
	this->velY = -this->getSpeed();
}

void Moves::Down()
{
    // Move na direção da gravidade normal
    this->velY = this->getSpeed();
}

void Moves::Left()
{
    this->velX = -this->getSpeed();
}

void Moves::Right()
{
    this->velX = this->getSpeed();
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
