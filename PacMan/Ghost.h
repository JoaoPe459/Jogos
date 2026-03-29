#ifndef _PACMAN_GHOST_H_
#define _PACMAN_GHOST_H_

#include "Entity.h"
#include "Player.h"

class Ghost : public Entity
{
private:
    Player* player = nullptr;
	Sprite* sprite = nullptr;

public:
    Ghost(Player* p);
    ~Ghost();

    void Control() override;

	void Draw() override;

	void OnCollision(Object* obj) override;


};

#endif