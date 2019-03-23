#include <graphics/Window.h>

#include <stdio.h>

using namespace graphics;

int main(int argc, char **args) {

	printf("[INFO] Hello, World!\n");

	Window w;
	w.getRenderer()->clearColor(1, 0, 1, 1);
	w.getRenderer()->swapBuffers();

	Sleep(2000);

}