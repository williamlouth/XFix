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

	pugi::xml_node messages = fixNode.child("messages");
	for (pugi::xml_node message : messages)
	{
		std::cout << message.attribute("name").value() << std::endl;
		for (pugi::xml_node field : message)
		{
			if (std::string(field.name()) == "field")
			{
			}
			else
			{
				std::cout << field.attribute("name").value() << std::endl;
				for (pugi::xml_node groupMember : field)
				{
					std::cout << groupMember.attribute("name").value() << std::endl;
				}
			}
		}
	}

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

	fH << "private:\n";

	fH << "std::string value_;\n";

	fH << "};\n";

	fH << "\n} //xfix\n";

	fH.close();
	generatedFiles_.push_back(fileName);
}
} // xfix