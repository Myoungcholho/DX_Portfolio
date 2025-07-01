#pragma once

void EnableConsole()
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);  // std::cout
	freopen("CONOUT$", "w", stderr);  // std::cerr
	freopen("CONIN$", "r", stdin);    // std::cin
}