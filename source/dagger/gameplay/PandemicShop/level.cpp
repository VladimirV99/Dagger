#include "level.h"

#include "core/game/transforms.h"

#include "core/graphics/sprite.h"
#include "core/graphics/animation.h"

#include "gameplay/PandemicShop/level_system.h"
#include "gameplay/PandemicShop/item.h"
#include "gameplay/common/simple_collisions.h"

#include "gameplay/PandemicShop/player.h"
#include "gameplay/PandemicShop/karen.h"

using namespace dagger;
using namespace pandemic;

unsigned Level::LEVEL_WIDTH = 0;
unsigned Level::LEVEL_HEIGHT = 0;

void Level::Load(String map)
{
	auto& reg = Engine::Registry();
	reg.clear();

	const auto* level = Engine::Res<LoadedData>()[map];

	LEVEL_WIDTH = level->mapWidth;
	LEVEL_HEIGHT = level->mapHeight;

    Vector2 scale(1, 1);
    constexpr float Space = 0.1f;

    Logger::info(LEVEL_HEIGHT);

	for (unsigned y = 0; y < LEVEL_HEIGHT; y++) {

		for(unsigned x = 0; x < LEVEL_WIDTH; x++) {

            Logger::info(level->tilemap[y][x]);
            if(level->tilemap[y][x] != 0){

                auto entity = reg.create();
                auto& sprite = reg.emplace<Sprite>(entity);
                if(level->name == "default"){
                    AssignSprite(sprite, "PandemicShop:tile");
                }else{
                     AssignSprite(sprite, "PandemicShop:tile4");
                }
                sprite.size = scale * TILE_WIDTH;


                auto& transform = reg.emplace<Transform>(entity);
                transform.position = { getPosition(x, y), 1};
            }

            if(level->tilemap[y][x] == 0){
                auto entity = reg.create();
                auto& sprite = reg.emplace<Sprite>(entity);
                AssignSprite(sprite, "PandemicShop:borderH");
                sprite.size = scale * TILE_WIDTH;

                sprite.color.r = 1.0f;
                sprite.color.g = 1.0f;
                sprite.color.b = 1.0f;

                auto &col = reg.emplace<SimpleCollision>(entity);
                reg.emplace<CollisionType::Wall>(entity);
                col.size.x = TILE_WIDTH;
                col.size.y = TILE_WIDTH;

                auto& transform = reg.emplace<Transform>(entity);
                transform.position = { getPosition(x, y), 1};

            }else if(level->tilemap[y][x] == 1){
                Character::Create("Pandemic", {1, 1, 1}, getPosition(x, y));

            }
            else if(level->tilemap[y][x] == 2){
                KarenCharacter::Create("Pandemic", {1, 1, 1}, getPosition(x, y));

            }
            else if(level->tilemap[y][x] == 3){
                auto entTR1 = reg.create();
                auto& spriteTR1 = reg.emplace<Sprite>(entTR1);
                AssignSprite(spriteTR1, "PandemicShop:tree1");
                auto &colliderTR1 = reg.emplace<SimpleCollision>(entTR1);
                reg.emplace<CollisionType::Wall>(entTR1);
                colliderTR1.size.x = 32;
                colliderTR1.size.y = 32;
                auto& transformTR1 = reg.emplace<Transform>(entTR1);
                transformTR1.position = { getPosition(x, y), 0};   


            }else if(level->tilemap[y][x] == 4){
                auto entTR2 = reg.create();
                auto& spriteTR2 = reg.emplace<Sprite>(entTR2);
                AssignSprite(spriteTR2, "PandemicShop:plant");
                auto &colliderTR2 = reg.emplace<SimpleCollision>(entTR2);
                reg.emplace<CollisionType::Wall>(entTR2);
                colliderTR2.size.x = 32;
                colliderTR2.size.y = 32;
                auto& transformTR2 = reg.emplace<Transform>(entTR2);
                transformTR2.position = { getPosition(x, y), 0}; 

            }else if(level->tilemap[y][x] == 10) {

                auto entTS = reg.create();
                auto& spriteTS = reg.emplace<Sprite>(entTS);
                AssignSprite(spriteTS, "PandemicShop:shelf1");
                auto &colliderTS = reg.emplace<SimpleCollision>(entTS);
                reg.emplace<CollisionType::Wall>(entTS);
                colliderTS.size.x = 100;
                colliderTS.size.y = 64;
                auto& transformTS = reg.emplace<Transform>(entTS);
                transformTS.position = { getPosition(x, y), 0};
				spriteTS.position = { getPosition(x, y), 0 };

			} else if (level->tilemap[y][x] == 11){
                auto entS1 = reg.create();
                auto& spriteS1 = reg.emplace<Sprite>(entS1);
                AssignSprite(spriteS1, "PandemicShop:shelf2");
                auto &colliderS1 = reg.emplace<SimpleCollision>(entS1);
                reg.emplace<CollisionType::Wall>(entS1);
                colliderS1.size.x = 32;
                colliderS1.size.y = 32;
                auto& transformS1 = reg.emplace<Transform>(entS1);
                transformS1.position = { getPosition(x, y), 0 };

			}else if(level->tilemap[y][x] == 12){
                auto entS1 = reg.create();
                auto& spriteS1 = reg.emplace<Sprite>(entS1);
                AssignSprite(spriteS1, "PandemicShop:kasa");
                auto &colliderS1 = reg.emplace<SimpleCollision>(entS1);
                reg.emplace<CollisionType::Wall>(entS1);
                colliderS1.size.x = 84;
                colliderS1.size.y = 103;
                auto& transformS1 = reg.emplace<Transform>(entS1);
                transformS1.position = { getPosition(x, y), 0 };

            }else if(level->tilemap[y][x] == 20){
                auto ent = reg.create();
                auto item = reg.emplace<Item>(ent);
                item.Create(ent, "spritesheets:pixel_mart:white_cheese", {1, 1, 1}, getPosition(x, y));

            }else if(level->tilemap[y][x] == 21){
                auto ent = reg.create();
                auto item = reg.emplace<Item>(ent);
                item.Create(ent, "spritesheets:pixel_mart:bacon", {1, 1, 1}, getPosition(x, y));

            }else if(level->tilemap[y][x] == 22){
                auto ent = reg.create();
                auto item = reg.emplace<Item>(ent);
                item.Create(ent, "spritesheets:pixel_mart:banana", {1, 1, 1}, getPosition(x, y));

            }else if(level->tilemap[y][x] == 23){
                auto ent = reg.create();
                auto item = reg.emplace<Item>(ent);
                item.Create(ent, "spritesheets:pixel_mart:bell_pepper", {1, 1, 1}, getPosition(x, y));

            }else if(level->tilemap[y][x] == 24){
                auto ent = reg.create();
                auto item = reg.emplace<Item>(ent);
                item.Create(ent, "spritesheets:pixel_mart:cabbage", {1, 1, 1}, getPosition(x, y));

            }else if(level->tilemap[y][x] == 25){
                auto ent = reg.create();
                auto item = reg.emplace<Item>(ent);
                item.Create(ent, "spritesheets:pixel_mart:cereal1", {1, 1, 1}, getPosition(x, y));

            }else if(level->tilemap[y][x] == 26){
                auto ent = reg.create();
                auto item = reg.emplace<Item>(ent);
                item.Create(ent, "spritesheets:pixel_mart:orange_juice", {1, 1, 1}, getPosition(x, y));

            }else if(level->tilemap[y][x] == 27){
                auto ent = reg.create();
                auto item = reg.emplace<Item>(ent);
                item.Create(ent, "spritesheets:pixel_mart:egg_box", {1, 1, 1}, getPosition(x, y));

            }else if(level->tilemap[y][x] == 28){
                auto ent = reg.create();
                auto item = reg.emplace<Item>(ent);
                item.Create(ent, "spritesheets:pixel_mart:flour", {1, 1, 1}, getPosition(x, y));

            }else if(level->tilemap[y][x] == 30){
                auto ent = reg.create();
                auto item = reg.emplace<Item>(ent);
                item.Create(ent, "spritesheets:pixel_mart:bathroom_cleaner", {1, 1, 1}, getPosition(x, y));

            }else if(level->tilemap[y][x] == 31){
                auto ent = reg.create();
                auto item = reg.emplace<Item>(ent);
                item.Create(ent, "spritesheets:pixel_mart:cleaning_gloves", {1, 1, 1}, getPosition(x, y));

            }else if(level->tilemap[y][x] == 32){
                auto ent = reg.create();
                auto item = reg.emplace<Item>(ent);
                item.Create(ent, "spritesheets:pixel_mart:detergent", {1, 1, 1}, getPosition(x, y));

            }else if(level->tilemap[y][x] == 33){
                auto ent = reg.create();
                auto item = reg.emplace<Item>(ent);
                item.Create(ent, "spritesheets:pixel_mart:hand_sanitiser", {1, 1, 1}, getPosition(x, y));

            }else if(level->tilemap[y][x] == 34){
                auto ent = reg.create();
                auto item = reg.emplace<Item>(ent);
                item.Create(ent, "spritesheets:pixel_mart:water", {1, 1, 1}, getPosition(x, y));

            }else if(level->tilemap[y][x] == 35){
                auto ent = reg.create();
                auto item = reg.emplace<Item>(ent);
                item.Create(ent, "spritesheets:pixel_mart:wet_wipe", {1, 1, 1}, getPosition(x, y));

            }else if(level->tilemap[y][x] == 36){
                auto ent = reg.create();
                auto item = reg.emplace<Item>(ent);
                item.Create(ent, "spritesheets:pixel_mart:cleaning_brush", {1, 1, 1}, getPosition(x, y));

            }else if(level->tilemap[y][x] == 37){
                auto ent = reg.create();
                auto item = reg.emplace<Item>(ent);
                item.Create(ent, "spritesheets:pixel_mart:kitchen_soap", {1, 1, 1}, getPosition(x, y));

            }else if(level->tilemap[y][x] == 38){
                auto ent = reg.create();
                auto item = reg.emplace<Item>(ent);
                item.Create(ent, "spritesheets:pixel_mart:shampoo", {1, 1, 1}, getPosition(x, y));

            }
		}
	}
}

Vector2 Level::getPosition(int x, int y)
{
	return { getPositionX(x), getPositionY(y)};
}

float Level::getPositionX(int x)
{
	return x * TILE_WIDTH + TILE_WIDTH/2 - 350;
}

float Level::getPositionY(int y)
{
	return y * TILE_HEIGHT + TILE_HEIGHT/2 - 190;
}