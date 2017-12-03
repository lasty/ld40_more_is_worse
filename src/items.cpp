
#include "items.hpp"


ItemFactory::ItemFactory()
{

  active_type_list = {Active_Type::passive,
    Active_Type::limited_use,
    Active_Type::toggle,
    Active_Type::hold_down};

  item_type_list = {Item_Type::health,
    Item_Type::gun};
}


Item ItemFactory::GetCommand(std::string what)
{
  Item i;
  i.name = what;
  i.activate = Active_Type::hold_down;
  i.type = Item_Type::command;

  return i;
}


Item ItemFactory::GenerateRandomItem()
{
  Item i;

  Item_Type type = random.PickList(item_type_list);

  switch (type)
  {
    case Item_Type::gun:
      i.name = "Gun";
      i.radius = 15;
      i.colour = random.ColourVarying({0.8f, 0.6f, 0.2f, 1.0f});
      break;

    case Item_Type::health:
      i.name = "HealthKit";
      i.radius = 20;
      i.colour = random.ColourVarying({0.8f, 0.2f, 0.2f, 1.0f});
      break;

    case Item_Type::command:
    case Item_Type::none:
      break;
  }


  return i;
}