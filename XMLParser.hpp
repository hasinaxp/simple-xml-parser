#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
namespace sp
{

	using namespace std;

	
	struct XMLAttribute
	{
		string key;
		string value;
	};

	//main class that represents xml data
	//member data: 
	//	tag - name of the tag that the xml node represents
	//	content - text content of the xml node
	// 	children - vector of children xml nodes (creates the tree)
	//	attributes - vector of all attributes (these are represented by XMLAttribute struct)
	//member function:
	//bool	Load(string str) - loades the xml data from the string str and stores the data as member data
	//void SetAttribute(string key, string value) - set the attribute with key and value. if attribute exists then the value is modified
	//void RemoveAttribute(string key) - remove the attribute with given key
	class XMLNode
	{
	public:
		string tag;
		string content;
		vector<XMLNode> children;
		vector<XMLAttribute> attributes;
		string doctype;
		
	public:
		XMLNode() : tag(""), content(""), children({}), attributes({}), doctype("")
		{
		}
		~XMLNode()
		{

		}
		//create xml object from a string 
		bool Load(string text)
		{
			vector<string> tokens = Tokenize(text);
			bool status = ParseTokens(tokens);
			return status;
		}

		//convert the xml object to string 
		string ToString()
		{
			stringstream ss;
			if(doctype != "")
				ss << "<!DOCTYPE " << doctype << ">";
			ss << "<";
			ss << tag;
			for(XMLAttribute & attrib : attributes)
			{
				ss << " " << attrib.key;
				if(attrib.value != "")
				{
					ss << "=" << "\"" << attrib.value << "\"";
				}
			}
			ss << ">";
			for(XMLNode & child : children)
				ss << child.ToString();
			if(children.size() == 0)
				ss << content;
			ss << "</" << tag << ">";
			return ss.str();
		}

		void SetAttribute(string key, string value = "")
		{
			int index = -1;
			for(int i = 0; i < attributes.size(); i++)
				if(attributes[i].key == key)
				{
					index - i;
					break;
				}
			if(index == -1)
			{
				XMLAttribute attr = {key, value};
				attributes.push_back(attr);
			}
			else
			{
				attributes[index].value = value;
			}
		}
		void RemoveAttribute(string key)
		{
			for(int i = 0; i < attributes.size(); i++) 
			{
				if(attributes[i].key == key)
				{
					attributes[i]= attributes.back();
					attributes.pop_back();
					break;
				}
			}
		}

	private:
		vector<string> Tokenize(string text)
		{
			vector<string> tokens;
			string temp;
			char state = 'n';
			bool isComment = false;
			for(int i = 0; i < text.length();i++)
			{
				switch (text[i])
				{
				case '\'':
					if(state == 'q') {
						if(temp != "") {
							tokens.push_back(temp);
							temp = "";
						}
						state = 't';
					}
					else if(state == 't')
					{
						if(temp != "") {
							tokens.push_back(temp);
							temp = "";
						}
						state = 'q';
					}
					else {
						temp += text[i];
					}
					break;
				case '"':
					if(state == 'Q') {
						if(temp != "") {
							tokens.push_back(temp);
							temp = "";
						}
						state = 't';
					}
					else if(state == 't')
					{
						if(temp != "") {
							tokens.push_back(temp);
							temp = "";
						}
						state = 'Q';
					}
					else {
						temp += text[i];
					}
					break;
				case '<':
					if(text[i+1] == '!' && text[i+2] == '-' && text[i+3] == '-')
					{
						while(!(text[i] == '-' && text[i+1] == '-' && text[i+2] == '>'))
						{
							i++;
						}
						i += 1;
					}
					else if(state == 'Q' || state == 'q')
					{
						temp += text[i];
					}
					else
					{
						state = 't';
						if(temp != "") {
							tokens.push_back(temp);
							temp = "";
						}
						tokens.push_back(string(1, text[i]));
					}
					break;
				case '>':
					if(state == 'Q' || state == 'q')
					{
						temp += text[i];
					}
					else
					{
						state = 'n';
						if(temp != "") {
							tokens.push_back(temp);
							temp = "";
						}
						tokens.push_back(string(1, text[i]));
					}
					break;
				case '/':
					if(state == 't') {
						tokens.push_back(string(1, text[i]));
					} else {
						temp += text[i];
					}
					break;
				case ' ':
					if(state == 't') {
						if(temp != "") {
							tokens.push_back(temp);
							temp = "";
						}
					}
					else {
						temp += text[i];
					}
					break;
				case '=':
					if(state == 't') {
						if(temp != "") {
							tokens.push_back(temp);
							temp = "";
						}
						tokens.push_back(string(1, text[i]));
					}
					else {
						temp += text[i];
					}
					break;
				case '\n':
					break;
				case '\r':
					break;
				case '\t':
					break;
				default:
					temp += text[i];
					break;
				}
				
			}

			if(temp != "")
				tokens.push_back(temp);

			return tokens;
		}
	
		enum class ParseState
		{
			tag,
			content,
			sibling			
		};


		bool ParseTokens(vector<string> &tokens)
		{
			vector<XMLNode *> stk = {};
			XMLNode * cNode = this;
			XMLNode * pNode = nullptr;
			int i = 0;
			ParseState state;
			bool errorParsing = false;
			while(i < tokens.size())
			{
				if(tokens[i] == "<") {
					if(tokens[i+1] == "!DOCTYPE")
					{
						i += 2;
						while(tokens[i] != ">")
						{
							doctype += tokens[i++];
						}
						i++;
					}
					if(tokens[i+1] == "/") //closing tag
					{
						if( tokens[i+3] == ">")
						{
							string closingTag = tokens[i+2];
							vector<XMLNode> xnodes = {};
							vector<XMLNode> lastChildren = {};
							while(stk.back()->tag != closingTag)
							{
								if(stk.back()->children.size()){
									for(int j = stk.back()->children.size() -1; j >= 0; j--)
									{
										lastChildren.push_back(stk.back()->children[j]);			
									}
								}
								stk.back()->children = {};
								stk.pop_back();
							}
							for(int j = lastChildren.size() -1; j >= 0; j--)
							{
								stk.back()->children.push_back(lastChildren[j]);
							}
							stk.pop_back();
							if(stk.size())
							{
								cNode = stk.back();
							}
							for(XMLNode& n : xnodes)
							{
								cNode->children.push_back(n);
							}
							i+= 4;
						}
						else
						{
							errorParsing = true;
							break;
						}
					}
					else //opening tag
					{
						if(stk.size())
						{
							XMLNode node;
							cNode->children.push_back(node);
							cNode = &(cNode->children.back());
						}
						state = ParseState::tag;
						cNode->tag = tokens[i+1];
						 i+= 2;

					}
				}
				else if(tokens[i] == ">")
				{
					state = ParseState::content;
					stk.push_back(cNode);
					i++;
				}
				else if(state == ParseState::tag)
				{
					if(tokens[i] == "/" && tokens[i+1] == ">")
					{
							
							if(stk.size())
							{
								cNode = stk.back();
							}
							i+= 2;
					}
					else
					{
						XMLAttribute attrib;
						attrib.key = tokens[i];
						if(tokens[i+1] == "=") {
							attrib.value = tokens[i+2];
							i += 3;
						}
						else
							i++;
						bool isAttribPresent = false;
						for(XMLAttribute &attr : cNode->attributes)
						{
							if(attr.key == attrib.key) {
								attr.value = attrib.value;
								isAttribPresent = false;
								break;
							}
						}
						cNode->attributes.push_back(attrib);
					}
				}
				else
				{
					cNode->content = tokens[i];
					i++;
				}

			}//end of while loop
			
			return !errorParsing;
		}

	public:
		static void PrintXLMNodeStructure(XMLNode & node, int level = 0)
		{
			for(int i = 0; i < level; i++) {
				cout << "    " ;
			}
			cout << node.tag;
			if(node.attributes.size())
			{
				cout << "( ";
				for( XMLAttribute & attrib : node.attributes)
					cout << attrib.key << "=" << attrib.value << ", ";
				cout << " )";
			
			}
			cout << "-> " << node.content << endl;
			if(node.children.size())
			{
				for(XMLNode & child : node.children)
					PrintXLMNodeStructure(child, level + 1);
			}

		}
	
	};
	

};