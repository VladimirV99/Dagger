#pragma once

#include "core/core.h"
#include "core/system.h"
#include "core/engine.h"

#include <map>
#include <utility>
#include <string>

using namespace dagger;

template <typename T>
class CooldownManager : public System
{

public:

    String SystemName() override {
        return "Cooldown Manager";
    }

    void Run() {
        for (auto& [key, val] : lookup) {
            val.time += Engine::DeltaTime();
        }
    }

    void registerCooldown(const Entity& entity, const T& identifier, Float32 duration) {
        auto key = std::make_pair(entity, identifier);
        auto res = lookup.find(key);

        if (res != lookup.end()) {
            lookup.erase(res);
        }

        lookup.insert(std::make_pair(key, Cooldown(entity, identifier, duration)));
    }

    Bool isOnCooldown(const Entity& entity, const T& identifier) {
        auto key = std::make_pair(entity, identifier);
        auto res = lookup.find(key);
        if (res == lookup.end()) {  // not found, so never registered
            return false;
        }
        else { // found

            auto& cd = res->second;
            if (cd.time < cd.duration) { // hasn't expired
                return true;
            }
            else { // expired
                return false;
            }
        }
    }

private:
    class Cooldown;

    std::map<std::pair<Entity, std::string>, Cooldown> lookup = std::map<std::pair<Entity,String>, Cooldown>();

    class Cooldown
    {
    public:
        Cooldown(const Entity& entity, const T& identifier, Float32 duration)
            : entity(entity), identifier(identifier), duration(duration) {}

        Cooldown(const Cooldown& other)
            : entity(other.entity), identifier(other.identifier), duration(other.duration) {}

        Entity entity;
        T identifier;
        Float32 duration;
        Float32 time = 0;
    };
};