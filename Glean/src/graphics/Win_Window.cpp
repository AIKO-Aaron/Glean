/*
 * Native implementation of window specific functions for windows
 */

#ifdef _WIN32

#include "Window.h"
#include <Windows.h>
#include <stdio.h>

#define WINDOWCLASS_NAME "aiko_glean_window"

using namespace graphics;

static LRESULT CALLBACK WindowProcedure(HWND window, UINT uMessage, WPARAM wParam, LPARAM lParam) {
	return DefWindowProc(window, uMessage, wParam, lParam);
}

static void registerWindowClass(HINSTANCE currentProgramInstance) {
	WNDCLASSEX windowClass;
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW; // Redraw the window if the size of it is changed
	windowClass.lpfnWndProc = WindowProcedure; // The handler for window events
	windowClass.cbClsExtra = 0; // Extra bytes to alloc for a window
	windowClass.cbWndExtra = 0; // Extra bytes after the window
	windowClass.hInstance = currentProgramInstance;
	windowClass.hIcon = nullptr;
	windowClass.hCursor = nullptr;
	windowClass.lpszMenuName = nullptr;
	windowClass.lpszClassName = WINDOWCLASS_NAME;
	windowClass.hIconSm = nullptr;
	windowClass.hbrBackground = nullptr; // (HBRUSH)(COLOR_WINDOW + 1);

	if (!RegisterClassEx(&windowClass)) {
		printf("[ERROR] Error registering window class\n");
		return;
	}
}

Window::Window() {
	HINSTANCE currentProgramInstance = GetModuleHandle(0); // Get the current program's instance

	if (!GetClassInfoEx(currentProgramInstance, WINDOWCLASS_NAME, nullptr)) registerWindowClass(currentProgramInstance);

	window = CreateWindow(WINDOWCLASS_NAME, "Hello World", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 960, 540, nullptr, nullptr, currentProgramInstance, nullptr);
	if (!window) {
		printf("[GLEAN][ERROR] Error creating window!\n");
		return;
	}

	ShowWindow(window, SW_SHOW);
	UpdateWindow(window);

	Sleep(1000);

	// Done, so call init
	init();
}

bool Window::fetchEvents() {
	MSG msg;
	bool a = GetMessage(&msg, window, 0, 0) > 0;
	if (!a) return false;
	TranslateMessage(&msg);
	DispatchMessage(&msg);

	return true;
}

void Window::start() { loop(); }

#endif
