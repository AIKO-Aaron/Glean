#pragma once

#include "Renderer.h"

namespace graphics {

	class Window {
	private:
		Renderer *renderer = nullptr;
		__internalWindow window;

	public:
		Window();

		inline Renderer *getRenderer() { return renderer; }

		void init();
	};

}