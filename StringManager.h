#pragma once

#include "../SexyAppFramework/SexyAppBase.h"
#include <string>

namespace Sexy
{

class XMLParser;
class XMLElement;

class StringManager
{
public:
	StringManager(void);
	StringManager(const std::string &theSourceFile);

	~StringManager(void);

public:
	std::string			GetString( const std::string &theKey );

private:
	std::string			mSourceFile;

	XMLParser*			mXMLParser;

};

}