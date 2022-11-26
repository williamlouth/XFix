//
// Created by will on 26/11/22.
//

#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include "Application.h"
#include "pugixml.hpp"

namespace xfix
{

Application::Application(const std::string& xmlFile, const std::string& generatedFolder)
	: generationFolder_{generatedFolder}
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(xmlFile.c_str());
	if (!result)
	{ throw std::runtime_error("xml parsing error"); }

	pugi::xml_node fixNode = doc.child("fix");

	//int fixMajor = std::stoi(fixNode.attribute("major").value());
	//int fixMinor = std::stoi(fixNode.attribute("minor").value());

	pugi::xml_node header = fixNode.child("header");
	for (pugi::xml_node field : header)
	{
		std::cout << field.attribute("name").value() << std::endl;
	}

	createMessages(fixNode);


	pugi::xml_node trailer = fixNode.child("trailer");
	for (pugi::xml_node field : trailer)
	{
		std::cout << field.attribute("name").value() << std::endl;
	}


	createFields(fixNode);

	/*
	for(pugi::xml_node node: root)
	{
		std::cout << node.text() << std::endl;
	}
	 */

}

void Application::createFields(const pugi::xml_node& fixNode)
{
	auto filesDir = generationFolder_ + "fields";
	if(!std::filesystem::is_directory(filesDir) || !std::filesystem::exists(filesDir))
	{
		std::filesystem::create_directory(filesDir);
	}

	pugi::xml_node fields = fixNode.child("fields");
	for (pugi::xml_node field : fields)
	{
		std::cout << field.attribute("name").value() << std::endl;

		if (std::string(field.attribute("type").value()) == "CHAR" ||
			std::string(field.attribute("type").value()) == "MULTIPLEVALUESTRING")
		{
			for (pugi::xml_node value : field)
			{
				std::cout << value.attribute("enum").value() << std::endl;
			}
		}
		else
		{
			std::string name = field.attribute("name").value();
			int number = std::stoi(field.attribute("number").value());
			std::string typeString = field.attribute("type").value();
			auto itr = std::find_if(stringToFieldType.begin(), stringToFieldType.end(), [&typeString](auto p)
			{ return p.first == typeString; });
			if (itr == stringToFieldType.end())
			{
				throw std::runtime_error("Missing type:" + typeString);
			}

			createField(name, number, itr->second);
		}
	}
}

void Application::createField(const std::string& name, int number, Application::FieldType type)
{
	std::string fileName = generationFolder_ + "fields/" + name + "_g.h";
	std::ofstream fH;
	fH.open(fileName);

	fH << "#pragma once\n";
	fH << "namespace xfix\n{\n\n";

	fH << "#include <string>\n";

	fH << "\nclass " + name + "\n{\n";

	fH << "public:\n";

	fH << name + "(const std::string& value)\n";
	fH << ":value_{value}\n";
	fH << "{}\n";

	fH << "static const int number = " + std::to_string(number) + ";\n" ;

	fH << "std::string value_;\n";

	fH << "};\n";

	fH << "\n} //xfix\n";

	fH.close();
	generatedFiles_.push_back(fileName);
}
void Application::createMessages(const pugi::xml_node& fixNode)
{
	auto filesDir = generationFolder_ + "messages";
	if(!std::filesystem::is_directory(filesDir) || !std::filesystem::exists(filesDir))
	{
		std::filesystem::create_directory(filesDir);
	}

	pugi::xml_node messages = fixNode.child("messages");
	for (pugi::xml_node message : messages)
	{
		std::string name = message.attribute("name").value();
		std::string msgType = message.attribute("msgtype").value();

		std::vector<std::pair<std::string,bool>> fields;

		for (pugi::xml_node field : message)
		{
			if (std::string(field.name()) == "field")
			{
				fields.push_back(std::make_pair(std::string(field.attribute("name").value()), std::string(field.attribute("required").value()) == "Y"));
			}
			else
			{
				//std::cout << field.attribute("name").value() << std::endl;
				//for (pugi::xml_node groupMember : field)
				//{
				//	std::cout << groupMember.attribute("name").value() << std::endl;
				//}
			}
		}
		createMessage(name, fields, msgType);
	}
}
void Application::createMessage(const std::string& name, const std::vector<std::pair<std::string, bool>>& fields, std::string msqType)
{
	std::string fileNameH = generationFolder_ + "messages/" + name + "_g.h";
	std::ofstream fH;
	fH.open(fileNameH);

	fH << "#pragma once\n";

	fH << "#include <string>\n";
	fH << "#include <optional>\n";
	for(const auto& field : fields)
	{
		fH << "#include <fields/" + field.first + "_g.h>;\n";
	}
	fH << "namespace xfix\n{\n\n";

	fH << "\nclass " + name + "\n{\n";

	fH << "public:\n";

	fH << "        " + name + "()\n";
	fH << "        {}\n\n";

	fH << "        bool hasAllRequired();\n\n";


	for(const auto& field : fields)
	{
		fH << "        std::optional<" + field.first + "> get" + field.first + "() const;\n";
		fH << "        void set" + field.first + "(std::optional<" + field.first + "> " + lowerField(field.first) + ");\n\n";
	}

	fH << "inline static const std::string msgType = \"" + msqType + "\";\n" ;

	fH << "private:\n";

	for(const auto& field : fields)
	{
		fH << "        std::optional<" + field.first + "> " + lowerField(field.first) + "_;\n";
	}

	fH << "};\n";

	fH << "\n} //xfix\n";

	fH.close();


	std::string fileNameCpp = generationFolder_ + "messages/" + name + "_g.cpp";
	fH.open(fileNameCpp);

	fH << "#include <messages/" + name + "_g.h>;\n\n";

	fH << "namespace xfix\n{\n\n";

	fH << "bool " + name + "::hasAllRequired() const\n";
	fH << "{\n";

	for(const auto& [fieldName,required] : fields)
	{
		if(required)
		{
			fH << "    if(!" + lowerField(fieldName) + "_) return false;\n";
		}
	}
	fH << "    return true;\n";

	fH << "}\n\n";

	for(const auto& field : fields)
	{
		fH << "std::optional<" + field.first + "> " + name + "::get" + field.first + "() const\n";
		fH << "{\n	return " + lowerField(field.first) + ";\n}\n";

		fH << "void set" + field.first + "(std::optional<" + field.first + "> " + lowerField(field.first) + ")\n";
		fH << "{\n	" + lowerField(field.first) + "_ = " + lowerField(field.first) + ";\n}\n\n";
	}

	fH << "\n} //xfix\n";
	fH.close();

	generatedFiles_.push_back(fileNameCpp);
}
std::string Application::lowerField(const std::string& field)
{
	std::string lowerField = field;
	lowerField[0] = std::tolower(field[0]);
	return lowerField;
}
} // xfix