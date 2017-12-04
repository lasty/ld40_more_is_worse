
#pragma once

// Some standard container helpers, and other misc stuff

#include <algorithm>
#include <random>

#include "maths_types.hpp"


template<typename CONT, typename PRED>
void remove_if_inplace(CONT &container, const PRED &pred)
{
  container.erase(
    std::remove_if(container.begin(), container.end(), pred), container.end());
}


template<typename CONT>
void insert_at_end(CONT &container, CONT &items)
{
  container.insert(container.end(), items.begin(), items.end());
}


template<typename CONT, typename KEY>
bool key_exists(CONT &container, const KEY &key)
{
  auto it = container.find(key);
  return (it != container.end());
}


////////////////////////


class Random
{
private:
  std::mt19937 mt_rand;

public:
  Random();

  int Int(int min, int max);
  float Float(float min, float max);

  vec2 Position(float min = 0.0f, float max = 1000.0f);
  col4 Colour();
  col4 ColourVarying(col4 colour);

  template<typename CONT>
  auto PickList(const CONT &container)
  {
    int index = Int(0, container.size() - 1);
    return container.at(index);
  }

  int Percent();
};
