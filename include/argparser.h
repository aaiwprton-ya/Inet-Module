#ifndef ARGPARSER_H
#define ARGPARSER_H

#include <string>
#include <string_view>
#include <sstream>
#include <map>
#include <list>
#include <algorithm>
#include <stdexcept>
#include <regex>
#include <limits>

enum class ArgType : uint8_t
{
	ARGTYPE_INT,
	ARGTYPE_UINT, 
	ARGTYPE_FLOAT, 
	ARGTYPE_DOUBLE, 
	ARGTYPE_CHAR, 
	ARGTYPE_STRING, 
	ARGTYPE_VECTOR, 
	ARGTYPE_LIST, 
	ARGTYPE_MAP
};

class Argparser
{
	std::string s_data;
	std::map<std::string, std::pair<void*, std::vector<ArgType>>> args;
public:
	Argparser();
	Argparser(std::string_view data);
	Argparser(int argc, char** argv);
	~Argparser();
public:
	void m_init(std::string_view data);
	void m_init(int argc, char** argv);
	bool findArg(
		ArgType type, 
		std::string name, 
		std::string shortName, 
		bool required = false, 
		ArgType subType1 = ArgType::ARGTYPE_STRING, 
		ArgType subType2 = ArgType::ARGTYPE_STRING);
	template <typename T>
	T* getArg(std::string name, T** pp_value)
	{
		*pp_value = (T*)this->args[name].first;
		return *pp_value;
	}
private:
	void m_shielding();
	void* m_constructArg(
		ArgType type, 
		std::string_view arg, 
		ArgType subType1, 
		ArgType subType2);
	void* m_constructVector(ArgType type, std::string_view arg);
	void* m_constructList(ArgType type, std::string_view arg);
	void* m_constructMap(ArgType type1, ArgType type2, std::string_view arg);
	std::vector<std::string> m_parseArrayArgs(std::string_view arg);
	std::vector<std::string> m_split(std::string_view input, std::string_view regex);
private:
	template <typename Type, typename SubType>
	Type* m_constructType(std::string_view arg)
	{
		Type* result = nullptr;
		std::vector<std::string> args = m_parseArrayArgs(arg);
		result = new Type();
		for (std::string& str: args)
		{
			std::stringstream sstream;
			sstream << str;
			SubType value;
			sstream >> value;
			result->push_back(value);
		}
		return result;
	}
	template <typename Type, typename SubType1, typename SubType2>
	Type* m_constructMapType(std::string_view arg)
	{
		Type* result = nullptr;
		std::vector<std::string> args = m_parseArrayArgs(arg);
		result = new Type();
		for (std::string& str: args)
		{
			std::vector<std::string> pair = this->m_split(str, ":");
			std::stringstream sstream;
			sstream << pair[0] << " ";
			sstream << pair[1];
			SubType1 value1;
			sstream >> value1;
			SubType2 value2;
			sstream >> value2;
			(*result)[value1] = value2;
		}
		return result;
	}
};

#endif // ARGPARSER_H
