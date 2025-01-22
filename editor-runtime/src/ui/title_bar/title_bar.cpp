#include "title_bar.h"

#include "../core/utils/log.h"
#include "../core/input/cursor.h"
#include "../core/context/application_context.h"

#include "../src/runtime/runtime.h"
#include "../src/ui/IconsFontAwesome6.h"

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

        ImDrawList& drawList = *ImGui::GetForegroundDrawList();
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
    //
    // INVALID FONTS; RETURN
    // 

    if (!style.primaryFont || !style.secondaryFont || !style.workspaceBarFont) return;

    //
    // DRAW BACKGROUND
    //

    drawList.AddRectFilled(titleBarPosition, titleBarPosition + titleBarSize, style.backgroundColor);

    //
    // DRAW CONTROL BUTTONS
    //

    float controlButtonOffset = 36.0f;
    ImVec2 controlButtonPos = ImVec2(titleBarPosition.x + titleBarSize.x - 46.0f, titleBarPosition.y + style.padding.y);
    if (controlButton(drawList, controlButtonPos, ICON_FA_XMARK)) close();
    controlButtonPos.x -= controlButtonOffset;
    if (controlButton(drawList, controlButtonPos, ICON_FA_WINDOW_RESTORE)) flipMaximize();
    controlButtonPos.x -= controlButtonOffset;
    if (controlButton(drawList, controlButtonPos, ICON_FA_WINDOW_MINIMIZE)) minimize();

    ImVec2 cursor = titleBarPosition;

    //
    // DRAW ICON
    //

    if (style.iconTexture) {
        float iconVerticalPadding = (titleBarSize.y - style.iconSize.y) * 0.5f;
        cursor = ImVec2(titleBarPosition.x + style.padding.x * 0.5f, titleBarPosition.y + iconVerticalPadding);
        drawList.AddImage(style.iconTexture, cursor, cursor + style.iconSize, ImVec2(0, 1), ImVec2(1, 0));
    }

    //
    // CREATE DRAWING CURSOR
    // 

    cursor = ImVec2(titleBarPosition.x + style.iconSize.x + style.padding.x, titleBarPosition.y + style.padding.y);

    //
    // DRAW MENU
    //

    std::array<const char*, 5> items = { "File", "Edit", "View", "Project", "Build" };

    for (int i = 0; i < items.size(); i++) {
        auto [size, clicked] = menuItem(drawList, cursor, items[i]);
        cursor.x += size.x;
        cursor.x += 4.0f;
    }

    //
    // DRAW WORKSPACE BAR
    //

    placeWorkspaceBar(drawList);

    //
    // DRAW TOP BORDER
    //

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
    bool doubleClicked = hovered && ImGui::IsMouseDoubleClicked(0);
    glm::ivec2 mousePosition = Cursor::getScreenPosition();

    // Check for starting to move window
    if (!movingWindow && dragging) {
        movingWindow = true;

        // If window is maximized, make it smaller
        if (maximized()) {
            flipMaximize();
            ApplicationContext::setPosition(mousePosition - glm::ivec2(ApplicationContext::readConfiguration().windowSize.x * 0.5f, 0.0f));
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
                maximize();
            }
        }

    }

    // Maximize window if double clicked
    if (doubleClicked) {
        maximize();
    }

    // Cache last mouse position
    lastMousePosition = mousePosition;
}

bool TitleBar::controlButton(ImDrawList& drawList, ImVec2 position, const char* icon)
{
    // Calculate text size
    float fontSize = style.controlButtonSize;
    ImVec2 textSize = style.primaryFont->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, icon);

    // Calculate menu item geometry
    ImVec2 size = textSize + style.controlButtonPadding * 2;
    ImVec2 p0 = position;
    ImVec2 p1 = p0 + size;

    // Evaluate interactions
    bool hovered = ImGui::IsMouseHoveringRect(p0, p1);
    bool clicked = hovered && ImGui::IsMouseClicked(0);

    // Evaluate color
    ImU32 color = style.controlButtonColor;
    if (hovered) color = style.controlButtonColorHovered;

    // Draw background
    drawList.AddRectFilled(p0, p1, color, style.controlButtonRounding);

    // Draw text
    drawList.AddText(style.primaryFont, fontSize, p0 + style.controlButtonPadding, style.controlButtonTextColor, icon);

    return clicked;
}

ImVec2 TitleBar::labelPrimary(ImDrawList& drawList, ImVec2 position, const char* text)
{
    // Draw label
    float fontSize = style.primaryFont->FontSize;
    drawList.AddText(style.primaryFont, fontSize, position, style.primaryTextColor, text);

    // Calculate and return size
    ImVec2 size = style.primaryFont->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, text) + style.padding;
    return size;
}

ImVec2 TitleBar::labelSecondary(ImDrawList& drawList, ImVec2 position, const char* text)
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
    return { size, clicked };
}

void TitleBar::placeWorkspaceBar(ImDrawList& drawList)
{
    // Prepare workspace bar if needed
    if (!workspaceBar.evaluated) {
        // Calculate total size
        workspaceBar.size = ImVec2(0.0f, 0.0f);
        float fontSize = style.workspaceBarFont->FontSize;
        for (int i = 0; i < workspaceBar.items.size(); i++) {
            ImVec2 itemSize = style.workspaceBarFont->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, workspaceBar.items[i]) + style.workspaceItemPadding * 2.0f;
            workspaceBar.itemSizes[i] = itemSize;
            workspaceBar.size = itemSize + ImVec2(workspaceBar.size.x, 0.0f);
        }

        // Set workspace bar to prepared
        workspaceBar.evaluated = true;
    }

    // Calculate position (centered)
    ImVec2 position = titleBarPosition + (titleBarSize - workspaceBar.size) * 0.5f;

    // Get font size
    float fontSize = style.workspaceBarFont->FontSize;

    // Draw background
    drawList.AddRectFilled(position, position + workspaceBar.size, style.workspaceBackgroundColor, style.workspaceRounding);

    // Draw items
    ImVec2 cursor = position + style.workspaceItemPadding;
    for (int i = 0; i < workspaceBar.items.size(); i++) {
        // Evaluate selection
        bool selected = i == workspaceBar.selection;

        // Draw text
        ImVec2 textSize = style.workspaceBarFont->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, workspaceBar.items[i]);
        drawList.AddText(style.workspaceBarFont, fontSize, cursor, selected ? style.primaryTextColor : style.secondaryTextColor, workspaceBar.items[i]);

        // Evaluate item interactions
        ImVec2 itemSize = workspaceBar.itemSizes[i];
        ImVec2 p0 = cursor - style.workspaceItemPadding;
        ImVec2 p1 = p0 + itemSize;
        bool hovered = ImGui::IsMouseHoveringRect(p0, p1);

        // If hovered or selected, draw item background and redraw item text
        if (selected || hovered) {
            ImU32 color = style.workspaceItemColorHovered;
            if (selected) color = style.workspaceItemColorActive;
            drawList.AddRectFilled(p0, p1, color, style.workspaceItemRounding);
            drawList.AddText(style.workspaceBarFont, fontSize, cursor, selected ? style.primaryTextColor : style.secondaryTextColor, workspaceBar.items[i]);
        }

        // Advance cursor
        cursor.x += itemSize.x;
    }
}

void TitleBar::minimize()
{
    ApplicationContext::minimizeWindow();
}

void TitleBar::maximize()
{
    ApplicationContext::maximizeWindow();
}

bool TitleBar::maximized()
{
    return (ApplicationContext::readConfiguration().windowPosition.x == 0);
}

void TitleBar::flipMaximize()
{
    // Window is maximized, make it smaller
    if (maximized()) {
        glm::ivec2 windowSize = glm::ivec2(1600, 900);
        ApplicationContext::resizeWindow(windowSize);
    }
    // Window is not maximized, maximize it
    else {
        maximize();
    }
}

void TitleBar::close()
{
    // Stop application
    Runtime::TERMINATE();
}
