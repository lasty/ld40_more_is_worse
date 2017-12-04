
#include "factories.hpp"




///////////////////////////////////////


ItemFactory::ItemFactory()
{

  //   active_type_list = {Active_Type::passive,
  //     Active_Type::limited_use,
  //     Active_Type::toggle,
  //     Active_Type::hold_down};

  item_type_list = {Item_Type::health,
    Item_Type::gun};

  monster_type_list = {Monster_Type::dummy, Monster_Type::melee, Monster_Type::shooter};
}


Item ItemFactory::GetCommand(std::string what)
{
  Item i;
  i.name = what;

  i.CreateCommand(what);
  //   i.activate = Active_Type::hold_down;
  i.type = Item_Type::command;

  return i;
}


Item ItemFactory::GenerateRandomItem()
{
  Item i;

  i.type = random.PickList(item_type_list);

  switch (i.type)
  {
    case Item_Type::gun:
      i.name = "Gun";
      i.radius = 15;
      i.colour = random.ColourVarying({0.8f, 0.6f, 0.2f, 1.0f});

      i.CreateGun(random.Int(1, 5));
      i.AddCooldown(random.Int(1, 2));
      break;

    case Item_Type::health:
      i.name = "HealthKit";
      i.radius = 20;
      i.colour = random.ColourVarying({0.8f, 0.2f, 0.2f, 1.0f});

      i.CreateHealing(random.Int(5, 20));
      i.AddCooldown(random.Int(3, 10));
      if (random.Percent() < 50)
      {
        i.AddLimitedUses(random.Int(3, 10));
      }
      break;


    case Item_Type::command:
    case Item_Type::none:
      break;
  }


  return i;
}


Monster ItemFactory::GenerateRandomMonster()
{
  Monster m;
  m.type = random.PickList(monster_type_list);

  switch (m.type)
  {
    case Monster_Type::dummy:
      m.health = {1000, 1000};
      m.name = "Training Dummy";
      m.radius = 50;
      break;

    case Monster_Type::melee:
    {
      int h = random.Int(1, 10);
      m.health = {h, h};
      m.name = "Melee monster";
      m.radius = 40;
    }
    break;

    case Monster_Type::shooter:
    {
      int h = random.Int(1, 5);
      m.health = {h, h};
      m.name = "Shooter monster";
      m.radius = 30;
    }
    break;

    case Monster_Type::none:
      break;
  }

  return m;
}

