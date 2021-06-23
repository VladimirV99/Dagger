#pragma once
#include "core/system.h"
#include "core/core.h"

using namespace dagger;

struct test_system : public System {
    public:
     test_system();
     virtual String SystemName();
     virtual inline void Run();
};