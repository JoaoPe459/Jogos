#ifndef _PACMAN_GHOST_H_
#define _PACMAN_GHOST_H_

#include "Entity.h"
#include "Player.h"

enum MovementType { HORIZONTAL, VERTICAL, DIAGONAL };

class Ghost : public Entity
{
private:
    Player* player = nullptr;
	Sprite* sprite = nullptr;

    MovementType moveType;
    int dirX = 1;
    int dirY = 1;

public:
    Ghost(Player* p);
    ~Ghost();

    void Control() override;

	void Draw() override;

	void OnCollision(Object* obj) override;

    void RandomizeMovement() {
        moveType = static_cast<MovementType>(rand() % 3);
        dirX = (rand() % 2 == 0) ? 1 : -1;
        dirY = (rand() % 2 == 0) ? 1 : -1;
    }

};

#endif