#include "Window.h"

using namespace graphics;

void Window::init() {
    renderer = new Renderer(window);
}

void Window::loop() {
    running = true;
    auto clock = std::chrono::high_resolution_clock();
	while (running && fetchEvents()) {
        auto start_time = clock.now();
		for (renderFunc rf : renderFunctions) rf(renderer);
        auto end_time = clock.now();
        auto diff = (end_time - start_time);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60) - diff);
	}
}
