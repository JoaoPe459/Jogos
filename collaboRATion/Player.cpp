#include "Player.h"
#include <cmath>
#include "PacMan.h"
#include "Engine.h"
#include "LevelMake.h"
#include "Portal.h"
#include "Enemy.h"
#include <string>
#include "Interactables.h"
#include "Level2.h"
#include "LevelSelect.h"

Player::Player() : Entity() {
    type = PLAYER;
    animation = new TileSet("Resources/Player/Rato.png", 64, 64, 15, 30);
    anim = new Animation(animation, 0.130f, true);  
    uint SeqRightIdle[1] = {29};
    uint SeqLeftIdle[1] = {0};
    uint SeqRightWalk[4] = { 28,27,26,25};
    uint SeqLeftWalk[4] = { 1,2,3,4};
    uint SeqRightJump[1] = { 15 };
    uint SeqLeftJump[1] = { 14 };
    uint SeqLeftDeath[9] = { 25,24,23,22,21,20,19,18,17 };
    uint SeqRightDeath[9] = { 5,6,7,8,9,10,11,12,13 };
    uint Death[1] = { 16 };

    // A spritesheet nao tem animacao vertical, entao W/S reaproveitam a fileira da direita.
    anim->Add(JUMPLEFT, SeqLeftJump, 1);
    anim->Add(JUMPRIGHT, SeqRightJump, 1);
    anim->Add(WALKLEFT, SeqLeftWalk, 4);
    anim->Add(WALKRIGHT, SeqRightWalk, 4);
    anim->Add(IDLELEFT, SeqLeftIdle, 1);
    anim->Add(IDLERIGHT, SeqRightIdle, 1);
    anim->Add(DEATHLEFT, SeqLeftDeath, 9);
    anim->Add(DEATHRIGHT, SeqRightDeath, 9);
    anim->Add(DEATH, Death, 1);

    state = IDLERIGHT;
    // Bounding box alinhada ao novo tamanho de quadro do rato.
    BBox(new Rect(-17, -25, 17, 25));
    moves->setSpeed(500.0f);

    type = PLAYER;      
    SetMaxHp(100);      
    SetHp(100);
    setMass(1.5f);

    calories = 0.0f;
    stamina = 100.0f;
    damage = 15;

}

void Player::OnCollision(Object* obj) {

    if (isDead) {
        return;
    }
    else if (obj->Type() == TYPE_DOOR) {

        LevelMake::avancarFase = true;        
        //this->MoveTo(-9999.0f, -9999.0f);
        moves->setVelX(0.0f);
        moves->setVelY(0.0f);

        return;
    }
    if (obj->Type() == WALL) {
        Block* block = (Block*)obj;

        float pLeft = this->X() - 17.0f;
        float pRight = this->X() + 17.0f;

        float bLeft = obj->X() - (block->width * 0.5f);
        float bRight = obj->X() + (block->width * 0.5f);
        float bTop = obj->Y() - (block->height * 0.5f);
        float bBottom = obj->Y() + (block->height * 0.5f);

        float prevBottom = prevY + 25.0f;
        float prevTop = prevY - 25.0f;

        float gravAtual = LevelMake::GetGravity();

        if (prevBottom <= bTop + 16.0f && moves->getVelY() >= 0.0f) {
            if (pRight > bLeft + 4.0f && pLeft < bRight - 4.0f) {
                this->MoveTo(this->X(), bTop - 25.0f);
                moves->setVelY(0.0f);

                // Só é "chão" se a gravidade estiver puxando para BAIXO
                if (gravAtual > 0.0f) moves->setOnGround(true);
                return;
            }
        }

        // 2. Colisão batendo por BAIXO do bloco (O Teto)
        if (prevTop >= bBottom - 8.0f && moves->getVelY() < 0.0f) {
            if (pRight > bLeft + 4.0f && pLeft < bRight - 4.0f) {
                this->MoveTo(this->X(), bBottom + 25.1f);
                moves->setVelY(0.0f);

                // MÁGICA: Se a gravidade estiver invertida, o TETO é o novo CHÃO!
                if (gravAtual < 0.0f) moves->setOnGround(true);
                return;
            }
        }

        float overlapTop = (this->Y() + 25.0f) - bTop;
        float overlapBottom = bBottom - (this->Y() - 25.0f);

        if (overlapTop > 4.0f && overlapBottom > 4.0f) {
            float overlapLeft = pRight - bLeft;
            float overlapRight = bRight - pLeft;

            if (overlapLeft < overlapRight) {
                this->MoveTo(bLeft - 17.1f, this->Y());
            }
            else {
                this->MoveTo(bRight + 17.1f, this->Y());
            }
            moves->setVelX(0.0f);
        }

        return;
    }


    Entity::OnCollision(obj);

    if (obj->Type() == TYPE_SPIKE || obj->Type() == KILLZONE) {
        if (!isDead) { Die(); }
        return;
    }

    else if (obj->Type() == TYPE_BUTTON) {
        ButtonObj* btn = (ButtonObj*)obj;

        if (!btn->IsPressed()) {
            btn->Press();
        }
    }
}

void Player::Die() {
    LevelMake::audioEngine->Volume(LevelMake::SoundIDs::DEATH_ID,0.05f);
    LevelMake::audioEngine->Play(LevelMake::SoundIDs::DEATH_ID);
    if (isDead) return;
    isDead = true;
    deathTimer = 1.5f; 
    SetHp(100);
    LevelMake::deathCount++;
    

    moves->setVelX(0.0f);
    moves->setVelY(-600.0f);  
    moves->setOnGround(false);

    if (state == WALKLEFT || state == IDLELEFT || state == JUMPLEFT)
        state = DEATHLEFT;
    else
        state = DEATHRIGHT;
}

void Player::Control() {
    if (isDead) {
        moves->setVelX(0.0f);
        static float frameTimer = 0.0f;
        frameTimer += gameTime;
        anim->Select(state);
        if (frameTimer >= 0.01f) {
            anim->NextFrame();
            frameTimer = 0.0f;
        }
        return;
    }

    float baseSpeed = moves->getSpeed() - (sizeLevel * 10.0f);
    float accelerationRate = 4.0f;
    float targetVX = 0;

    // ========================================================
    // 1. LER TECLADO
    // ========================================================
    bool btnEsq = window->KeyDown('A');
    bool btnDir = window->KeyDown('D');

    // --- MODIFICADOR 1: INVERTER CONTROLES ---
    if (LevelMake::modInvertControls) {
        std::swap(btnEsq, btnDir); // A esquerda vira direita e vice-versa
    }

    // ========================================================
    // 2. LÓGICA HORIZONTAL (Andar)
    // ========================================================
    bool gravInvertida = (LevelMake::GetGravity() < 0.0f);

    if (btnEsq) {
        if (!LevelMake::modMoveWorld) targetVX = -baseSpeed;

        // Se a gravidade está invertida, usamos a animação da DIREITA para ele olhar para a ESQUERDA
        if (gravInvertida) {
            state = moves->getOnGround() ? WALKRIGHT : JUMPRIGHT;
        }
        else {
            state = moves->getOnGround() ? WALKLEFT : JUMPLEFT;
        }
    }
    else if (btnDir) {
        if (!LevelMake::modMoveWorld) targetVX = baseSpeed;

        if (gravInvertida) {
            state = moves->getOnGround() ? WALKLEFT : JUMPLEFT;
        }
        else {
            state = moves->getOnGround() ? WALKRIGHT : JUMPRIGHT;
        }
    }
    else {
        // Se soltar os botões, mantém a direção (esquerda ou direita)
        if (state == WALKLEFT || state == IDLELEFT || state == JUMPLEFT) {
            state = moves->getOnGround() ? IDLELEFT : JUMPLEFT;
        }
        else {
            state = moves->getOnGround() ? IDLERIGHT : JUMPRIGHT;
        }
    }

    // ========================================================
    // 3. LÓGICA VERTICAL (Pulo)
    // ========================================================
    if (( window->KeyPress('W') || window->KeyPress(VK_SPACE)) && moves->getOnGround()) {
        
        LevelMake::audioEngine->Volume(LevelMake::SoundIDs::JUMP_ID,0.15f);
        LevelMake::audioEngine->Play(LevelMake::SoundIDs::JUMP_ID);

        if (LevelMake::modGravityJump) {
            float gravAtual = LevelMake::GetGravity();

            LevelMake::SetGravity(-gravAtual);

            if (gravAtual > 0) {
                moves->setVelY(-50.0f);
            }
            else {
                moves->setVelY(50.0f); 
            }
        }
        // --- PULO NORMAL / GIGANTE ---
        else {
            moves->Up();
            moves->setVelY(moves->getVelY() * 0.75f * LevelMake::modJumpForce);
        }

        moves->setOnGround(false);

        if (state == WALKLEFT || state == IDLELEFT) state = JUMPLEFT;
        else state = JUMPRIGHT;
    }

    anim->Select(state);
    anim->NextFrame();

    // ========================================================
    // 4. APLICAÇÃO DA INÉRCIA HORIZONTAL
    // ========================================================
    float currentVX = moves->getVelX();
    float lerpFactor = accelerationRate * gameTime;
    if (lerpFactor > 1.0f) lerpFactor = 1.0f;

    moves->setVelX(currentVX + (targetVX - currentVX) * lerpFactor);
    if (attackTimer > 0) { attackTimer -= gameTime; }
}

void Player::Draw()
{
    float angulo = 0.0f;

    // Se a gravidade for negativa, roda a imagem 180 graus (PI radianos)
    if (LevelMake::GetGravity() < 0.0f) {
        angulo = 180;
    }

    // Passamos o X, Y, Z, a Escala (1.0f) e o nosso novo Ângulo!
    anim->Draw(x, y, z, 1.0f, angulo);
}

void Player::Update() {
    prevY = Y();
    Entity::Update();
    

    if (isDead) {
        deathTimer -= gameTime;  // ← aqui, 1x por frame

        moves->setVelX(0.0f);

        static float frameTimer = 0.0f;
        frameTimer += gameTime;
        anim->Select(state);
        if (frameTimer >= 0.009f) {
            anim->NextFrame();
            frameTimer = 0.0f;
        }

        if (deathTimer <= 1.0f)
            state = DEATH;

        return;
    }
    Control();
    moves->setOnGround(false);
}


Player::~Player() {
    delete anim;
    delete animation;
}
