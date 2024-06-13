#include "Assets.h" // for some reason, the CMake file won't link assets.h without the include
#include <iostream>


Assets::Assets() {

	init();
}

void Assets::init() {

	std::cout << "####### Loading assets... ########" << std::endl;
	// Load assets here
}