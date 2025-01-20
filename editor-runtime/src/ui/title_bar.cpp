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

    // Menu items
    std::array<const char*, 5> items = { "File", "Edit", "View", "Project", "Build" };

    // Draw title
    for (int i = 0; i < items.size(); i++) {
        auto [size, clicked] = menuItem(drawList, cursor, items[i]);
        cursor.x += size.x;
        cursor.x += 4.0f;
    }

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

inline ImVec2 TitleBar::labelPrimary(ImDrawList& drawList, ImVec2 position, const char* text)
{
    // Draw label
    float fontSize = style.primaryFont->FontSize;
    drawList.AddText(style.primaryFont, fontSize, position, style.primaryTextColor, text);

    // Calculate and return size
    ImVec2 size = style.primaryFont->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, text) + style.padding;
    return size;
}

inline ImVec2 TitleBar::labelSecondary(ImDrawList& drawList, ImVec2 position, const char* text)
{
    // Draw label
    float fontSize = style.secondaryFont->FontSize;
    drawList.AddText(style.secondaryFont, fontSize, position, style.secondaryTextColor, text);

    // Calculate and return size
    ImVec2 size = style.secondaryFont->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, text) + style.padding;
    return size;
}

std::tuple<ImVec2, bool> TitleBar::menuItem(ImDrawList& drawList, ImVec2 position, const char* text)
{
    // Calculate text size
    float fontSize = style.primaryFont->FontSize;
    ImVec2 textSize = style.primaryFont->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, text);

    // Calculate menu item geometry
    ImVec2 size = textSize + style.menuItemPadding * 2;
    ImVec2 p0 = position;
    ImVec2 p1 = p0 + size;

    // Evaluate interactions
    bool hovered = ImGui::IsMouseHoveringRect(p0, p1);
    bool clicked = hovered && ImGui::IsMouseClicked(0);

    // Evaluate color
    ImU32 color = style.menuItemColor;
    if (hovered) color = style.menuItemColorHovered;

    // Draw background
    drawList.AddRectFilled(p0, p1, color, style.menuItemRounding);

    // Draw text
    drawList.AddText(style.primaryFont, fontSize, p0 + style.menuItemPadding, style.primaryTextColor, text);

    // Return size and if menu item is clicked
    return std::make_tuple(size, clicked);
}
