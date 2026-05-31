#include "LevelSelect.h"
#include "Engine.h"
#include "LevelMake.h"
#include "Level2.h"
#include "LevelEditor.h"

// ─────────────────────────────────────────────────────────────────────────────
// Retorna true se o mundo (número da porta) está desbloqueado.
//
// Regras:
//   Mundo 1 (door 1) → sempre liberado
//   Mundo 2 (door 2) → LevelEditor, sempre liberado (ferramenta interna)
//   Mundo 3 (door 3) → exige ter concluído o Mundo 1  (nivelDesbloqueado >= 1)
//   Mundo 4 (door 4) → ainda não implementado, sempre bloqueado
// ─────────────────────────────────────────────────────────────────────────────
static bool MundoDesbloqueado(int numMundo)
{
    switch (numMundo)
    {
    case 1: return true;                              // Sempre liberado
    case 2: return LevelMake::nivelDesbloqueado >= 1; // Exige concluir Mundo 1
    case 3: return LevelMake::nivelDesbloqueado >= 2; // Exige concluir Mundo 2
    case 4: return LevelMake::nivelDesbloqueado >= 3; // Exige concluir Mundo 3
    default: return false;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
void LevelSelect::Init()
{
    scene = new Scene();
    backg = new Sprite("Resources/Mapa.png");
    transitionBlock = new Sprite("Resources/Tijolo.png");
    isOpening = true;
    openingTimer = 0.0f;

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
    LevelMake::SalvarProgresso();
    LevelMake::CarregarProgresso();
}

// ─────────────────────────────────────────────────────────────────────────────
void LevelSelect::Update()
{
    if (isOpening)
    {
        openingTimer += gameTime;
        if (openingTimer >= 0.5f)
            isOpening = false;
    }

    scene->Update();

    if (window->KeyPress('H')) {
        Engine::Next<LevelEditor>();
        return;
    }

    // ── Transição em andamento: aguarda timer e troca de cena ────────────────
    if (isTransitioning)
    {
        transitionTimer += gameTime;

        if (transitionTimer >= 0.5f)
        {
            switch (targetLevel)
            {
            case 1:
                LevelMake::mapasDoNivel = {
                    //"Resources/Mundo1_Fase1.txt",
                    /*"Resources/Mundo1_Fase2.txt",
                    "Resources/Mundo1_Fase3.txt",
                    "Resources/Mundo1_Fase4.txt",*/
                };
                LevelMake::nivelDesbloqueado = 1;
                LevelMake::faseAtual = 0;
                Engine::Next<Level2>();
                return;

            case 2:
                LevelMake::mapasDoNivel = {
                    //"Resources/Mundo1_Fase1.txt",
                    /*"Resources/Mundo2_Fase1.txt",
                    "Resources/Mundo2_Fase2.txt",
                    "Resources/Mundo2_Fase3.txt",
                    "Resources/Mundo2_Fase4.txt",*/
                };
                LevelMake::nivelDesbloqueado = 2;
                LevelMake::faseAtual = 0;
                Engine::Next<Level2>();
                return;

            case 3:
                LevelMake::mapasDoNivel = {
                    //"Resources/Mundo1_Fase1.txt",
                    /*"Resources/Mundo3_Fase1.txt",
                    "Resources/Mundo3_Fase2.txt",
                    "Resources/Mundo3_Fase3.txt",
                    "Resources/Mundo3_Fase4.txt",*/
                };
                LevelMake::faseAtual = 0;
                LevelMake::nivelDesbloqueado = 3;
                Engine::Next<Level2>();
                return;

            case 4:
                LevelMake::mapasDoNivel = {
                    "Resources/Mundo1_Fase1.txt",
                    /*"Resources/Mundo4_Fase1.txt",
                    "Resources/Mundo4_Fase2.txt",
                    "Resources/Mundo4_Fase3.txt",
                    "Resources/Mundo4_Fase4.txt",*/
                };
                LevelMake::faseAtual = 0;
                LevelMake::nivelDesbloqueado = 4;
                Engine::Next<Level2>();
            }
        }
        return;
    }

    // ── Input normal ─────────────────────────────────────────────────────────
    mouse->Update();

    for (MapDoor* door : doors)
    {
        const bool bloqueado = !MundoDesbloqueado(door->GetLevel());
        const bool colidindo = scene->Collision(mouse, door);

        if (colidindo && !bloqueado)
        {
            door->Select();

            if (mouse->Clicked())
            {
                targetLevel = door->GetLevel();
                isTransitioning = true;
                transitionTimer = 0.0f;
            }
        }
        else
        {
            // Porta bloqueada ou mouse fora: mantém visual padrão/fechado
            door->UnSelect();
        }

        door->Update();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
void LevelSelect::Draw()
{
    backg->Draw(window->CenterX(), window->CenterY(), Layer::BACK);
    scene->Draw();


    // ── Transição: quadrados aleatórios pretos ───────────────────────────────
    if (isTransitioning && blackBlock)
    {
        float progress = transitionTimer / 0.5f;
        if (progress > 1.0f) progress = 1.0f;

        Color preto(0.0f, 0.0f, 0.0f, 1.0f);
        const float size = 32.0f;

        int colunas = int((window->Width() / size) + 1);
        int linhas = int((window->Height() / size) + 1);

        for (int c = 0; c < colunas; c++)
        {
            for (int r = 0; r < linhas; r++)
            {
                int   numeroSorteado = ((c * 89) ^ (r * 43)) % 100;
                float momentoParaAparecer = numeroSorteado / 100.0f;

                if (progress >= momentoParaAparecer)
                {
                    float x = (c * size) + (size / 2.0f);
                    float y = (r * size) + (size / 2.0f);
                    blackBlock->Draw(x, y, Layer::FRONT, 2.0f, 0.0f, preto);
                }
            }
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
void LevelSelect::Finalize()
{
    delete backg;
    delete scene;
    if (blackBlock) { delete blackBlock;      blackBlock = nullptr; }
    if (transitionBlock) { delete transitionBlock; transitionBlock = nullptr; }
    doors.clear();
}