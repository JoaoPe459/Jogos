#include "PacMan.h"
#include "Player.h"

Player::Player() : Entity() // Chama o construtor da base
{
    spriteL = new Sprite("Resources/PacManL.png");
    spriteR = new Sprite("Resources/PacManR.png");
    spriteU = new Sprite("Resources/PacManU.png");
    spriteD = new Sprite("Resources/PacManD.png");

    BBox(new Rect(-20, -20, 20, 20));
    MoveTo(480.0f, 450.0f);
    type = PLAYER;

    moves->setSpeed(200.0f);
}

Player::~Player()
{
    delete spriteL;
    delete spriteR;
    delete spriteU;
    delete spriteD;
}

void Player::Control()
{
    if (window->KeyPress('G')) {
        moves->invertGravity();
    }

    if (window->KeyPress('I')) {
        incrementSpeed();
    }

    if (window->KeyPress('O')) {
        decressSpeed();
    }

    if (window->KeyDown(VK_LEFT)) {
        nextState = LEFT;
        moves->Left();
    }

    if (window->KeyDown(VK_RIGHT)) {
        nextState = RIGHT;
        moves->Right();
    }

    if (window->KeyDown(VK_UP)) {
        if (moves->getOnGround()) {
            nextState = UP;
            moves->Up();
            moves->setOnGround(false);
        }
    }

    if (window->KeyDown(VK_DOWN)) {
        nextState = DOWN;
        moves->Down();
    }
}

void Player::Draw()
{
    Sprite* current = spriteL;
    uint state = (currState == STOPED) ? nextState : currState;

    switch (state) {
    case LEFT:  spriteL->Draw(x, y, Layer::UPPER); break;
    case RIGHT: spriteR->Draw(x, y, Layer::UPPER); break;
    case UP:    spriteU->Draw(x, y, Layer::UPPER); break;
    case DOWN:  spriteD->Draw(x, y, Layer::UPPER); break;
    default:    spriteL->Draw(x, y, Layer::UPPER); break;
    }
}

void Player::incrementSpeed() {
    moves->setSpeed(moves->getSpeed() * 2);
}

void Player::decressSpeed() {
    moves->setSpeed(moves->getSpeed() / 2);
}