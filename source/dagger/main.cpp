#include "core/engine.h"
#include "gameplay/editor/editor_main.h"
#include "gameplay/ping_pong/ping_pong_main.h"
#include "gameplay/platformer/platformer_main.h"
#include "gameplay/racing/racing_main.h"
#include "gameplay/team_game/team_game_main.h"
#include "gameplay/tiles_example/tiles_example_main.h"

#include "core/network/message.h"
#include "core/network/server.h"
#include "core/network/client.h"

enum class MessageTypes : UInt32
{
	Move,
	Fire
};

class TestServer : public NetworkServer<MessageTypes>
{
public:
	TestServer(UInt16 port_) : NetworkServer<MessageTypes>(port_)
	{

	}
protected:
	bool OnClientConnect(std::shared_ptr<Connection> client) override
	{
		return true;
	}

	void OnClientDisconnect(std::shared_ptr<Connection> client) override
	{

	}

	void OnMessage(std::shared_ptr<Connection> client, Message<MessageTypes>& message) override
    {
		switch(message.header.id)
		{
			case MessageTypes::Move:
				int x;
				message >> x;
				Logger::info(x);
				// Stop();
				break;
			case MessageTypes::Fire:
				break;
		}
    }

	void OnClientValidated(std::shared_ptr<Connection> client) override
    {
		Message<MessageTypes> m;
		m.header.id = MessageTypes::Fire;
		m << 123;
		Send(client, m);
    }
};

class TestClient : public NetworkClient<MessageTypes>
{
public:
	void Fire()
	{
		Message<MessageTypes> message;
		message.header.id = MessageTypes::Move;
		message << 321;
		Send(message);
	}
};

int main(int argc_, char** argv_)
{
	dagger::Engine engine;
	//	return engine.Run<tiles_example::TilesExampleMain>();
	//	return engine.Run<team_game::TeamGame>();
	//	return engine.Run<ping_pong::PingPongGame>();
	//	return engine.Run<racing_game::RacingGame>();
	//	return engine.Run<platformer::Platformer>();
	// return engine.Run<editor::EditorTestGame>();

	// Message<MessageTypes> msg;
	// msg.header.id = MessageTypes::Fire;
	// int a = 1;
	// bool b = true;
	// float c = 3.14156f;
	// struct
	// {
	// 	float x;
	// 	float y;

	// } d[5];

	// msg << a << b << c << d;

	// a = 99;
	// b = false;
	// c = 99.0f;

	// msg >> d >> c >> b >> a;

	int x;
	std::cin >> x;

	if (x==1)
	{
		TestClient client;
		client.Connect("127.0.0.1", 5000);

		bool quit = false;
		while(!quit)
		{
			if (client.IsConnected())
			{
				if(!client.Incoming().Empty())
				{
					auto message = client.Incoming().Pop();
					switch(message.header.id)
					{
						case MessageTypes::Move:
							break;
						case MessageTypes::Fire:
							int x;
							message >> x;
							Logger::info(x);
							client.Fire();
							break;
					}
				}
			}
			else
			{
				std::cout << "Server Down. Quitting" << std::endl;
				quit = true;
			}
		}
		
	}
	else if(x==2)
	{
		TestServer server(5000);
		server.Start();

		while(true)
		{
			server.Update(-1, true);
		}
	}

	return 0;
}
