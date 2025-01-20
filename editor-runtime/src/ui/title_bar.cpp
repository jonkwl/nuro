#include "title_bar.h"

#include "../core/utils/log.h"
#include "../core/input/cursor.h"
#include "../core/context/application_context.h"

TitleBar::TitleBar() : style(),
titleBarPosition(ImVec2(0.0f, 0.0f)),
titleBarSize(ImVec2(0.0f, 0.0f)),
lastMousePosition(glm::ivec2(0, 0)),
movingWindow(false)
{
}

void TitleBar::render(const ImGuiViewport& viewport)
{
    titleBarPosition = viewport.Pos;
    titleBarSize = ImVec2(viewport.Size.x, style.height);

    ImGui::SetNextWindowPos(titleBarPosition);
    ImGui::SetNextWindowSize(titleBarSize);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    if (ImGui::Begin("TitleBar", nullptr, flags)) {

        ImDrawList& drawList = *ImGui::GetWindowDrawList();
        renderContent(drawList);

    }
    ImGui::End();
    ImGui::PopStyleVar();

    performDrag();
}

TitleBarStyle& TitleBar::getStyle()
{
	return style;
}

void TitleBar::renderContent(ImDrawList& drawList)
{
    // Invalid fonts
    if (!style.primaryFont || !style.secondaryFont) return;

    // Draw background
    drawList.AddRectFilled(titleBarPosition, titleBarPosition + titleBarSize, style.backgroundColor);

    ImVec2 cursor = titleBarPosition;

    // Draw icon
    if (style.iconTexture) {
        float iconVerticalPadding = (titleBarSize.y - style.iconSize.y) * 0.5f;
        cursor = ImVec2(titleBarPosition.x + style.padding.x * 0.5f, titleBarPosition.y + iconVerticalPadding);
        drawList.AddImage(style.iconTexture, cursor, cursor + style.iconSize, ImVec2(0, 1), ImVec2(1, 0));
    }

    // Move cursor
    cursor = ImVec2(titleBarPosition.x + style.iconSize.x + style.padding.x, titleBarPosition.y + style.padding.y);

    // Draw title
    labelPrimary(drawList, cursor, "File     Edit    View      Build      Project");

    // Draw top border
    ImVec2 borderStart = titleBarPosition;
    ImVec2 borderEnd = borderStart + ImVec2(titleBarSize.x, 0.0f);
    drawList.AddLine(borderStart, borderEnd, style.borderColor, style.borderThickness);
}

void TitleBar::performDrag()
{
    // Evaluate interaction
    ImVec2 zoneP0 = ImVec2(titleBarPosition.x, titleBarPosition.y);
    ImVec2 zoneP1 = titleBarPosition + titleBarSize;
    bool hovered = ImGui::IsMouseHoveringRect(zoneP0, zoneP1, false);
    bool mouseDown = ImGui::IsMouseDown(0);
    bool dragging = hovered && mouseDown;

    // Get mouse position on screen
    glm::ivec2 mousePosition = Cursor::getScreenPosition();

    // Check for starting to move window
    if (!movingWindow && dragging) {
        movingWindow = true;

        // Window is maximized, make it smaller
        if (ApplicationContext::readConfiguration().windowPosition.x == 0) {
            glm::ivec2 windowSize = glm::ivec2(1080, 600);
            ApplicationContext::resizeWindow(windowSize);
            ApplicationContext::setPosition(mousePosition - glm::ivec2(windowSize.x * 0.5f, 0.0f));
        }
    }

    // Move window if still moving
    if (movingWindow) {

        // Move window
        if (mouseDown) {
            glm::ivec2 mouseDelta = mousePosition - lastMousePosition;
            glm::ivec2 windowPosition = ApplicationContext::getPosition() + mouseDelta;
            ApplicationContext::setPosition(windowPosition);
        }
        // Stop moving window
        else {
            movingWindow = false;

            // Maximize window if moving stopped when mouse was at the top of screen
            if (mousePosition.y <= 10) {
                ApplicationContext::maximizeWindow();
            }
        }

    }

    // Cache last mouse position
    lastMousePosition = mousePosition;
}

inline void TitleBar::labelPrimary(ImDrawList& drawList, ImVec2 position, const char* text)
{
    drawList.AddText(style.primaryFont, style.primaryFont->FontSize, position, style.primaryTextColor, text);
}

inline void TitleBar::labelSecondary(ImDrawList& drawList, ImVec2 position, const char* text)
{
    drawList.AddText(style.secondaryFont, style.secondaryFont->FontSize, position, style.primaryTextColor, text);
}
