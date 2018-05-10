#ifdef _WIN32
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

#include <cstring>

#include "game/messagebox.h"
#include "game/exceptions/fatalexception.h"
#include "game/client.h"

int main(int argc, const char** argv)
{
	TankGame::ClientArgs arguments;
	
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-prof") == 0)
			arguments.profiling = true;
		if (strcmp(argv[i], "-nocursorgrab") == 0)
			arguments.noCursorGrab = true;
		if (strcmp(argv[i], "-dsawrapper") == 0)
			arguments.useDSAWrapper = true;
	}
	
	try
	{
		TankGame::StartClient(arguments);
	}
catch (const TankGame::FatalException& exception)
	{
		TankGame::ShowErrorMessage(exception.what(), "Error");
		return 1;
	}
#ifdef NDEBUG
	catch (const std::exception& exception)
	{
		ShowErrorMessage(exception.what(), "Unexpected Error");
		return 1;
	}
#endif
}