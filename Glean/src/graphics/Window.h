#pragma once

#include "Renderer.h"

namespace graphics {

	class Window {
	private:
		Renderer *renderer = nullptr;
		__internalWindow window;

	public:
		Window();

		void init();
	};

}