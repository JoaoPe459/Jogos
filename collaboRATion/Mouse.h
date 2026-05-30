#ifndef _MOUSE_H_
#define _MOUSE_H_

#include "Object.h"

class Mouse : public Object
{
public:
    Mouse();
    ~Mouse();

    void Update() override;
    void Draw() override;

    bool Clicked();
};

#endif