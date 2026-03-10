#pragma once
class Moves
{

private:
    float velX = 0;                     // velocidade horizontal do player
    float velY = 0;                     // velocidade vertical do player
    float speed = 0.0f;
    bool ghostMode = false;
    bool onGround = false;


public:

    Moves(float x, float y, float s, bool b) 
    {
        velX = x;
		velY = y;
		speed = s;
		ghostMode = b;

    };

    ~Moves() {};
    void Stop();                        // pára jogador
    void Up();                          // muda direção para cima
    void Down();                        // muda direção para baixo
    void Left();                        // muda direção para esquerda
	void Right();                       // muda direção para direita

    void setVelX(float x) { this->velX = x; };
    void setVelY(float y) { this->velY = y; };
    void setSpeed(float s);
    void setGhostMode(bool g) { this->ghostMode = g; };

    float getVelX();
    float getVelY();

    float getSpeed();
    bool getGhostMode();
    bool getOnGround();
    void setOnGround(bool g);

    // apply gravity over a time step (dt in seconds)
    void applyGravity(float dt);

};

