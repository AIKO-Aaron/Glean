#include <graphics/Window.h>

#include <stdio.h>

using namespace graphics;

float color1 = 0, colorIncrease = 0.01;

static void render(graphics::Renderer *renderer) {
	renderer->clearColor(0.5, color1 += colorIncrease, 0.5, 1);
	renderer->swapBuffers();
}

int main(int argc, char **args) {
	printf("[INFO] Hello, World!\n");

	Window w;
	w.addRenderFunc(render);
	w.start();
}
