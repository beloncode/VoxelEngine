#ifndef WINDOW_WINDOW_H
#define WINDOW_WINDOW_H

class GLFWwindow;

class Window {
public:
	static int width;
	static int height;
	static GLFWwindow* window; // не лучшее решение делать window публичным
	static int initialize(int widthLocal, int heightLocal, const char* title);
	static void terminate();

	static void setCursorMode(int mode);
	static bool isShouldClose();
	static void setShouldClose(bool flag);
	static void swapBuffers();
};

#endif /* WINDOW_WINDOW_H */
