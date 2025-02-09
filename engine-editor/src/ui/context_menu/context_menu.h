#pragma once

#include <string>

namespace ContextMenu
{

	// Begins a new context menu
	bool begin();

	// Ends the current context menu
	void end();

	// Pops context menu variations
	void pop();

	// Creates a new item within the current context menu
	bool item(const char* icon, std::string title);
	
	// Creates a new item without icon and margin within the current context menu
	bool itemLight(std::string title);

	// Creates a new menu within the current context menu
	bool menu(const char* icon, std::string title);

	// Ends the current menu
	void endMenu();

	// Draws a seperator
	void seperator();

};

