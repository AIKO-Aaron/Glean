#include <graphics/Window.h>

#include <stdio.h>

using namespace graphics;

float color1 = 0, colorIncrease = 0.01;

// FPS counter
int fps = 0;
auto _clock = std::chrono::high_resolution_clock();
auto lastTime = _clock.now();

static void render(graphics::Renderer *renderer) {
	renderer->clearColor(0.5, color1 += colorIncrease, 0.5, 1);
    if(color1 >= 1) colorIncrease = -0.01;
    else if(color1 <= 0) colorIncrease = 0.01;
	renderer->swapBuffers();
    
    // Simple fps counter
    ++fps;
    auto now = _clock.now();
    if((now - lastTime) >= std::chrono::milliseconds(1000)) {
        printf("[SANDBOX][INFO] FPS: %d\n", fps);
        fps = 0;
        lastTime = now;
    }
}

int main(int argc, char **args) {
	printf("[INFO] Hello, World!\n");

	Window w;
	w.addRenderFunc(render);
	w.start();
}
