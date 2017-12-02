
#include "items.hpp"


ItemFactory::ItemFactory()
{
}


Item ItemFactory::GetCommand(std::string what)
{
  Item i;
  i.name = what;
  i.activate = Active_Type::hold_down;
  i.type = Item_Type::command;

  return i;
}
