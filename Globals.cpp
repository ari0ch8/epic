#include "Globals.h"

using namespace Sexy;


int gPauseLevel;
float gGraphicsScale;

Floater					gTitleLogo;
LoadingProgress			gLoadingProgress;

FloatingNumber  floatingnumber[100];
Tile*			floatingtile[100];


ActorManager		gActorManager;
TileManager			gTileManager;
Map*				gCurrentMap = NULL;
GameProperties		gGameProperties;
EventManager		gEventManager;

#ifdef MATCHBLOX_ASSERT
char debug[1024];
#endif



namespace Sexy
{
	void Split(const std::string& str, const std::string& delim, std::vector<std::string>& output)
	{
		unsigned int offset = 0;
		unsigned int delimIndex = 0;
	    
		delimIndex = str.find(delim, offset);

		while (delimIndex != std::string::npos)
		{
			output.push_back(str.substr(offset, delimIndex - offset));
			offset += delimIndex - offset + delim.length();
			delimIndex = str.find(delim, offset);
		}

		output.push_back(str.substr(offset));
	}


	void convert( std::string &in, std::wstring &out )
	{
		out.resize(in.size());
		const char *in_next  = &*in.begin();
		wchar_t *out_next = &*out.begin();

		typedef std::codecvt<wchar_t, char, std::mbstate_t> converter;
		const converter &my_converter = std::use_facet<converter>(std::locale());

		std::mbstate_t my_state;

		my_converter.in( my_state,
		in.c_str(), in.c_str()+in.length(), in_next,
		&out[0], (&out[0])+in.length(), out_next);
	}

typedef bool (__stdcall *InstallKeyFn)(const char *name, const char *code);
typedef bool (__stdcall *InstallKeyLaterFn)(const char *name, const char *code);

int InstallArmadilloCode(const char *name, const char *codestring) {

       int returnvalue=0;

       HINSTANCE libInst=LoadLibrary("ArmAccess.DLL");

       if (!libInst) return 0; /* Couldn't load library */

       /* Substitute the typedefs above for functions other than InstallKey */

       InstallKeyLaterFn InstallKey=(InstallKeyLaterFn)GetProcAddress(libInst, "InstallKey");

       if (InstallKey==0) returnvalue=0; /* Couldn't find function */

       else returnvalue=InstallKey(name, codestring);

       FreeLibrary(libInst); /* Not needed for the virtual ArmAccess.DLL, but it won't hurt anything. */

       return returnvalue;

};

typedef bool (__stdcall *UpdateEnvironmentFn)(void);

int UpdateArmadilloEnvironment(void) {

       int returnvalue=0;

       HINSTANCE libInst=LoadLibrary("ArmAccess.DLL");

       if (!libInst) return 0; /* Couldn't load library */

       /* Substitute the typedefs above for functions other than InstallKey */

       UpdateEnvironmentFn UpdateEnvironment=(UpdateEnvironmentFn)GetProcAddress(libInst, "UpdateEnvironment");

       if (UpdateEnvironment==0) returnvalue=0; /* Couldn't find function */

       else returnvalue=UpdateEnvironment();

       FreeLibrary(libInst); /* Not needed for the virtual ArmAccess.DLL, but it won't hurt anything. */

       return returnvalue;

};


typedef bool (__stdcall *SetDefaultKeyFn)(void);

int SetArmadilloDefaultKey(void) {

       int returnvalue=0;

       HINSTANCE libInst=LoadLibrary("ArmAccess.DLL");

       if (!libInst) return 0; /* Couldn't load library */

       /* Substitute the typedefs above for functions other than InstallKey */

       SetDefaultKeyFn SetDefaultKey=(SetDefaultKeyFn)GetProcAddress(libInst, "SetDefaultKey");

       if (SetDefaultKey==0) returnvalue=0; /* Couldn't find function */

       else returnvalue=SetDefaultKey();

       FreeLibrary(libInst); /* Not needed for the virtual ArmAccess.DLL, but it won't hurt anything. */

       return returnvalue;

};




bool IsArmadilloDefaultTrial(void) {

   char name[256]="", key[256]="", display[512];

   /* Check the ALTUSERNAME; it should always be set if the program is protected */

   if (!GetEnvironmentVariable("ALTUSERNAME", name, 255)) {

       return false;

   };

   if (!GetEnvironmentVariable("USERKEY", key, 255)) strcpy(key, "No key!");

   if (stricmp(name, "DEFAULT")==0) {

       return true;

   } else {

       return false;

   };

}


bool IsArmadillo5MinRepeatingTrial(void) {

   char name[256]="", key[256]="", display[512];

   /* Check the ALTUSERNAME; it should always be set if the program is protected */

   if (!GetEnvironmentVariable("ALTUSERNAME", name, 255)) {
	   //OutputDebugString("No altusername found.\r\n");

       return false;

   };

   //OutputDebugString(name);
   //OutputDebugString("\r\n");


   if (!GetEnvironmentVariable("USERKEY", key, 255)) strcpy(key, "No key!");

   if (stricmp(name, "5MINDEMO")==0) {

       return true;

   } else {

       return false;

   };

}

bool IsArmadilloWrapperInstalled(void) {

   char name[256]="", key[256]="", display[512];

   /* Check the ALTUSERNAME; it should always be set if the program is protected */

   if (!GetEnvironmentVariable("ALTUSERNAME", name, 255)) {
	   //OutputDebugString("No altusername found.\r\n");

       return false;

   }
   else
   {
	   return true;
   }
}

}