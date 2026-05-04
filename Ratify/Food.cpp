#include "Food.h"
#include "LevelMake.h"
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;
Food::Food() : Entity()
{
    type = FOOD;

    // --- 1. CARREGAMENTO AUTOMÁTICO DA PASTA ---
    std::string path = "Resources/Foods"; // Pasta onde estão as imagens das comidas
    std::vector<std::string> filePaths;

    if (fs::exists(path) && fs::is_directory(path)) {
        for (const auto& entry : fs::directory_iterator(path)) {
            std::string ext = entry.path().extension().string();
            if (ext == ".png" || ext == ".jpg") {
                filePaths.push_back(entry.path().string());
            }
        }
    }

    spriteCount = static_cast<int>(filePaths.size());

    // Inicializa o array dinâmico
    if (spriteCount > 0) {
        sprites = new Sprite * [spriteCount];
        for (int i = 0; i < spriteCount; i++) {
            sprites[i] = new Sprite(filePaths[i].c_str());
        }
    }

    // --- 2. SORTEIO DO SPRITE E DEFINIÇÃO DA BBOX ---
    RandomizeSprite();

    if (currentSprite) {
        // Ajusta a BBox ao tamanho do sprite sorteado
        float h = (float)currentSprite->Height();
        float w = (float)currentSprite->Width();
        BBox(new Rect(-h / 2, -w / 2, h / 2, w / 2));
    }
    else {
        // Fallback caso a pasta esteja vazia
        BBox(new Rect(-5, -5, 5, 5));
    }


    float randomX = (window->Width()/2);
    float randomY = (window->Height()/2);
    this->MoveTo(randomX, randomY);

    moves->setSpeed(0.0f);
}

Food::~Food()
{
    // Limpeza da memória dos sprites
    if (sprites != nullptr) {
        for (int i = 0; i < spriteCount; i++) {
            delete sprites[i];
        }
        delete[] sprites;
    }
}

void Food::Draw()
{
    if (currentSprite) {
        currentSprite->Draw(x, y, z);
    }
}

void Food::RandomizeSprite() {
    if (spriteCount > 0 && sprites != nullptr) {
        currentSprite = sprites[rand() % spriteCount];
    }
}

void Food::OnCollision(Object* obj) 
{
    if (obj->Type() == PLAYER) {
        LevelMake* lvl = static_cast<LevelMake*>(Engine::game);
        lvl->GetScene()->Delete(this, STATIC); 
    }
}

void Food::Update()
{
    // 3. Totalmente estática: não processa controle nem física de movimento
}

void Food::Control()
{
    // Vazio: sem inteligência de movimento
}

void Food::HandleScreenWrap()
{
    // Vazio: não atravessa bordas
}
