#pragma once

#include "game_state.hpp"

#include <vector>

class ItemFactory
{
    public:
    ItemFactory();


    Item GetCommand(std::string what);
};