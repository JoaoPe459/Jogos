#ifndef _LEVEL_EDITOR_H_
#define _LEVEL_EDITOR_H_

#include "Game.h"
#include "Scene.h"
#include "Font.h"
#include <vector>
#include <string>

enum class EType {
    WALL, RECT2, KILLZONE, SPAWN,
    BOLA, ESPINHO, BOTAO, PORTA,
    DECO_BLOCK, DECO_RECT2,
    _COUNT
};

struct EditorObj {
    EType type;
    float x = 0, y = 0;
    std::string id;

    // Agora guardamos uma lista de Manequins visuais (necessário porque o RECT2 usa 6 blocos)
    std::vector<Object*> visuals;
};

class LevelEditor : public Game {
private:
    Scene* scene = nullptr;
    Font* font = nullptr;
    std::vector<EditorObj> objects;

    int selected = -1;
    bool dragging = false;
    EType curType = EType::WALL;
    int idCount = 0;
    bool viewBBox = true;

    float Snap(float v) const { return roundf(v / 32.0f) * 32.0f; }
    std::string NextId();
    int NearestObj(float mx, float my) const;
    void PlaceObject(float x, float y);
    void CreateVisual(EditorObj& o);
    void DestroyVisual(EditorObj& o);
    void Save(const std::string& path) const;
    const char* GetTypeName(EType t) const;
    Sprite* decoSprite = nullptr;

public:
    void Init() override;
    void Update() override;
    void Draw() override;
    void Finalize() override;

    // Adicionado por segurança para a Engine não bugar
    Scene* GetScene() { return scene; }
};

#endif