#include "atonement_main.h"

#include "core/core.h"
#include "core/engine.h"
#include "core/audio.h"
#include "core/input/inputs.h"
#include "core/graphics/sprite.h"
#include "core/graphics/animation.h"
#include "core/graphics/shaders.h"
#include "core/graphics/window.h"
#include "core/game/transforms.h"
#include "core/graphics/sprite_render.h"
#include "core/graphics/textures.h"
#include "core/graphics/animations.h"
#include "core/graphics/gui.h"
#include "tools/diagnostics.h"
#include "gameplay/common/simple_collisions.h"
#include "gameplay/common/camera_focus.h"
#include "gameplay/editor/savegame_system.h"
#include "core/savegame.h"
#include <iostream>
#include <cstring>

#include "gameplay/atonement/char_controller_fsm.h"
#include "gameplay/atonement/atonement_controller.h"
#include "gameplay/atonement/systems/groundedness_detection_system.h"
#include "gameplay/atonement/systems/collision_handler_system.h"
#include "gameplay/atonement/systems/character_collisions.h"
#include "gameplay/atonement/systems/atonement_pause_system.h"
#include "gameplay/common/cooldown_manager.h"
#include "gameplay/atonement/systems/atonement_start_menu.h"
#include "gameplay/atonement/systems/atonement_pause_menu.h"
#include "gameplay/atonement/systems/atonement_end_screen.h"
#include "gameplay/atonement/systems/checkpoint_system.h"
#include "gameplay/atonement/systems/interaction_system.h"
#include "gameplay/atonement/systems/parallax.h"


using namespace dagger;
using namespace atonement;


struct Character
{
    Entity entity;
    Sprite& sprite;
    Animator& animator;
    InputReceiver& input;
    AtonementController::AtonementCharacter& character;
    Transform& transform;
    CharacterCollision& collision;

    static Character Get(Entity entity)
    {
        auto& reg = Engine::Registry();
        auto& sprite = reg.get_or_emplace<Sprite>(entity);
        auto& input = reg.get_or_emplace<InputReceiver>(entity);
        auto& character = reg.get_or_emplace<AtonementController::AtonementCharacter>(entity);

        auto& anim = reg.get_or_emplace<Animator>(entity);
        auto acs = Engine::GetDefaultResource<AtonementController::AtonementControllerSystem>();
        anim.onAnimationEnded.connect<&CharControllerFSM::OnAnimationEnd>(acs->characterFSM);

        auto& transform = reg.get_or_emplace<Transform>(entity);
        auto& collision = reg.get_or_emplace<CharacterCollision>(entity);

        return Character{ entity, sprite, anim, input, character, transform, collision };
    }

    static Character Create(
        String input_ = "",
        ColorRGB color_ = { 1, 1, 1 },
        Vector2 position_ = { 0, 0 },
        Vector2 collision_size_ = {0, 0})
    {
        auto& reg = Engine::Registry();
        auto entity = reg.create();

        ATTACH_TO_FSM(CharControllerFSM, entity);

        auto chr = Character::Get(entity);

        chr.sprite.scale = { 1, 1 };
        chr.sprite.position = { position_, 15.0f };
        chr.sprite.color = { color_, 1.0f };

        chr.collision.size = collision_size_;
        chr.transform.position = { position_, 15.0f };

        AssignSprite(chr.sprite, "BlueWizard:IDLE:idle1");
        AnimatorPlay(chr.animator, "BlueWizard:IDLE");

        if (input_ != "")
            chr.input.contexts.push_back(input_);

        //chr.character.speed = 50;

        return chr;
    }
};

void AtonementGame::CoreSystemsSetup()
{
    auto& engine = Engine::Instance();

    engine.AddSystem<AudioSystem>();
    engine.AddSystem<WindowSystem>();
    engine.AddSystem<InputSystem>();
    engine.AddSystem<ShaderSystem>();
    engine.AddSystem<TextureSystem>();
    engine.AddSystem<SpriteRenderSystem>();
    engine.AddSystem<TransformSystem>();
    engine.AddPausableSystem<AnimationSystem>();
#if !defined(NDEBUG)
    engine.AddSystem<DiagnosticSystem>();
    engine.AddSystem<GUISystem>();
    engine.AddSystem<ToolMenuSystem>();
#endif //!defined(NDEBUG)
}

void AtonementGame::GameplaySystemsSetup()
{
    auto& engine = Engine::Instance();

    engine.AddSystem<AtonementStartMenu>();
    engine.AddSystem<AtonementPauseMenu>();
    engine.AddSystem<AtonementEndScreen>();
    engine.AddPausableSystem<SimpleCollisionsSystem>();
    engine.AddPausableSystem<CharacterCollisionsSystem>();
    engine.AddSystem<SaveGameSystem<ECommonSaveArchetype>>(this);
    engine.AddPausableSystem<AtonementController::AtonementControllerSystem>();
    engine.AddPausableSystem<GroundednessDetectionSystem>();
    engine.AddPausableSystem<CollisionHandlerSystem>();
    engine.AddPausableSystem<CooldownManager<std::string>>();
    engine.AddSystem<AtonementPauseSystem>();
    engine.AddSystem<CameraFollowSystem>();
    engine.AddPausableSystem<CheckpointSystem>();
    engine.AddPausableSystem<IntearactionSystem>();
    engine.AddSystem<ParallaxSystem>();

#if defined(DAGGER_DEBUG)
#endif //defined(DAGGER_DEBUG)
}

void AtonementGame::WorldSetup(){

    Engine::GetDefaultResource<Audio>()->PlayLoop("music");

    auto* camera = Engine::GetDefaultResource<Camera>();
    camera->mode = ECameraMode::FixedResolution;
    camera->size = { 1920, 1080 };
    camera->zoom = 1;
    camera->position = { 0, 0, 0 };
    camera->Update();

    Engine::Dispatcher().trigger<SaveGameSystem<ECommonSaveArchetype>::LoadRequest>(

    SaveGameSystem<ECommonSaveArchetype>::LoadRequest{ "level_1.json" });

    auto mainChar = Character::Create("ATON", { 1, 1, 1 }, { -100, -200 }, {70, 176});
    mainChar.sprite.scale = { 0.6, 0.6 };
  
    Engine::Registry().emplace<CameraFollowFocus>(mainChar.entity);

}

ECommonSaveArchetype AtonementGame::Save(Entity entity_, JSON::json& saveTo_)
{
    auto& registry = Engine::Registry();

    ECommonSaveArchetype archetype = ECommonSaveArchetype::None;

    if (registry.has<Sprite>(entity_))
    {
        saveTo_["sprite"] = SerializeComponent<Sprite>(registry.get<Sprite>(entity_));
        archetype = archetype | ECommonSaveArchetype::Sprite;
    }

    if (registry.has<Transform>(entity_))
    {
        saveTo_["transform"] = SerializeComponent<Transform>(registry.get<Transform>(entity_));
        archetype = archetype | ECommonSaveArchetype::Transform;
    }

    if (registry.has<Animator>(entity_))
    {
        saveTo_["animator"] = SerializeComponent<Animator>(registry.get<Animator>(entity_));
        archetype = archetype | ECommonSaveArchetype::Animator;
    }

    if (registry.has<SimpleCollision>(entity_))
    {
        saveTo_["simple_collision"] = SerializeComponent<SimpleCollision>(registry.get<SimpleCollision>(entity_));
        archetype = archetype | ECommonSaveArchetype::Physics;
    }

    if (registry.has<BouncyComponent>(entity_))
    {
        saveTo_["bouncy_component"] = SerializeComponent<BouncyComponent>(registry.get<BouncyComponent>(entity_));
        archetype = archetype | ECommonSaveArchetype::Bouncy;
    }

    if (registry.has<DeadlyComponent>(entity_))
    {
        saveTo_["deadly_component"] = SerializeComponent<DeadlyComponent>(registry.get<DeadlyComponent>(entity_));
        archetype = archetype | ECommonSaveArchetype::Deadly;
    }

    if (registry.has<InteractableComponent>(entity_))
    {
        saveTo_["interactable_component"] = SerializeComponent<InteractableComponent>(registry.get<InteractableComponent>(entity_));
        archetype = archetype | ECommonSaveArchetype::Interactable;
    }

    // todo: add new if-block here and don't forget to change archetype

    return archetype;
}

void AtonementGame::Load(ECommonSaveArchetype archetype_, Entity entity_, JSON::json& loadFrom_)
{
    auto& registry = Engine::Registry();

    if (IS_ARCHETYPE_SET(archetype_, ECommonSaveArchetype::Sprite))
        DeserializeComponent<Sprite>(loadFrom_["sprite"], registry.emplace<Sprite>(entity_));

    if (IS_ARCHETYPE_SET(archetype_, ECommonSaveArchetype::Transform))
        DeserializeComponent<Transform>(loadFrom_["transform"], registry.emplace<Transform>(entity_));

    if (IS_ARCHETYPE_SET(archetype_, ECommonSaveArchetype::Animator))
        DeserializeComponent<Animator>(loadFrom_["animator"], registry.emplace<Animator>(entity_));

    if (IS_ARCHETYPE_SET(archetype_, ECommonSaveArchetype::Physics))
        DeserializeComponent<SimpleCollision>(loadFrom_["simple_collision"], registry.emplace<SimpleCollision>(entity_));

    if (IS_ARCHETYPE_SET(archetype_, ECommonSaveArchetype::Bouncy))
        DeserializeComponent<BouncyComponent>(loadFrom_["bouncy_component"], registry.emplace<BouncyComponent>(entity_));

    if (IS_ARCHETYPE_SET(archetype_, ECommonSaveArchetype::Deadly))
        DeserializeComponent<DeadlyComponent>(loadFrom_["deadly_component"], registry.emplace<DeadlyComponent>(entity_));

    if (IS_ARCHETYPE_SET(archetype_, ECommonSaveArchetype::Interactable))
        DeserializeComponent<InteractableComponent>(loadFrom_["interactable_component"], registry.emplace<InteractableComponent>(entity_));

    // todo: add new if-block here and don't forget to change archetype
}

void AtonementGame::RestartGame()
{
        auto* camera = Engine::GetDefaultResource<Camera>();
        camera->mode = ECameraMode::FixedResolution;
        camera->size = { 1920, 1080 };
        camera->zoom = 1;
        camera->position = { 0, 0, 0 };
        camera->Update();
    
    auto&& view = Engine::Registry().view<AtonementController::AtonementCharacter, InputReceiver, Transform>();
    
        for (const auto& entity : view)
        {
            auto&& input = view.get<InputReceiver>(entity);
            auto&& transf = view.get<Transform>(entity);

            input.contexts.pop_back();
            input.contexts.push_back("ATON");


            

            transf.position = Vector3{ -100, -200, 15 };
    }
}