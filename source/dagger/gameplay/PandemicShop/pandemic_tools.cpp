
#include "gameplay/PandemicShop/pandemic_tools.h"
#include "tools/plotvar.h"
#include "core/engine.h"
#include "gameplay/common/simple_collisions.h"
#include "core/game/transforms.h"
#include "core/input/inputs.h"
#include "core/graphics/window.h"
#include "gameplay/PandemicShop/pandemic_player_input.h"
#include "core/graphics/sprite.h"
#include "core/graphics/animations.h"
#include "gameplay/PandemicShop/pandemic_player_input.h"
#include "gameplay/PandemicShop/pandemic_character_controller.h"
#include <imgui/imgui.h>
#include <spdlog/spdlog.h>
#include "gameplay/PandemicShop/item.h"


using namespace pandemic_shop;
using namespace pandemic;
void CollisionDetectionSystem::Tick()
{
	m_FrameCounter++;
}

void CollisionDetectionSystem::RenderGUI()
{
	ImGui::SetNextWindowSize(ImVec2(200, 60), ImGuiCond_FirstUseEver);
	ImGui::Begin("Collisions");
	{
		auto isPaused = dagger::Engine::s_IsPaused;
		ImGui::Text(isPaused ? "SYSTEMS PAUSED" : "SYSTEMS RUNNING");
	}
	ImGui::Separator();

	auto &reg = Engine::Registry();

	auto moving_obj = reg.view<Sprite, Transform, SimpleCollision, Animator, PandemicCharacter>();
	auto static_obj = reg.view<Sprite, Transform, SimpleCollision>();

	static_obj.each([&](Entity ent, Sprite &sprite_, Transform& transform_, SimpleCollision &col_)
		{
			const auto& other = col_.colidedWith;

			if(reg.valid(other)){
				if(reg.has<Sprite, Transform, SimpleCollision, Animator, PandemicCharacter>(other)){
					
					if(reg.has<Item>(ent)){
						if(reg.valid(ent)){
							ImGui::Text("ITEM");
							auto &transf = reg.get<Transform>(other);
							ImGui::Text("Player position: %f %f", transf.position.x, transf.position.y);
							ImGui::Text("Colided with position: %f %f", transform_.position.x, transform_.position.y);
						}
						
					}
					else{
						auto &transf = reg.get<Transform>(other);
						ImGui::Text("Player position: %f %f", transf.position.x, transf.position.y);
						ImGui::Text("Colided with position: %f %f", transform_.position.x, transform_.position.y);
					}
					
					
				}
			}
			
		});

	ImGui::End();
}

void CollisionDetectionSystem::SpinUp()
{
	Engine::Dispatcher().sink<GUIRender>().connect<&CollisionDetectionSystem::RenderGUI>(this);
	Engine::Dispatcher().sink<NextFrame>().connect<&CollisionDetectionSystem::Tick>(this);
}

void CollisionDetectionSystem::Run()
{
	
}

void CollisionDetectionSystem::WindDown()
{
	Engine::Dispatcher().sink<NextFrame>().disconnect<&CollisionDetectionSystem::Tick>(this);
	Engine::Dispatcher().sink<GUIRender>().disconnect<&CollisionDetectionSystem::RenderGUI>(this);
}
