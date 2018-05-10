#include <cstring>

#include "game/messagebox.h"
#include "game/exceptions/fatalexception.h"
#include "game/net/server/server.h"

int main(int argc, const char** argv)
{
	try
	{
		TankGame::StartServer();
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
