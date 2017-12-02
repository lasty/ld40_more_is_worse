
#pragma once

// Some standard container helpers

#include <algorithm>


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
