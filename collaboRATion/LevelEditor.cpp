#include "LevelEditor.h"
#include "Engine.h"
#include "Home.h"
#include "Sprite.h"
#include "Object.h"
#include "mouse.h"
#include <fstream>
#include <cmath>


// =========================================================================
// DUMMY VISUAL
// =========================================================================
class DummyVisual : public Object {
private:
    Sprite* spr;
    Color tint;
public:
    // CORREÇÃO: Usamos ": tint(c)" para inicializar a cor corretamente no C++
    DummyVisual(float px, float py, float w, float h, const std::string& tex, Color c = Color(1.0f, 1.0f, 1.0f, 1.0f))
        : tint(c)
    {
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

    void Update() override {}

    void Draw() override {
        if (spr) spr->Draw(x, y, Layer::FRONT, 1.0f, 0.0f, tint);
    }
};
// =========================================================================

void LevelEditor::Init() {
    scene = new Scene();
    viewBBox = true;
    font = new Font("Resources/consolas12.png");
    if (font) font->Spacing("Resources/consolas12.dat");
    decoSprite = new Sprite("Resources/Tijolo.png");

    LoadProject("Resources/Editor_Backup.proj");
}

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
    case EType::PLATFORM_TOOL: return "PLATAFORMA MOVEL (Vai e Vem)";
    case EType::FALLING_TOOL: return "CHAO FALSO (Desaba)";
    case EType::TRIGGER_TOOL: return "AREA DE GATILHO (Invisivel)";
    case EType::ESPINHO_INV: return "ESPINHOS (TETO)";
    default: return "???";
    }
}

int LevelEditor::NearestObj(float mx, float my) const {
    int best = -1;
    float bestDist = 99999.0f;

    // LÊ DE TRÁS PARA A FRENTE (Agarra o Espinho em vez da Parede de baixo)
    for (int i = (int)objects.size() - 1; i >= 0; i--) {
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
        DummyVisual* v = new DummyVisual(o.x, o.y, 32, 32, "");
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
        DummyVisual* vCima = new DummyVisual(o.x, o.y + 9, 32, 14, "Resources/Espinhos.png");
        scene->Add(vCima, STATIC);
        o.visuals.push_back(vCima);
    }
    else if (o.type == EType::ESPINHO_INV) {
        DummyVisual* vBaixo = new DummyVisual(o.x, o.y + 23, 32, 14, "Resources/Espinhos2.png");
        scene->Add(vBaixo, STATIC);
        o.visuals.push_back(vBaixo);
    }
    else if (o.type == EType::BOTAO) {
        DummyVisual* v = new DummyVisual(o.x, o.y+16, 32, 8, "Resources/botao.png");
        scene->Add(v, STATIC); o.visuals.push_back(v);
    }
    else if (o.type == EType::PORTA) {
        DummyVisual* v = new DummyVisual(o.x, o.y, 32, 32, "Resources/Porta.png");
        scene->Add(v, STATIC); o.visuals.push_back(v);
    }
    else if (o.type == EType::DECO_BLOCK) {
        Color corFantasma(0.4f, 0.4f, 0.4f, 0.7f); // Cinzento escuro com 70% de opacidade
        DummyVisual* v = new DummyVisual(o.x, o.y, 32, 32, "Resources/Tijolo.png", corFantasma);
        scene->Add(v, STATIC);
        o.visuals.push_back(v);
    }
    else if (o.type == EType::DECO_RECT2) {
        Color corFantasma(0.4f, 0.4f, 0.4f, 0.7f);
        for (int i = 0; i < 6; i++) {
            DummyVisual* v = new DummyVisual(o.x + (i * 32.0f), o.y, 32, 32, "Resources/Tijolo.png", corFantasma);
            scene->Add(v, STATIC);
            o.visuals.push_back(v);
        }
    }
    else if (o.type == EType::PLATFORM_TOOL) {
        for (int i = 0; i < 3; i++) {
            DummyVisual* v = new DummyVisual(o.x + (i * 32.0f), o.y, 32, 32, "Resources/Tijolo.png");
            scene->Add(v, STATIC);
            o.visuals.push_back(v);
        }
    }
    else if (o.type == EType::FALLING_TOOL) {
        for (int i = 0; i < 6; i++) {
            DummyVisual* v = new DummyVisual(o.x + (i * 32.0f), o.y, 32, 32, "Resources/Tijolo.png");
            scene->Add(v, STATIC);
            o.visuals.push_back(v);
        }
    }
    else if (o.type == EType::TRIGGER_TOOL) {
        Color corGatilho(0.2f, 0.5f, 1.0f, 0.6f);
        DummyVisual* v = new DummyVisual(o.x, o.y, 32, 32, "Resources/Tijolo.png", corGatilho);
        scene->Add(v, STATIC);
        o.visuals.push_back(v);
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

        // Verifica se clicou exatamente no mesmo lugar
        if (std::abs(obj.x - x) < 1.0f && std::abs(obj.y - y) < 1.0f) {

            if (curType == EType::KILLZONE && obj.type != EType::KILLZONE) continue;
            if (obj.type == EType::KILLZONE && curType != EType::KILLZONE) continue;

            if (curType == EType::DECO_BLOCK && obj.type != EType::DECO_BLOCK) continue;
            if (obj.type == EType::DECO_BLOCK && curType != EType::DECO_BLOCK) continue;

            if (curType == EType::DECO_RECT2 && obj.type != EType::DECO_RECT2) continue;
            if (obj.type == EType::DECO_RECT2 && curType != EType::DECO_RECT2) continue;

            if (curType == EType::TRIGGER_TOOL && obj.type != EType::TRIGGER_TOOL) continue;
            if (obj.type == EType::TRIGGER_TOOL && curType != EType::TRIGGER_TOOL) continue;

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
    f << "BACKGROUND Resources/Fundo.png\n";
    f << "GRAVITY 1250.0\n\n";

    f << "MOD_INVERT_CONTROLS " << (edModInvert ? 1 : 0) << "\n";
    f << "MOD_MOVE_WORLD " << (edModMoveWorld ? 1 : 0) << "\n";
    f << "MOD_GRAVITY_JUMP " << (edModGravity ? 1 : 0) << "\n\n";

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
            f << "KILLZONE " << o.id << " " << o.x << " " << o.y << " 32 32 1 mortal\n\n";
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
        else if (o.type == EType::ESPINHO_INV) {
            f << "ESPINHO_INV " << o.id << " " << o.x << " " << o.y << "\n\n";
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
        else if (o.type == EType::TRIGGER_TOOL) {
            f << "TRIGGER " << o.id << " " << o.x << " " << o.y << " " << o.w << " " << o.h << "\n\n";
        }else if (o.type == EType::PLATFORM_TOOL) {
            f << "WALL " << o.id << " " << o.x << " " << o.y << "\n";
            f << "RECT1 " << o.id << " 0 0 32 32 Resources/Tijolo.png " << o.id << "_tag\n";
            f << "PLATFORM " << o.id << " " << o.x << " " << o.y << " 3.0 150.0\n\n";
        }
        else if (o.type == EType::FALLING_TOOL) {
            f << "WALL " << o.id << " " << o.x << " " << o.y << "\n";
            f << "RECT2 " << o.id << " 0 0 32 32 Resources/Tijolo.png " << o.id << "_tag\n";
            f << "FALLING " << o.id << " 0.5\n\n";
        } 
    }

    for (const auto& p : customPaths) {
        if (p.nodes.size() > 1) {
            float speed = 200.0f; 
            float trigger = 350.0f;

            f << "PATH " << p.wallId << " " << p.speed << " " << p.triggerMode << " "
                << p.triggerParam << " " << (p.isLoop ? 1 : 0) << " " << p.nodes.size() << " ";

            for (const auto& n : p.nodes) {
                f << n.x << " " << n.y << " ";
            }
            f << "\n\n";
        }
    }
    for (const auto& o : objects) {
        if (!o.modToggle.empty() && (o.type == EType::BOTAO || o.type == EType::TRIGGER_TOOL)) {
            f << "MOD_TOGGLE " << o.id << " " << o.modToggle << "\n\n";
        }
    }
    f.close();
    std::ofstream fEd("Resources/Editor_Backup.proj");
    if (fEd.is_open()) {

        fEd << "GLOBAL_MODS " << edModInvert << " " << edModMoveWorld << " " << edModGravity << "\n";

        for (const auto& o : objects) {
            fEd << "OBJ " << (int)o.type << " " << o.id << " " << o.x << " " << o.y << " " << o.w << " " << o.h << "\n";
        }
        for (const auto& o : objects) {
            if (!o.modToggle.empty()) {
                fEd << "MOD_ED " << o.id << " " << o.modToggle << "\n";
            }
        }
        for (const auto& p : customPaths) {
            // GRAVA: ID -> Velocidade -> Modo -> Parametro -> Loop -> Quantidade de Pontos
            fEd << "PATH_ED " << p.wallId << " " << p.speed << " " << p.triggerMode << " "
                << p.triggerParam << " " << (p.isLoop ? 1 : 0) << " " << p.nodes.size() << " ";

            for (const auto& n : p.nodes) {
                fEd << n.x << " " << n.y << " ";
            }
            fEd << "\n";
        }
    }
}



void LevelEditor::LoadProject(std::string path) {
    std::ifstream file(path);
    if (!file.is_open()) return;

    idCount = 0; // Garante que a contagem reinicia antes de ler

    std::string key;
    while (file >> key) {

        if (key == "GLOBAL_MODS") {
            file >> edModInvert >> edModMoveWorld >> edModGravity;
        }

        if (key == "OBJ") {
            EditorObj o;
            int typeInt;
            file >> typeInt >> o.id >> o.x >> o.y >> o.w >> o.h;
            o.type = (EType)typeInt;

            // ========================================================
            // CORREÇÃO DO ID COUNT
            // Extrai o número do "Obj_X" para a contagem acompanhar o mapa!
            if (o.id.find("Obj_") == 0) {
                try {
                    int num = std::stoi(o.id.substr(4));
                    if (num > idCount) {
                        idCount = num;
                    }
                }
                catch (...) {} // Se houver algum texto estranho, ignora
            }
            // ========================================================

            // Recria os manequins na tela
            CreateVisual(o);
            objects.push_back(o);
        }
        else if (key == "PATH_ED") {
            EditorPath p;
            int loop, count;

            // LÊ NA MESMA ORDEM: ID -> Velocidade -> Modo -> Parametro -> Loop -> Qtd Pontos
            file >> p.wallId >> p.speed >> p.triggerMode >> p.triggerParam >> loop >> count;

            p.isLoop = (loop != 0);

            // Lê as coordenadas (X e Y) de cada waypoint do caminho
            for (int i = 0; i < count; i++) {
                EdWaypoint wp;
                file >> wp.x >> wp.y;
                p.nodes.push_back(wp);
            }
            customPaths.push_back(p);
        }
        else if (key == "MOD_ED") {
            std::string alvoId, tipoMod;
            file >> alvoId >> tipoMod;

            // Procura o objeto na lista do editor e devolve-lhe a memória!
            for (auto& obj : objects) {
                if (obj.id == alvoId) {
                    obj.modToggle = tipoMod;
                    break;
                }
            }
        }
    }
    file.close();
}
void LevelEditor::Update() {
    if (window->KeyPress(VK_ESCAPE)) { Engine::Next<Home>(); return; }
    if (window->KeyPress('B')) { viewBBox = !viewBBox; }

    bool editandoBotao = false;

    if (selected >= 0 && selected < objects.size()) {
        EType selType = objects[selected].type;
        if (selType == EType::BOTAO || selType == EType::TRIGGER_TOOL) {
            editandoBotao = true; // Bloqueia a alteração da fase inteira

            // Atribui os poderes ao Botão/Gatilho
            if (window->KeyPress('I')) objects[selected].modToggle = "INVERT_CONTROLS";
            if (window->KeyPress('G')) objects[selected].modToggle = "GRAVITY_JUMP";
            if (window->KeyPress('C')) objects[selected].modToggle = "";
        }
    }

    if (!editandoBotao) {
        if (window->KeyPress('I')) edModInvert = !edModInvert;
        if (window->KeyPress('G')) edModGravity = !edModGravity;
    }

    float rawMouseX = window->MouseX();
    float rawMouseY = window->MouseY();

    float mx = std::floor(rawMouseX / 32.0f) * 32.0f+16;
    float my = std::floor(rawMouseY / 32.0f) * 32.0f+16;

    // --- Lógica de Criação de Caminhos (CTRL) ---
    if (window->KeyDown(VK_CONTROL)) {

        if (window->KeyPress(VK_LBUTTON)) {

            if (currentEditingPath == -1) {
                for (int i = (int)objects.size() - 1; i >= 0; i--) {
                    const auto& obj = objects[i];
                    if (std::abs(obj.x - mx) < 32 && std::abs(obj.y - my) < 32) {
                        EditorPath newPath;
                        newPath.wallId = obj.id;
                        newPath.nodes.push_back({ mx, my });
                        customPaths.push_back(newPath);
                        currentEditingPath = customPaths.size() - 1;
                        break;
                    }
                }
            }
            else {
                float distToStart = std::abs(customPaths[currentEditingPath].nodes[0].x - mx) +
                    std::abs(customPaths[currentEditingPath].nodes[0].y - my);

                if (distToStart < 32.0f) {
                    customPaths[currentEditingPath].isLoop = true;
                    currentEditingPath = -1; // Finaliza
                }
                else {
                    customPaths[currentEditingPath].nodes.push_back({ mx, my }); // Adiciona waypoint
                }
            }
        }
    }
    else {
        currentEditingPath = -1;
    }


    if (window->KeyPress(VK_TAB)) {
        int n = (int)EType::_COUNT;
        curType = (EType)(((int)curType + 1) % n);
    }

    if (window->KeyPress('S')) { Save("Resources/Level_Editor_Output.txt"); }

    // --- Lógica de APAGAR (Delete) ---
    if (window->KeyPress(VK_DELETE) && selected >= 0) {

        for (auto* v : objects[selected].visuals) {
            scene->Delete(v, STATIC);
        }

        std::string idApagado = objects[selected].id;
        for (auto it = customPaths.begin(); it != customPaths.end(); ) {
            if (it->wallId == idApagado) {
                it = customPaths.erase(it);
            }
            else {
                ++it;
            }
        }

        objects.erase(objects.begin() + selected);
        selected = -1;
        dragging = false;
    }

    // --- Lógica de ARRASTAR (Botão Direito) ---
    if (window->KeyDown(VK_RBUTTON)) {
        if (!dragging) {
            selected = NearestObj(mx, my);
            if (selected >= 0 && selected < objects.size()) dragging = true;
        }
        else {
            if (selected >= 0 && selected < objects.size()) {
                if (objects[selected].x != mx || objects[selected].y != my) {
                    objects[selected].x = mx;
                    objects[selected].y = my;

                    for (auto* v : objects[selected].visuals) scene->Delete(v, STATIC);
                    objects[selected].visuals.clear();

                    CreateVisual(objects[selected]);
                }

                EType selType = objects[selected].type;
                if (selType == EType::BOTAO || selType == EType::TRIGGER_TOOL) {
                    if (window->KeyPress('I')) objects[selected].modToggle = "INVERT_CONTROLS";
                    if (window->KeyPress('G')) objects[selected].modToggle = "GRAVITY_JUMP";
                    if (window->KeyPress('M')) objects[selected].modToggle = "MOVE_WORLD";
                    if (window->KeyPress('C')) objects[selected].modToggle = "";
                }
            }
        }
    }
    else {
        dragging = false;
    }


    if (window->KeyPress(VK_LBUTTON) && !dragging && !window->KeyDown(VK_CONTROL)) {
        PlaceObject((mx), (my));
    }

    if (selected >= 0 && selected < objects.size()) {
        std::string selId = objects[selected].id;
        for (auto& p : customPaths) {
            if (p.wallId == selId) {

                if (window->KeyPress('1')) p.speed = 300.0f;
                if (window->KeyPress('2')) p.speed = 600.0f;
                if (window->KeyPress('3')) p.speed = 900.0f;
                if (window->KeyPress('4')) p.speed = 1200.0f;

                if (window->KeyDown(VK_UP)) p.speed += 100.0f * gameTime;
                if (window->KeyDown(VK_DOWN)) p.speed -= 100.0f * gameTime;
                if (p.speed < 50.0f) p.speed = 50.0f;

                if (window->KeyPress('M')) {
                    p.triggerMode = (p.triggerMode + 1) % 3;
                    p.triggerParam = (p.triggerMode == 2) ? "NENHUM_ALVO" : "0";
                }
                if (p.triggerMode == 2 && window->KeyPress('T')) {
                    int hovered = NearestObj(mx, my);
                    if (hovered >= 0) {
                        EType ht = objects[hovered].type;
                        if (ht == EType::TRIGGER_TOOL || ht == EType::BOTAO) {
                            p.triggerParam = objects[hovered].id;
                        }
                    }
                }
                break;
            }
        }
    }
    if (selected >= 0 && objects[selected].type == EType::TRIGGER_TOOL && selected < objects.size()) {
        bool resize = false;

        if (window->KeyPress(VK_UP)) { objects[selected].h += 32.0f; resize = true; }
        if (window->KeyPress(VK_DOWN)) { if (objects[selected].h > 32.0f) { objects[selected].h -= 32.0f; resize = true; } }
        if (window->KeyPress(VK_RIGHT)) { objects[selected].w += 32.0f; resize = true; }
        if (window->KeyPress(VK_LEFT)) { if (objects[selected].w > 32.0f) { objects[selected].w -= 32.0f; resize = true; } }

        if (resize) {
            for (auto* v : objects[selected].visuals) scene->Delete(v, STATIC);
            objects[selected].visuals.clear();

            Color corGatilho(0.2f, 0.5f, 1.0f, 0.6f);

            int cols = objects[selected].w / 32;
            int rows = objects[selected].h / 32;

            for (int c = 0; c < cols; c++) {
                for (int r = 0; r < rows; r++) {
                    DummyVisual* v = new DummyVisual(objects[selected].x + (c * 32.0f), objects[selected].y + (r * 32.0f), 32, 32, "Resources/Tijolo.png", corGatilho);
                    scene->Add(v, STATIC);
                    objects[selected].visuals.push_back(v);
                }
            }
        }
    }

    scene->Update();
}

void LevelEditor::Draw() {
    scene->Draw();
    if (viewBBox) scene->DrawBBox();
    if (decoSprite) {
        
        float rawMouseX = window->MouseX() + 0.0f;
        float rawMouseY = window->MouseY() + 0.0f;

       
        Color corPonteiro(1.0f, 0.0f, 0.0f, 1.0f);

        
        float escala3px = 3.0f / 32.0f;

        decoSprite->Draw(rawMouseX, rawMouseY, Layer::FRONT, escala3px, 0.0f, corPonteiro);
    }


    if (font) {
        Color amarelo(1.0f, 1.0f, 0.0f, 1.0f);
        Color branco(1.0f, 1.0f, 1.0f, 1.0f);

        std::string ferramenta = std::string("FERRAMENTA: ") + GetTypeName(curType);

        font->Draw(20, 10, ferramenta, amarelo, Layer::FRONT);
        font->Draw(20, 40, "TAB: Trocar Ferramenta", branco, Layer::FRONT);
        font->Draw(20, 70, "LMB: Colocar  |  RMB: Arrastar  |  DEL: Apagar", branco, Layer::FRONT);
        font->Draw(20, 100, "S: Salvar Nivel  |  B: Mostrar Caixas de Colisao", branco, Layer::FRONT);
    }
    if (selected >= 0 && objects[selected].type == EType::TRIGGER_TOOL && selected < objects.size()) {
        font->Draw(20, 130, "AREA DE GATILHO [" + objects[selected].id + "]", Color(0.0f, 1.0f, 1.0f, 1.0f));
        font->Draw(20, 160, "Largura (Esq/Dir): " + std::to_string((int)objects[selected].w), Color(0.0f, 1.0f, 1.0f, 1.0f));
        font->Draw(20, 190, "Altura (Cima/Baixo): " + std::to_string((int)objects[selected].h), Color(0.0f, 1.0f, 1.0f, 1.0f));
    }
    if (selected >= 0 && font != nullptr && selected < objects.size()) {
        std::string selId = objects[selected].id;
        for (auto& p : customPaths) {
            if (p.wallId == selId) {
                std::string modoNome = (p.triggerMode == 0) ? "SEMPRE LIGADO (Automatico)" :
                    (p.triggerMode == 1) ? "PISAR" : "AREA DE GATILHO";

                std::string loopStatus = p.isLoop ? "SIM" : "NAO";

                font->Draw(20, 130, "PLATAFORMA [" + selId + "]", Color(1.0f, 1.0f, 0.0f, 1.0f));
                font->Draw(20, 160, "Velocidade (1,2,3,4 ou Setas): " + std::to_string((int)p.speed), Color(1.0f, 1.0f, 0.0f, 1.0f));
                font->Draw(20, 190, "Gatilho (M): " + modoNome, Color(1.0f, 1.0f, 0.0f, 1.0f));
                font->Draw(20, 215, "Loop Automatico: " + loopStatus, Color(1.0f, 1.0f, 0.0f, 1.0f)); // Linha nova

                if (p.triggerMode == 2) {
                    font->Draw(20, 240, "Alvo (Aperte 'T' no Gatilho): " + p.triggerParam, Color(0.0f, 1.0f, 1.0f, 1.0f));
                }
            }
        }
    } if (selected >= 0 && (objects[selected].type == EType::BOTAO || objects[selected].type == EType::TRIGGER_TOOL) && selected < objects.size()) {

        font->Draw(20, 400, "MODIFICADOR DO BOTAO/GATILHO:", Color(1.0f, 0.5f, 0.0f, 1.0f));

        std::string qualMod = objects[selected].modToggle == "" ? "NENHUM" : objects[selected].modToggle;
        font->Draw(20, 450, "Poder Atual: " + qualMod, Color(0.0f, 1.0f, 1.0f, 1.0f));
        font->Draw(20, 500, "I: Inverter | G: Gravidade | C: Limpar", Color(1.0f, 1.0f, 1.0f, 1.0f));
    }

    if (selected >= 0 && objects[selected].type == EType::TRIGGER_TOOL && selected < objects.size()) {
        font->Draw(20, 80, "AREA DE GATILHO [" + objects[selected].id + "]", Color(0.0f, 1.0f, 1.0f, 1.0f));
        font->Draw(20, 100, "Largura (Esq/Dir): " + std::to_string((int)objects[selected].w), Color(0.0f, 1.0f, 1.0f, 1.0f));
        font->Draw(20, 120, "Altura (Cima/Baixo): " + std::to_string((int)objects[selected].h), Color(0.0f, 1.0f, 1.0f, 1.0f));
    }
    if (decoSprite) {
        Color corCaminho(1.0f, 0.0f, 0.0f, 0.5f);
        for (const auto& p : customPaths) {
            for (const auto& node : p.nodes) {
                decoSprite->Draw(node.x, node.y, Layer::FRONT, 0.25f, 0.0f, corCaminho);
            }
        }
    }
    font->Draw(20, 280, "--- MODIFICADORES DA FASE ---", Color(1.0f, 0.5f, 0.0f, 1.0f));
    font->Draw(20, 300, std::string("Inverter Controles (I): ") + (edModInvert ? "ON" : "OFF"), Color(1.0f, 1.0f, 1.0f, 1.0f));
    font->Draw(20, 340, std::string("Gravidade Bizarra (G): ") + (edModGravity ? "ON" : "OFF"), Color(1.0f, 1.0f, 1.0f, 1.0f));
}

void LevelEditor::Finalize() {
    for (auto& o : objects) DestroyVisual(o);
    objects.clear();
    delete scene;
    if (font) delete font;
    customPaths.clear();
    selected = -1;
    currentEditingPath = -1;
    dragging = false;
    if (decoSprite) delete decoSprite;
}