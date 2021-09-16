#include "core/engine.h"
#include "gameplay/editor/editor_main.h"
#include "gameplay/ping_pong/ping_pong_main.h"
#include "gameplay/platformer/platformer_main.h"
#include "gameplay/racing/racing_main.h"
#include "gameplay/team_game/team_game_main.h"
#include "gameplay/tiles_example/tiles_example_main.h"
#include "gameplay/multiplayer/multiplayer_client.h"
#include "gameplay/multiplayer/multiplayer_server.h"

int main(int argc_, char** argv_)
{
	dagger::Engine engine;
	// return engine.Run<tiles_example::TilesExampleMain>();
	// return engine.Run<team_game::TeamGame>();
	// return engine.Run<ping_pong::PingPongGame>();
	// return engine.Run<racing_game::RacingGame>();
	// return engine.Run<platformer::Platformer>();
	// return engine.Run<editor::EditorTestGame>();

	int x;
	std::cin >> x;

	if (x==1)
	{
		return engine.Run<multiplayer::MultiplayerGame>();
	}
	else if(x==2)
	{
		multiplayer::MultiplayerServer server { 5000 };
		server.Run();
	}

	return 0;
}
