//
// Created by will on 26/11/22.
//


#include <iostream>
#include "Application.h"

int main(int argc, char* argv[])
{
 	if( argc != 3)
    {
        std::cout << "Usage: XFixGen <xml file> <Generated Folder>" << std::endl;
        exit(-1);
    }

	xfix::Application application(argv[1], argv[2]);
	return 0;
}