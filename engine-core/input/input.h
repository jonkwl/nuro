#pragma once

#include <cstdint>
#include <glm/glm.hpp>

namespace Input
{

	// Sets up input system
	void setup();

	// Fetches inputs of current frame
	void update();

	// Returns WASD input axis
	glm::vec2 moveAxis();

	// Returns mouse movement delta
	glm::vec2 mouseDelta();

	// Returns mouse scroll delta
	glm::vec2 scrollDelta();

	// Returns mouse position on screen
	glm::vec2 mousePosition();

	// Returns if given key is pressed
	bool keyDown(int32_t key);

	// Returns if given mouse button is pressed
	bool mouseDown(int32_t mouseButton);

	// Returns if given key and any control key is pressed
	bool command(int32_t key);

};

namespace Key {

    //
    // Letters
    //

    static constexpr int32_t A = 65; // ASCII value for 'A'
    static constexpr int32_t B = 66; // ASCII value for 'B'
    static constexpr int32_t C = 67; // ASCII value for 'C'
    static constexpr int32_t D = 68; // ASCII value for 'D'
    static constexpr int32_t E = 69; // ASCII value for 'E'
    static constexpr int32_t F = 70; // ASCII value for 'F'
    static constexpr int32_t G = 71; // ASCII value for 'G'
    static constexpr int32_t H = 72; // ASCII value for 'H'
    static constexpr int32_t I = 73; // ASCII value for 'I'
    static constexpr int32_t J = 74; // ASCII value for 'J'
    static constexpr int32_t K = 75; // ASCII value for 'K'
    static constexpr int32_t L = 76; // ASCII value for 'L'
    static constexpr int32_t M = 77; // ASCII value for 'M'
    static constexpr int32_t N = 78; // ASCII value for 'N'
    static constexpr int32_t O = 79; // ASCII value for 'O'
    static constexpr int32_t P = 80; // ASCII value for 'P'
    static constexpr int32_t Q = 81; // ASCII value for 'Q'
    static constexpr int32_t R = 82; // ASCII value for 'R'
    static constexpr int32_t S = 83; // ASCII value for 'S'
    static constexpr int32_t T = 84; // ASCII value for 'T'
    static constexpr int32_t U = 85; // ASCII value for 'U'
    static constexpr int32_t V = 86; // ASCII value for 'V'
    static constexpr int32_t W = 87; // ASCII value for 'W'
    static constexpr int32_t X = 88; // ASCII value for 'X'
    static constexpr int32_t Y = 89; // ASCII value for 'Y'
    static constexpr int32_t Z = 90; // ASCII value for 'Z'

    //
    // Numbers
    //

    static constexpr int32_t _0 = 48; // ASCII value for '0'
    static constexpr int32_t _1 = 49; // ASCII value for '1'
    static constexpr int32_t _2 = 50; // ASCII value for '2'
    static constexpr int32_t _3 = 51; // ASCII value for '3'
    static constexpr int32_t _4 = 52; // ASCII value for '4'
    static constexpr int32_t _5 = 53; // ASCII value for '5'
    static constexpr int32_t _6 = 54; // ASCII value for '6'
    static constexpr int32_t _7 = 55; // ASCII value for '7'
    static constexpr int32_t _8 = 56; // ASCII value for '8'
    static constexpr int32_t _9 = 57; // ASCII value for '9'

    //
    // Function keys
    //

    static constexpr int32_t F1 = 290;
    static constexpr int32_t F2 = 291;
    static constexpr int32_t F3 = 292;
    static constexpr int32_t F4 = 293;
    static constexpr int32_t F5 = 294;
    static constexpr int32_t F6 = 295;
    static constexpr int32_t F7 = 296;
    static constexpr int32_t F8 = 297;
    static constexpr int32_t F9 = 298;
    static constexpr int32_t F10 = 299;
    static constexpr int32_t F11 = 300;
    static constexpr int32_t F12 = 301;

    //
    // Arrow keys
    //

    static constexpr int32_t UP = 265;
    static constexpr int32_t DOWN = 264;
    static constexpr int32_t LEFT = 263;
    static constexpr int32_t RIGHT = 262;

    //
    // Modifier keys
    //

    static constexpr int32_t LEFT_SHIFT = 340;
    static constexpr int32_t LEFT_CONTROL = 341;
    static constexpr int32_t LEFT_ALT = 342;
    static constexpr int32_t LEFT_SUPER = 343;
    static constexpr int32_t RIGHT_SHIFT = 344;
    static constexpr int32_t RIGHT_CONTROL = 345;
    static constexpr int32_t RIGHT_ALT = 346;
    static constexpr int32_t RIGHT_SUPER = 347;

    //
    // Special keys
    //

    static constexpr int32_t ESCAPE = 256;
    static constexpr int32_t ENTER = 257;
    static constexpr int32_t TAB = 258;
    static constexpr int32_t BACKSPACE = 259;
    static constexpr int32_t INSERT = 260;
    static constexpr int32_t DELETE = 261;
    static constexpr int32_t HOME = 268;
    static constexpr int32_t END = 269;
    static constexpr int32_t PAGE_UP = 266;
    static constexpr int32_t PAGE_DOWN = 267;
    static constexpr int32_t CAPS_LOCK = 280;
    static constexpr int32_t SCROLL_LOCK = 281;
    static constexpr int32_t NUM_LOCK = 282;
    static constexpr int32_t PRINT_SCREEN = 283;
    static constexpr int32_t PAUSE = 284;

    //
    // Keypad (numpad) keys
    //

    static constexpr int32_t KP_0 = 320;
    static constexpr int32_t KP_1 = 321;
    static constexpr int32_t KP_2 = 322;
    static constexpr int32_t KP_3 = 323;
    static constexpr int32_t KP_4 = 324;
    static constexpr int32_t KP_5 = 325;
    static constexpr int32_t KP_6 = 326;
    static constexpr int32_t KP_7 = 327;
    static constexpr int32_t KP_8 = 328;
    static constexpr int32_t KP_9 = 329;
    static constexpr int32_t KP_DECIMAL = 330;
    static constexpr int32_t KP_DIVIDE = 331;
    static constexpr int32_t KP_MULTIPLY = 332;
    static constexpr int32_t KP_SUBTRACT = 333;
    static constexpr int32_t KP_ADD = 334;
    static constexpr int32_t KP_ENTER = 335;
    static constexpr int32_t KP_EQUAL = 336;

    //
    // Miscellaneous keys
    //

    static constexpr int32_t SPACE = 32;           // ASCII value for space
    static constexpr int32_t APOSTROPHE = 39;      // ASCII value for '\''
    static constexpr int32_t COMMA = 44;           // ASCII value for ','
    static constexpr int32_t MINUS = 45;           // ASCII value for '-'
    static constexpr int32_t PERIOD = 46;          // ASCII value for '.'
    static constexpr int32_t SLASH = 47;           // ASCII value for '/'
    static constexpr int32_t SEMICOLON = 59;       // ASCII value for ';'
    static constexpr int32_t EQUAL = 61;           // ASCII value for '='
    static constexpr int32_t LEFT_BRACKET = 91;    // ASCII value for '['
    static constexpr int32_t BACKSLASH = 92;       // ASCII value for '\\'
    static constexpr int32_t RIGHT_BRACKET = 93;   // ASCII value for ']'
    static constexpr int32_t GRAVE_ACCENT = 96;    // ASCII value for '`'
    static constexpr int32_t WORLD_1 = 161;        // Non-US #1
    static constexpr int32_t WORLD_2 = 162;        // Non-US #2

}

namespace MouseButton {
    
    static constexpr int32_t LEFT = 0;             // GLFW_MOUSE_BUTTON_1
    static constexpr int32_t RIGHT = 1;            // GLFW_MOUSE_BUTTON_2
    static constexpr int32_t MIDDLE = 2;           // GLFW_MOUSE_BUTTON_3

    static constexpr int32_t BACK = 3;             // GLFW_MOUSE_BUTTON_4
    static constexpr int32_t FORWARD = 4;          // GLFW_MOUSE_BUTTON_5

    static constexpr int32_t EXTRA_1 = 5;          // GLFW_MOUSE_BUTTON_6
    static constexpr int32_t EXTRA_2 = 6;          // GLFW_MOUSE_BUTTON_7
    static constexpr int32_t EXTRA_3 = 7;          // GLFW_MOUSE_BUTTON_8

}