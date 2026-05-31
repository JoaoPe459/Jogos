#include "LevelSelect.h"
#include "Engine.h"
#include "LevelMake.h" // Para carregar a fase depois
#include "Level2.h"
#include "LevelEditor.h"

void LevelSelect::Init() {
    scene = new Scene();
    backg = new Sprite("Resources/Mapa.png");
    transitionBlock = new Sprite("Resources/Tijolo.png");
    isOpening = true;
    openingTimer = 0.0f;;

    blackBlock = new Sprite("Resources/Tijolo.png");
    isTransitioning = false;

    mouse = new Mouse();
    scene->Add(mouse, MOVING);

    MapDoor* fase1 = new MapDoor(230, 325, 1);
    scene->Add(fase1, STATIC);
    doors.push_back(fase1);

    MapDoor* fase2 = new MapDoor(396, 148, 2);
    scene->Add(fase2, STATIC);
    doors.push_back(fase2);

    MapDoor* fase3 = new MapDoor(796, 128, 3);
    scene->Add(fase3, STATIC);
    doors.push_back(fase3);

    MapDoor* fase4 = new MapDoor(936, 386, 4);
    scene->Add(fase4, STATIC);
    doors.push_back(fase4);
}

void LevelSelect::Update() {

    if (isOpening) {
        openingTimer += gameTime;
        if (openingTimer >= 0.5f) {
            isOpening = false;
        }
    }

    scene->Update();

    if (isTransitioning) {
        transitionTimer += gameTime;

        if (transitionTimer >= 0.5f) {
            switch (targetLevel) {
            case 1: Engine::Next<Level2>(); return;
            case 2: Engine::Next<LevelEditor>(); return;
            case 3: Engine::Next<Level2>(); return;
            case 4: return;
            }
        }
        return; 
    }

    mouse->Update();

    for (MapDoor* door : doors) {
        if (scene->Collision(mouse, door)) {

            door->Select();

            if (mouse->Clicked()) {
                targetLevel = door->GetLevel();
                isTransitioning = true;
                transitionTimer = 0.0f;
            }
        }
        else {
            door->UnSelect();
        }

        door->Update();
    }
}

void LevelSelect::Draw() {
    backg->Draw(window->CenterX(), window->CenterY(), Layer::BACK);
    scene->Draw();

    // --- TRANSIÇÃO: QUADRADOS ALEATÓRIOS ---
    if (isTransitioning && blackBlock) {
       
        float progress = transitionTimer / 0.5f;
        if (progress > 1.0f) progress = 1.0f;

        Color preto(0.0f, 0.0f, 0.0f, 1.0f);
        float size = 32.0f; 

      
        int colunas = (window->Width() / size) + 1;
        int linhas = (window->Height() / size) + 1;

        for (int c = 0; c < colunas; c++) {
            for (int r = 0; r < linhas; r++) {

               
                int numeroSorteado = ((c * 89) ^ (r * 43)) % 100;
                float momentoParaAparecer = numeroSorteado / 100.0f; 

                
                if (progress >= momentoParaAparecer) {
                    float x = (c * size) + (size / 2.0f);
                    float y = (r * size) + (size / 2.0f);
                    blackBlock->Draw(x, y, Layer::FRONT, 2.0f, 0.0f, preto);
                }
            }
        }
    }
}

void LevelSelect::Finalize() {
    delete backg;
    delete scene;         
    if (blackBlock) delete blackBlock;
    if (transitionBlock) {
        delete transitionBlock;
        transitionBlock = nullptr;
    }
    doors.clear();         
}