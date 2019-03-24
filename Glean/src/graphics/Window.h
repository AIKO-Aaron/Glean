#pragma once

#include "Renderer.h"

namespace graphics {

	class Window {
	private:
		Renderer *renderer = nullptr;
		__internalWindow window;

		bool fetchEvents();
	public:
		Window();

		inline Renderer *getRenderer() { return renderer; }


		void init();
		void loop();
	};

}