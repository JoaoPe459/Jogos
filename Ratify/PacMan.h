/**********************************************************************************
// PacMan (Arquivo de Cabeçalho)
//
// Criação:     01 Jan 2013
// Atualização: 04 Mar 2023
// Compilador:  Visual C++ 2022
//
// Descrição:   Exercício sobre detecção de colisão
//
**********************************************************************************/

#ifndef _PACMAN_H_
#define _PACMAN_H_

// ------------------------------------------------------------------------------

enum PacManObjects
{
    ATTACK,
    ENEMY,
    ENTITY,
    FOOD,
    FLOOR,
    GHOST,
    PIVOT,
    PLAYER,
    PORTAL,
    SPECIAL,
    WALL
};

enum MovementType { HORIZONTAL, VERTICAL, DIAGONAL };

// Limites aproximados do chao desenhado no background 1200x800 centralizado na janela 1300x800.
// Player e inimigos usam este retangulo para nao pisar sobre as paredes do sprite.
namespace PlayArea
{
    constexpr float Left = 182.0f;
    constexpr float Right = 1118.0f;
    constexpr float Top = 132.0f;
    constexpr float Bottom = 666.0f;
    constexpr float SpawnMargin = 90.0f;
}

// ------------------------------------------------------------------------------

#endif

