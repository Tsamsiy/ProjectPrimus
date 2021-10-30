#include <Gosu/Gosu.hpp>
#include <Gosu/AutoLink.hpp>

#include <iostream>
#include <fstream>
#include <cstdint>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <forward_list>
#include <string>

#include "Sources/Maps.hpp"
#include "Sources/Characters.hpp"
#include "Sources/Parser.hpp"

std::string worldFolder = "Data/Maps/TestMap";
World world;

Character entity;
bool rotate = false;
double tempAngle = 0.0;
bool move = false;
FACING moveDir = FACING::DOWN;
double tempPos = 0.0;
bool animDone = true;

double mapScale = 2.0;
int16_t xPos = 0;
int16_t yPos = 0;
int16_t dx = 0;
int16_t dy = 0;

bool MS_LEFT_PRESS = false;
bool MS_RIGHT_PRESS = false;
bool KB_A_PRESS = false;
bool KB_W_PRESS = false;
bool KB_S_PRESS = false;
bool KB_D_PRESS = false;
//bool KB_Q_PRESS = false;
//bool KB_E_PRESS = false;

class GameWindow : public Gosu::Window
{
public:

	GameWindow()
		: Window(800, 600)
	{
		set_caption("Project Primus");
	}

	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Wird bis zu 60x pro Sekunde aufgerufen.
	// Wenn die Grafikkarte oder der Prozessor nicht mehr hinterherkommen,
	// dann werden `draw` Aufrufe ausgelassen und die Framerate sinkt
	void draw() override
	{
		//this is needed to be able to move the map
		//while dragging
		if (input().down(Gosu::MS_LEFT))
		{
			//draw map
			world.maps.at(0).drawMap((xPos + (input().mouse_x() - dx)), (yPos + (input().mouse_y() - dy)), 0, mapScale);
			//draw character sprite
			//character is moving or rotating
			if (!animDone)
			{
				if (move)
				{
					if (entity.getFacing() != moveDir)
					{
						rotate = true;
					}
					else
					{
						uint16_t toXTile = entity.getXTile();
						uint16_t toYTile = entity.getYTile();
						switch (moveDir)
						{
							case FACING::DOWN:
								toYTile++;
							break;

							case FACING::LEFT:
								toXTile--;
							break;

							case FACING::UP:
								toYTile--;
							break;

							case FACING::RIGHT:
								toXTile++;
							break;
						}

						//move if the tile is free/exists
						if (!world.maps.at(0).tileBlocked(toXTile, toYTile))
						{
							move = !entity.drawMove(world.maps.at(0), (xPos + (input().mouse_x() - dx)), (yPos + (input().mouse_y() - dy)), tempPos, 4, mapScale, moveDir);
						}
						else
						{
							move = false;
						}
					}
				}
				if (rotate)
				{
					rotate = !entity.drawRotate(world.maps.at(0), (xPos + (input().mouse_x() - dx)), (yPos + (input().mouse_y() - dy)), tempAngle, 4, mapScale, moveDir);
				}
			}
			//character is still
			else
			{
				entity.draw(world.maps.at(0), (xPos + (input().mouse_x() - dx)), (yPos + (input().mouse_y() - dy)), mapScale);
			}
		}
		//while dropped
		else
		{
			//draw map
			world.maps.at(0).drawMap(xPos, yPos, 0, mapScale);
			//draw character sprite
			//character is moving or rotating
			if (!animDone)
			{
				if (move)
				{
					if (entity.getFacing() != moveDir)
					{
						rotate = true;
					}
					else
					{
						uint16_t toXTile = entity.getXTile();
						uint16_t toYTile = entity.getYTile();
						switch (moveDir)
						{
						case FACING::DOWN:
							toYTile++;
							break;

						case FACING::LEFT:
							toXTile--;
							break;

						case FACING::UP:
							toYTile--;
							break;

						case FACING::RIGHT:
							toXTile++;
							break;
						}

						//move if the tile is free/exists
						if (!world.maps.at(0).tileBlocked(toXTile, toYTile))
						{
							move = !entity.drawMove(world.maps.at(0), xPos, yPos, tempPos, 4, mapScale, moveDir);
						}
						else
						{
							move = false;
						}
					}
				}
				if (rotate)
				{
					rotate = !entity.drawRotate(world.maps.at(0), xPos, yPos, tempAngle, 4, mapScale, moveDir);
				}
			}
			//character is still
			else
			{
				entity.draw(world.maps.at(0), xPos, yPos, mapScale);
			}
		}

		if (!rotate && !move)
		{
			animDone = true;
		}
	}

	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Wird 60x pro Sekunde aufgerufen
	void update() override
	{
		//faling edge
		if (input().down(Gosu::MS_LEFT) && !MS_LEFT_PRESS)
		{
			//std::cout << "MS_LEFT pressed\n";
			MS_LEFT_PRESS = true;

			dx = input().mouse_x();
			dy = input().mouse_y();
		}
		//rising edge
		if (!input().down(Gosu::MS_LEFT) && MS_LEFT_PRESS)
		{
			//std::cout << "MS_LEFT released\n";
			MS_LEFT_PRESS = false;

			xPos += input().mouse_x() - dx;
			yPos += input().mouse_y() - dy;
		}

		//faling edge
		if (input().down(Gosu::MS_RIGHT) && !MS_RIGHT_PRESS)
		{
			//std::cout << "MS_RIGHT pressed\n";
			MS_RIGHT_PRESS = true;

		}
		//rising edge
		if (!input().down(Gosu::MS_RIGHT) && MS_RIGHT_PRESS)
		{
			//std::cout << "MS_RIGHT released\n";
			MS_RIGHT_PRESS = false;

		}

		//faling edge
		if (input().down(Gosu::KB_A) && !KB_A_PRESS)
		{
			//std::cout << "KB_A pressed\n";
			KB_A_PRESS = true;

			//another animation is going on
			if (animDone)
			{
				animDone = false;
				move = true;
				moveDir = FACING::LEFT;
			}
		}
		//rising edge
		if (!input().down(Gosu::KB_A) && KB_A_PRESS)
		{
			//std::cout << "KB_A released\n";
			KB_A_PRESS = false;

		}

		//faling edge
		if (input().down(Gosu::KB_W) && !KB_W_PRESS)
		{
			//std::cout << "KB_W pressed\n";
			KB_W_PRESS = true;

			//another animation is going on
			if (animDone)
			{
				animDone = false;
				move = true;
				moveDir = FACING::UP;
			}
		}
		//rising edge
		if (!input().down(Gosu::KB_W) && KB_W_PRESS)
		{
			//std::cout << "KB_W released\n";
			KB_W_PRESS = false;

		}

		//faling edge
		if (input().down(Gosu::KB_S) && !KB_S_PRESS)
		{
			//std::cout << "KB_S pressed\n";
			KB_S_PRESS = true;

			//another animation is going on
			if (animDone)
			{
				animDone = false;
				move = true;
				moveDir = FACING::DOWN;
			}
		}
		//rising edge
		if (!input().down(Gosu::KB_S) && KB_S_PRESS)
		{
			//std::cout << "KB_S released\n";
			KB_S_PRESS = false;

		}

		//faling edge
		if (input().down(Gosu::KB_D) && !KB_D_PRESS)
		{
			//std::cout << "KB_D pressed\n";
			KB_D_PRESS = true;

		}
		//rising edge
		if (!input().down(Gosu::KB_D) && KB_D_PRESS)
		{
			//std::cout << "KB_D released\n";
			KB_D_PRESS = false;

			//another animation is going on
			if (animDone)
			{
				animDone = false;
				move = true;
				moveDir = FACING::RIGHT;
			}
		}

		

		if (input().down(Gosu::KB_I))
		{
			mapScale += 0.1;
			if (mapScale > 2.5)
			{
				mapScale = 2.5;
			}
		}
		if (input().down(Gosu::KB_O))
		{
			mapScale -= 0.1;
			if (mapScale < 0.5)
			{
				mapScale = 0.5;
			}
		}
	}
};

//#######################################################################################################################################################################
int main()
{
	if (!world.items.loadItems())
	{
		std::cout << "\tNo valid Item definitions found!\n";
	}
	else
	{
		std::cout << "\tItem lists loaded\n";
	}

	if (!world.loadWorldFile(worldFolder))
	{
		std::cout << "Unable to load any maps for world: \"" << worldFolder << "\"\n";
		std::cout << "Check the supplied path to world folder;\n";
		std::cout << "Check if world folder contains Summary.txt;\n";
		std::cout << "Check paths to map definition files in Summary.txt;\n";
		std::cout << "Check paths to tile textures in map definitions file;\n";
		std::cout << "Check content in map definitions file;\n\n";
	}
	else
	{
		std::cout << "\tWorld loaded: \"" << worldFolder << "\"\n";
	}

	/*entity.texture.loadSprite("Data/Entities/PC/MrGeneric/Textures/idle_a1.png");
	entity.texture.loadSprite("Data/Entities/PC/MrGeneric/Textures/idle_a2.png");
	entity.texture.loadSprite("Data/Entities/PC/MrGeneric/Textures/idle_a3.png");
	entity.texture.loadSprite("Data/Entities/PC/MrGeneric/Textures/idle_a4.png");
	entity.texture.loadSprite("Data/Entities/PC/MrGeneric/Textures/idle_a5.png");
	entity.texture.loadSprite("Data/Entities/PC/MrGeneric/Textures/idle_a6.png");
	entity.texture.loadSprite("Data/Entities/PC/MrGeneric/Textures/idle_a7.png");

	entity.texture.loadSprite("Data/Entities/PC/MrGeneric/Textures/idle_a8.png");
	entity.texture.loadSprite("Data/Entities/PC/MrGeneric/Textures/idle_a9.png");
	entity.texture.loadSprite("Data/Entities/PC/MrGeneric/Textures/idle_a10.png");
	entity.texture.loadSprite("Data/Entities/PC/MrGeneric/Textures/idle_a11.png");
	entity.texture.loadSprite("Data/Entities/PC/MrGeneric/Textures/idle_a12.png");*/
	if (!entity.loadCharacterSheet("Data/Entities/PC/MrGeneric/CharacterSheet.txt"))
	{
		//while (true);
	}
	entity.xTile = 3;
	entity.yTile = 2;
	entity.facing = FACING::RIGHT;

	tempAngle = (double)entity.facing;

	GameWindow window;
	window.show();
	

	/*std::string test = "-1523.0150";
	
	//strExtractByDelim('/', worldFolder);
	
	double num = 0;
	std::cout << strParseDouble(test, num) << "\n";
	std::cout << num << "\n";
	std::cout << std::stod(test) << "\n";*/

	//for (int i = 0; i < 3; i++)
	//{
	//	std::cout << strAccessCSV(test, '.', i) << "\n";
	//}
	
}
