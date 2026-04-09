#include "PacMan.h"
#include "Ghost.h"



Ghost::Ghost() : Entity()
{
    type = GHOST;
    RandomizeSprite();

    BBox(new Rect(-20, -20, 20, 20));
    MoveTo(200.0f, 450.0f);

    // Ajusta a velocidade inicial através do objeto moves da Entity
    moves->setSpeed(static_cast<float>(500.0f - (rand() % 200)));
    moveType = static_cast<MovementType>(rand() % 3);

    dirX = (rand() % 2 == 0) ? 1 : -1;
    dirY = (rand() % 2 == 0) ? 1 : -1;
    setMass(1.2f);
}

Ghost::~Ghost()
{
    delete redSprite; redSprite = nullptr;
    delete blueSprite; blueSprite = nullptr;
    delete orangeSprite; orangeSprite = nullptr;
    delete pinkSprite; pinkSprite = nullptr;
    //delete sprite; sprite = nullptr;
}

void Ghost::Draw()
{
    sprite->Draw(X(), Y());
}

void Ghost::OnCollision(Object* obj) {
    Entity::OnCollision(obj);

    // 2. Lógica específica do Ghost: mudar direção ao bater em algo sólido
    if (obj->Type() == WALL || obj->Type() == GHOST) {
        this->RandomizeMovement();
    }
}

void Ghost::Control() {
    float speed = moves->getSpeed();
    float targetVX = 0;
    float targetVY = 0;

    switch (moveType) {
    case HORIZONTAL:
        targetVX = dirX * speed;
        break;

    case VERTICAL:
        targetVY = dirY * speed;
        break;

    case DIAGONAL:
        targetVX = dirX * speed;
        targetVY = dirY * speed;
        break;
    }


    float acceleration = 0.08f;

    float currentVX = moves->getVelX();
    float currentVY = moves->getVelY();

    moves->setVelX(currentVX + (targetVX - currentVX) * acceleration);
    moves->setVelY(currentVY + (targetVY - currentVY) * acceleration);

    HandleScreenWrap();
}

void Ghost::RandomizeSprite() {
    int randomValue = rand() % 4;
    SetSpriteByIndex(randomValue);
}

void Ghost::SetSpriteByIndex(int index) {
    switch (index) {
    case 0: this->sprite = redSprite; break;
    case 1: this->sprite = blueSprite; break;
    case 2: this->sprite = orangeSprite; break;
    case 3: this->sprite = pinkSprite; break;
    default:
        this->sprite = redSprite;
        break;
    }
}