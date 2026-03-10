/**********************************************************************************
// Player (Código Fonte)
// 
// Criação:     01 Jan 2013
// Atualização: 04 Mar 2023
// Compilador:  Visual C++ 2022
//
// Descrição:   Player do jogo PacMan
//
**********************************************************************************/

#include "PacMan.h"
#include "Player.h"
#include "Pivot.h"

// ---------------------------------------------------------------------------------

Player::Player()
{
    spriteL = new Sprite("Resources/PacManL.png");
    spriteR = new Sprite("Resources/PacManR.png");
    spriteU = new Sprite("Resources/PacManU.png");
    spriteD = new Sprite("Resources/PacManD.png");

    // imagem do pacman é 48x48 (com borda transparente de 4 pixels)
    BBox(new Rect(-20, -20, 20, 20));
    MoveTo(480.0f, 450.0f);
    type = PLAYER;

    moves = new Moves(0, 0, 200, false);

}

// ---------------------------------------------------------------------------------

Player::~Player()
{
    delete spriteL;
    delete spriteR;
    delete spriteU;
    delete spriteD;
    delete moves;
    moves = nullptr;
}


// ---------------------------------------------------------------------------------



// ---------------------------------------------------------------------------------

void Player::OnCollision(Object * obj)
{
    if (moves->getGhostMode()) {
        return;
    }

    // Se algum dos objetos não tiver caixa delimitadora, ignore
    if (!this->BBox() || !obj->BBox())
        return;

    // Por enquanto, só é possível lidar com retângulos contra retângulos.
    if (this->BBox()->Type() != RECTANGLE_T || obj->BBox()->Type() != RECTANGLE_T)
        return;

    Rect* r1 = (Rect*)this->BBox(); // jogador
    Rect* r2 = (Rect*)obj->BBox();  // outro

    // Calcula a área de sobreposição
    float left = (r1->Left() > r2->Left()) ? r1->Left() : r2->Left();
    float right = (r1->Right() < r2->Right()) ? r1->Right() : r2->Right();
    float top = (r1->Top() > r2->Top()) ? r1->Top() : r2->Top();
    float bottom = (r1->Bottom() < r2->Bottom()) ? r1->Bottom() : r2->Bottom();

    float overlapX = right - left;
    float overlapY = bottom - top;

	// Sem colisão se não houver sobreposição
    if (overlapX <= 0.0f || overlapY <= 0.0f)
        return;

    // Centros
    float cx1 = (r1->Left() + r1->Right()) * 0.5f;
    float cy1 = (r1->Top() + r1->Bottom()) * 0.5f;
    float cx2 = (r2->Left() + r2->Right()) * 0.5f;
    float cy2 = (r2->Top() + r2->Bottom()) * 0.5f;

    // resolve com vetor de translação mínimo
    if (overlapX < overlapY)
    {
        // resolução horizontal
        if (cx1 < cx2)
        {
			// Bota pro lado esquerdo
            Translate(-overlapX, 0.0f);
        }
        else
        {
            // Bota pro lado direito
            Translate(overlapX, 0.0f);
        }

		// Para o movimento horizontal
        moves->setVelX(0.0f);
    }
    else
    {
        if (cy1 < cy2)
        {
            Translate(0.0f, -overlapY);
            moves->setVelY(0.0f);
            moves->setOnGround(true);
        }
        else
        {
            Translate(0.0f, overlapY);
            moves->setVelY(0.0f);
        }
    }

}



// ---------------------------------------------------------------------------------

void Player::Update() {
    // 1. Processa a entrada do jogador
    HandleInput();

    // 2. Aplica a gravidade ao movimento
    moves->applyGravity(gameTime);

    // 3. Aplica o movimento baseado na velocidade e tempo
    ApplyMovement();

    // 3. Garante que o jogador permanece nos limites (Teletransporte)
    ScreenWrap();
}

// --- Funções Auxiliares (Encapsuladas) ---

void Player::HandleInput() {

    if (window->KeyPress('I')) {
        incrementSpeed();
    }

    if (window->KeyPress('O')) {
        decressSpeed();
    }

    // Esquerda
    if (window->KeyDown(VK_LEFT)) {
        nextState = LEFT;
        moves->Left();
    }

    // Direita
    if (window->KeyDown(VK_RIGHT)) {
		nextState = RIGHT;
        moves->Right();
    }

    // Cima
    if (window->KeyDown(VK_UP)) {
        // jump only when on ground
        if (moves->getOnGround()) {
            nextState = UP;
            moves->Up();
            moves->setOnGround(false);
        }
    }

    // Baixo
    if (window->KeyDown(VK_DOWN)) {
		nextState = DOWN;
        moves->Down();
    }
    

}

void Player::ApplyMovement() {
    Translate(moves->getVelX() * gameTime, moves->getVelY() * gameTime);
}

void Player::ScreenWrap() {
    // Limite Esquerda -> Direita
    if (x + 20 < 0)
        MoveTo(window->Width() + 20.0f, Y());

    // Limite Direita -> Esquerda
    if (x - 20 > window->Width())
        MoveTo(-20.0f, Y());

    // Vertical: não teleporta mais. Mantém o jogador dentro da tela e zera
    // a velocidade vertical ao tocar nas bordas superior/inferior.
    // Limite Superior
    if (Y() - 20.0f < 0.0f) {
        MoveTo(x, 20.0f);
        moves->setVelY(0.0f);
        moves->setOnGround(false);
    }

    // Limite Inferior (chão)
    if (Y() + 20.0f > window->Height()) {
        MoveTo(x, window->Height() - 20.0f);
        moves->setVelY(0.0f);
        moves->setOnGround(true);
    }
}


void Player::incrementSpeed()
{
    moves->setSpeed(moves->getSpeed() * 2);
}

void Player::decressSpeed() {
    moves->setSpeed(moves->getSpeed() / 2);
}



// ---------------------------------------------------------------------------------

void Player::Draw()
{ 
    switch(currState)
    {
    case LEFT:  spriteL->Draw(x, y, Layer::UPPER); break;
    case RIGHT: spriteR->Draw(x, y, Layer::UPPER); break;
    case UP:    spriteU->Draw(x, y, Layer::UPPER); break;
    case DOWN:  spriteD->Draw(x, y, Layer::UPPER); break;
    default: 
        switch(nextState)
        {
        case LEFT:  spriteL->Draw(x, y, Layer::UPPER); break;
        case RIGHT: spriteR->Draw(x, y, Layer::UPPER); break;
        case UP:    spriteU->Draw(x, y, Layer::UPPER); break;
        case DOWN:  spriteD->Draw(x, y, Layer::UPPER); break;
        default:    spriteL->Draw(x, y, Layer::UPPER);
        }
    }
}

// ---------------------------------------------------------------------------------

