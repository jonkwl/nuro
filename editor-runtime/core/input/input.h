#pragma once

#include <cstdint>
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

	bool keyDown(int32_t key); // Returns if given key is pressed
	bool mouseDown(int32_t mouseButton); // Returns if given mouse button is pressed
	bool command(int32_t key); // Returns if given key and any control key is pressed

};

namespace Key {
	// Letters
	static constexpr int32_t A = GLFW_KEY_A;
	static constexpr int32_t B = GLFW_KEY_B;
	static constexpr int32_t C = GLFW_KEY_C;
	static constexpr int32_t D = GLFW_KEY_D;
	static constexpr int32_t E = GLFW_KEY_E;
	static constexpr int32_t F = GLFW_KEY_F;
	static constexpr int32_t G = GLFW_KEY_G;
	static constexpr int32_t H = GLFW_KEY_H;
	static constexpr int32_t I = GLFW_KEY_I;
	static constexpr int32_t J = GLFW_KEY_J;
	static constexpr int32_t K = GLFW_KEY_K;
	static constexpr int32_t L = GLFW_KEY_L;
	static constexpr int32_t M = GLFW_KEY_M;
	static constexpr int32_t N = GLFW_KEY_N;
	static constexpr int32_t O = GLFW_KEY_O;
	static constexpr int32_t P = GLFW_KEY_P;
	static constexpr int32_t Q = GLFW_KEY_Q;
	static constexpr int32_t R = GLFW_KEY_R;
	static constexpr int32_t S = GLFW_KEY_S;
	static constexpr int32_t T = GLFW_KEY_T;
	static constexpr int32_t U = GLFW_KEY_U;
	static constexpr int32_t V = GLFW_KEY_V;
	static constexpr int32_t W = GLFW_KEY_W;
	static constexpr int32_t X = GLFW_KEY_X;
	static constexpr int32_t Y = GLFW_KEY_Y;
	static constexpr int32_t Z = GLFW_KEY_Z;

	// Numbers
	static constexpr int32_t _0 = GLFW_KEY_0;
	static constexpr int32_t _1 = GLFW_KEY_1;
	static constexpr int32_t _2 = GLFW_KEY_2;
	static constexpr int32_t _3 = GLFW_KEY_3;
	static constexpr int32_t _4 = GLFW_KEY_4;
	static constexpr int32_t _5 = GLFW_KEY_5;
	static constexpr int32_t _6 = GLFW_KEY_6;
	static constexpr int32_t _7 = GLFW_KEY_7;
	static constexpr int32_t _8 = GLFW_KEY_8;
	static constexpr int32_t _9 = GLFW_KEY_9;

	// Function keys
	static constexpr int32_t F1 = GLFW_KEY_F1;
	static constexpr int32_t F2 = GLFW_KEY_F2;
	static constexpr int32_t F3 = GLFW_KEY_F3;
	static constexpr int32_t F4 = GLFW_KEY_F4;
	static constexpr int32_t F5 = GLFW_KEY_F5;
	static constexpr int32_t F6 = GLFW_KEY_F6;
	static constexpr int32_t F7 = GLFW_KEY_F7;
	static constexpr int32_t F8 = GLFW_KEY_F8;
	static constexpr int32_t F9 = GLFW_KEY_F9;
	static constexpr int32_t F10 = GLFW_KEY_F10;
	static constexpr int32_t F11 = GLFW_KEY_F11;
	static constexpr int32_t F12 = GLFW_KEY_F12;

	// Arrow keys
	static constexpr int32_t UP = GLFW_KEY_UP;
	static constexpr int32_t DOWN = GLFW_KEY_DOWN;
	static constexpr int32_t LEFT = GLFW_KEY_LEFT;
	static constexpr int32_t RIGHT = GLFW_KEY_RIGHT;

	// Modifier keys
	static constexpr int32_t LEFT_SHIFT = GLFW_KEY_LEFT_SHIFT;
	static constexpr int32_t LEFT_CONTROL = GLFW_KEY_LEFT_CONTROL;
	static constexpr int32_t LEFT_ALT = GLFW_KEY_LEFT_ALT;
	static constexpr int32_t LEFT_SUPER = GLFW_KEY_LEFT_SUPER;
	static constexpr int32_t RIGHT_SHIFT = GLFW_KEY_RIGHT_SHIFT;
	static constexpr int32_t RIGHT_CONTROL = GLFW_KEY_RIGHT_CONTROL;
	static constexpr int32_t RIGHT_ALT = GLFW_KEY_RIGHT_ALT;
	static constexpr int32_t RIGHT_SUPER = GLFW_KEY_RIGHT_SUPER;

	// Special keys
	static constexpr int32_t ESCAPE = GLFW_KEY_ESCAPE;
	static constexpr int32_t ENTER = GLFW_KEY_ENTER;
	static constexpr int32_t TAB = GLFW_KEY_TAB;
	static constexpr int32_t BACKSPACE = GLFW_KEY_BACKSPACE;
	static constexpr int32_t INSERT = GLFW_KEY_INSERT;
	static constexpr int32_t DELETE = GLFW_KEY_DELETE;
	static constexpr int32_t HOME = GLFW_KEY_HOME;
	static constexpr int32_t END = GLFW_KEY_END;
	static constexpr int32_t PAGE_UP = GLFW_KEY_PAGE_UP;
	static constexpr int32_t PAGE_DOWN = GLFW_KEY_PAGE_DOWN;
	static constexpr int32_t CAPS_LOCK = GLFW_KEY_CAPS_LOCK;
	static constexpr int32_t SCROLL_LOCK = GLFW_KEY_SCROLL_LOCK;
	static constexpr int32_t NUM_LOCK = GLFW_KEY_NUM_LOCK;
	static constexpr int32_t PRINT_SCREEN = GLFW_KEY_PRINT_SCREEN;
	static constexpr int32_t PAUSE = GLFW_KEY_PAUSE;

	// Keypad (numpad) keys
	static constexpr int32_t KP_0 = GLFW_KEY_KP_0;
	static constexpr int32_t KP_1 = GLFW_KEY_KP_1;
	static constexpr int32_t KP_2 = GLFW_KEY_KP_2;
	static constexpr int32_t KP_3 = GLFW_KEY_KP_3;
	static constexpr int32_t KP_4 = GLFW_KEY_KP_4;
	static constexpr int32_t KP_5 = GLFW_KEY_KP_5;
	static constexpr int32_t KP_6 = GLFW_KEY_KP_6;
	static constexpr int32_t KP_7 = GLFW_KEY_KP_7;
	static constexpr int32_t KP_8 = GLFW_KEY_KP_8;
	static constexpr int32_t KP_9 = GLFW_KEY_KP_9;
	static constexpr int32_t KP_DECIMAL = GLFW_KEY_KP_DECIMAL;
	static constexpr int32_t KP_DIVIDE = GLFW_KEY_KP_DIVIDE;
	static constexpr int32_t KP_MULTIPLY = GLFW_KEY_KP_MULTIPLY;
	static constexpr int32_t KP_SUBTRACT = GLFW_KEY_KP_SUBTRACT;
	static constexpr int32_t KP_ADD = GLFW_KEY_KP_ADD;
	static constexpr int32_t KP_ENTER = GLFW_KEY_KP_ENTER;
	static constexpr int32_t KP_EQUAL = GLFW_KEY_KP_EQUAL;

	// Miscellaneous keys
	static constexpr int32_t SPACE = GLFW_KEY_SPACE;
	static constexpr int32_t APOSTROPHE = GLFW_KEY_APOSTROPHE;
	static constexpr int32_t COMMA = GLFW_KEY_COMMA;
	static constexpr int32_t MINUS = GLFW_KEY_MINUS;
	static constexpr int32_t PERIOD = GLFW_KEY_PERIOD;
	static constexpr int32_t SLASH = GLFW_KEY_SLASH;
	static constexpr int32_t SEMICOLON = GLFW_KEY_SEMICOLON;
	static constexpr int32_t EQUAL = GLFW_KEY_EQUAL;
	static constexpr int32_t LEFT_BRACKET = GLFW_KEY_LEFT_BRACKET;
	static constexpr int32_t BACKSLASH = GLFW_KEY_BACKSLASH;
	static constexpr int32_t RIGHT_BRACKET = GLFW_KEY_RIGHT_BRACKET;
	static constexpr int32_t GRAVE_ACCENT = GLFW_KEY_GRAVE_ACCENT;
	static constexpr int32_t WORLD_1 = GLFW_KEY_WORLD_1;
	static constexpr int32_t WORLD_2 = GLFW_KEY_WORLD_2;
}

namespace MouseButton {
	static constexpr int32_t LEFT = GLFW_MOUSE_BUTTON_1;
	static constexpr int32_t RIGHT = GLFW_MOUSE_BUTTON_2;
	static constexpr int32_t MIDDLE = GLFW_MOUSE_BUTTON_3;

	static constexpr int32_t BACK = GLFW_MOUSE_BUTTON_4;
	static constexpr int32_t FORWARD = GLFW_MOUSE_BUTTON_5;

	static constexpr int32_t EXTRA_1 = GLFW_MOUSE_BUTTON_6;
	static constexpr int32_t EXTRA_2 = GLFW_MOUSE_BUTTON_7;
	static constexpr int32_t EXTRA_3 = GLFW_MOUSE_BUTTON_8;
}