#pragma once

#include "core/core.h"
#include "core/system.h"
#include "core/game.h"
#include "core/engine.h"
#include "tools/toolmenu.h"
#include "gameplay/editor/editor_defs.h"
#include "gameplay/editor/savegame_system.h"
#include "core/graphics/tool_render.h"

using namespace dagger;

namespace editor
{
    class EditorToolSystem : public System
    {
        static inline EditorFocusTarget ms_NoTarget{ entt::null, "" };

        Bool m_IsInEditor;
        Registry m_Registry;
        Entity m_Focus;
        EditorFocusTarget m_Selected{ ms_NoTarget };
        String m_Filename;
        Sequence<EditorFocusTarget> m_Targets;

        String SystemName() override { return "SaveGame"; }

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
                //sprite.UseAsUI();                         //sta radi ova linija?
                sprite.position = Vector3{ 0, 0, 0 };
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
        void GUIDrawSpriteEditor();
        void GUIDrawTransformEditor();
        void GUIDrawAnimationEditor();
        void GUIDrawPhysicsEditor();
        void GUIDrawBouncyEditor();
        void GUIDrawDeadlyEditor();
        void GUIDrawInteractableEditor();
        bool GUIDrawEntityFocusSelection(int& selectedItem);

    };

    class EditorTestGame : public Game, public SaveLoadProcess<ECommonSaveArchetype>
    {
        inline String GetIniFile() override
        {
            return "editor.ini";
        };

        void GameplaySystemsSetup() override;
        void WorldSetup() override;

        ECommonSaveArchetype Save(Entity entity_, JSON::json& saveTo_) override;
        void Load(ECommonSaveArchetype archetype_, Entity entity_, JSON::json& loadFrom_) override;
    };
}
