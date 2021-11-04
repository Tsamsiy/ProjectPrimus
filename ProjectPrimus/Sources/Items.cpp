#include "Items.hpp"
#include "Parser.hpp"

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Item::loadItems(std::string stdPath, std::string defPath, std::string armPath)
{
	bool parseOk = false;

	std::cout << "Parsing Item definitions\n";

	//try to load the standard items
	if (this->loadSTDItemFile(stdPath))
	{
		parseOk = true;
	}
	else
	{
		std::cout << "\t\tNo Valid Item definitions found in \"" << stdPath << "\"\n";
	}

	//try to load the defensive items
	if (this->loadDEFItemFile(defPath))
	{
		parseOk = true;
	}
	else
	{
		std::cout << "\t\tNo Valid Item definitions found in \"" << defPath << "\"\n";
	}
	
	//try to load the offensive items
	if (this->loadARMItemFile(armPath))
	{
		parseOk = true;
	}
	else
	{
		std::cout << "\t\tNo Valid Item definitions found in \"" << armPath << "\"\n";
	}

	return parseOk;
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Item::loadSTDItemFile(std::string path)
{
	bool parseOk = false;
	std::string temp;
	std::streampos start;
	std::streampos stop;

	std::cout << "\tReading Item definition file \"" << path << "\"\n";

	//open file
	std::ifstream file(path);
	if (!file)
	{
		std::cout << "\t\tFile not found: \"" << path << "\"\n";
		return false;
	}

	if (!seekField(file, "Items", '[', ']', start, stop))
	{
		//main definitions are missing in the document, so skip it
		std::cout << "\t\tMissing data list \"Items\"[...]\n";
		file.close();
		return false;
	}

	uint32_t blockCount = 0;
	//empty temporary Item where info is gathered
	//is blank by default
	STDItem tempItem;

	do
	{
		bool gotName = false;
		bool gotDesc = false;
		bool gotConsum = false;
		bool gotUses = false;
		std::streampos blockEnd;

		//find start of next block in the list 
		while (fileGetNoComments(file) != '{')
		{
			//no block start found at first pass through Content field: no blocks in the field
			if (!(file.tellg() < stop) && !parseOk)
			{
				std::cout << "\t\tMissing data field in \"Items\"[...]\n";
				std::cout << "\t\t\tNeeds to contain at least one item definition\n";
				return false;
			}
			//end of Items list
			if (!(file.tellg() < stop))
			{
				if (parseOk)
				{
					std::cout << "\t\tParsed " << blockCount << " Items.\n";
				}
				return parseOk;
			}
		}
		if (!seekBlockBracket(file, '}', blockEnd))
		{
			//syntax error if it doesn't find the closing bracket
			std::cout << "\t\tSyntax error: missing bracket in \"Items\"[...]\n";
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
			} while ((in != '\n') && (file.tellg() < blockEnd));

			//remove unwanted chars
			removeFormatting(temp);

			//if string is not empty
			if (!temp.empty())
			{
				//found keyword
				if (temp.find("Name") != std::string::npos)
				{
					//extract value
					temp = temp.substr(temp.find('=') + 1);
					//get content inbetween the delimiter
					strExtractByDelim(temp, false, '"');
					//std::cout << temp << "\n";

					tempItem.name = temp;
					gotName = true;
				}
				//found keyword
				if (temp.find("Description") != std::string::npos)
				{
					//extract value
					temp = temp.substr(temp.find('=') + 1);
					//get content inbetween the delimiter
					strExtractByDelim(temp, false, '"');
					//std::cout << temp << "\n";

					tempItem.description= temp;
					gotDesc = true;
				}
				//found keyword,  may be preset by Type = blnk
				if (temp.find("Consumable") != std::string::npos)
				{
					//extract value
					temp = temp.substr(temp.find('=') + 1);
					//std::cout << temp << "\n";

					if ((temp == "true") || (temp == "1"))
					{
						tempItem.consumable = true;
						gotConsum = true;
					}
					else if ((temp == "false") || (temp == "0"))
					{
						tempItem.consumable = false;
						gotConsum = true;
					}
					else
					{
						std::cout << "\t\tSyntax error: unknown value of \"Consumable\" in \"Items\"[" << blockCount << "]\n";
						//skip this one
						return false;
					}
				}
				//found keyword
				if (temp.find("Uses") != std::string::npos)
				{
					//extract value
					temp = temp.substr(temp.find('=') + 1);
					//std::cout << temp << "\n";

					if (!strParseUnsigned(temp, tempItem.uses))
					{
						std::cout << "\t\tSyntax error: unknown value of \"AC\" in \"Armors\"[" << blockCount << "]\n";
						//skip this one
						return false;
					}
					else
					{
						gotUses = true;
					}
				}
				
			}

			temp.clear();
		}
		while (file.tellg() < blockEnd);

		//if all values have been found, save the tile
		if (gotName && gotDesc && gotConsum && gotUses)
		{
			this->items.push_back(tempItem);
			//std::cout << "\t\tItem definition loaded\n";
			parseOk = true;
		}
		else
		{
			std::cout << "\t\tMissing variable in \"Items\"[" << blockCount << "]\n";
			std::cout << "\t\t\tMissing elements:";
			if (!gotName)
			{
				std::cout << "  \"Name = <string>\"";
			}
			if (!gotDesc)
			{
				std::cout << "  \"Description = <string>\"";
			}
			if (!gotConsum)
			{
				std::cout << "  \"Consumable = <bool>\"";
			}
			if (!gotUses)
			{
				std::cout << "  \"Uses = <unsigned>\"";
			}
			std::cout << "\n";
			return false;
		}

		blockCount++;
	}
	while (file.tellg() < stop);

	file.close();
	return false;
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Item::loadDEFItemFile(std::string path)
{
	bool parseOk = false;
	std::string temp;
	std::streampos start;
	std::streampos stop;

	std::cout << "\tReading Item definition file \"" << path << "\"\n";

	//open file
	std::ifstream file(path);
	if (!file)
	{
		std::cout << "\t\tFile not found: \"" << path << "\"\n";
		return false;
	}

	if (!seekField(file, "Armors", '[', ']', start, stop))
	{
		//main definitions are missing in the document, so skip it
		std::cout << "\t\tMissing data list \"Armors\"[...]\n";
		file.close();
		return false;
	}

	uint32_t blockCount = 0;
	//empty temporary Item where info is gathered
	//is blank by default
	DEFItem tempItem;

	do
	{
		bool gotName = false;
		bool gotDesc = false;
		bool gotAC = false;
		bool gotAV = false;
		std::streampos blockEnd;

		//find start of next block in the list 
		while (fileGetNoComments(file) != '{')
		{
			//no block start found at first pass through Content field: no blocks in the field
			if (!(file.tellg() < stop) && !parseOk)
			{
				std::cout << "\t\tMissing data field in \"Armors\"[...]\n";
				std::cout << "\t\t\tNeeds to contain at least one item definition\n";
				return false;
			}
			//end of Items list
			if (!(file.tellg() < stop))
			{
				if (parseOk)
				{
					std::cout << "\t\tParsed " << blockCount << " Items.\n";
				}
				return parseOk;
			}
		}
		if (!seekBlockBracket(file, '}', blockEnd))
		{
			//syntax error if it doesn't find the closing bracket
			std::cout << "\t\tSyntax error: missing bracket in \"Armors\"[...]\n";
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
			} while ((in != '\n') && (file.tellg() < blockEnd));

			//remove unwanted chars
			removeFormatting(temp);

			//if string is not empty
			if (!temp.empty())
			{
				//found keyword
				if (temp.find("Name") != std::string::npos)
				{
					//extract value
					temp = temp.substr(temp.find('=') + 1);
					//get content inbetween the delimiter
					strExtractByDelim(temp, false, '"');
					//std::cout << temp << "\n";

					tempItem.name = temp;
					gotName = true;
				}
				//found keyword
				if (temp.find("Description") != std::string::npos)
				{
					//extract value
					temp = temp.substr(temp.find('=') + 1);
					//get content inbetween the delimiter
					strExtractByDelim(temp, false, '"');
					//std::cout << temp << "\n";

					tempItem.description = temp;
					gotDesc = true;
				}
				//found keyword
				if (temp.find("AC") != std::string::npos)
				{
					//extract value
					temp = temp.substr(temp.find('=') + 1);
					//std::cout << temp << "\n";

					int tempNum = 0;
					if (!strParseInt(temp, tempNum))
					{
						std::cout << "\t\tSyntax error: unknown value of \"AC\" in \"Armors\"[" << blockCount << "]\n";
						//skip this one
						return false;
					}
					else
					{
						tempItem.AC = (int8_t)tempNum;
						gotAC = true;
					}
				}
				//found keyword
				if (temp.find("AV") != std::string::npos)
				{
					//extract value
					temp = temp.substr(temp.find('=') + 1);
					//get content inbetween the delimiter
					strExtractByDelim(temp, false, '[', ']');
					//std::cout << temp << "\n";
					
					for (int i = 0; i < 2; i++)
					{
						//get the wanted element
						std::string elem = strAccessCSV(temp, i);

						unsigned tempNum = 0;
						if (!strParseUnsigned(elem, tempNum))
						{
							std::cout << "\t\tSyntax error: unknown value of \"AV\"[" << i << "] in \"Armors\"[" << blockCount << "]\n";
							//skip this one
							return false;
						}
						else
						{
							tempItem.AV[i] = (uint8_t)tempNum;
						}
					}
					gotAV = true;
				}
			}

			temp.clear();
		}
		while (file.tellg() < blockEnd);
		
		//if all values have been found, save the tile
		if (gotName && gotDesc && gotAC && gotAV)
		{
			this->armors.push_back(tempItem);
			//std::cout << "\t\tItem definition loaded\n";
			parseOk = true;
		}
		else
		{
			std::cout << "\t\tMissing variable in \"Armors\"[" << blockCount << "]\n";
			std::cout << "\t\t\tMissing elements:";
			if (!gotName)
			{
				std::cout << "  \"Name = <string>\"";
			}
			if (!gotDesc)
			{
				std::cout << "  \"Description = <string>\"";
			}
			if (!gotAC)
			{
				std::cout << "  \"AC = <int8_t>\"";
			}
			if (!gotAV)
			{
				std::cout << "  \"AV = [<uint8_t>, <uint8_t>]\"";
			}
			std::cout << "\n";
			return false;
		}

		blockCount++;
	}
	while (file.tellg() < stop);

	file.close();
	return false;
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Item::loadARMItemFile(std::string path)
{
	bool parseOk = false;
	std::string temp;
	std::streampos start;
	std::streampos stop;

	std::cout << "\tReading Item definition file \"" << path << "\"\n";

	//open file
	std::ifstream file(path);
	if (!file)
	{
		std::cout << "\t\tFile not found: \"" << path << "\"\n";
		return false;
	}

	if (!seekField(file, "Weapons", '[', ']', start, stop))
	{
		//main definitions are missing in the document, so skip it
		std::cout << "\t\tMissing data list \"Weapons\"[...]\n";
		file.close();
		return false;
	}

	uint32_t blockCount = 0;
	//empty temporary Item where info is gathered
	//is blank by default
	ARMItem tempItem;

	do
	{
		bool gotName = false;
		bool gotDesc = false;
		bool gotBseDmg = false;
		bool gotAcuMod = false;
		bool gotDmgMod = false;
		bool gotSlvMod = false;
		std::streampos blockEnd;

		//find start of next block in the list 
		while (fileGetNoComments(file) != '{')
		{
			//no block start found at first pass through Content field: no blocks in the field
			if (!(file.tellg() < stop) && !parseOk)
			{
				std::cout << "\t\tMissing data field in \"Weapons\"[...]\n";
				std::cout << "\t\t\tNeeds to contain at least one item definition\n";
				return false;
			}
			//end of Items list
			if (!(file.tellg() < stop))
			{
				if (parseOk)
				{
					std::cout << "\t\tParsed " << blockCount << " Items.\n";
				}
				return parseOk;
			}
		}
		if (!seekBlockBracket(file, '}', blockEnd))
		{
			//syntax error if it doesn't find the closing bracket
			std::cout << "\t\tSyntax error: missing bracket in \"Weapons\"[...]\n";
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
			} while ((in != '\n') && (file.tellg() < blockEnd));

			//remove unwanted chars
			removeFormatting(temp);

			//if string is not empty
			if (!temp.empty())
			{
				//found keyword
				if (temp.find("Name") != std::string::npos)
				{
					//extract value
					temp = temp.substr(temp.find('=') + 1);
					//get content inbetween the delimiter
					strExtractByDelim(temp, false, '"');
					//std::cout << temp << "\n";

					tempItem.name = temp;
					gotName = true;
				}
				//found keyword
				if (temp.find("Description") != std::string::npos)
				{
					//extract value
					temp = temp.substr(temp.find('=') + 1);
					//get content inbetween the delimiter
					strExtractByDelim(temp, false, '"');
					//std::cout << temp << "\n";

					tempItem.description = temp;
					gotDesc = true;
				}
				//found keyword
				if (temp.find("BaseDamage") != std::string::npos)
				{
					//extract value
					temp = temp.substr(temp.find('=') + 1);
					//std::cout << temp << "\n";
					
					if (!strParseDouble(temp, tempItem.baseDamage))
					{
						std::cout << "\t\tSyntax error: unknown value of \"BaseDamage\" in \"Weapons\"[" << blockCount << "]\n";
						//skip this one
						return false;
					}
					else
					{
						gotBseDmg = true;
					}
				}
				//found keyword
				if (temp.find("AcuracyModifier") != std::string::npos)
				{
					//extract value
					temp = temp.substr(temp.find('=') + 1);
					//std::cout << temp << "\n";
					
					if (!strParseDouble(temp, tempItem.acuracyMod))
					{
						std::cout << "\t\tSyntax error: unknown value of \"AcuracyModifier\" in \"Weapons\"[" << blockCount << "]\n";
						//skip this one
						return false;
					}
					else
					{
						gotAcuMod = true;
					}
				}
				//found keyword
				if (temp.find("DamageModifier") != std::string::npos)
				{
					//extract value
					temp = temp.substr(temp.find('=') + 1);
					//std::cout << temp << "\n";
					
					if (!strParseDouble(temp, tempItem.damageMod))
					{
						std::cout << "\t\tSyntax error: unknown value of \"DamageModifier\" in \"Weapons\"[" << blockCount << "]\n";
						//skip this one
						return false;
					}
					else
					{
						gotDmgMod = true;
					}
				}
				//found keyword
				if (temp.find("SalvoModifier") != std::string::npos)
				{
					//extract value
					temp = temp.substr(temp.find('=') + 1);
					//std::cout << temp << "\n";
					
					if (!strParseDouble(temp, tempItem.salvoMod))
					{
						std::cout << "\t\tSyntax error: unknown value of \"SalvoModifier\" in \"Weapons\"[" << blockCount << "]\n";
						//skip this one
						return false;
					}
					else
					{
						gotSlvMod = true;
					}
				}
			}

			temp.clear();
		}
		while (file.tellg() < blockEnd);

		//if all values have been found, save the tile
		if (gotName && gotDesc && gotBseDmg && gotAcuMod && gotDmgMod && gotSlvMod)
		{
			this->weapons.push_back(tempItem);
			//std::cout << "\t\tItem definition loaded\n";
			parseOk = true;
		}
		else
		{
			std::cout << "\t\tMissing variable in \"Weapons\"[" << blockCount << "]\n";
			std::cout << "\t\t\tMissing elements:";
			if (!gotName)
			{
				std::cout << "  \"Name = <string>\"";
			}
			if (!gotDesc)
			{
				std::cout << "  \"Description = <string>\"";
			}
			if (!gotBseDmg)
			{
				std::cout << "  \"BaseDamage = <double>\"";
			}
			if (!gotAcuMod)
			{
				std::cout << "  \"AcuracyModifier = <double>\"";
			}
			if (!gotDmgMod)
			{
				std::cout << "  \"DamageModifier = <double>\"";
			}
			if (!gotSlvMod)
			{
				std::cout << "  \"SalvoModifier = <double>\"";
			}
			std::cout << "\n";
			return false;
		}

		blockCount++;
	}
	while (file.tellg() < stop);

	file.close();
	return false;
};