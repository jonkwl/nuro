#include "engine_dialog.h"

void EngineDialog::vec3_dialog(std::string name, glm::vec3& value, float min, float max) {
    ImGui::Begin(std::string("Vector3: " + name).c_str());

    ImGui::SliderFloat("X", &value.x, min, max);
    ImGui::SliderFloat("Y", &value.y, min, max);
    ImGui::SliderFloat("Z", &value.z, min, max);

    ImGui::End();
}

void EngineDialog::float_dialog(std::string name, float& value, float min, float max) {
    ImGui::Begin(std::string("Float: " + name).c_str());

    ImGui::SliderFloat(name.c_str(), &value, min, max);

    ImGui::End();
}

void EngineDialog::bool_dialog(std::string name, bool& value) {
    ImGui::Begin(std::string("Bool: " + name).c_str());

    ImGui::Checkbox(name.c_str(), &value);

    ImGui::End();
}

void EngineDialog::color_dialog(std::string name, glm::vec4& value) {
    ImGui::Begin(std::string("Color: " + name).c_str());

    ImGui::ColorPicker4(name.c_str(), (float*)&value);

    ImGui::End();
}

void EngineDialog::input_dialog(std::string name, std::vector<InputPair> inputs)
{
    ImGui::Begin(std::string("Float: " + name).c_str());

    for (int i = 0; i < inputs.size(); i++) {
        InputPair pair = inputs[i];
        ImGui::SliderFloat(pair.name.c_str(), &pair.floatValue, pair.sliderMin, pair.sliderMax);
    }

    ImGui::End();
}

void EngineDialog::show_diagnostics(int fps) {
    ImGui::Begin("Diagnostics");

    ImGui::LabelText("FPS", "%d", fps);

    ImGui::End();
}