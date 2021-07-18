#pragma once

#include "core/core.h"
#include "core/engine.h"
#include "core/game.h"
#include "core/graphics/tool_render.h"
#include "core/system.h"
#include "gameplay/editor/savegame_system.h"
#include "tools/toolmenu.h"

using namespace dagger;

namespace editor
{
	enum class ECommonSaveArchetype
	{
		None = 0b00000000,
		Sprite = 0b00000001,
		Transform = 0b00000010,
		Animator = 0b00000100,
		Physics = 0b00001000,
		// todo: add new values here
	};

#define IS_ARCHETYPE_SET(in, test) ((in & test) == test)

	inline ECommonSaveArchetype operator|(ECommonSaveArchetype a_, ECommonSaveArchetype b_)
	{
		return static_cast<ECommonSaveArchetype>(static_cast<int>(a_) | static_cast<int>(b_));
	}

	inline ECommonSaveArchetype operator&(ECommonSaveArchetype a_, ECommonSaveArchetype b_)
	{
		return static_cast<ECommonSaveArchetype>(static_cast<int>(a_) & static_cast<int>(b_));
	}

	struct EditorFocus
	{
		Bool dirty;
	};

	struct EditorFocusTarget
	{
		Entity entity;
		String name;
	};

	class EditorToolSystem : public System
	{
		static inline EditorFocusTarget s_NoTarget {entt::null, ""};

		Bool m_IsInEditor;
		Registry m_Registry;
		Entity m_Focus {entt::null};
		EditorFocusTarget m_Selected {s_NoTarget};
		String m_Filename;
		Sequence<EditorFocusTarget> m_Targets;

		String SystemName() const override
		{
			return "SaveGame";
		}

		void SpinUp() override
		{
			m_Filename = "default_saved_scene.json";
			Engine::Dispatcher().sink<KeyboardEvent>().connect<&EditorToolSystem::OnKeyboardEvent>(this);
			Engine::Dispatcher().sink<ToolMenuRender>().connect<&EditorToolSystem::OnToolMenuRender>(this);
			Engine::Dispatcher().sink<GUIRender>().connect<&EditorToolSystem::OnRenderGUI>(this);

			{
				m_Focus = m_Registry.create();
				m_Registry.emplace<EditorFocus>(m_Focus);
				auto& sprite = m_Registry.emplace<Sprite>(m_Focus);
				AssignSprite(sprite, "tools:knob1");
				sprite.position = Vector3 {0, 0, 0};
			}

			Engine::GetDefaultResource<ToolRenderSystem>()->registry = &m_Registry;
		}

		void WindDown() override
		{
			Engine::Dispatcher().sink<KeyboardEvent>().disconnect<&EditorToolSystem::OnKeyboardEvent>(this);
			Engine::Dispatcher().sink<GUIRender>().disconnect<&EditorToolSystem::OnRenderGUI>(this);
			Engine::Dispatcher().sink<ToolMenuRender>().disconnect<&EditorToolSystem::OnToolMenuRender>(this);
		}

		void Run() override;

		void OnKeyboardEvent(KeyboardEvent event_);
		void OnToolMenuRender();
		void OnRenderGUI();

		void GUIExecuteCreateEntity();
		void GUIDrawSpriteEditor() const;
		void GUIDrawAnimationEditor() const;
		void GUIDrawPhysicsEditor() const;
		bool GUIDrawEntityFocusSelection(int& selectedItem_);
	};

	class EditorTestGame
		: public Game
		, public SaveLoadProcess<ECommonSaveArchetype>
	{
		inline String GetIniFile() const override
		{
			return "editor.ini";
		};

		void GameplaySystemsSetup() override;
		void WorldSetup() override;

		ECommonSaveArchetype Save(Entity entity_, JSON::json& saveTo_) override;
		void Load(ECommonSaveArchetype archetype_, Entity entity_, JSON::json& loadFrom_) override;
	};
} // namespace editor
