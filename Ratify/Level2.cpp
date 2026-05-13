#include "Engine.h"
#include "Home.h"
#include "Level2.h"
#include "Pivot.h"
#include <string>

void Level2::Init()
{
    LevelMake::Init(600, 0, 0, "Resources/Level2.jpg");

    // Cria uma wall composta por 3 blocos empilhados para testar
    testWall = new Wall();
    testWall->MoveTo(400.0f, 300.0f);

    testWall->AddBlock(0, -44, 44, 44, "Resources/Tijolo.png", "top");
    testWall->AddBlock(0, 0, 44, 44, "Resources/Tijolo.png", "mid");
    testWall->AddBlock(0, 44, 44, 44, "Resources/Tijolo.png", "base");

    scene->Add(testWall, STATIC);
}

void Level2::Update()
{
    if (window->KeyPress(VK_ESCAPE)) {
        Engine::Next<Home>();
        return;
    }

    // ── Controles de teste da Wall ──────────────────────────────

    // R / E  →  Rotaciona +/- 5 graus por frame enquanto segurado
    if (window->KeyDown('R'))
        testWall->Rotate(+180.0f * gameTime);

    if (window->KeyDown('E'))
        testWall->Rotate(-180.0f * gameTime);

    // F  →  Inicia queda (só ativa uma vez)
    if (window->KeyPress('F'))
        testWall->StartFalling();

    // G  →  Para a queda manualmente
    if (window->KeyPress('G'))
        testWall->StopFalling();

    // Numpad / 1-3  →  Define ângulo absoluto pré-definido
    if (window->KeyPress('1'))  testWall->SetAngle(0.0f);
    if (window->KeyPress('2'))  testWall->SetAngle(45.0f);
    if (window->KeyPress('3'))  testWall->SetAngle(90.0f);
    if (window->KeyPress('4'))  testWall->SetAngle(135.0f);

    // Setas  →  Move a wall manualmente no mundo
    const float moveSpeed = 200.0f;
    if (window->KeyDown(VK_LEFT))
        testWall->MoveTo(testWall->X() - moveSpeed * gameTime, testWall->Y());
    if (window->KeyDown(VK_RIGHT))
        testWall->MoveTo(testWall->X() + moveSpeed * gameTime, testWall->Y());
    if (window->KeyDown(VK_UP))
        testWall->MoveTo(testWall->X(), testWall->Y() - moveSpeed * gameTime);
    if (window->KeyDown(VK_DOWN))
        testWall->MoveTo(testWall->X(), testWall->Y() + moveSpeed * gameTime);

    // ── Fim dos controles de teste ──────────────────────────────

    LevelMake::Update();
}