#include "test_system.h"
#include "core/engine.h"
#include "core/system.h"
#include <iostream>
using namespace dagger;

test_system::test_system(){
    canBePaused = true;
};

void test_system::Run(){
    while (this->isPaused == false){
        std::cout<<"active ";
    }
};

String test_system::SystemName(){
    return "test_system";
};