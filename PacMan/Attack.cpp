#include "Attack.h"
#include "PacMan.h"


Attack::Attack(Entity* creator, float lifeTime, float impulseX, float impulseY) {
    owner = creator;
    duration = lifeTime;
    timer = 0.0f;
    type = ATTACK;

    BBox(new Rect(-5, -5, 5, 5));

    MoveTo(owner->X(), owner->Y());

    mass = 0.5f;
    moves->setVelX(impulseX);
    moves->setVelY(impulseY);
}

Attack::~Attack() {

}


void Attack::Control() {
    
}

void Attack::Update() {
    // 1. Gestão de tempo
    timer += gameTime;
    if (timer >= duration) {
		this->Die();
        return;
    }

    // 2. Aplica Física
    ApplyPhysics();
}

void Attack::OnCollision(Object* obj) {
    // Se bater em alguma parede ele papoca
    if (obj->Type() == WALL) {
        alive = false; 
		this->Die();
    }
}

void Attack::Draw() {
    Rect* box = (Rect*)BBox();
    if (!box) return;
}

Entity* Attack::GetOwner() const {
    return owner;
}