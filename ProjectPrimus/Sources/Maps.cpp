#include "Maps.hpp"
#include "Parser.hpp"

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool World::loadWorldFile(std::string path)
{
	bool parseOk = false;
	std::string temp;
	std::streampos start;
	std::streampos stop;

	std::cout << "reading world summary file: \"" << path << "/Summary.txt\"\n";

	//open file
	std::ifstream file(path + "/Summary.txt");
	if (!file)
	{
		std::cout << "\tFile not found: \"" << path << "/Summary.txt\"\n";
		return false;
	}

	if (!seekField(file, "World", '[', ']', start, stop))
	{
		//main definitions are missing in the document, so skip it
		std::cout << "\tMissing data list \"World\"[...]\n";
		file.close();
		return false;
	}
	//parse the summary line by line
	do
	{
		char in = NULL;
		//read one line
		do
		{
			in = fileGetNoComments(file);
			if (in != '\n')
			{
				temp += in;
			}
		} while ((in != '\n') && (!file.eof()));

		//remove unwanted chars
		removeFormatting(temp);

		//if string is not empty
		if (!temp.empty())
		{
			//std::cout << "detected: " << temp << "\n";
			//only care about the file part of the path
			//paths can also be written without formatting
			strExtractByDelim(temp, false, '<', '>');

			Map tempMap;
			//try to load the map on the detected path
			if (tempMap.loadMapFile(path + "/" + temp))
			{
				this->maps.push_back(tempMap);
				parseOk = true;			//world now contains at least one map
				std::cout << "\t\tMap loaded:\"" << path + "/" + temp << "\"\n";
			}
			else
			{
				std::cout << "\t\tUnable to parse map file:\"" << path + "/" + temp << "\"\n";
			}
		}

		temp.clear();
	} while (file.tellg() < stop);

	file.close();
	return parseOk;
};

//#######################################################################################################################################################################

uint16_t Map::getRows() const
{
	return this->rows;
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t Map::getCols() const
{
	return this->cols;
};

unsigned Map::getTileHeight() const
{
	return this->height;
};

unsigned Map::getTileWidth() const
{
	return this->width;
};

bool Map::tileBlocked(uint16_t x, uint16_t y) const
{
	int i = 0;

	//check if the given coordinates are out of bounds
	if ((x >= this->cols) || (y >= this->rows))
	{
		return true;
	}

	for (unsigned r = 0; r < this->rows; r++)
	{
		for (unsigned c = 0; c < this->cols; c++)
		{
			if ((x == c) && (y == r))
			{
				return this->content.at(i).blocked;
			}

			i++;
		}
	}

	//return true;
};

/*/-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Map::drawMap(double x, double y, Gosu::ZPos z = 0, double angle = 0, double center_x = 0.5, double center_y = 0.5, double scale_x = 1, double scale_y = 1, Gosu::Color c = Gosu::Color::WHITE )//, Gosu::AlphaMode mode = AM_DEFAULT)
{
	//std::cout << "Drawing Map: Size = " << this->content.size() << "\n";
	int i = 0;

	for (unsigned r = 0; r < this->rows; r++)
	{
		for (unsigned c = 0; c < this->cols; c++)
		{
			if (!(i < this->content.size()))
			{
				//reached end of provided map tiles
				//rest of the tiles will be left blank
				return;
			}
			//draw the tile if its not blank
			if ((this->content.at(i).type != blnk))
			{
				double rad = (angle * 3.141592653589793238462643383279) / 180.0;

				double xTilePos = 0.0;
				double yTilePos = 0.0;

				double radius = 0.0;
				double phi = 0.0;

				//calculate coordinates of upper left corner of each tile while unrotated
				xTilePos = ((double)this->width * (double)c);
				yTilePos = ((double)this->height * (double)r);

				//convert to radius and angle
				radius = sqrt(pow(xTilePos, 2) + pow(yTilePos, 2));
				phi = asin(xTilePos / radius);
				//add angle
				phi += rad;
				if (phi > 6.283185307179586476925286766559)
				{
					phi -= 6.283185307179586476925286766559;
				}
				if (phi < 0)
				{
					phi += 6.283185307179586476925286766559;
				}
				//convert back
				xTilePos = radius * cos(phi);
				yTilePos = radius * sin(phi);

				//add offset to origin (upper left corner of window)
				xTilePos += xPos;
				yTilePos += yPos;

				this->tiles.at(this->content.at(i).texture).draw_rot(xTilePos, yTilePos, z, angle, 0.0, 0.0);
			}
			i++;
		}
	}
};*/

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Map::drawMap(double xPos, double yPos, Gosu::ZPos z, double scale) const
{
	//std::cout << "Drawing Map: Size = " << this->content.size() << "\n";
	int i = 0;

	for (unsigned r = 0; r < this->rows; r++)
	{
		for (unsigned c = 0; c < this->cols; c++)
		{
			if (!(i < this->content.size()))
			{
				//reached end of provided map tiles
				//rest of the tiles will be left blank
				return;
			}
			//draw the tile if its not blank
			if ((this->content.at(i).type != blnk))
			{
				double xTilePos = 0.0;
				double yTilePos = 0.0;

				//calculate coordinates of upper left corner of each tile while unrotated
				xTilePos = ((double)this->width * (double)c * scale);
				yTilePos = ((double)this->height * (double)r * scale);

				//needs correction so that no gaps are schowing between tiles
				xTilePos *= (1 - 0.01);
				yTilePos *= (1 - 0.01);

				//add offset to origin (upper left corner of window)
				xTilePos += xPos;
				yTilePos += yPos;

				this->tiles.at(this->content.at(i).texture).draw_rot(xTilePos, yTilePos, z, 0, 0.0, 0.0, scale, scale);
			}
			i++;
		}
	}
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//returns the coordinates where the given tile's upper left corner would be (map at 0, 0)
void Map::getTileCoords(uint16_t xTile, uint16_t yTile, double& xPos, double& yPos, double scale) const
{
	for (unsigned r = 0; r < this->rows; r++)
	{
		for (unsigned c = 0; c < this->cols; c++)
		{
			if ((xTile == c) && (yTile == r))
			{
				double xTilePos = 0.0;
				double yTilePos = 0.0;

				//calculate coordinates of upper left corner of each tile while unrotated
				xTilePos = ((double)this->width * (double)c * scale);
				yTilePos = ((double)this->height * (double)r * scale);

				//needs correction so that no gaps are schowing between tiles
				xTilePos *= (1 - 0.01);
				yTilePos *= (1 - 0.01);

				xPos = xTilePos;
				yPos = yTilePos;
			}
		}
	}
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Map::loadMapFile(std::string path)
{
	//bool parseTilesOk = false;
	//bool parseContentOk = false;
	std::string temp;
	std::streampos start;
	std::streampos stop;

	std::cout << "\tReading map file: \"" << path << "\"\n";

	//open file
	std::ifstream file(path);
	if (!file)
	{
		std::cout << "\t\tFile not found: \"" << path << "\"\n";
		return false;
	}

	//try to find the Tiles[...] data list
	if (seekField(file, "Tiles", '[', ']', start, stop))
	{
		//try to load content of tile list
		//returns true if at least one tile is loaded
		//parseTilesOk = this->parseTiles(file, start, stop);
		if (!this->parseTiles(path, file, start, stop))
		{
			file.close();
			std::cout << "\t\tNo valid tiles in data list \"Tiles\"[...]\n";
			return false;
		}
	}
	else
	{
		//skip content parsing if there are no tiles
		file.close();
		std::cout << "\t\tMissing data list \"Tiles\"[...]\n";
		return false;
	}
	//try to find the Map{...} data field
	if (seekField(file, "Map", '{', '}', start, stop))
	{
		//try to load content definitions
		//returns true if map contains at least one valid tile
		//parseContentOk = this->parseContent(file, start, stop);
		if(!this->parseContent(path, file, start, stop))
		{
			file.close();
			std::cout << "\t\tMissing element in data field \"Map\"{...} or invalid content definition in data list \"Map/Content\"[...]\n";
			return false;
		}
	}
	else
	{
		file.close();
		std::cout << "\t\tMissing data field \"Map\"{...}\n";
		return false;
	}

	file.close();
	//return (parseTilesOk && parseContentOk);
	return true;
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Map::parseTiles(std::string path, std::ifstream& file, std::streampos start, std::streampos stop)
{
	bool parseOk = false;
	std::string temp;
	unsigned count = 0;

	//construct new local path: remove file part of last path
	path = path.substr(0, path.rfind('/'));

	//parse field line by line
	do
	{
		char in = NULL;
		//read one line
		do
		{
			in = fileGetNoComments(file);
			if (in != '\n')
			{
				temp += in;
			}
		} while ((in != '\n') && (file.tellg() < stop));

		//remove unwanted chars
		removeFormatting(temp);

		//if string is not empty
		if (!temp.empty())
		{
			//std::cout << "\t\tLoading tile texture: " << path + "/" + temp << "\n";

			//only care about the file part of the path
			//paths can also be written without formatting
			strExtractByDelim(temp, false, '<', '>');

			//check file validity via tempFile to avoid exception from Gosu
			//if file opens, it can be loaded into Gosu::Image
			std::ifstream tempFile(path + "/" + temp);
			//try to open the file
			if (tempFile)
			{
				tempFile.close();

				Gosu::Image textureFile(path + "/" + temp);
				this->tiles.push_back(textureFile);

				//std::cout << "\t\tTexture loaded: " << path + "/" + temp << "\n";
				count++;
				parseOk = true;
			}
			else
			{
				std::cout << "\t\tUnable to load texture: " << path + "/" + temp << "\n";
			}
		}

		temp.clear();

	}
	while (file.tellg() < stop);

	if (parseOk)
	{
		std::cout << "\t\tParsed " << count << " Textures.\n";
	}

	return parseOk;
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Map::parseContent(std::string path, std::ifstream& file, std::streampos start, std::streampos stop)
{
	bool parseOk = false;
	bool gotRows = false;
	bool gotCols = false;
	std::string temp;
	std::streampos contentStart;
	std::streampos contentStop;

	//construct new local path: remove file part of last path
	path = path.substr(0, path.rfind('/'));

	//parse field line by line
	//first try to find the first to solo variables in the Map field
	do
	{
		char in = NULL;
		//read one line
		do
		{
			in = fileGetNoComments(file);
			if (in != '\n')
			{
				temp += in;
			}
		}
		while ((in != '\n') && (file.tellg() < stop));

		//remove unwanted chars
		removeFormatting(temp);

		//if string is not empty
		if (!temp.empty())
		{
			//std::cout << temp << "\n";

			//found keyword
			if (temp.find("Rows") != std::string::npos)
			{
				//extract value
				temp = temp.substr(temp.find('=') + 1);
				//std::cout << temp << "\n";

				int num = 0;
				//convert to integer
				for (int i = 0; i < temp.size(); i++)
				{
					//this will ignore any char that is not a number
					//unsigned numbers only
					if ((temp.at(i) >= '0') && (temp.at(i) <= '9'))
					{
						num *= 10;
						num += temp.at(i) - '0';
					}
				}
				//std::cout << num << "\n";
				this->rows = num;
				gotRows = true;
			}
			//found keyword
			if (temp.find("Cols") != std::string::npos)
			{
				//extract value
				temp = temp.substr(temp.find('=') + 1);
				//std::cout << temp << "\n";

				int num = 0;
				//convert to integer
				for (int i = 0; i < temp.size(); i++)
				{
					//this will ignore any char that is not a number
					//unsigned numbers only
					if ((temp.at(i) >= '0') && (temp.at(i) <= '9'))
					{
						num *= 10;
						num += temp.at(i) - '0';
					}
				}
				//std::cout << num << "\n";
				this->cols = num;
				gotCols = true;
			}
		}

		temp.clear();
	}
	while((file.tellg() < stop) && (!gotRows || !gotCols));
	//already at the end of the Map field and one of those is missing
	if (!gotRows || !gotCols)
	{
		std::cout << "\t\tMissing variable in \"Map\"{...}\n";
		std::cout << "\t\t\tMissing elements:";
		if (!gotRows)
		{
			std::cout << "  \"Rows = <uint16_t>\"";
		}
		if (!gotCols)
		{
			std::cout << "  \"Cols = <uint16_t>\"";
		}
		std::cout << "\n";
		return false;
	}

	//move on to parsing the content definition
	//first try to find the Content field
	if (!seekField(file, "Content", '[', ']', contentStart, contentStop))
	{
		//main definitions are missing in the document, so skip it
		std::cout << "\t\tMissing data list \"Content\"[...]\n";
		return false;
	}
	temp.clear();
	uint32_t blockCount = 0;
	//empty temporary tile where info is gathered
	//is blank by default
	Tile tempTile;

	do
	{
		bool gotType = false;
		bool gotTile = false;
		bool gotBlocked = false;
		bool gotParameter = false;
		std::streampos blockEnd;

		//find start of next block in the list 
		while (fileGetNoComments(file) != '{')
		{
			//no block start found at first pass through Content field: no blocks in the field
			if(!(file.tellg() < contentStop) && !parseOk)
			{
				std::cout << "\t\tMissing data field in \"Content\"[...]\n";
				std::cout << "\t\t\tNeeds to contain at least one tile definition\n";
				return false;
			}
			//end of content block
			if (!(file.tellg() < contentStop))
			{
				if (parseOk)
				{
					std::cout << "\t\tParsed " << blockCount << " Tiles. (Map size: [" << this->rows * this->cols << "])\n";
				}
				return parseOk;
			}
		}
		if (!seekBlockBracket(file, '}', blockEnd))
		{
			//syntax error if it doesn't find the closing bracket
			std::cout << "\t\tSyntax error: missing bracket in \"Content\"[...]\n";
			return false;
		}

		do
		{
			char in = NULL;
			//read one line
			do
			{
				in = fileGetNoComments(file);
				if (in != '\n')
				{
					temp += in;
				}
			}
			while ((in != '\n') && (file.tellg() < blockEnd));

			//remove unwanted chars
			removeFormatting(temp);

			//if string is not empty
			if (!temp.empty())
			{
				//found keyword
				if (temp.find("Type") != std::string::npos)
				{
					//extract value
					temp = temp.substr(temp.find('=') + 1);
					//std::cout << temp << "\n";

					if (temp == "blnk")
					{
						tempTile.type = blnk;
						gotType = true;

						tempTile.blocked = true;
						gotBlocked = true;
					}
					else if (temp == "strd")
					{
						tempTile.type = strd;
						gotType = true;
					}
					else if (temp == "spFu")
					{
						tempTile.type = spFu;
						gotType = true;
					}
					else
					{
						std::cout << "\t\tSyntax error: unknown value of \"Type\" in \"Content\"[" << blockCount << "]\n";
						//skip this one
						return false;
					}
				}
				//found keyword
				if (temp.find("Tile") != std::string::npos)
				{
					//extract value
					temp = temp.substr(temp.find('=') + 1);
					//std::cout << temp << "\n";

					unsigned num = 0;
					//convert to integer
					for (int i = 0; i < temp.size(); i++)
					{
						//this will ignore any char that is not a number
						//unsigned numbers only
						if ((temp.at(i) >= '0') && (temp.at(i) <= '9'))
						{
							num *= 10;
							num += temp.at(i) - '0';
						}
						else
						{
							std::cout << "\t\tSyntax error: unknown value of \"Tile\" in \"Content\"[" << blockCount << "]\n";
							//skip this one
							return false;
						}
					}

					//is the wanted tile part of the list
					if (num < this->tiles.size())
					{
						//save reference to tile list
						//tempTile.texture = &(this->tiles.at(num));
						tempTile.texture = num;
						gotTile = true;
					}
					else
					{
						tempTile.type = blnk;
						tempTile.blocked = true;

						std::cout << "\t\tWarning: unable to find referenced texture in \"Content\"[" << blockCount << "]\n";
						std::cout << "\t\t\t!! This may cause textures to appear on the wrong tiles !!\n";
						std::cout << "\t\t\tCheck paths in \"Tiles\"[...]\n";
						std::cout << "\t\t\tThis tile will be left blank\n";
						//although this most certainly will cause the map to be displayed wrong because
						//all tile references will be shifted, continuing the parsing process gives the map creator the
						//chance to correct more than one mistake in one go
						gotTile = true;
						gotBlocked = true;
						//return false;
					}
				}
				//found keyword,  may be preset by Type = blnk
				if ((temp.find("Blocked") != std::string::npos) && (!gotBlocked))
				{
					//extract value
					temp = temp.substr(temp.find('=') + 1);
					//std::cout << temp << "\n";

					if ((temp == "true") || (temp == "1"))
					{
						tempTile.blocked = true;
						gotBlocked = true;
					}
					else if ((temp == "false") || (temp == "0"))
					{
						tempTile.blocked = false;
						gotBlocked = true;
					}
					else
					{
						std::cout << "\t\tSyntax error: unknown value of \"Blocked\" in \"Content\"[" << blockCount << "]\n";
						//skip this one
						return false;
					}
				}
				//found keyword
				if (temp.find("Parameter") != std::string::npos)
				{
					//extract value
					temp = temp.substr(temp.find('=') + 1);
					//get content inbetween the delimiter
					strExtractByDelim(temp, false, '"');
					//std::cout << temp << "\n";

					tempTile.parameter = temp;
					gotParameter = true;
				}
			}

			temp.clear();
		}
		while (file.tellg() < blockEnd);

		//if all values have been found, save the tile
		if (gotType && gotTile && gotBlocked && gotParameter)
		{
			this->content.push_back(tempTile);
			//std::cout << "\t\tTile definition loaded\n";
			parseOk = true;
		}
		else
		{
			std::cout << "\t\tMissing variable in \"Content\"[" << blockCount << "]\n";
			std::cout << "\t\t\tMissing elements:";
			if (!gotType)
			{
				std::cout << "  \"Type = <TileType>\"";
			}
			if (!gotTile)
			{
				std::cout << "  \"Tile = <uint>\"";
			}
			if (!gotBlocked)
			{
				std::cout << "  \"Blocked = <bool>\"";
			}
			if (!gotParameter)
			{
				std::cout << "  \"Parameter = <string>\"";
			}
			std::cout << "\n";
			return false;
		}

		blockCount++;
	}
	while (file.tellg() < contentStop);

	return parseOk;
};