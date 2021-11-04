#pragma once

#include <Gosu/Gosu.hpp>
#include <Gosu/AutoLink.hpp>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <math.h>

#include "Items.hpp"
#include "Maps.hpp"

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
	int STR = 0;
	int DEX = 0;
	int CON = 0;
	int INT = 0;
	int WIS = 0;
	int CHA = 0;
};
enum class FACING
{
	DOWN = 0,	//°
	LEFT = 90,	//°
	UP = 180,   //°
	RIGHT = 270	//°
};
FACING incFacing(FACING lhs);
FACING decFacing(FACING lhs);
//used to keep track of what animation is currently running
enum class AnimState
{
	idle,
	walk,
	attack,		//may not be interrupted
	hit			//may not be interrupted
};
struct Animation
{
	int invervall = -1;				//interval between frames in milliseconds, -1 if no animation frames available
	std::vector<Gosu::Image> frames;	//list of textures that get periodically updated
};


class Sprite
{
private:
	//unsigned height;
	//unsigned width;
	//frames should be drawn facing down per standard
	Animation idleAnim;			//list of textures that get periodically updated
	Animation walkAnim;			//list of textures that get periodically updated
	Animation attackAnim;		//list of textures that get periodically updated
	Animation hitAnim;			//list of textures that get periodically updated
	AnimState animState = AnimState::idle;				//used to keep track of what animation is currently running
	size_t animPointer = 0;					//used as a local bookmark to remember which frame the animation is currently on
	int animTimer = 0;

	bool loadAnimation(std::string path, std::ifstream& file, Animation& anim, std::streampos start, std::streampos stop);		//loads the animation frames. Called by Sprite::LoadSprite()

public:
	//unsigned getHeight();
	//unsigned getWidth();
	//unsigned get_framecount(AnimState state) const;

	bool loadSprite(std::string path, std::ifstream& file, std::streampos start, std::streampos stop);			//loads all animations of the sprite. Called by Character::loadCharactersheet()
	void draw(double xPos, double yPos, Gosu::ZPos z = 1, AnimState state = AnimState::idle, double angle = 0.0, double scale = 1.0);
};

class Map;

class Character
{
public:
//private:
	std::string name = "Entity";		//identifier string, displayed name

	uint8_t HP[256];		//Array of stat bars:
							//HP[0] is always the actual HP
							//the other entries can be used for hidden stats and internal values (like stamina or heath reserve)
	uint8_t AC;				//Armor Class: how difficult you are to hit
	uint8_t AV;				//Armor Value: how much damage is absorbed
	Ability abilityMods;	//Ability modifiers
	uint16_t movementSpeed;

	std::vector<ItemInventory> items;	//contains references to actual items in different list and additional info
	std::vector<ArmorInventory> armors;
	std::vector<WeaponInventory> weapons;

	Sprite texture;				//what your character looks like
	FACING facing = FACING::DOWN;				//determines which sprite/animation is displayed

	uint16_t xTile;	//Coordinates on the map
	uint16_t yTile;	//

	bool loadCharacterInfo(std::string path);		//reads the rest of the characters world-dependant properties. is called by Character::loadCharacterFiles()
	bool loadCharacterSheet(std::string path);		//reads character base properties. is called by Character::loadCharacterInfo()

public:
	bool loadCharacterFiles(std::string path);	//loads the characters that play in a world. is called by World::loadWorldFile()

	FACING getFacing();
	uint16_t getXTile();
	uint16_t getYTile();
	
	void update();								//this goes in GameWindow::update() . Calls Character::move()
	bool move(FACING dir);						//set x and y coordinate on the map
	void draw(const Map& map, double xPos, double yPos, double scale = 1.0, AnimState state = AnimState::idle);					//this goes in GameWindow::draw() . Calls Map::getTileCoords()
	bool drawRotate(const Map& map, double xPos, double yPos, double& angle, unsigned interval = 1.0, double scale = 1.0, FACING dir = FACING::DOWN);					//animates rotating the character. dir: new facing, interval: delta angle per frame
	bool drawMove(const Map& map, double xPos, double yPos, double& pos, unsigned interval = 1.0, double scale = 1.0, FACING dir = FACING::DOWN);					//animates moving character to neighbouring tile. dir: which tile, interval: delta pixel per frame
};