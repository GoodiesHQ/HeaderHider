#include "HeaderHider.hpp"
#include <iostream>

typedef int (WINAPI *MBA)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);

int main(int argc, char **argv) {
	HMODULE user32 = LoadLibraryA("User32.dll");
	if (user32 == NULL)
		std::cout << "The library is loaded into memory.\n";
	else return std::cin.get(), 0;

	HeaderHider hh(user32);
	hh.hide();

	// headers are hidden. This should fail.
	MBA testMva = (MBA)GetProcAddress(user32, "MessageBoxA");
	if (testMva == NULL)
		std::cout << "You cannot resolve a function if the headers do not exist.\n";

	hh.restore();
	MBA mba = (MBA)GetProcAddress(user32, "MessageBoxA");
	hh.hide();

	if (mba != NULL)
		std::cout << "Now functions can be resolved.\n";
	else return std::cin.get(), -1;

	mba(NULL, "Test", "HI THERE", MB_OK);

	std::cin.get();
	
	return 0;
}
