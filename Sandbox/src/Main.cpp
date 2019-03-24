#include <graphics/Window.h>

#include <stdio.h>

using namespace graphics;

int main(int argc, char **args) {

	printf("[INFO] Hello, World!\n");

	Window w;
	w.loop();

	Sleep(2000);

}