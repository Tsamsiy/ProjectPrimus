#pragma once

#include <fstream>
#include <string>

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
static char fileGetNoComments(std::ifstream& file)
{
	char in;
	bool repeat = false;

	if (file.eof())
	{
		return NULL;
	}
	in = file.get();
	do
	{
		repeat = false;

		//check for single line comment
		if ((in == '/') && (file.peek() == '/'))
		{
			//file.get();
			while ((file.get() != '\n') && (!file.eof()));
			file.unget();
			in = file.get();
			repeat = true;
		}
		//check for block comment
		else if ((in == '/') && (file.peek() == '*'))
		{
			file.get();
			while (!((file.get() == '*') && (file.peek() == '/')) && (!file.eof()));
			file.get();
			in = file.get();
			repeat = true;
		}
	//repeat is needed in case there is another comment directly after the first one (f.ex. two single line)
	} while (repeat);
	
	return in;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
static bool removeChars(std::string& s, const char x)
{
	bool removed = false;
	if (s.empty())
	{
		return false;
	}

	for (int i = 0; i < s.size(); i++)
	{
		if (s.at(i) == x)
		{
			//s = s.substr(0, i) + s.substr(i + 1);
			s.erase(s.begin() + i);
			removed = true;
		}
	}
	return removed;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
static bool removeFormatting(std::string& s)
{
	bool removed = false;
	if (s.empty())
	{
		return false;
	}

	for (int i = 0; i < s.size(); i++)
	{
		if ((s.at(i) == ' ') || (s.at(i) == '\n') || (s.at(i) == '\t'))
		{
			//s = s.substr(0, i) + s.substr(i + 1);
			s.erase(s.begin() + i);
			removed = true;
		}
	}
	return removed;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//finds the matching closing bracket if streampos points to the start of the block
//tracks intermediate blocks if encountered: {}, [], <>
static bool seekBlockBracket(std::ifstream& file, const char bracket, std::streampos& end)
{
	bool found = false;
	char in = NULL;
	int16_t curl = 0;
	int16_t squ = 0;
	int16_t tri = 0;

	std::streampos here = file.tellg();

	do
	{
		in = fileGetNoComments(file);
		//even brackets before end of file: DONE
		if ((in == bracket) && (curl == 0) && (squ == 0) && (tri == 0))
		{
			end = file.tellg();
			file.seekg(here);
			return true;
		}

		switch (in)
		{
		case '{':
			curl++;
			break;

		case '}':
			curl--;
			if (curl < 0)
			{
				curl = 0;
			}
			break;

		case '[':
			squ++;
			break;

		case ']':
			squ--;
			if (squ < 0)
			{
				squ = 0;
			}
			break;

		case '<':
			tri++;
			break;

		case '>':
			tri--;
			if (tri < 0)
			{
				tri = 0;
			}
			break;

		default:
			//nope
			break;
		}
	} while (!file.eof());
	//no even brackets till end of file
	file.seekg(here);
	return false;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//finds the matching closing bracket if streampos points to the start of the block
//tracks intermediate blocks if encountered: {}, [], <>
//moves streampos to after the block
static bool skipBlockBracket(std::ifstream& file, const char bracket)
{
	bool found = false;
	char in;
	int16_t curl = 0;
	int16_t squ = 0;
	int16_t tri = 0;

	do
	{
		in = fileGetNoComments(file);
		//even brackets before end of file: DONE
		if ((in == bracket) && (curl == 0) && (squ == 0) && (tri == 0))
		{
			return true;
		}

		switch (in)
		{
		case '{':
			curl++;
		break;

		case '}':
			curl--;
			if (curl < 0)
			{
				curl = 0;
			}
		break;

		case '[':
			squ++;
			break;

		case ']':
			squ--;
			if (squ < 0)
			{
				squ = 0;
			}
		break;

		case '<':
			tri++;
			break;

		case '>':
			tri--;
			if (tri < 0)
			{
				tri = 0;
			}
		break;

		default:
			//nope
			break;
		}
	} while (!file.eof());
	//no even brackets till end of file
	return false;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
static bool seekField(std::ifstream& file, const std::string& field, const char delimA, const char delimB, std::streampos& start, std::streampos& stop)
{
	std::string temp;
	bool found = false;

	//parse the file line by line
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
			//the line contains the keyword
			if (temp.find(field) != std::string::npos)
			{
				//mark beginning of data field
				while (fileGetNoComments(file) != delimA)
				{
					if (file.eof())
					{
						return false;
					}
				}
				start = file.tellg();
				//mark end of data field
				if (!skipBlockBracket(file, delimB))
				{
					//no even brackets until end of file
					return false;
				}
				/*while (fileGetNoComments(file) != delimB)
				{
					if (file.eof())
					{
						return false;
					}
				}*/
				file.unget();			//needed to not skip past the field end delimiter
				stop = file.tellg();
				//return to start of data list
				file.seekg(start);

				//std::cout << "Detected field: \"" << field << "\"" << delimA << "..." << delimB << "\n";
				found = true;
			}
		}

		temp.clear();
	} while ((found == false) && (!file.eof()));

	return found;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Extracts a portion of a string marked by the given delimiter
//if invert is true, the portion between the delimiters is deleted
static bool strExtractByDelim(std::string& str, bool invert, const char delim)
{
	//will extract a substring embraced by the specified delimiters
	//only uses the first and very last appearance of the delimiter
	size_t a = str.find(delim);
	size_t b = str.rfind(delim);

	//check for delimiters
	if (a == b)
	{
		//no delimiter found
		return false;
	}
	if ((a == std::string::npos) || (b == std::string::npos))
	{
		//no delimiter found
		return false;
	}

	//extract content between and excluding the delimiters
	if (!invert)
	{
		str = str.substr((a + 1), (b - a - 1));
	}
	//remove content between and including the delimiters
	else
	{
		str = str.substr(0, a) + str.substr(b + 1);
	}
	return true;
}
//Extracts a portion of a string marked by the given delimiter
//if invert is true, the portion between the delimiters is deleted
static bool strExtractByDelim(std::string& str, bool invert, const char delimA, const char delimB)
{
	//will extract a substring embraced by the specified delimiters
	//only uses the first and very last appearance of the delimiter
	size_t a = str.find(delimA);
	size_t b = str.rfind(delimB);

	//check for delimiters
	if (a == b)
	{
		//only one delimiter found
		return false;
	}
	if ((a == std::string::npos) || (b == std::string::npos))
	{
		//no delimiter found
		return false;
	}

	//extract content between and excluding the delimiters
	if (!invert)
	{
		str = str.substr((a + 1), (b - a - 1));
	}
	//remove content between and including the delimiters
	else
	{
		str = str.substr(0, a) + str.substr(b + 1);
	}
	return true;
}

