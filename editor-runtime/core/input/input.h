#pragma once

#include <glm.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Input
{

	void setContext(GLFWwindow* window); // Sets context for input system
	void step(); // Fetches inputs of current frame

	glm::vec2 moveAxis(); // Returns WASD input axis
	glm::vec2 mouseDelta(); // Returns mouse movement delta
	glm::vec2 scrollDelta(); // Returns mouse scroll delta

	glm::vec2 mousePosition(); // Returns mouse position on screen

	bool keyDown(int key); // Returns if given key is pressed
	bool mouseDown(int mouseButton); // Returns if given mouse button is pressed
	bool command(int key); // Returns if given key and any control key is pressed

};

namespace Key {
	// Letters
	static constexpr int A = GLFW_KEY_A;
	static constexpr int B = GLFW_KEY_B;
	static constexpr int C = GLFW_KEY_C;
	static constexpr int D = GLFW_KEY_D;
	static constexpr int E = GLFW_KEY_E;
	static constexpr int F = GLFW_KEY_F;
	static constexpr int G = GLFW_KEY_G;
	static constexpr int H = GLFW_KEY_H;
	static constexpr int I = GLFW_KEY_I;
	static constexpr int J = GLFW_KEY_J;
	static constexpr int K = GLFW_KEY_K;
	static constexpr int L = GLFW_KEY_L;
	static constexpr int M = GLFW_KEY_M;
	static constexpr int N = GLFW_KEY_N;
	static constexpr int O = GLFW_KEY_O;
	static constexpr int P = GLFW_KEY_P;
	static constexpr int Q = GLFW_KEY_Q;
	static constexpr int R = GLFW_KEY_R;
	static constexpr int S = GLFW_KEY_S;
	static constexpr int T = GLFW_KEY_T;
	static constexpr int U = GLFW_KEY_U;
	static constexpr int V = GLFW_KEY_V;
	static constexpr int W = GLFW_KEY_W;
	static constexpr int X = GLFW_KEY_X;
	static constexpr int Y = GLFW_KEY_Y;
	static constexpr int Z = GLFW_KEY_Z;

	// Numbers
	static constexpr int _0 = GLFW_KEY_0;
	static constexpr int _1 = GLFW_KEY_1;
	static constexpr int _2 = GLFW_KEY_2;
	static constexpr int _3 = GLFW_KEY_3;
	static constexpr int _4 = GLFW_KEY_4;
	static constexpr int _5 = GLFW_KEY_5;
	static constexpr int _6 = GLFW_KEY_6;
	static constexpr int _7 = GLFW_KEY_7;
	static constexpr int _8 = GLFW_KEY_8;
	static constexpr int _9 = GLFW_KEY_9;

	// Function keys
	static constexpr int F1 = GLFW_KEY_F1;
	static constexpr int F2 = GLFW_KEY_F2;
	static constexpr int F3 = GLFW_KEY_F3;
	static constexpr int F4 = GLFW_KEY_F4;
	static constexpr int F5 = GLFW_KEY_F5;
	static constexpr int F6 = GLFW_KEY_F6;
	static constexpr int F7 = GLFW_KEY_F7;
	static constexpr int F8 = GLFW_KEY_F8;
	static constexpr int F9 = GLFW_KEY_F9;
	static constexpr int F10 = GLFW_KEY_F10;
	static constexpr int F11 = GLFW_KEY_F11;
	static constexpr int F12 = GLFW_KEY_F12;

	// Arrow keys
	static constexpr int UP = GLFW_KEY_UP;
	static constexpr int DOWN = GLFW_KEY_DOWN;
	static constexpr int LEFT = GLFW_KEY_LEFT;
	static constexpr int RIGHT = GLFW_KEY_RIGHT;

	// Modifier keys
	static constexpr int LEFT_SHIFT = GLFW_KEY_LEFT_SHIFT;
	static constexpr int LEFT_CONTROL = GLFW_KEY_LEFT_CONTROL;
	static constexpr int LEFT_ALT = GLFW_KEY_LEFT_ALT;
	static constexpr int LEFT_SUPER = GLFW_KEY_LEFT_SUPER;
	static constexpr int RIGHT_SHIFT = GLFW_KEY_RIGHT_SHIFT;
	static constexpr int RIGHT_CONTROL = GLFW_KEY_RIGHT_CONTROL;
	static constexpr int RIGHT_ALT = GLFW_KEY_RIGHT_ALT;
	static constexpr int RIGHT_SUPER = GLFW_KEY_RIGHT_SUPER;

	// Special keys
	static constexpr int ESCAPE = GLFW_KEY_ESCAPE;
	static constexpr int ENTER = GLFW_KEY_ENTER;
	static constexpr int TAB = GLFW_KEY_TAB;
	static constexpr int BACKSPACE = GLFW_KEY_BACKSPACE;
	static constexpr int INSERT = GLFW_KEY_INSERT;
	static constexpr int DELETE = GLFW_KEY_DELETE;
	static constexpr int HOME = GLFW_KEY_HOME;
	static constexpr int END = GLFW_KEY_END;
	static constexpr int PAGE_UP = GLFW_KEY_PAGE_UP;
	static constexpr int PAGE_DOWN = GLFW_KEY_PAGE_DOWN;
	static constexpr int CAPS_LOCK = GLFW_KEY_CAPS_LOCK;
	static constexpr int SCROLL_LOCK = GLFW_KEY_SCROLL_LOCK;
	static constexpr int NUM_LOCK = GLFW_KEY_NUM_LOCK;
	static constexpr int PRINT_SCREEN = GLFW_KEY_PRINT_SCREEN;
	static constexpr int PAUSE = GLFW_KEY_PAUSE;

	// Keypad (numpad) keys
	static constexpr int KP_0 = GLFW_KEY_KP_0;
	static constexpr int KP_1 = GLFW_KEY_KP_1;
	static constexpr int KP_2 = GLFW_KEY_KP_2;
	static constexpr int KP_3 = GLFW_KEY_KP_3;
	static constexpr int KP_4 = GLFW_KEY_KP_4;
	static constexpr int KP_5 = GLFW_KEY_KP_5;
	static constexpr int KP_6 = GLFW_KEY_KP_6;
	static constexpr int KP_7 = GLFW_KEY_KP_7;
	static constexpr int KP_8 = GLFW_KEY_KP_8;
	static constexpr int KP_9 = GLFW_KEY_KP_9;
	static constexpr int KP_DECIMAL = GLFW_KEY_KP_DECIMAL;
	static constexpr int KP_DIVIDE = GLFW_KEY_KP_DIVIDE;
	static constexpr int KP_MULTIPLY = GLFW_KEY_KP_MULTIPLY;
	static constexpr int KP_SUBTRACT = GLFW_KEY_KP_SUBTRACT;
	static constexpr int KP_ADD = GLFW_KEY_KP_ADD;
	static constexpr int KP_ENTER = GLFW_KEY_KP_ENTER;
	static constexpr int KP_EQUAL = GLFW_KEY_KP_EQUAL;

	// Miscellaneous keys
	static constexpr int SPACE = GLFW_KEY_SPACE;
	static constexpr int APOSTROPHE = GLFW_KEY_APOSTROPHE;
	static constexpr int COMMA = GLFW_KEY_COMMA;
	static constexpr int MINUS = GLFW_KEY_MINUS;
	static constexpr int PERIOD = GLFW_KEY_PERIOD;
	static constexpr int SLASH = GLFW_KEY_SLASH;
	static constexpr int SEMICOLON = GLFW_KEY_SEMICOLON;
	static constexpr int EQUAL = GLFW_KEY_EQUAL;
	static constexpr int LEFT_BRACKET = GLFW_KEY_LEFT_BRACKET;
	static constexpr int BACKSLASH = GLFW_KEY_BACKSLASH;
	static constexpr int RIGHT_BRACKET = GLFW_KEY_RIGHT_BRACKET;
	static constexpr int GRAVE_ACCENT = GLFW_KEY_GRAVE_ACCENT;
	static constexpr int WORLD_1 = GLFW_KEY_WORLD_1;
	static constexpr int WORLD_2 = GLFW_KEY_WORLD_2;
}

namespace MouseButton {
	static constexpr int LEFT = GLFW_MOUSE_BUTTON_1;
	static constexpr int RIGHT = GLFW_MOUSE_BUTTON_2;
	static constexpr int MIDDLE = GLFW_MOUSE_BUTTON_3;

	static constexpr int BACK = GLFW_MOUSE_BUTTON_4;
	static constexpr int FORWARD = GLFW_MOUSE_BUTTON_5;

	static constexpr int EXTRA_1 = GLFW_MOUSE_BUTTON_6;
	static constexpr int EXTRA_2 = GLFW_MOUSE_BUTTON_7;
	static constexpr int EXTRA_3 = GLFW_MOUSE_BUTTON_8;
}