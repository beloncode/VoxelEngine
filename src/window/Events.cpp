#include "Events.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstring>

bool* Events::keys;
uint* Events::frames;
uint Events::current = 0;
float Events::deltaX = 0.0f;
float Events::deltaY = 0.0f;
float Events::x = 0.0f;
float Events::y = 0.0f;
bool Events::cursor_locked = false;
bool Events::cursor_started = false;

constexpr std::uint16_t MOUSE_BUTTONS = 1024;

static void cursorPositionCallback([[maybe_unused]] GLFWwindow* window, double xpos, double ypos){
	if (Events::cursor_started){
		Events::deltaX += static_cast<float>(xpos-Events::x);
		Events::deltaY += static_cast<float>(ypos-Events::y);
	}
	else {
		Events::cursor_started = true;
	}
	Events::x = static_cast<float>(xpos);
	Events::y = static_cast<float>(ypos);
}

static void mouseButtonCallback([[maybe_unused]] GLFWwindow* window, int button, int action, [[maybe_unused]] int mode){
	if (action == GLFW_PRESS){
		Events::keys[MOUSE_BUTTONS + button] = true;
		Events::frames[MOUSE_BUTTONS + button] = Events::current;
	}
	else if (action == GLFW_RELEASE){
		Events::keys[MOUSE_BUTTONS + button] = false;
		Events::frames[MOUSE_BUTTONS + button] = Events::current;
	}
}

static void keyCallback([[maybe_unused]] GLFWwindow* window, int key, [[maybe_unused]] int scancode,
                        int action, [[maybe_unused]] int mode) {
	if (action == GLFW_PRESS){
		Events::keys[key] = true;
		Events::frames[key] = Events::current;
	}
	else if (action == GLFW_RELEASE){
		Events::keys[key] = false;
		Events::frames[key] = Events::current;
	}
}

static void windowSizeCallback([[maybe_unused]] GLFWwindow* window, int width, int height){
	glViewport(0,0, width, height);
	Window::width = width;
	Window::height = height;
}

int Events::initialize(){
	GLFWwindow* window = Window::window;
    keys = new bool[1032];
    frames = new uint[1032];

	memset(keys, false, 1032 * sizeof(bool));
	memset(frames, 0, 1032 * sizeof(uint));

    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetWindowSizeCallback(window, windowSizeCallback);
	return 0;
}

void Events::finalize(){
	delete[] keys;
	delete[] frames;
}

bool Events::pressed(int keycode){
	if (keycode < 0 || keycode >= MOUSE_BUTTONS)
		return false;
	return keys[keycode];
}

bool Events::jpressed(int keycode){
	if (keycode < 0 || keycode >= MOUSE_BUTTONS)
		return false;
	return keys[keycode] && frames[keycode] == current;
}

[[maybe_unused]] bool Events::clicked(int button){
	int index = MOUSE_BUTTONS+button;
	return keys[index];
}

bool Events::jclicked(int button){
	int index = MOUSE_BUTTONS+button;
	return keys[index] && frames[index] == current;
}

void Events::toggleCursor(){
    cursor_locked = !cursor_locked;
	Window::setCursorMode(cursor_locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void Events::pullEvents(){
	current++;
	deltaX = 0.0f;
	deltaY = 0.0f;
	glfwPollEvents();
}
