#include "StringManager.h"

using namespace Sexy;

StringManager::StringManager(void)
{
}

StringManager::~StringManager(void)
{
}


StringManager::StringManager(const std::string &theSourceFile)
{
	mSourceFile = theSourceFile;
}

std::string	StringManager::GetString( const std::string &theKey )
{
	return "Test";
}