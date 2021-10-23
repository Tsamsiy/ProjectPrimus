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
	uint8_t AC = 0;
	uint8_t AV = 0;
	//resistances
	//conditions
};

//offensive items, Arms, Weapons
struct ARMItem
{
	//class proficency
	//damageType
	//statusEffects

	std::string name;
	std::string description;
	double baseDamage = 0.0;
	double acuracyMod = 0.0;
	double damageMod = 0.0;
	double salvoMod = 0.0;
};