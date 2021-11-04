#include "Characters.hpp"
#include "Parser.hpp"



//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
FACING incFacing(FACING lhs)
{
	switch (lhs)
	{
	case FACING::DOWN:
		return FACING::LEFT;
		break;

	case FACING::LEFT:
		return FACING::UP;
		break;

	case FACING::UP:
		return FACING::RIGHT;
		break;

	case FACING::RIGHT:
		return FACING::DOWN;
		break;
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
FACING decFacing(FACING lhs)
{
	switch (lhs)
	{
	case FACING::DOWN:
		return FACING::RIGHT;
		break;

	case FACING::LEFT:
		return FACING::DOWN;
		break;

	case FACING::UP:
		return FACING::LEFT;
		break;

	case FACING::RIGHT:
		return FACING::UP;
		break;
	}
}

//#######################################################################################################################################################################

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Sprite::loadAnimation(std::string path, std::ifstream& file, Animation& anim, std::streampos start, std::streampos stop)
{
	bool parseOk = false;
	bool gotInterval = false;
	std::string temp;
	std::streampos contentStart;
	std::streampos contentStop;

	//construct new local path: remove file part of last path
	path = path.substr(0, path.rfind('/'));

	//parse field line by line
	//first try to find the solo interval variable in the Animation field
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
			//std::cout << temp << "\n";

			//found keyword
			if (temp.find("Interval") != std::string::npos)
			{
				//extract value
				temp = temp.substr(temp.find('=') + 1);
				//std::cout << temp << "\n";

				if (!strParseInt(temp, anim.invervall))
				{
					std::cout << "\t\tSyntax error: unknown value of \"Interval\" in animation filed\n";
					//skip this one
					return false;
				}
				else
				{
					gotInterval = true;
				}
			}
		}

		temp.clear();
	}
	while ((file.tellg() < stop) && !gotInterval);
	//already at the end of the Animation field and one of those is missing
	if (!gotInterval)
	{
		std::cout << "\t\tMissing variable in animation field\n";
		std::cout << "\t\t\tMissing elements:";
		if (!gotInterval)
		{
			std::cout << "  \"Interval = <int>\"";
		}
		std::cout << "\n";
		return false;
	}

	//move on to parsing the frames
	//first try to find the AnimationFiles list
	if (!seekField(file, "AnimationFiles", '[', ']', contentStart, contentStop))
	{
		//main definitions are missing in the document, so skip it
		std::cout << "\t\tMissing data list \"AnimationFiles\"[...]\n";
		return false;
	}
	temp.clear();
	unsigned count = 0;

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
		} while ((in != '\n') && (file.tellg() < contentStop));

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
				anim.frames.push_back(textureFile);

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

	} while (file.tellg() < contentStop);

	if (parseOk)
	{
		std::cout << "\t\tParsed " << count << " Textures.\n";
	}

	return parseOk;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Sprite::loadSprite(std::string path, std::ifstream& file, std::streampos start, std::streampos stop)
{
	std::string temp;
	std::streampos blockStart;
	std::streampos blockStop;

	//try to find the Animation field.
	if (!seekField(file, "IdleAnimation", '{', '}', blockStart, blockStop))
	{
		//main definitions are missing in the document, so skip it
		std::cout << "\t\tMissing data field \"IdleAnimation\"{...} in \"Sprite\"{...}\n";
		return false;
	}
	//try to load that animation
	if (!this->loadAnimation(path, file, this->idleAnim, blockStart, blockStop))
	{
		//error while loading animation
		std::cout << "\t\tInvalid or missing Animation definitions in \"Sprite/IdleAnimation\"{...}\n";
		return false;
	}
	//return to start of Sprite field
	file.seekg(start);

	//try to find the Animation field.
	if (!seekField(file, "WalkAnimation", '{', '}', blockStart, blockStop))
	{
		//main definitions are missing in the document, so skip it
		std::cout << "\t\tMissing data field \"WalkAnimation\"{...} in \"Sprite\"{...}\n";
		return false;
	}
	//try to load that animation
	if (!this->loadAnimation(path, file, this->walkAnim, blockStart, blockStop))
	{
		//error while loading animation
		std::cout << "\t\tInvalid or missing Animation definitions in \"Sprite/WalkAnimation\"{...}\n";
		return false;
	}
	//return to start of Sprite field
	file.seekg(start);

	//try to find the Animation field.
	if (!seekField(file, "AttackAnimation", '{', '}', blockStart, blockStop))
	{
		//main definitions are missing in the document, so skip it
		std::cout << "\t\tMissing data field \"AttackAnimation\"{...} in \"Sprite\"{...}\n";
		return false;
	}
	//try to load that animation
	if (!this->loadAnimation(path, file, this->attackAnim, blockStart, blockStop))
	{
		//error while loading animation
		std::cout << "\t\tInvalid or missing Animation definitions in \"Sprite/AttackAnimation\"{...}\n";
		return false;
	}
	//return to start of Sprite field
	file.seekg(start);

	//try to find the Animation field.
	if (!seekField(file, "HitAnimation", '{', '}', blockStart, blockStop))
	{
		//main definitions are missing in the document, so skip it
		std::cout << "\t\tMissing data field \"HitAnimation\"{...} in \"Sprite\"{...}\n";
		return false;
	}
	//try to load that animation
	if (!this->loadAnimation(path, file, this->hitAnim, blockStart, blockStop))
	{
		//error while loading animation
		std::cout << "\t\tInvalid or missing Animation definitions in \"Sprite/HitAnimation\"{...}\n";
		return false;
	}
	
	//skip to end of Sprite field
	file.seekg(stop);

	//all animation fields need at least one frame
	return true;
};
//temporary version saves only to idleAnim
/*bool Sprite::loadSprite(std::string path)
{
	std::ifstream tempFile(path);
	//try to open the file
	if (tempFile)
	{
		tempFile.close();

		Gosu::Image textureFile(path);
		this->idleAnim.frames.push_back(textureFile);

		//std::cout << "\t\tTexture loaded: " << path << "\n";
		return true;
	}
	else
	{
		std::cout << "\t\tUnable to load texture: " << path << "\n";
	}
	return false;
};*/

void Sprite::draw(double xPos, double yPos, Gosu::ZPos z, AnimState state, double angle, double scale)
{
	//a new sequence is requested
	if (state != this->animState)
	{
		//idle and walk may be interrupted
		if ((this->animState == AnimState::idle) || (this->animState == AnimState::walk))
		{
			//set to start of new sequence
			this->animState = state;
			animPointer = 0;
		}
	}

	//draw the frame of the current sequence
	switch (this->animState)
	{
		case AnimState::idle:
			//no interval means only one frame
			if (this->idleAnim.invervall < 0)
			{
				//draw the first and only frame of the list
				animPointer = 0;
			}

			//draw the frame at the given coordinates, with reference point also at the middle of the lower edge.
			idleAnim.frames.at(animPointer).draw_rot(xPos, yPos, z, angle, 0.5, 0.5, scale, scale);

			//don't draw a new frame jet if Timer hasn't run out
			if (this->animTimer == 0)
			{
				//move pointer to next frame
				animPointer++;
				//reset if at end of sequence
				if (animPointer >= idleAnim.frames.size())
				{
					animPointer = 0;
				}
			}
			
			//reset timer if it has run out
			if (this->animTimer <= 0)
			{
				//draw is called up to 60 times a second:
				//a timer of 60 would equal 1 second
				this->animTimer = (int)(this->idleAnim.invervall * 0.06);
			}
		break;

		case AnimState::walk:
			//no interval means only one frame
			if (this->walkAnim.invervall < 0)
			{
				//draw the first and only frame of the list
				animPointer = 0;
			}

			//draw the frame at the given coordinates, with reference point also at the middle of the lower edge.
			walkAnim.frames.at(animPointer).draw_rot(xPos, yPos, z, angle, 0.5, 0.5, scale, scale);

			//don't draw a new frame jet if Timer hasn't run out
			if (this->animTimer == 0)
			{
				//move pointer to next frame
				animPointer++;
				//reset if at end of sequence
				if (animPointer >= walkAnim.frames.size())
				{
					animPointer = 0;
				}
			}

			//reset timer if it has run out
			if (this->animTimer <= 0)
			{
				//draw is called up to 60 times a second:
				//a timer of 60 would equal 1 second
				this->animTimer = (int)(this->walkAnim.invervall * 0.06);
			}
		break;

		case AnimState::attack:
			//this sequence may not be interrupted

			//no interval means only one frame
			if (this->attackAnim.invervall < 0)
			{
				//draw the first and only frame of the list
				animPointer = 0;
			}
			
			//draw the frame at the given coordinates, with reference point also at the middle of the lower edge.
			attackAnim.frames.at(animPointer).draw_rot(xPos, yPos, z, angle, 0.5, 0.5, scale, scale);

			//don't draw a new frame jet if Timer hasn't run out
			if (this->animTimer == 0)
			{
				//move pointer to next frame
				animPointer++;
				//reset if at end of sequence
				if (animPointer >= attackAnim.frames.size())
				{
					animPointer = 0;
				}
			}

			//reset timer if it has run out
			if (this->animTimer <= 0)
			{
				//draw is called up to 60 times a second:
				//a timer of 60 would equal 1 second
				this->animTimer = (int)(this->attackAnim.invervall * 0.06);
			}
		break;

		case AnimState::hit:
			//this sequence may not be interrupted

			//no interval means only one frame
			if (this->hitAnim.invervall < 0)
			{
				//draw the first and only frame of the list
				animPointer = 0;
			}

			//draw the frame at the given coordinates, with reference point also at the middle of the lower edge.
			hitAnim.frames.at(animPointer).draw_rot(xPos, yPos, z, angle, 0.5, 0.5, scale, scale);

			//don't draw a new frame jet if Timer hasn't run out
			if (this->animTimer == 0)
			{
				//move pointer to next frame
				animPointer++;
				//reset if at end of sequence
				if (animPointer >= hitAnim.frames.size())
				{
					animPointer = 0;
				}
			}

			//reset timer if it has run out
			if (this->animTimer <= 0)
			{
				//draw is called up to 60 times a second:
				//a timer of 60 would equal 1 second
				this->animTimer = (int)(this->hitAnim.invervall * 0.06);
			}
		break;

		default:
			//nope
		break;
	}

	//decrement timer
	this->animTimer = this->animTimer - 1;
}

//#######################################################################################################################################################################

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
FACING Character::getFacing()
{
	return this->facing;
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t Character::getXTile()
{
	return this->xTile;
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t Character::getYTile()
{
	return this->yTile;
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Character::draw(const Map& map, double xPos, double yPos, double scale, AnimState state)
{
	double x = 0.0;
	double y = 0.0;

	//get the coordinates of the upper left corner of the tile the character is standing on
	map.getTileCoords(this->xTile, this->yTile, x, y, scale);

	//calculate midpoint of the tile
	x += (map.getTileWidth() / 2 * scale);
	y += (map.getTileHeight() / 2 * scale);

	this->texture.draw((x + xPos), (y + yPos), 1, state, (double)this->facing, scale);
};

bool Character::drawRotate(const Map& map, double xPos, double yPos, double& angle, unsigned interval, double scale, FACING dir)
{
	//already there
	if (dir == this->facing)
	{
		return true;
	}

	double x = 0.0;
	double y = 0.0;
	bool rotDir = true;	//true is +, rotRight

	//get the coordinates of the upper left corner of the tile the character is standing on
	map.getTileCoords(this->xTile, this->yTile, x, y, scale);

	//calculate midpoint of the tile
	x += (map.getTileWidth() / 2 * scale);
	y += (map.getTileHeight() / 2 * scale);

	//find which way is fastest
	if (this->facing == decFacing(dir))
	{
		rotDir = true;
	}
	if (this->facing == incFacing(dir))
	{
		rotDir = false;
	}

	//std::cout << angle << "\n";
	this->texture.draw((x + xPos), (y + yPos), 1, AnimState::walk, angle, scale);
	if (rotDir)
	{
		angle = angle + interval;
	}
	else
	{
		angle = angle - interval;
	}

	if (angle >= 360)
	{
		angle = 0;
	}
	if (angle < 0)
	{
		angle = 360;
	}

	//reached destination
	//need this tollerance buffer instaed of angle == (double)dir for intervals that are not multiples of the full angles
	if ((angle <= (double)dir + interval) && (angle >= (double)dir - interval))
	{
		this->facing = dir;
		return true;
	}
	return false;
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Character::drawMove(const Map& map, double xPos, double yPos, double& pos, unsigned interval, double scale, FACING dir)
{
	double x = 0.0;
	double y = 0.0;

	//get the coordinates of the upper left corner of the tile the character is standing on
	map.getTileCoords(this->xTile, this->yTile, x, y, scale);

	//calculate midpoint of the tile
	x += (map.getTileWidth() / 2 * scale);
	y += (map.getTileHeight() / 2 * scale);

	//std::cout << pos << "\n";
	switch (dir)
	{
		case FACING::DOWN:
			this->texture.draw((x + xPos), (y + yPos + pos), 1, AnimState::walk, (double)dir, scale);

			//reached destination
			//need this tollerance buffer instaed of angle == (double)dir for intervals that are not multiples of the full angles
			if (pos >= map.getTileHeight() * scale)
			{
				this->yTile++;
				pos = 0;
				return true;
			}
		break;

		case FACING::LEFT:
			this->texture.draw((x + xPos - pos), (y + yPos), 1, AnimState::walk, (double)dir, scale);

			//reached destination
			//need this tollerance buffer instaed of angle == (double)dir for intervals that are not multiples of the full angles
			if (pos >= map.getTileWidth() * scale)
			{
				this->xTile--;
				pos = 0;
				return true;
			}
		break;

		case FACING::UP:
			this->texture.draw((x + xPos), (y + yPos - pos), 1, AnimState::walk, (double)dir, scale);

			//reached destination
			//need this tollerance buffer instaed of angle == (double)dir for intervals that are not multiples of the full angles
			if (pos >= map.getTileHeight() * scale)
			{
				this->yTile--;
				pos = 0;
				return true;
			}
		break;

		case FACING::RIGHT:
			this->texture.draw((x + xPos + pos), (y + yPos), 1, AnimState::walk, (double)dir, scale);

			//reached destination
			if (pos >= map.getTileWidth() * scale)
			{
				this->xTile++;
				pos = 0;
				return true;
			}
		break;
	}
	pos++;

	return false;
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Character::loadCharacterSheet(std::string path)
{
	bool parseOk = false;
	std::string temp;
	std::streampos start;
	std::streampos stop;
	bool gotSprite = false;

	std::cout << "Reading character sheet: \"" << path << "\n";

	//open file
	std::ifstream file(path);
	if (!file)
	{
		std::cout << "\tFile not found: \"" << path << "\n";
		return false;
	}

	//!!! Parse other stuff like class and race here first !!!
	parseOk = true;

	//try to find the Sprite field. it contains the animation fields
	if (!seekField(file, "Sprite", '{', '}', start, stop))
	{
		//main definitions are missing in the document, so skip it
		std::cout << "\t\tMissing data field \"Sprite\"{...}\n";
		return false;
	}
	//try to load the sprite
	if (this->texture.loadSprite(path, file, start, stop))
	{
		gotSprite = true;
		std::cout << "\t\tSprite loaded\n";
	}
	else
	{
		std::cout << "\t\tUnable to parse Sprite in character sheet: \"" << path << "\n";
		return false;
	}
	

	file.close();
	return parseOk;
};














bool Character::loadCharacterFiles(std::string path)
{
	return this->loadCharacterSheet(path);
}


void Character::update()
{
	uint16_t moveactions = this->movementSpeed;		//Aktuelle Bewegungsrate

	while (moveactions != 0)
	{
		Gosu::Button lastButton = Gosu::KB_D;
		if (Gosu::KB_LEFT/*Keyboard Links*/)	//Alternativ mit switch case?
		{
			//xpos = xpos - width
			if ((lastButton != Gosu::KB_UP) && (lastButton != Gosu::KB_DOWN))	//Für den Fall, dass man in die Schräge geht. Kein Abzug der Bewegungsrate, obwohl eigentlich zwei Bewegungen gemacht werden
			{
				moveactions = moveactions - 1;		//Verringert die Bewegungsrate nach diesem Zug
			}

			lastButton = Gosu::KB_LEFT;
		}
		else
		{
			if (Gosu::KB_RIGHT/*Keyboard Rechts*/)
			{
				//xpos = xpos + width
				if ((lastButton != Gosu::KB_UP) && (lastButton != Gosu::KB_DOWN))	//Für den Fall, dass man in die Schräge geht. Kein Abzug der Bewegungsrate, obwohl eigentlich zwei Bewegungen gemacht werden
				{
					moveactions = moveactions - 1;		//Verringert die Bewegungsrate nach diesem Zug
				}

				lastButton = Gosu::KB_RIGHT;
			}
			else
			{
				if (Gosu::KB_DOWN/*Keyboard Unten*/)
				{
					//ypos = ypos - length
					if ((lastButton != Gosu::KB_LEFT) && (lastButton != Gosu::KB_RIGHT))	//Für den Fall, dass man in die Schräge geht. Kein Abzug der Bewegungsrate, obwohl eigentlich zwei Bewegungen gemacht werden
					{
						moveactions = moveactions - 1;		//Verringert die Bewegungsrate nach diesem Zug
					}

					lastButton = Gosu::KB_DOWN;
				}
				else
				{
					if (Gosu::KB_UP/*Keyboard Oben*/)
					{
						//ypos = ypos + length
						if ((lastButton != Gosu::KB_LEFT) && (lastButton != Gosu::KB_RIGHT))	//Für den Fall, dass man in die Schräge geht. Kein Abzug der Bewegungsrate, obwohl eigentlich zwei Bewegungen gemacht werden
						{
							moveactions = moveactions - 1;		//Verringert die Bewegungsrate nach diesem Zug
						}

						lastButton = Gosu::KB_UP;
					}
				}
			}
		}
	}
}