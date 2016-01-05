#include <string>

#include "Main.h"

#include "Game.h"

using namespace Learnings;

auto main() -> int
{
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	std::wstring cmdLine(GetCommandLineW());

	Game game(cmdLine);

	return game.Run();
}