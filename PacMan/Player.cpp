#include "Player.h"
#include "Ghost.h"
#include <cmath>
#include "PacMan.h"

Player::Player() : Entity() {
    type = PLAYER;
    sprite = new Sprite("Resources/PacManR.png");
    BBox(new Rect(-20, -20, 20, 20));
    moves->setSpeed(500.0f);
}

void Player::OnCollision(Object* obj) {
    Entity::OnCollision(obj);

    // 2. Lógica específica do Player (Comida)
    if (obj->Type() == FOOD) {
		Eat(10.0f);
    }
}

void Player::Eat(float amount) {
	calories += amount;
    setMass(1.0f + (sizeLevel * 0.5f));
}

void Player::Control() {
    float baseSpeed = moves->getSpeed() - (sizeLevel * 10.0f);

    // Quanto menor a aceleração, mais tempo dura o efeito do empurrão
    float acceleration = 0.06f;

    float targetVX = 0;
    float targetVY = 0;

    if (window->KeyDown(VK_LEFT))  targetVX = -baseSpeed;
    if (window->KeyDown(VK_RIGHT)) targetVX = baseSpeed;
    if (window->KeyDown(VK_UP))    targetVY = -baseSpeed;
    if (window->KeyDown(VK_DOWN))  targetVY = baseSpeed;

    // INTERPOLAÇÕES NOS MOVIMENTOS:
    // O player tenta alcançar a targetVelocity
    float currentVX = moves->getVelX();
    float currentVY = moves->getVelY();

    moves->setVelX(currentVX + (targetVX - currentVX) * acceleration);
    moves->setVelY(currentVY + (targetVY - currentVY) * acceleration);
}

void Player::Draw() {
    sprite->Draw(X(), Y());
}


Player::~Player() {
    if (sprite) {
        delete sprite;
    }
}