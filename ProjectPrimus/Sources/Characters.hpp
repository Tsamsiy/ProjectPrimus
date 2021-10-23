#pragma once

#include <Gosu/Gosu.hpp>
#include <Gosu/AutoLink.hpp>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <math.h>

#include "Items.hpp"

//inventory objects: references to elements in item lists
struct ItemInventory
{
	STDItem* item;
	unsigned int count;
	bool equipped;
};
struct ArmorInventory
{
	DEFItem* item;
	bool equipped;
};
struct WeaponInventory
{
	ARMItem* item;
	bool equipped;
};

struct Ability
{
	int STR;
	int DEX;
	int CON;
	int INT;
	int WIS;
	int CHA;
};
enum FACING
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};
class Sprite
{
	//unsigned height;
	//unsigned width;
	double invervall = -1.0;				//-1 if no animation frames available
	std::vector<Gosu::Image> idleAnim;		//list of textures that get periodically updated
	std::vector<Gosu::Image> walkAnim;		//list of textures that get periodically updated
	std::vector<Gosu::Image> attackAnim;	//list of textures that get periodically updated
	std::vector<Gosu::Image> hitAnim;		//list of textures that get periodically updated

public:
	//unsigned getHeight();
	//unsigned getWidth();
	unsigned get_framecount();
	bool loadSprite(std::string path);
};

class Character
{
private:
	std::string name = "Entity";		//identifier string, displayed name

	uint8_t HP[256];		//Array of stat bars:
							//HP[0] is always the actual HP
							//the other entries can be used for hidden stats and internal values (like stamina or heath reserve)
	uint8_t AC;				//Armor Class: how difficult you are to hit
	uint8_t AV;				//Armor Value: how much damage is absorbed
	Ability abilityMods;	//Ability modifiers
	uint16_t movementspeed;

	std::vector<ItemInventory> items;	//contains references to actual items in different list and additional info
	std::vector<ArmorInventory> armors;
	std::vector<WeaponInventory> weapons;

	Sprite texture;				//what your character looks like
	FACING facing;				//determines which sprite/animation is displayed

	uint16_t xTile;	//Coordinates on the map
	uint16_t yTile;	//

	bool loadCharacterInfo();		//reads the rest of the characters world-dependant properties. is called by Character::loadCharacterFiles()
	bool loadCharacterSheet();		//reads character base properties. is called by Character::loadCharacterInfo()

public:
	bool loadCharacterFiles(std::string path);	//loads the characters that play in a world. is called by World::loadWorldFile()
	bool move(uint16_t x, uint16_t y);		//set x and y coordinate on the map
};









//In Arbeit
//Rassen und Characterklassen?
class Subcharacter : public Character 
{
public:
	struct Specab	//Special Abilities
	{
		int special;
	};
};