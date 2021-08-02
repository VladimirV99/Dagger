#include "editor_main.h"

#include "core/core.h"
#include "core/engine.h"
#include "core/game/transforms.h"
#include "core/graphics/animation.h"
#include "core/graphics/animations.h"
#include "core/graphics/gui.h"
#include "core/graphics/shaders.h"
#include "core/graphics/sprite.h"
#include "core/graphics/sprite_render.h"
#include "core/graphics/textures.h"
#include "core/graphics/window.h"
#include "core/input/inputs.h"
#include "core/savegame.h"
#include "gameplay/common/simple_collisions.h"
#include "gameplay/editor/savegame_system.h"
#include "tools/diagnostics.h"

using namespace dagger;
using namespace editor;

ECommonSaveArchetype EditorTestGame::Save(Entity entity_, JSON::json& saveTo_)
{
	auto& registry = Engine::Registry();

	ECommonSaveArchetype archetype = ECommonSaveArchetype::None;

	if (registry.all_of<Sprite>(entity_))
	{
		saveTo_["sprite"] = SerializeComponent<Sprite>(registry.get<Sprite>(entity_));
		archetype = archetype | ECommonSaveArchetype::Sprite;
	}

	if (registry.all_of<Transform>(entity_))
	{
		saveTo_["transform"] = SerializeComponent<Transform>(registry.get<Transform>(entity_));
		archetype = archetype | ECommonSaveArchetype::Transform;
	}

	if (registry.all_of<Animator>(entity_))
	{
		saveTo_["animator"] = SerializeComponent<Animator>(registry.get<Animator>(entity_));
		archetype = archetype | ECommonSaveArchetype::Animator;
	}

	if (registry.all_of<SimpleCollision>(entity_))
	{
		saveTo_["simple_collision"] = SerializeComponent<SimpleCollision>(registry.get<SimpleCollision>(entity_));
		archetype = archetype | ECommonSaveArchetype::Physics;
	}

	// todo: add new if-block here and don't forget to change archetype

	return archetype;
}

void EditorTestGame::Load(ECommonSaveArchetype archetype_, Entity entity_, JSON::json& loadFrom_)
{
	auto& registry = Engine::Registry();

	if (IS_ARCHETYPE_SET(archetype_, ECommonSaveArchetype::Camera))
		DeserializeComponent<Camera>(loadFrom_["camera"], *Engine::GetDefaultResource<Camera>());

	if (IS_ARCHETYPE_SET(archetype_, ECommonSaveArchetype::Sprite))
		DeserializeComponent<Sprite>(loadFrom_["sprite"], registry.emplace<Sprite>(entity_));

	if (IS_ARCHETYPE_SET(archetype_, ECommonSaveArchetype::Transform))
		DeserializeComponent<Transform>(loadFrom_["transform"], registry.emplace<Transform>(entity_));

	if (IS_ARCHETYPE_SET(archetype_, ECommonSaveArchetype::Animator))
		DeserializeComponent<Animator>(loadFrom_["animator"], registry.emplace<Animator>(entity_));

	if (IS_ARCHETYPE_SET(archetype_, ECommonSaveArchetype::Physics))
		DeserializeComponent<SimpleCollision>(
			loadFrom_["simple_collision"], registry.emplace<SimpleCollision>(entity_));

	// todo: add new if-block here and don't forget to change archetype
}

void EditorTestGame::GameplaySystemsSetup()
{
	auto& engine = Engine::Instance();
	engine.AddSystem<EditorToolSystem>();
	engine.AddSystem<SaveGameSystem<ECommonSaveArchetype>>(this);
}

void EditorTestGame::WorldSetup()
{
	ShaderSystem::Use("standard");

	auto* camera = Engine::GetDefaultResource<Camera>();
	camera->mode = ECameraMode::ShowAsMuchAsPossible;
	camera->position = {0, 0, 0};
	camera->Update();
}

void EditorToolSystem::OnToolMenuRender()
{
	if (ImGui::BeginMenu("Scene"))
	{
		if (ImGui::MenuItem("Clear"))
		{
			Engine::Registry().clear();
		}

		if (ImGui::MenuItem("Save"))
		{
			Engine::Dispatcher().trigger<SaveGameSystem<ECommonSaveArchetype>::SaveRequest>(
				SaveGameSystem<ECommonSaveArchetype>::SaveRequest {m_Filename});
		}

		if (ImGui::MenuItem("Load"))
		{
			Engine::Dispatcher().trigger<SaveGameSystem<ECommonSaveArchetype>::LoadRequest>(
				SaveGameSystem<ECommonSaveArchetype>::LoadRequest {m_Filename});
		}

		ImGui::EndMenu();
	}
}

void EditorToolSystem::OnKeyboardEvent(KeyboardEvent event_)
{
	if (event_.key == EDaggerKeyboard::KeyTab && event_.action == EDaggerInputState::Released)
		if (Input::IsInputDown(EDaggerKeyboard::KeyLeftControl))
			m_IsInEditor = !m_IsInEditor;
}

void EditorToolSystem::Run()
{
	if (m_IsInEditor)
	{
		auto& knob = m_Registry.get<Sprite>(m_Focus);
		knob.visible = true;
		auto& focus = m_Registry.get<EditorFocus>(m_Focus);

		if (Input::IsInputDown(EDaggerMouse::MouseButton3))
		{
			auto cursorPos = Input::CursorPositionInWorld();
			knob.position.x = cursorPos.x;
			knob.position.y = cursorPos.y;
			UpdateTargets();
			if (!m_Targets.empty())
			{
				m_Selected = m_Targets[0].entity;
			}
		}

		if (Input::IsInputDown(EDaggerKeyboard::KeyLeftShift))
		{
			auto& reg = Engine::Registry();
			if (reg.valid(m_Selected))
			{
				knob.position = Vector3 {Input::CursorPositionInWorld(), 0};
				if (reg.all_of<Transform>(m_Selected))
				{
					auto& transform = reg.get<Transform>(m_Selected);
					transform.position = knob.position;
				}
				if (reg.all_of<Sprite>(m_Selected))
				{
					auto& sprite = reg.get<Sprite>(m_Selected);
					sprite.position = knob.position;
				}
			}
		}

		if (!Input::IsInputDown(EDaggerMouse::MouseButton1) && focus.dirty)
		{
			focus.dirty = false;
			UpdateTargets();
		}
	}
	else
	{
		auto& knob = m_Registry.get<Sprite>(m_Focus);
		knob.visible = false;
	}
}

void EditorToolSystem::ProcessTextures()
{
	m_AvailableTextures.clear();
	for (const auto& [k, n] : Engine::Res<Texture>())
	{
		m_AvailableTextures.push_back(k.c_str());
	}
	std::sort(
		m_AvailableTextures.begin(), m_AvailableTextures.end(),
		[](const char* lhs, const char* rhs) { return strcmp(lhs, rhs) < 0; });
}

void EditorToolSystem::ProcessAnimations()
{
	m_AvailableAnimations.clear();
	for (const auto& [k, n] : Engine::Res<Animation>())
	{
		m_AvailableAnimations.push_back(k.c_str());
	}
	std::sort(
		m_AvailableAnimations.begin(), m_AvailableAnimations.end(),
		[](const char* lhs, const char* rhs) { return strcmp(lhs, rhs) < 0; });
}

void EditorToolSystem::UpdateTargets()
{
	auto& knob = m_Registry.get<Sprite>(m_Focus);
	m_Targets.clear();

	const auto* cam = Engine::GetDefaultResource<Camera>();

	Engine::Registry().view<Sprite, SaveGame<ECommonSaveArchetype>>().each(
		[&](Entity entity_, const Sprite& sprite_, const SaveGame<ECommonSaveArchetype>& save_)
		{
			const auto left = sprite_.position.x - (sprite_.size.x / 2) * sprite_.scale.x * cam->zoom;
			const auto top = sprite_.position.y - (sprite_.size.y / 2) * sprite_.scale.y * cam->zoom;

			const auto right = sprite_.position.x + (sprite_.size.x / 2) * sprite_.scale.x * cam->zoom;
			const auto bottom = sprite_.position.y + (sprite_.size.y / 2) * sprite_.scale.y * cam->zoom;

			// Rotate the knob position by negative sprite rotation angle around the sprite position
			// Then check collision as usual
			const Vector3 knobRelative = knob.position - sprite_.position;
			const double sine = sin(-sprite_.rotation * M_PI / 180.0f);
			const double cosine = cos(-sprite_.rotation * M_PI / 180.0f);
			const auto knobX = sprite_.position.x + knobRelative.x * cosine - knobRelative.y * sine;
			const auto knobY = sprite_.position.y + knobRelative.x * sine + knobRelative.y * cosine;

			if (knobX >= left && knobY >= top && knobX <= right && knobY <= bottom)
			{
				if (Engine::Registry().all_of<Animator>(entity_))
				{
					auto& animator = Engine::Registry().get<Animator>(entity_);
					m_Targets.push_back(EditorFocusTarget {entity_, animator.currentAnimation});
				}
				else
				{
					m_Targets.push_back(EditorFocusTarget {entity_, sprite_.image->Name()});
				}
			}
		});
}

void EditorToolSystem::GUIDrawCameraEditor()
{
	auto* camera = Engine::GetDefaultResource<Camera>();
	static int mode = 1;
	static const char* modes[] {"Fixed Resoulution", "Show As Much As Possible"};

	if (ImGui::CollapsingHeader("Camera"))
	{

		if (ImGui::Combo("Camera Mode", &mode, modes, 2))
		{
			switch (mode)
			{
			case 0:
				camera->mode = ECameraMode::FixedResolution;
				break;
			case 1:
				camera->mode = ECameraMode::ShowAsMuchAsPossible;
				break;
			}
			camera->Update();
		}

		/* Position */ {
			float position[] {camera->position.x, camera->position.y};
			if (ImGui::DragFloat2("Camera Position", position, 10.0f, -FLT_MAX, FLT_MAX, "%.2f"))
			{
				camera->position.x = position[0];
				camera->position.y = position[1];
			}
		}

		/* Zoom */ {
			if (ImGui::DragFloat("Camera Zoom", &camera->zoom, 0.1f, 0.1f, 10.0f, "%.1f", 1))
			{
				auto& knob = m_Registry.get<Sprite>(m_Focus);
				knob.scale = Vector2 {1.0f / camera->zoom};
			}
		}
	}
}

Entity EditorToolSystem::GUIExecuteCreateEntity()
{
	auto focusPosition = m_Registry.get<Sprite>(m_Focus).position;

	auto& reg = Engine::Registry();
	auto newEntity = reg.create();
	auto& newTransform = reg.emplace<Transform>(newEntity);
	newTransform.position.x = focusPosition.x;
	newTransform.position.y = focusPosition.y;
	auto& newSprite = reg.emplace<Sprite>(newEntity);
	newSprite.position.x = focusPosition.x;
	newSprite.position.y = focusPosition.y;
	AssignSprite(newSprite, "tools:knob2");
	reg.emplace<SaveGame<ECommonSaveArchetype>>(newEntity);

	return newEntity;
}

void EditorToolSystem::GUIDrawSpriteEditor()
{
	static String filter;

	auto& reg = Engine::Registry();

	if (reg.all_of<Sprite>(m_Selected) && ImGui::CollapsingHeader("Sprite"))
	{
		ImGui::InputText("Texture Filter", filter.data(), 80);

		Sprite& compSprite = reg.get<Sprite>(m_Selected);

		/* Texture */ {
			static int selectedTexture = 0;
			Sequence<const char*> textures;

			int i = 0;
			for (const auto* textureName : m_AvailableTextures)
			{
				if (strstr(textureName, filter.data()) != nullptr)
				{
					textures.push_back(textureName);
					if (textureName == compSprite.image->Name())
					{
						selectedTexture = i;
					}
					++i;
				}
			}

			int currentSelected = selectedTexture;
			String title {};
			title.reserve(100);
			sprintf(title.data(), "Image (%zu)", textures.size());
			if (ImGui::Combo(title.data(), &selectedTexture, textures.data(), textures.size()))
			{
				if (currentSelected != selectedTexture)
				{
					AssignSprite(compSprite, textures[selectedTexture]);
					m_Registry.get<EditorFocus>(m_Focus).dirty = true;
				}
			}
		}

		/* Size */ {
			float size[] {compSprite.size.x, compSprite.size.y};
			ImGui::DragFloat2("Pixel Size", size, 1, -2000.0f, 2000.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
			compSprite.size.x = size[0];
			compSprite.size.y = size[1];
		}

		/* Color */ {
			float color[] {compSprite.color.r, compSprite.color.g, compSprite.color.b};
			ImGui::ColorEdit3("Color", color);
			compSprite.color.r = color[0];
			compSprite.color.g = color[1];
			compSprite.color.b = color[2];
		}

		/* Position */ {
			float posXY[] {compSprite.position.x, compSprite.position.y};
			float posZ = compSprite.position.z;
			ImGui::DragFloat2("Sprite Position", posXY, 1, -FLT_MAX, FLT_MAX, "%.2f");
			ImGui::DragFloat("Z Order", &posZ, 1, 0, FLT_MAX, "%.2f");
			if (reg.all_of<Transform>(m_Selected))
			{
				Transform& compTransform = reg.get<Transform>(m_Selected);
				compTransform.position.x = posXY[0];
				compTransform.position.y = posXY[1];
				compTransform.position.z = posZ;
			}
			else
			{
				compSprite.position.x = posXY[0];
				compSprite.position.y = posXY[1];
				compSprite.position.y = posZ;
			}
		}

		/* Rotation */ {
			ImGui::DragFloat("Rotation", &compSprite.rotation, 0.2f, 0, 360, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		}

		/* Scale */ {
			float size[] {compSprite.scale.x, compSprite.scale.y};
			ImGui::DragFloat2("Scale", size, 0.1f, -10, 10, "%.2f", ImGuiSliderFlags_AlwaysClamp);
			compSprite.scale.x = size[0];
			compSprite.scale.y = size[1];
		}

		/* Pivot */ {
			float pivot[] {compSprite.pivot.x, compSprite.pivot.y};
			ImGui::DragFloat2("Pivot", pivot, 0.01f, -0.5f, 0.5f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
			compSprite.pivot.x = pivot[0];
			compSprite.pivot.y = pivot[1];
		}

		/* Is UI */ {
			bool isUI = compSprite.isUI > 0.5f;
			if (ImGui::Checkbox("Is UI", &isUI))
			{
				if (isUI)
					compSprite.UseAsUI();
				else
					compSprite.UseInWorld();
			}
		}
	}
	else if (!reg.all_of<Sprite>(m_Selected))
	{
		if (ImGui::Button("Attach Sprite", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
		{
			reg.emplace<Sprite>(m_Selected);
		}
	}
}

void EditorToolSystem::GUIDrawTransformEditor()
{
	auto& reg = Engine::Registry();

	if (reg.all_of<Transform>(m_Selected) && ImGui::CollapsingHeader("Transform"))
	{
		Transform& compTransform = reg.get<Transform>(m_Selected);
		/* Position */ {
			float posXY[] {compTransform.position.x, compTransform.position.y};
			float posZ = compTransform.position.z;
			ImGui::DragFloat2("Position", posXY, 1, -FLT_MAX, FLT_MAX, "%.2f");
			ImGui::DragFloat("Z Order", &posZ, 1, 0, FLT_MAX, "%.2f");
			compTransform.position.x = posXY[0];
			compTransform.position.y = posXY[1];
			compTransform.position.z = posZ;
		}

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.1f, 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.1f, 0.1f, 1.0f));
		if (ImGui::Button("Detach Transform", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
		{
			reg.remove<Transform>(m_Selected);
		}
		ImGui::PopStyleColor(3);
	}
	else if (!reg.all_of<Transform>(m_Selected))
	{
		if (ImGui::Button("Attach Transform", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
		{
			auto& compTransform = reg.emplace<Transform>(m_Selected);
			if (reg.all_of<Sprite>(m_Selected))
			{
				auto& compSprite = reg.get<Sprite>(m_Selected);
				compTransform.position.x = compSprite.position.x;
				compTransform.position.y = compSprite.position.y;
				compTransform.position.z = compSprite.position.z;
			}
		}
	}
}

void EditorToolSystem::GUIDrawAnimationEditor()
{
	static String filter;

	auto& reg = Engine::Registry();

	if (reg.all_of<Animator>(m_Selected) && ImGui::CollapsingHeader("Animator"))
	{
		ImGui::InputText("Animation Filter", filter.data(), 80);

		Animator& compAnim = reg.get<Animator>(m_Selected);
		/* Animation */ {
			static int selectedAnim = 0;
			Sequence<const char*> animations;
			int i = 0;
			for (const auto* animationName : m_AvailableAnimations)
			{
				if (strstr(animationName, filter.data()) != nullptr)
				{
					animations.push_back(animationName);
					if (animationName == compAnim.currentAnimation)
					{
						selectedAnim = i;
					}
					++i;
				}
			}

			int currentSelected = selectedAnim;
			if (ImGui::Combo("Animation", &selectedAnim, animations.data(), animations.size()))
			{
				if (currentSelected != selectedAnim)
				{
					AnimatorPlay(compAnim, animations[selectedAnim]);
					m_Registry.get<EditorFocus>(m_Focus).dirty = true;
				}
			}
		}

		/* Is Playing */ {
			ImGui::Checkbox("Is Playing", &compAnim.isPlaying);
		}

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.1f, 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.1f, 0.1f, 1.0f));
		if (ImGui::Button("Detach Animator", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
		{
			reg.remove<Animator>(m_Selected);
			m_Registry.get<EditorFocus>(m_Focus).dirty = true;
		}
		ImGui::PopStyleColor(3);
	}
	else if (!reg.all_of<Animator>(m_Selected))
	{
		if (ImGui::Button("Attach Animator", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
		{
			auto& compAnim = reg.emplace<Animator>(m_Selected);
			if (!m_AvailableAnimations.empty())
				AnimatorPlay(compAnim, m_AvailableAnimations[0]);
			m_Registry.get<EditorFocus>(m_Focus).dirty = true;
		}
	}
}

void EditorToolSystem::GUIDrawPhysicsEditor()
{
	auto& reg = Engine::Registry();

	if (reg.all_of<SimpleCollision>(m_Selected) && ImGui::CollapsingHeader("Collision"))
	{
		SimpleCollision& compCol = reg.get<SimpleCollision>(m_Selected);

		/* Pivot */ {
			float pivot[] {compCol.pivot.x, compCol.pivot.y};
			ImGui::DragFloat2("Collision Pivot", pivot, 0.01f, -0.5f, 0.5f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
			compCol.pivot.x = pivot[0];
			compCol.pivot.y = pivot[1];
		}

		/* Size */ {
			float size[] {compCol.size.x, compCol.size.y};
			ImGui::DragFloat2("Collision Size", size, 1, -2000.0f, 2000.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
			compCol.size.x = size[0];
			compCol.size.y = size[1];
		}

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.1f, 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.1f, 0.1f, 1.0f));
		if (ImGui::Button("Detach Collision", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
		{
			reg.remove<SimpleCollision>(m_Selected);
		}
		ImGui::PopStyleColor(3);
	}
	else if (!reg.all_of<SimpleCollision>(m_Selected))
	{
		if (ImGui::Button("Attach Collision", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
		{
			reg.emplace<SimpleCollision>(m_Selected);
		}
	}
}

bool EditorToolSystem::GUIDrawEntityFocusSelection()
{
	auto& reg = Engine::Registry();

	Sequence<const char*> items;

	int selectedItem = 0;

	items.push_back("<none>");
	int i = 1;
	for (auto& item : m_Targets)
	{
		items.push_back(item.name.c_str());
		if (m_Selected == item.entity)
		{
			selectedItem = i;
		}
		++i;
	}

	ImGui::ListBox("In Focus", &selectedItem, items.data(), items.size(), 10);

	if (selectedItem == 0 && m_Selected != entt::null)
	{
		m_Selected = entt::null;
	}

	if (selectedItem > 0)
	{
		const int index = selectedItem - 1;
		m_Selected = m_Targets[index].entity;

		if (!reg.valid(m_Selected))
		{
			return false;
		}

		return true;
	}

	return false;
}

void EditorToolSystem::OnRenderGUI()
{
	if (m_IsInEditor)
	{
		ImGui::Begin("Scene Editor");

		ImGui::InputText("Filename", m_Filename, sizeof(m_Filename));
		ImGui::Separator();

		GUIDrawCameraEditor();
		ImGui::Separator();

		if (ImGui::Button("Create Entity", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
		{
			m_Selected = GUIExecuteCreateEntity();
			UpdateTargets();
		}

		if (GUIDrawEntityFocusSelection())
		{
			GUIDrawSpriteEditor();
			GUIDrawTransformEditor();
			GUIDrawAnimationEditor();
			GUIDrawPhysicsEditor();

			// to add more components, replicate the above functions carefully
			// if you're lost, ping Mika on discord :)
			// ps. DON'T forget to make your components serializable by replicating the following:
			//  - de/serialiazation in savegame.h/cpp
			//  - changing the savegame enum in save_archetype.h
			//  - changing the editor GUI in editor_main.h/cpp

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.2f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.1f, 0.1f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.1f, 0.1f, 1.0f));
			if (ImGui::Button("Delete Entity", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
			{
				Engine::Registry().destroy(m_Selected);
				m_Registry.get<EditorFocus>(m_Focus).dirty = true;
			}
			ImGui::PopStyleColor(3);
		}

		ImGui::End();
	}
}