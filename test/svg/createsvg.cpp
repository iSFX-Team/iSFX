// Fourier.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>

using std::cout;
using std::endl;


void CreateSvgFromPCM(const char *filename, std::vector<char> pcmarray)
{
	
	std::ofstream svgfile;
	int height = 64;
	int width = pcmarray.size();
	
	svgfile.open(filename);
	svgfile << "<?xml version=\"1.0\" standalone=\"yes\"?>" << endl;
	svgfile << "" << endl;
	svgfile << "<svg version=\"1.1\"\n"
					<< "     viewBox=\"0.0 -32.0 " << width << ".0 " << height <<".0\"\n"
					<< "     fill=\"none\"\n"
					<< "     stroke=\"none\"\n"
					<< "     stroke-linecap=\"square\"\n"
					<< "     stroke-miterlimit=\"10\"\n"
					<< "     clip-path=\"url(#clip0)\"\n" 
					<< "     xmlns=\"http://www.w3.org/2000/svg\"\n" 
          << "     xmlns:xlink=\"http://www.w3.org/1999/xlink\">" << endl;
    svgfile << "<clipPath id=\"clip0\">" << endl;
    svgfile << "<path d=\"M0 0L" << width << " 0L" << width << " " << height << "L0 " << height << "L0 0Z\" clip-rule=\"non-zero\">" << endl;
	svgfile << "</path>" << endl;
	svgfile << "</clipPath>" << endl;
	
	svgfile << "<path d=\"M0 " << (int)pcmarray[0] << endl;
	for(size_t i=1; i<pcmarray.size();i++)
	{
		svgfile << "          L" << i << " " << (int)pcmarray[i] << endl;
	}  
	svgfile << "\"" << endl;
	svgfile << "fill-rule=\"non-zero\" fill=\"#000000\" stroke=\"#000000\" stroke-width=\"1.0\" stroke-linejoin=\"round\" stroke-linecap=\"butt\">" << endl;
	svgfile << "</path>" << endl;
	svgfile << "</svg>" << endl;
	
	svgfile.close();
}

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		cout << "Incorrect number of arguments." << endl;
		return -1;
	}
	
	std::vector<char> pcmdata(100);
	for(size_t i=0;i<100;i++)
	{
		pcmdata[i] = (char)rand()%32;
	}
	const char *filename = argv[1];
	
	CreateSvgFromPCM(filename, pcmdata);
	
	return 1;
	
}
