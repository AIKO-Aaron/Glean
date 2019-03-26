#pragma once

#include <vector>

#define NUM_FRAMEBUFFERS 2

// Platform specific stuff
#include "RendererPlatform.hpp"

namespace graphics {

	class Renderer {
	private:
		__rendererData rd;

	public:
		Renderer(__internalWindow window);

		void swapBuffers();
		void synchronize(); // Wait for frame to render (maybe not needed for every renderer)
		void clearColor(float r, float g, float b, float a);
	};

}
