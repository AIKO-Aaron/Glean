#pragma once

#include <vector>

#if defined(_WIN32)
#  include <Windows.h>
typedef HWND __internalWindow;
#elif defined(__APPLE__)

#else

#endif

namespace graphics {

	class Renderer {
	private:

	public:
		Renderer(__internalWindow window);

	};

}