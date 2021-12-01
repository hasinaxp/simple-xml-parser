#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include "XMLParser.hpp"

using namespace std;

string ReadFileAsString(string filePath)
{
	stringstream ss;
	ifstream file(filePath.c_str());
	if(file.is_open())
	{
		ss << file.rdbuf();
		file.close();
	}
	return ss.str();
}


int main()
{
	//load XML from file
	string fileContent = ReadFileAsString("test.html");
	sp::XMLNode xml;
	xml.Load(fileContent);

	//add data to xml
	sp::XMLNode footer;
	footer.tag = "footer";
	footer.content = "this is footer";
	xml.children[1].children.push_back(footer);
	xml.children[1].SetAttribute("style", "width: 100vw; height: 100vh; margin: 0");
	
	//log xml structure in console
	sp::XMLNode::PrintXLMNodeStructure(xml);
	cout << endl;
	
	//convert xml object to stiring
	cout << xml.ToString();
	return 0;
}