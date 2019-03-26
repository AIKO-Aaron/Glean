#pragma once

#include "Renderer.h"

namespace graphics {

	typedef void(*renderFunc)(Renderer *renderer);

	class Window {
	private:
		Renderer *renderer = nullptr;
		__internalWindow window;
		std::vector<renderFunc> renderFunctions;

		bool fetchEvents();
	public:
		Window();
        bool running = false;

		inline void addRenderFunc(renderFunc rf) { renderFunctions.push_back(rf); }
		inline Renderer *getRenderer() { return renderer; }


		void init();
		void loop();
        void start();
	};

}
