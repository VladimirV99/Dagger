#include "gameplay/PandemicShop/ai_system.h"
#include "gameplay/PandemicShop/item.h"
#include "gameplay/PandemicShop/karen.h"
#include <time.h>
void AISystem::Run(){


    auto &reg = Engine::Registry();
    auto item_view = reg.view<Item>();
    auto karens = reg.view<PandemicKarenCharacter>();
    

    counter += Engine::DeltaTime();
	if (counter >= delay) {
		counter = 0.0f;
        for(auto it_karen:karens){
            auto karen = KarenCharacter::Get(it_karen);
            
            Vector2 karen_pos = {karen.transform.position.x, karen.transform.position.y};

            if(karen.command.finished || karen.command.picked){
                Vector2 next_position = {(rand() % (2*border_width)) - border_width, rand() % (2*border_height) - border_height};
                karen.command.finished = false;
                karen.command.finishedY = false;
                karen.command.finishedX = false;
                karen.command.next = next_position;

            }
            auto &prev = karen.command.previous;
            auto &curr = karen.command.current;
            auto &next = karen.command.next;
            
            Vector2 distanceVec = Vector2(next.x - curr.x, next.y - curr.y);
            Vector2 distancePrev = Vector2(next.x - prev.x, next.y - prev.y);
            auto curr_distance = glm::length(distanceVec);
            auto prev_distance = glm::length(distancePrev);

            // Logger::info("\nNext position: {} {}", karen.command.next.x, karen.command.next.y);
            // Logger::info("\nCurrent position: {} {}", karen.command.current.x, karen.command.current.y);


            // Logger::info("\nDISTANCE {}\n", curr_distance);
            if(abs(curr_distance) < 1){
                // prev = curr;
                // Logger::info("curr_distance < 1");
                karen.command.previous = karen.command.next;
                // curr = next;
                karen.command.current = karen.command.next;
                karen.command.finished=true;
                karen.command.finishedX=true;
                karen.command.finishedY=true;
                continue;
            }

            if(abs(curr_distance) <= abs(prev_distance)){
                // Logger::info("\nCURR DIST IS LESS THEN PREV DIST {}\n", karen.command.finishedX);
                if (!karen.command.finishedX) {
                    // Logger::info("\nMOVING HORIZONTALLY\n");
                    if (distanceVec.x < 0) 
                        karen.command.curr_action = Action::LEFT;
                    else 
                        karen.command.curr_action = Action::RIGHT;

                }
                else if(!karen.command.finishedY && karen.command.finishedX){
                    // Logger::info("\nMOVING VERTICALY\n");
                    if (distanceVec.y < 0) 
                        karen.command.curr_action = Action::UP;
                    else 
                        karen.command.curr_action = Action::DOWN;

                }
            }
            else{
                // Logger::info("\nCURR DIST IS BIGGER THEN PREV DIST\n");
                if ( !karen.command.finishedX) {
                    if (distanceVec.x < 0) 
                        karen.command.curr_action = Action::RIGHT;
                    else 
                        karen.command.curr_action = Action::LEFT;

                }
                else if(!karen.command.finishedY && karen.command.finishedX){
                    if (distanceVec.y < 0) 
                        karen.command.curr_action = Action::DOWN;
                    else 
                        karen.command.curr_action = Action::UP;

                }
            }

        }
    }
    
}