#pragma once

#include <vector>

#include "inspectable.h"

#include "../src/ui/windows/console_window.h"

class WelcomeInspectable : public Inspectable
{
public:
	WelcomeInspectable();

	void renderStaticContent(ImDrawList& drawList) override;
	void renderDynamicContent(ImDrawList& drawList) override;

private:
	// Snapshot of the engines console logs at instantiation of this welcome inspectable
	std::vector<ConsoleLog> consoleSnapshot;

	// Set if game was played through welcome inspectable already
	bool playedGame;
};