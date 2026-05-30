#ifndef _MAP_DOOR_H_
#define _MAP_DOOR_H_

#include "Object.h"
#include "Animation.h"

class MapDoor : public Object {
private:
    TileSet* tileset;
    Animation* anim;
    int levelId; // Guarda qual fase essa porta carrega

public:
    MapDoor(float px, float py, int level);
    ~MapDoor();

    void Update() override;
    void Draw() override;

    void Select();
    void UnSelect();

    int GetLevel() const { return levelId; }
};
#endif