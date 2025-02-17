#pragma once

#include "core/core.h"
#include "core/game.h"
#include "system.h"

#include <SimpleIni.h>
#include <entt/entt.hpp>
#include <spdlog/spdlog.h>
#include <tsl/sparse_map.h>
#include <tsl/sparse_set.h>

#include <memory>
#include <typeinfo>
#include <utility>

#undef main

namespace dagger
{
	class Engine
		: public Subscriber<Exit, Error>
		, public Publisher<NextFrame>
	{
		UInt64 m_LastFrameCounter {0};
		UInt64 m_FrameCounter {0};
		Duration m_DeltaTime {0.0};
		TimePoint m_CurrentTime {};

		IniFile m_Ini;
		OwningPtr<Game> m_Game;
		std::vector<System*> m_Systems;
		OwningPtr<entt::registry> m_Registry;
		OwningPtr<entt::dispatcher> m_EventDispatcher;
		Bool m_ShouldStayUp {true};
		UInt32 m_ExitStatus;

		static inline Engine* s_Instance = nullptr;

	public:
		static inline Bool s_IsPaused {false};
		static inline uint64_t s_EntityId = 0;

		template<typename Sys, typename... Args>
		inline void AddSystem(Args&&... args_)
		{
			auto sys = new Sys(std::forward<Args>(args_)...);
			PutDefaultResource<Sys>(sys);
			m_Systems.push_back(sys);
		}

		template<typename Sys>
		inline void AddPausableSystem()
		{
			auto sys = new Sys();
			sys->canBePaused = true;
			PutDefaultResource<Sys>(sys);
			m_Systems.push_back(sys);
		}

		static void ToggleSystemsPause(Bool toPause_);

		static inline IniFile& GetIniFile()
		{
			return s_Instance->m_Ini;
		}

		static inline Engine& Instance()
		{
			return *s_Instance;
		}

		static inline Float32 DeltaTime()
		{
			return s_Instance->m_DeltaTime.count();
		}

		static inline TimePoint CurrentTime()
		{
			return s_Instance->m_CurrentTime;
		}

		static inline UInt64 FrameCount()
		{
			return s_Instance->m_FrameCounter;
		}

		static inline entt::dispatcher& Dispatcher()
		{
			return *(s_Instance->m_EventDispatcher.get());
		}

		static inline entt::registry& Registry()
		{
			return *(s_Instance->m_Registry.get());
		}

		template<typename K, typename Archetype>
		inline static tsl::sparse_map<K, Archetype>& Cache()
		{
			static tsl::sparse_map<K, Archetype> cachedMap;
			return cachedMap;
		}

		template<typename Archetype>
		inline static Archetype* GetDefaultResource()
		{
			return Res<Archetype>()[""];
		}

		template<typename Archetype>
		inline static void PutDefaultResource(Archetype* ptr_)
		{
			Res<Archetype>()[""] = ptr_;
		}

		template<typename Archetype>
		inline static Archetype* GetResource(String name_)
		{
			return Res<Archetype>()[name_];
		}

		template<typename Archetype>
		inline static void PutResource(String name_, Archetype* ptr_)
		{
			Res<Archetype>()[name_] = ptr_;
		}

		template<typename Archetype>
		inline static tsl::sparse_map<std::string, Archetype*>& Res()
		{
			static tsl::sparse_map<std::string, Archetype*> cachedMap;
			return cachedMap;
		}

		Engine();

		Engine(const Engine&) = delete;

		~Engine() = default;

		void EngineShutdown(Exit&);

		void EngineError(Error& error_);

		void EngineInit();

		void EngineLoop();

		void EngineStop();

		template<typename GameType>
		UInt32 Run()
		{
			m_Game.reset(new GameType());

			m_Ini.SetUnicode();
			auto iniPath = m_Game->GetIniFile();

			FilePath path {iniPath};
			if (m_Ini.LoadFile(Files::absolute(path).string().c_str()) < 0)
			{
				Logger::critical("Ini file missing: {}", Files::absolute(path).string());
				exit(-1);
			}

			m_Game->CoreSystemsSetup();
			m_Game->GameplaySystemsSetup();

			EngineInit();
			m_Game->WorldSetup();

			while (m_ShouldStayUp)
				EngineLoop();

			EngineStop();

			return m_ExitStatus;
		};
	};
} // namespace dagger
