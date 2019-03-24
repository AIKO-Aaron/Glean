#include "Window.h"

using namespace graphics;

void Window::init() {

}

void Window::loop() {
	while (fetchEvents()) {
		for (renderFunc rf : renderFunctions) rf(renderer);
	}
}