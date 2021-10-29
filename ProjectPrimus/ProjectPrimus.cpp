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

double mapScale = 2.0;
int16_t xPos = 0;
int16_t yPos = 0;
int16_t dx = 0;
int16_t dy = 0;

bool MS_LEFT_PRESS = false;

class GameWindow : public Gosu::Window
{
public:

	GameWindow()
		: Window(800, 600)
	{
		set_caption("Project Primus");
	}

	// Wird bis zu 60x pro Sekunde aufgerufen.
	// Wenn die Grafikkarte oder der Prozessor nicht mehr hinterherkommen,
	// dann werden `draw` Aufrufe ausgelassen und die Framerate sinkt
	void draw() override
	{
		if (input().down(Gosu::MS_LEFT))
		{
			world.maps.at(0).drawMap((xPos + (input().mouse_x() - dx)), (yPos + (input().mouse_y() - dy)), 0, mapScale);
			entity.draw(world.maps.at(0), (xPos + (input().mouse_x() - dx)), (yPos + (input().mouse_y() - dy)), mapScale);
		}
		else
		{
			world.maps.at(0).drawMap(xPos, yPos, 0, mapScale);
			entity.draw(world.maps.at(0), xPos, yPos, mapScale);
		}
	}

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

		if (input().down(Gosu::MS_WHEEL_DOWN))
		{
			mapScale += 0.1;
			if (mapScale > 2.5)
			{
				mapScale = 2.5;
			}
		}
		if (input().down(Gosu::MS_WHEEL_UP))
		{
			mapScale -= 0.1;
			if (mapScale < 0.5)
			{
				mapScale = 0.5;
			}
		}
	}
};

// C++ Hauptprogramm
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

	entity.texture.loadSprite("Data/Entities/PC/MrGeneric/Textures/idle_a1.png");
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
	entity.texture.loadSprite("Data/Entities/PC/MrGeneric/Textures/idle_a12.png");
	
	entity.xTile = 3;
	entity.yTile = 2;
	entity.facing = FACING::UP;

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
