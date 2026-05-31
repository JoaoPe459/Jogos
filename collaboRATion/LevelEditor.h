#ifndef _LEVEL_EDITOR_H_
#define _LEVEL_EDITOR_H_

#include "Game.h"
#include "Scene.h"
#include "Font.h"
#include <vector>
#include <string>

enum class EType {
    WALL, RECT2, KILLZONE, SPAWN,
    BOLA, ESPINHO,ESPINHO_INV, BOTAO, PORTA,
    DECO_BLOCK, DECO_RECT2,
    PLATFORM_TOOL, FALLING_TOOL,
    TRIGGER_TOOL,
    _COUNT
};

struct EditorObj {
    EType type;
    float x = 0, y = 0;
    std::string id;
    float w = 32.0f;
    float h = 32.0f;
    std::vector<Object*> visuals;
};
struct EdWaypoint { float x, y; };
struct EditorPath {
    std::string wallId;
    std::vector<EdWaypoint> nodes;
    bool isLoop = false;
    float speed = 150.0f;
    int triggerMode = 1;
    std::string triggerParam = "0";
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

    std::vector<EditorPath> customPaths;
    int currentEditingPath = -1;

public:
    void LoadProject(std::string path);
    void Init() override;
    void Update() override;
    void Draw() override;
    void Finalize() override;

    // Adicionado por segurança para a Engine não bugar
    Scene* GetScene() { return scene; }
};

#endif