#ifndef WINDOW_EVENTS_H
#define WINDOW_EVENTS_H

#include "Window.h"

typedef unsigned int uint;

class Events {
public:
	static bool* keys;
	static uint* frames;
	static uint current;
	static float deltaX;
	static float deltaY;
	static float x;
	static float y;
	static bool cursor_locked;
	static bool cursor_started;

	static int initialize();
	static void finalize();
	static void pullEvents();

	static bool pressed(int keycode);
	static bool jpressed(int keycode);

    [[maybe_unused]] static bool clicked(int button);
	static bool jclicked(int button);

	static void toggleCursor();
};

#endif /* WINDOW_EVENTS_H */
