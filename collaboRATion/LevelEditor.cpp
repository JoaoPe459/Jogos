#include "LevelEditor.h"
#include "Engine.h"
#include "Home.h"
#include "Sprite.h"
#include "Object.h" // Modificado de Entity.h para Object.h
#include <fstream>
#include <cmath>

// =========================================================================
// DUMMY VISUAL: O Manequim à prova de falhas
// Herda diretamente de Object (assim como o seu Mouse e Background fazem), 
// o que resolve o erro de classe abstrata (C2259).
// =========================================================================
class DummyVisual : public Object {
private:
    Sprite* spr;
public:
    DummyVisual(float px, float py, float w, float h, const std::string& tex) {
        MoveTo(px, py);
        BBox(new Rect(-w / 2.0f, -h / 2.0f, w / 2.0f, h / 2.0f));
        if (!tex.empty()) {
            spr = new Sprite(tex.c_str());
        }
        else {
            spr = nullptr;
        }
    }
    ~DummyVisual() { if (spr) delete spr; }

    // Object só exige Update e Draw obrigatórios!
    void Update() override {}
    void Draw() override { if (spr) spr->Draw(x, y, Layer::FRONT); }
};
// =========================================================================

std::string LevelEditor::NextId() {
    return "Obj_" + std::to_string(++idCount);
}

const char* LevelEditor::GetTypeName(EType t) const {
    switch (t) {
    case EType::WALL: return "BLOCO SIMPLES (1x1)";
    case EType::RECT2: return "CORREDOR (6 Blocos)";
    case EType::KILLZONE: return "ZONA DE MORTE";
    case EType::SPAWN: return "SPAWN DO RATO";
    case EType::BOLA: return "BOLA GIGANTE";
    case EType::ESPINHO: return "ESPINHOS";
    case EType::BOTAO: return "BOTAO (INTERRUPTOR)";
    case EType::PORTA: return "PORTA DE SAIDA";
    case EType::DECO_BLOCK: return "FANTASMA: BLOCO (Cenario)";
    case EType::DECO_RECT2: return "FANTASMA: CORREDOR (Cenario)";
    default: return "???";
    }
}

int LevelEditor::NearestObj(float mx, float my) const {
    int best = -1;
    float bestDist = 99999.0f;
    for (int i = 0; i < (int)objects.size(); i++) {
        float dx = objects[i].x - mx;
        float dy = objects[i].y - my;
        float dist = dx * dx + dy * dy;

        if (dist < bestDist && dist < 1600.0f) {
            bestDist = dist;
            best = i;
        }
    }
    return best;
}

void LevelEditor::CreateVisual(EditorObj& o) {
    if (o.type == EType::WALL) {
        DummyVisual* v = new DummyVisual(o.x, o.y, 32, 32, "Resources/Tijolo.png");
        scene->Add(v, STATIC);
        o.visuals.push_back(v);
    }
    else if (o.type == EType::RECT2) {
        for (int i = 0; i < 6; i++) {
            DummyVisual* v = new DummyVisual(o.x + (i * 32), o.y, 32, 32, "Resources/Tijolo.png");
            scene->Add(v, STATIC);
            o.visuals.push_back(v);
        }
    }
    else if (o.type == EType::KILLZONE) {
        DummyVisual* v = new DummyVisual(o.x, o.y, 128, 32, "");
        scene->Add(v, STATIC);
        o.visuals.push_back(v);
    }
    else if (o.type == EType::SPAWN) {
        DummyVisual* v = new DummyVisual(o.x, o.y, 64, 64, "Resources/Player/Rato2.png");
        scene->Add(v, STATIC);
        o.visuals.push_back(v);
    }
    else if (o.type == EType::BOLA) {
        DummyVisual* v = new DummyVisual(o.x, o.y, 32, 32, "Resources/Bola.png");
        scene->Add(v, STATIC); o.visuals.push_back(v);
    }
    else if (o.type == EType::ESPINHO) {
        DummyVisual* v = new DummyVisual(o.x, o.y, 32, 32, "Resources/Espinhos.png");
        scene->Add(v, STATIC); o.visuals.push_back(v);
    }
    else if (o.type == EType::BOTAO) {
        // O botão é menorzinho, ajustei a hitbox dele no editor para 32x16
        DummyVisual* v = new DummyVisual(o.x, o.y, 32, 16, "Resources/botao.png");
        scene->Add(v, STATIC); o.visuals.push_back(v);
    }
    else if (o.type == EType::PORTA) {
        // A porta geralmente é mais alta (32x64)
        DummyVisual* v = new DummyVisual(o.x, o.y, 32, 64, "Resources/Porta.png");
        scene->Add(v, STATIC); o.visuals.push_back(v);
    }
    else if (o.type == EType::DECO_BLOCK) {
        DummyVisual* v = new DummyVisual(o.x, o.y, 32, 32, "Resources/Tijolo.png");
        scene->Add(v, STATIC);
        o.visuals.push_back(v);
    }
    else if (o.type == EType::DECO_RECT2) {
        // O RECT2 fantasma cria 6 manequins em fila
        for (int i = 0; i < 6; i++) {
            DummyVisual* v = new DummyVisual(o.x + (i * 32.0f), o.y, 32, 32, "Resources/Tijolo.png");
            scene->Add(v, STATIC);
            o.visuals.push_back(v);
        }
    }
}

void LevelEditor::DestroyVisual(EditorObj& o) {
    for (Object* v : o.visuals) {
        scene->Delete(v, STATIC);
    }
    o.visuals.clear();
}

void LevelEditor::PlaceObject(float x, float y) {

    for (const auto& obj : objects) {
        
        if (std::abs(obj.x - x) < 1.0f && std::abs(obj.y - y) < 1.0f) {
            return;
        }
    }
    // ------------------------------------------------------

    EditorObj o;
    o.type = curType;
    o.x = x;
    o.y = y;
    o.id = NextId();
    CreateVisual(o);
    objects.push_back(o);
}

void LevelEditor::Save(const std::string& path) const {
    std::ofstream f(path);
    if (!f.is_open()) return;

    f << "# Mapa Gerado pelo Level Editor\n";
    f << "GRAVITY 1250.0\n\n";

    for (const auto& o : objects) {
        if (o.type == EType::WALL) {
            f << "WALL " << o.id << " " << o.x << " " << o.y << "\n";
            f << "BLOCK " << o.id << " 0 0 32 32 Resources/Tijolo.png " << o.id << "_tag\n\n";
        }
        else if (o.type == EType::RECT2) {
            f << "WALL " << o.id << " " << o.x << " " << o.y << "\n";
            f << "RECT2 " << o.id << " 0 0 32 32 Resources/Tijolo.png " << o.id << "_tag\n\n";
        }
        else if (o.type == EType::KILLZONE) {
            f << "KILLZONE " << o.id << " " << o.x << " " << o.y << " 128 32 1 mortal\n\n";
        }
        else if (o.type == EType::SPAWN) {
            f << "SPAWN " << o.x << " " << o.y << "\n\n";
        }
        else if (o.type == EType::BOLA) {
            f << "BOLA " << o.id << " " << o.x << " " << o.y << "\n\n";
        }
        else if (o.type == EType::ESPINHO) {
            f << "ESPINHO " << o.id << " " << o.x << " " << o.y << "\n\n";
        }
        else if (o.type == EType::BOTAO) {
            f << "BOTAO " << o.id << " " << o.x << " " << o.y << "\n\n";
        }
        else if (o.type == EType::PORTA) {
            f << "PORTA " << o.id << " " << o.x << " " << o.y << "\n\n";
        }
        else if (o.type == EType::DECO_BLOCK) {
            f << "DECO_BLOCK " << o.x << " " << o.y << "\n\n";
        }
        else if (o.type == EType::DECO_RECT2) {
            f << "DECO_RECT2 " << o.x << " " << o.y << "\n\n";
        }
    }
    f.close();
}

void LevelEditor::Init() {
    scene = new Scene();
    viewBBox = true;
    font = new Font("Resources/consolas12.png");
    if (font) font->Spacing("Resources/consolas12.dat");

    // Carrega o carimbo 1 única vez!
    decoSprite = new Sprite("Resources/Tijolo.png");
}

void LevelEditor::Update() {
    if (window->KeyPress(VK_ESCAPE)) { Engine::Next<Home>(); return; }
    if (window->KeyPress('B')) { viewBBox = !viewBBox; }

    float mx = window->MouseX();
    float my = window->MouseY();

    if (window->KeyPress(VK_TAB)) {
        int n = (int)EType::_COUNT;
        curType = (EType)(((int)curType + 1) % n);
    }

    if (window->KeyPress('S')) { Save("Resources/Level_Editor_Output.txt"); }

    if (window->KeyPress(VK_DELETE) && selected >= 0) {
        DestroyVisual(objects[selected]);
        objects.erase(objects.begin() + selected);
        selected = -1;
        dragging = false;
    }

    if (window->KeyDown(VK_RBUTTON)) {
        if (!dragging) {
            selected = NearestObj(mx, my);
            if (selected >= 0) dragging = true;
        }
        else {
            objects[selected].x = Snap(mx);
            objects[selected].y = Snap(my);

            if (objects[selected].type == EType::RECT2) {
                for (int i = 0; i < 6; i++) {
                    objects[selected].visuals[i]->MoveTo(objects[selected].x + (i * 32), objects[selected].y);
                }
            }
            else if (!objects[selected].visuals.empty()) {
                objects[selected].visuals[0]->MoveTo(objects[selected].x, objects[selected].y);
            }
        }
    }
    else {
        dragging = false;
    }

    if (window->KeyPress(VK_LBUTTON) && !dragging) {
        PlaceObject(Snap(mx), Snap(my));
    }

    scene->Update();
}

void LevelEditor::Draw() {
    scene->Draw();
    if (viewBBox) scene->DrawBBox();


    if (font) {
        Color amarelo(1.0f, 1.0f, 0.0f, 1.0f);
        Color branco(1.0f, 1.0f, 1.0f, 1.0f);

        std::string ferramenta = std::string("FERRAMENTA: ") + GetTypeName(curType);

        font->Draw(10, 10, ferramenta, amarelo, Layer::FRONT);
        font->Draw(10, 40, "TAB: Trocar Ferramenta", branco, Layer::FRONT);
        font->Draw(10, 60, "LMB: Colocar  |  RMB: Arrastar  |  DEL: Apagar", branco, Layer::FRONT);
        font->Draw(10, 80, "S: Salvar Nivel  |  B: Mostrar Caixas de Colisao", branco, Layer::FRONT);
    }
}

void LevelEditor::Finalize() {
    for (auto& o : objects) DestroyVisual(o);
    objects.clear();
    delete scene;
    if (font) delete font;

    // Limpa o carimbo
    if (decoSprite) delete decoSprite;
}