#include "Window.h"

using namespace graphics;

void Window::init() {
    renderer = new Renderer(window);
}

void Window::loop() {
    running = true;
	while (running && fetchEvents()) {
		for (renderFunc rf : renderFunctions) rf(renderer);
	}
}
