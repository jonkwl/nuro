#include "texture_inspectable.h"

#include <functional>

#include "../ui/components/im_components.h"

void TextureInspectable::renderStaticContent(ImDrawList& drawList)
{
    IMComponents::label("TEXTURE");
}

void TextureInspectable::renderDynamicContent(ImDrawList& drawList)
{

}