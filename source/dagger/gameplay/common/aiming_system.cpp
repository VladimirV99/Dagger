#define _USE_MATH_DEFINES

#include "aiming_system.h"

#include "core/graphics/sprite.h"
#include "core/input/inputs.h"

#include <cmath>

void AimingSystem::Run()
{
	// The sprite component with the same entity as this crosshair component is of a sprite that is used for center of
	// rotation (like character for example)
	Engine::Registry().view<InputReceiver, Sprite, Crosshair>().each(
		[](InputReceiver input_, Sprite& sprite_, Crosshair& crosshair_)
		{
			/*
			Example setup of for "rotate" command in input-context :
				"command-name": "rotate",
				"actions" : [
					{
						"trigger": "KeyA",
						"value" : 0.05
					},
					{
						"trigger": "KeyD",
						"value" : -0.05
					}
				]
			*/
			Float32 rotate = input_.Get("rotate"); // Get the input value of the rotate command - the amount the angle
												   // changes if the button is pressed

			if (rotate != 0.0f)
			{
				crosshair_.angle += rotate; // Change the angle of the crosshair

				if (crosshair_.angle >= 2 * M_PI)
				{ // Keep the angle in the first revolution
					crosshair_.angle -= 2 * M_PI;
				}
				else if (crosshair_.angle < 0)
				{
					crosshair_.angle = 2 * M_PI - (-crosshair_.angle);
				}

				auto& sprite =
					Engine::Registry().get<Sprite>(crosshair_.crosshairSprite); // Get the crosshair spite component

				Float32 x = crosshair_.playerDistance * cos(crosshair_.angle);
				Float32 y =
					crosshair_.playerDistance *
					sin(crosshair_.angle); // Calculate the new x and y positions of the crosshair sprite based on the
										   // current angle , and offset them by the distance to the center of rotation

				sprite.position.x = x + sprite_.position.x;
				sprite.position.y =
					y + sprite_.position
							.y; // Move the center of rotation to the wanted sprite (character sprite for example)

				// Change the scale of the sprite based on the current angle
				if (crosshair_.angle > 0.5 * M_PI && crosshair_.angle < 1.5 * M_PI)
				{
					sprite_.scale = {-1, 1};
				}
				else
				{
					sprite_.scale = {1, 1};
				}
			}
		});
}
