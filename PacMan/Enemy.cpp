#include "Enemy.h"
#include "PacMan.h"
#include "Physics.h"

Enemy::Enemy() : sprite(nullptr), health(100) {
    type = ENEMY;
    moves = new Moves(0, 0, 150.0f, false);
}

Enemy::~Enemy() {
    if (sprite) delete sprite;
    delete moves;
}



void Enemy::Update() {
    // 1. Lógica de decisão (IA) definida na classe filha
    IA();

    // 2. Aplica a gravidade GLOBAL (Physics::GetGravity)
    moves->applyGravity(gameTime);

    // 3. Move o objeto no mundo
    Translate(moves->getVelX() * gameTime, moves->getVelY() * gameTime);

    // Chamada automática para todos os filhos
    ScreenWrap();
}

void Enemy::Draw() {
    if (sprite)
        sprite->Draw(X(), Y(), Layer::UPPER);
}

void Enemy::OnCollision(Object* obj) {
    // Lógica de colisão similar ao Player para não atravessar paredes
    if (obj->Type() == WALL) {
        // Aqui você pode copiar a lógica de resolução (overlapX/Y) do Player.cpp
        // para garantir que os inimigos também fiquem no "chão" ou "teto".
    }
}

void Enemy::ScreenWrap() {
    // Limite Lateral (Teletransporte igual ao Player)
    if (X() + 20.0f < 0) MoveTo(window->Width() + 20.0f, Y());
    if (X() - 20.0f > window->Width()) MoveTo(-20.0f, Y());

    // Limite Inferior (Chão da tela)
    if (Y() + 20.0f > window->Height()) {
        MoveTo(X(), window->Height() - 20.0f);
        moves->setVelY(0.0f);

        // Se a gravidade aponta para baixo, estamos no chão
        if (Physics::Direction > 0) moves->setOnGround(true);
    }

    // Limite Superior (Teto da tela)
    if (Y() - 20.0f < 0.0f) {
        MoveTo(X(), 20.0f);
        moves->setVelY(0.0f);

        // Se a gravidade está invertida, o teto vira o nosso chão
        if (Physics::Direction < 0) moves->setOnGround(true);
    }
}