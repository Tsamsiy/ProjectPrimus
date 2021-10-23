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
#include "Sources/Parser.hpp"

std::string worldFolder = "Data/Maps/TestMap";
World world;

double mapScale = 1.0;
unsigned xPos = 0.0;
unsigned yPos = 0.0;
unsigned dx = 0.0;
unsigned dy = 0.0;

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
		}
		else
		{
			world.maps.at(0).drawMap(xPos, yPos, 0, mapScale);
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

	//strExtractByDelim('/', worldFolder);

	GameWindow window;
	window.show();
	//testMap.maps.at(0).drawMap(0, 0);
}
