#include "Characters.hpp"
#include "Parser.hpp"

//#include "Maps.hpp"

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Sprite::loadSprite(std::string path)
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
};

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














bool Character::loadCharacterFiles(std::string path)	//Nur vorübergehend --> wird ersetzt durch parcing
{
	if (true)			//Laden des Bildes für den Character
	{
		this->texture.loadSprite("TestCharacter.png");
		return true;
	}
	else
	{
		return false;
	}
}


bool Character::move(uint16_t x, uint16_t y)
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
	return true;
}