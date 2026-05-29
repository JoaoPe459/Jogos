#ifndef _PACMAN_KILLZONE_H_
#define _PACMAN_KILLZONE_H_

/**********************************************************************************
// KillZone (Arquivo de Cabeçalho)
//
// Descrição:   Zona retangular invisível que pode matar ou drenar HP do player.
//              Completamente separada de Wall/Block — não precisa de sprite.
//
//  lethal = true  → mata instantaneamente ao tocar
//  lethal = false → drena `damage` HP a cada `tickInterval` segundos
//
//  Para espinhos decorativos: crie o Wall normalmente (visual),
//  e uma KillZone com lethal=true na mesma posição (lógica de morte).
//  Assim você pode ter espinhos sem killzone (decorativos) simplesmente
//  não criando a KillZone correspondente.
**********************************************************************************/

#include "Object.h"
#include <string>

class Player;

class KillZone : public Object
{
public:
    // ── Configuração ──────────────────────────────────────────────
    bool        lethal        = true;   // true=morte instantânea, false=drena HP
    int         damage        = 10;     // dano por tick (só usado se !lethal)
    float       tickInterval  = 0.5f;   // segundos entre ticks de dano
    std::string tag;

    // ── Estado interno ────────────────────────────────────────────
    bool        active        = true;   // pode ser desligado sem remover da cena
    float       tickTimer     = 0.0f;

    // ── Construtor ────────────────────────────────────────────────
    // x, y    — centro da zona
    // w, h    — dimensões da bbox
    // lethal  — true=morte, false=dano por tick
    // tag     — identificador opcional
    KillZone(float x, float y, float w, float h,
             bool lethal = true,
             const std::string& tag = "");
    ~KillZone() = default;

    // ── Interface ─────────────────────────────────────────────────
    void SetActive(bool value)     { active = value; }
    bool IsActive()          const { return active; }

    void Update()                        override;
    void Draw()                          override;
    void OnCollision(Object* obj)        override;
};

#endif
