#include "Window.h"

using namespace graphics;

void Window::init() {

}

void Window::loop() {
	while (fetchEvents()) {
		renderer->clearColor(1, 0, 1, 1);
		renderer->swapBuffers();
	}
}