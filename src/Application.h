//
// Created by will on 26/11/22.
//

#pragma once

#include <string>
#include <map>
#include <vector>
#include "pugixml.hpp"
namespace xfix
{

class Application
{
	enum FieldType{
		INT,
		FLOAT,
		CHAR,
		STRING,
		UTCDATE,
		UTCTIMESTAMP,
		UTCTIMEONLY,
		MONTHYEAR,
		DAYOFMONTH,
		LENGTH,
		DATA,
		LOCALMKTDATE,
		QTY,
		PRICE,
		AMT,
		BOOLEAN,
		PRICEOFFSET,
		EXCHANGE,
		CURRENCY
	};

	std::map<std::string, FieldType> stringToFieldType{
		{"INT",INT},
		{"FLOAT",FLOAT},
		{"CHAR",CHAR},
		{"STRING",STRING},
		{"UTCTIMESTAMP",UTCTIMESTAMP},
		{"UTCTIMEONLY",UTCTIMEONLY},
		{"UTCDATE",UTCDATE},
		{"MONTHYEAR",MONTHYEAR},
		{"DAYOFMONTH",DAYOFMONTH},
		{"LENGTH",LENGTH},
		{"DATA",DATA},
		{"LOCALMKTDATE",LOCALMKTDATE},
		{"QTY",QTY},
		{"PRICE",PRICE},
		{"AMT",AMT},
		{"BOOLEAN",BOOLEAN},
		{"PRICEOFFSET",PRICEOFFSET},
		{"CURRENCY",CURRENCY},
		{"EXCHANGE",EXCHANGE},
	};

public:
	Application(const std::string& xmlFile, const std::string& generationFolder);

private:
	void createFields(const pugi::xml_node& fixNode);
	void createField(const std::string& name, int number, FieldType type);


private:
	std::string generationFolder_;
	std::vector<std::string> generatedFiles_;

};

} // xfix
