#include "simple_collisions.h"

#include "core/engine.h"
#include "core/graphics/sprite.h"
#include "core/graphics/animations.h"
#include "gameplay/PandemicShop/pandemic_character_controller.h"
#include "gameplay/PandemicShop/item.h"
#include "gameplay/PandemicShop/karen.h"
#include <gameplay/PandemicShop/player.h>

using namespace dagger;
using namespace pandemic;

void SimpleCollisionsSystem::Run()
{
    auto &reg = Engine::Registry();
    auto view = reg.view<SimpleCollision, Transform>();
    auto bot_view = reg.view<CollisionType::Char, PandemicKarenCharacter, AICommand>();
    auto hero_view = reg.view<CollisionType::Char, PandemicCharacter>();
    auto wall_view = reg.view<CollisionType::Wall, SimpleCollision>();
    auto item_view = reg.view<Item>();


    auto all_it = view.begin();
    while (all_it != view.end())
    {
        auto& col = view.get<SimpleCollision>(*all_it);
        col.colided = false;
        all_it++;
    }

    int destroyed_items = 0;
    for(auto it:item_view){
        auto item = reg.get<Item>(it);
        if(item.hidden){
            destroyed_items ++;
        }
    }
    if(destroyed_items == 36){
        Engine::s_IsPaused = true;
    }

    auto it = bot_view.begin();
    while(it != bot_view.end())
    {
        auto &collision = view.get<SimpleCollision>(*it);
        auto &transform = view.get<Transform>(*it);

        auto it2 = wall_view.begin();
        while(it2 != wall_view.end())
        {
            auto &col = view.get<SimpleCollision>(*it2);
            auto &tr = view.get<Transform>(*it2);
            
            // processing one collision per frame for each colider
            if (collision.IsCollided(transform.position, col, tr.position))
            {
                collision.colidedWith = *it2;   
                col.colidedWith = *it;

                collision.colided = true;
                col.colided = true;
                auto &command = reg.get<AICommand>(*it);
                resolveWalls( collision, transform, col, tr, command);

            }
            it2++;
        }
        auto i_it2 = item_view.begin();
        while(i_it2 != item_view.end())
        {
            
            if(reg.has<SimpleCollision>(*i_it2)){
                auto &col = view.get<SimpleCollision>(*i_it2);
                auto &tr = view.get<Transform>(*i_it2);
                
                if (collision.IsCollided(transform.position, col, tr.position))
                {
                    collision.colidedWith = *i_it2;   
                    col.colidedWith = *it;

                    collision.colided = true;
                    col.colided = true;

                    auto &karen_command = reg.get<AICommand>(*it);
                    auto prob = rand() / (float) RAND_MAX;
                    Logger::info("\nPROB {}\n", prob);
                    if( prob > karen_command.pick_probability){
                        Logger::info("\nKaren pickup_prob {}\n", karen_command.pick_probability);
                        auto &item = reg.get<Item>(*i_it2);
                        auto &sprite = reg.get<Sprite>(*i_it2);
                        auto karen = KarenCharacter::Get(*it);
                        if(!item.hidden){
                            item.hidden = true;
                            item.pickable = true;
                            reg.remove<SimpleCollision>(*i_it2);
                            sprite.scale = {0, 0};
                            karen.inventory.emplace_back(*i_it2);
                            karen_command.picked = true;
                        }

                    }
                    resolveItem( collision, transform, col, tr, karen_command);
                    Logger::info("\nPICKED AFTER COLLISION: {}\n", karen_command.picked);

                }
            }
            i_it2++;
        }
        it++;
    }


    auto h_it = hero_view.begin();
    while(h_it != hero_view.end())
    {
        auto &collision = view.get<SimpleCollision>(*h_it);
        auto &transform = view.get<Transform>(*h_it);

        auto it2 = wall_view.begin();
        while(it2 != wall_view.end())
        {
            if(reg.has<SimpleCollision>(*it2)){
                            
                            
                auto &col = view.get<SimpleCollision>(*it2);
                auto &tr = view.get<Transform>(*it2);
                
                // processing one collision per frame for each colider
                if (collision.IsCollided(transform.position, col, tr.position))
                {
                    collision.colidedWith = *it2;   
                    col.colidedWith = *h_it;

                    collision.colided = true;
                    col.colided = true;

                    resolveDirection( collision, transform, col, tr);

                }
            }

            it2++;
        }
        auto i_it2 = item_view.begin();
        while(i_it2 != item_view.end())
        {
            if(reg.has<SimpleCollision>(*i_it2)){
                auto &col = view.get<SimpleCollision>(*i_it2);
                auto &tr = view.get<Transform>(*i_it2);
                
                if (collision.IsCollided(transform.position, col, tr.position))
                {
                    collision.colidedWith = *i_it2;   
                    col.colidedWith = *h_it;

                    collision.colided = true;
                    col.colided = true;

                }
            }
            i_it2++;
        }

        auto b_it2 = bot_view.begin();
        //da li se udario sa karen
        while(b_it2 != bot_view.end())
        {
            if(reg.has<SimpleCollision>(*b_it2)){
                auto &col = view.get<SimpleCollision>(*b_it2);
                auto &tr = view.get<Transform>(*b_it2);
                
                if (collision.IsCollided(transform.position, col, tr.position))
                {
                    collision.colidedWith = *b_it2;   
                    col.colidedWith = *h_it;

                    collision.colided = true;
                    col.colided = true;
                    Logger::info("Player hit a bot");
                    Engine::s_IsPaused = true;


                }
            }
            b_it2++;
        }

        h_it++;
    }
}

void SimpleCollisionsSystem::resolveWalls(SimpleCollision &collision, Transform &col_transform, 
                                            SimpleCollision &other, Transform& other_transform,
                                            AICommand& command ){
    
    
   
    resolveDirection(collision, col_transform, other, other_transform); 
    other.colided = false;
    collision.colided = false;
    command.previous = command.next;
    command.current = command.next;   
    // srand(time(0)); 
    
    command.next = {(rand() % (2*AISystem::border_width)) - AISystem::border_width, 
                    (rand() % (2*AISystem::border_height)) - AISystem::border_height};
    command.finishedX = false;
    command.finishedY = false;
    command.finished = false;
}


void SimpleCollisionsSystem::resolveItem(SimpleCollision &collision, Transform &col_transform, 
                                            SimpleCollision &other, Transform& other_transform,
                                            AICommand& command ){
    
    
    if(command.picked){
        resolveDirection(collision, col_transform, other, other_transform);
        command.picked = false;
        other.colided = false;
    }
    else{
        other.colided = false;
        collision.colided = false;
        command.previous = command.next;
        command.current = command.next;
        command.finishedX = true;
        command.finishedY = true;
        command.finished =  true;
        
           
    }
}




void SimpleCollisionsSystem::resolveDirection(SimpleCollision &collision, Transform &col_transform, SimpleCollision &other, Transform& other_transform ){
    
    if (collision.GetCollisionSides(col_transform.position, collision, other_transform.position).x == 1){
        col_transform.position.x -= collision.size.x/10.f;
    }
    else if(collision.GetCollisionSides(col_transform.position, collision, other_transform.position).x == -1){
        col_transform.position.x += collision.size.x/10.f;
    }
    else if(collision.GetCollisionSides(col_transform.position, collision, other_transform.position).y == 1){
        col_transform.position.y -= collision.size.y/10.f;
    }
    else if(collision.GetCollisionSides(col_transform.position, collision, other_transform.position).y == -1){
        col_transform.position.y += collision.size.y/10.f;
    }
}
// SimpleCollision

bool SimpleCollision::IsCollided(const Vector3& pos_, const SimpleCollision& other_, const Vector3& posOther_)
{
    Vector2 p(pos_.x + pivot.x * size.x, pos_.y + pivot.y * size.y);
    Vector2 p2(posOther_.x + other_.pivot.x * other_.size.x, posOther_.y + other_.pivot.y * other_.size.y);

    if (p.x < p2.x + other_.size.x &&
        p.x + size.x > p2.x &&
        p.y < p2.y + other_.size.y &&
        p.y + size.y > p2.y)
    {
        return true;
    }

    return false;
}

Vector2 SimpleCollision::GetCollisionSides(const Vector3& pos_, const SimpleCollision& other_, const Vector3& posOther_)
{
    Vector2 res(0, 0);

    Vector2 p(pos_.x + pivot.x * size.x, pos_.y + pivot.y * size.y);
    Vector2 p2(posOther_.x + other_.pivot.x * other_.size.x, posOther_.y + other_.pivot.y * other_.size.y);

    Float32 xDif = std::min(std::abs(p.x + size.x - p2.x), std::abs(p2.x + other_.size.x - p.x));
    Float32 yDif = std::min(std::abs(p.y + size.y - p2.y), std::abs(p2.y + other_.size.y - p.y));

    if (xDif < yDif)
    {
        res.x = std::abs(p.x + size.x - p2.x) < std::abs(p2.x + other_.size.x - p.x) ? 1 : -1;
    }
    else
    {
        res.y = std::abs(p.y + size.y - p2.y) < std::abs(p2.y + other_.size.y - p.y) ? 1 : -1;
    }

    return res;
}

Vector3 SimpleCollision::GetCollisionCenter(const Vector3& pos_, const SimpleCollision& other_, const Vector3& posOther_)
{
    Vector3 res;

    Vector2 p(pos_.x + pivot.x * size.x, pos_.y + pivot.y * size.y);
    Vector2 p2(posOther_.x + other_.pivot.x * other_.size.x, posOther_.y + other_.pivot.y * other_.size.y);

    res.x = (p.x + size.x / 2.f + p2.x + other_.size.x) / 2.f;
    res.y = (p.y + size.y / 2.f + p2.y + other_.size.y) / 2.f;
    res.z = 1.f;

    return res;
}

