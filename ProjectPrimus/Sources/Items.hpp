#pragma once

#include <Gosu/Gosu.hpp>
#include <Gosu/AutoLink.hpp>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <math.h>

struct ItemEffect
{
	//stat = 
	char mode = '=';		//how to calculate the value
							//'=' replace
							//'+' = add
							//'-' = subtract
							//'*' = multiply
	double value = 0;
};

struct WeaponEffect
{
	ItemEffect effect;
	uint8_t DC = 0;
};

//standard items, may have effects
struct STDItem
{
	std::string name;
	std::string description;
	bool consumable = false;
	unsigned uses = 0;
	std::vector<ItemEffect> active;
	std::vector<ItemEffect> passive;
};

//defensive items, Armor
struct DEFItem
{
	std::string name;
	std::string description;
	int8_t AC = 0;
	uint8_t AV[2] = {0, 0};
	//resistances
	//conditions
};

//offensive items, Arms, Weapons
struct ARMItem
{
	//class proficency
	//damageType

	std::string name;
	std::string description;
	double baseDamage = 0.0;
	double acuracyMod = 0.0;
	double damageMod = 0.0;
	double salvoMod = 0.0;
	std::vector<WeaponEffect> effects;
};

class Item
{
private:
	std::vector<STDItem> items;			//global list of all standard items in the game. not dependant on which world is loaded
	std::vector<DEFItem> armors;		//global list of all armors in the game. not dependant on which world is loaded
	std::vector<ARMItem> weapons;		//global list of all weapons in the game. not dependant on which world is loaded

	bool loadSTDItemFile(std::string path);		//loads the contents standard items definition list. called by Item::loadItems()
	bool loadDEFItemFile(std::string path);		//loads the contents armor items definition list. called by Item::loadItems()
	bool loadARMItemFile(std::string path);		//loads the contents weapon items definition list. called by Item::loadItems()
public:
	bool loadItems(std::string stdPath = "Data/Items/Standard.txt", std::string defPath = "Data/Items/Armor.txt", std::string armPath = "Data/Items/Weapons.txt");
};