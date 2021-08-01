#pragma once

#include "core/core.h"
#include "core/engine.h"
#include "core/game.h"
#include "core/graphics/tool_render.h"
#include "core/system.h"
#include "gameplay/editor/savegame_system.h"
#include "save_archetype.h"
#include "tools/toolmenu.h"

using namespace dagger;

namespace editor
{
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
		char m_Filename[41];
		Sequence<EditorFocusTarget> m_Targets;
		Sequence<const char*> m_AvailableTextures;
		Sequence<const char*> m_AvailableAnimations;

		String SystemName() const override
		{
			return "Editor Tool System";
		}

		void SpinUp() override
		{
			std::strncpy(m_Filename, "default_saved_scene.json", sizeof(m_Filename) - 1);
			Engine::Dispatcher().sink<AssetLoadFinished<Texture>>().connect<&EditorToolSystem::ProcessTextures>(this);
			Engine::Dispatcher().sink<AssetLoadFinished<Animation>>().connect<&EditorToolSystem::ProcessAnimations>(
				this);
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

			ProcessTextures();
			ProcessAnimations();

			Engine::GetDefaultResource<ToolRenderSystem>()->registry = &m_Registry;
		}

		void WindDown() override
		{
			Engine::Dispatcher().sink<AssetLoadFinished<Texture>>().disconnect<&EditorToolSystem::ProcessTextures>(
				this);
			Engine::Dispatcher().sink<AssetLoadFinished<Animation>>().disconnect<&EditorToolSystem::ProcessAnimations>(
				this);
			Engine::Dispatcher().sink<KeyboardEvent>().disconnect<&EditorToolSystem::OnKeyboardEvent>(this);
			Engine::Dispatcher().sink<GUIRender>().disconnect<&EditorToolSystem::OnRenderGUI>(this);
			Engine::Dispatcher().sink<ToolMenuRender>().disconnect<&EditorToolSystem::OnToolMenuRender>(this);
		}

		void Run() override;

		void ProcessTextures();
		void ProcessAnimations();

		void OnKeyboardEvent(KeyboardEvent event_);
		void OnToolMenuRender();
		void OnRenderGUI();

		void GUIDrawCameraEditor();
		void GUIExecuteCreateEntity();
		void GUIDrawSpriteEditor() const;
		void GUIDrawTransformEditor() const;
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
