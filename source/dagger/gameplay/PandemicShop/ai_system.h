#pragma once 
#include "core/core.h"
#include "core/engine.h"
#include "core/graphics/animation.h"
#include "core/graphics/sprite.h"
#include "core/input/inputs.h"
#include "core/game/transforms.h"
#include <deque>
#include <map>

using namespace dagger;

enum struct Action {IDLE, UP, DOWN, RIGHT, LEFT};

struct AICommand
{
    Vector2 current, previous, next;
    Action curr_action = Action::IDLE;
    bool finished  = true;
    bool finishedX = true;
    bool finishedY = true;
    float pick_probability;
    bool picked = true;
  
};
class AISystem : public System
{
private: 
    
    float delay;
    float counter;
public:
    static const int border_height = 160;
    static const int border_width = 250;

    AISystem() { delay = 1.0f; counter = 0.0f;};
    inline String SystemName() { return "AI System"; }

    void Run() override;

};
