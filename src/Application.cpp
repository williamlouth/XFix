//
// Created by will on 26/11/22.
//

#include <iostream>
#include "Application.h"
#include "pugixml.hpp"

namespace xfix
{

Application::Application(const std::string& xmlFile)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(xmlFile.c_str());
	if(!result) throw std::runtime_error("xml parsing error");

	pugi::xml_node fixNode = doc.child("fix");

	//int fixMajor = std::stoi(fixNode.attribute("major").value());
	//int fixMinor = std::stoi(fixNode.attribute("minor").value());

	pugi::xml_node header = fixNode.child("header");
	for(pugi::xml_node field : header)
	{
		std::cout << field.attribute("name").value() << std::endl;
	}

	pugi::xml_node messages = fixNode.child("messages");
	for(pugi::xml_node message : messages)
	{
		std::cout << message.attribute("name").value() << std::endl;
		for(pugi::xml_node field : message)
		{
			if(std::string(field.name()) == "field")
			{
				std::cout << field.attribute("name").value() << std::endl;
			}
			else
			{
				std::cout << field.attribute("name").value() << std::endl;
				for(pugi::xml_node groupMember : field)
				{
					std::cout << groupMember.attribute("name").value() << std::endl;
				}
			}
		}
	}

	pugi::xml_node trailer = fixNode.child("trailer");
	for(pugi::xml_node field : trailer)
	{
		std::cout << field.attribute("name").value() << std::endl;
	}

	pugi::xml_node fields = fixNode.child("fields");
	for(pugi::xml_node field : fields)
	{
		std::cout << field.attribute("name").value() << std::endl;
		if (std::string(field.attribute("type").value()) == "CHAR" ||
			std::string(field.attribute("type").value()) == "MULTIPLEVALUESTRING")
		{
			for(pugi::xml_node value : field)
			{
				std::cout << value.attribute("enum").value() << std::endl;
			}
		}
	}

	/*
	for(pugi::xml_node node: root)
	{
		std::cout << node.text() << std::endl;
	}
	 */

}

} // xfix