#pragma once

#include <Gosu/Gosu.hpp>
#include <Gosu/AutoLink.hpp>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <math.h>

#include "Characters.hpp"

/*

-y
^
|
*--> x

*/

class Map;
class Character;

class World
{
//private:
public:
	std::vector<Map> maps;				//each map/level of the world. organisation in regions and levels is implemented later
	std::vector<Character> entities;	//all the characters that play on this world
	Item items;							//global list of all standard items in the game. not dependant on which world is loaded

//public:

	bool loadWorldFile(std::string path);		//loads the map contents from the world folder. needs path to the folder where Summary.txt is stored 
};

//individual building blocks of the map, that consists of a background image and if that tile is walkable by the player
enum TileType
{
	blnk,	//blank: Tile "not existant", no texture needed, automatically blocked
	strd,	//standard: with texture, can be blocked, no effects on character
	spFu	//special function: with texture, can be blocked, effect defined in parameter string
};
struct Tile
{
	TileType type = blnk;
	//Gosu::Image* texture = NULL;
	unsigned texture = 0;
	bool blocked = true;
	std::string parameter;
};

class Map
{
private:
	uint16_t rows = 0;			//number of tiles in y direction (outer vector)
	uint16_t cols = 0;			//number of tiles in x direction (inner vector)
	unsigned height = 64;		//size of the displayed area per tile, not the size of the contained images
	unsigned width = 64;		//standard is 64, but can be changed 
	std::vector<Gosu::Image> tiles;			//list of all tile textures (saves memory)
	std::vector<struct Tile> content;		//actual content refferes to tiles

	bool parseTiles(std::string path, std::ifstream& file, std::streampos start, std::streampos stop);		//loads textures. is called by Map::loadMapFile()
	bool parseContent(std::string path, std::ifstream& file, std::streampos start, std::streampos stop);		//loads and processes map content definitions. is called by Map::loadMapFile()

public:
	uint16_t getRows() const;			//returns number of tiles in y direction 
	uint16_t getCols() const;			//returns number of tiles in x direction
	unsigned getTileHeight() const;			//returns height of the tiles
	unsigned getTileWidth() const;			//returns width of the tiles
	bool tileBlocked(uint16_t x, uint16_t y) const;		//returns the blocked variable of the tile at the given coordinates
	//void drawMap(double x, double y, Gosu::ZPos z = 0, double angle = 0, double center_x = 0.5, double center_y = 0.5, double scale_x = 1, double scale_y = 1, Gosu::Color c = Gosu::Color::WHITE /*, Gosu::AlphaMode mode = AM_DEFAULT*/);				//this goes in GameWindow::draw()
	void drawMap(double xPos, double yPos, Gosu::ZPos z = 0, double scale = 1.0) const;			//simple version
	void getTileCoords(uint16_t xTile, uint16_t yTile, double& xPos, double& yPos, double scale = 1.0) const;			//returns the coordinates where the given tile's upper left corner would be (map at 0, 0)
	bool loadMapFile(std::string path);			//this reads the map files provided by Summary.txt and loads the contents. is called by World::loadWorldFile()

};