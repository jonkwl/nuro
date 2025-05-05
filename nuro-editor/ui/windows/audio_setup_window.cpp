#include "audio_setup_window.h"

#include <vector>
#include <string>

#include <audio/audio_context.h>
#include <context/application_context.h>

void AudioSetupWindow::render()
{
	std::string title = UIUtils::windowTitle("Audio Setup");
	ImGui::Begin(title.c_str(), nullptr, EditorFlag::fixed);
	{
		IMComponents::headline("Audio Setup", ICON_FA_VOLUME_LOW);

		AudioContext& audio = ApplicationContext::audioContext();
		const AudioDevice& device = audio.device();

		//
		// AUDIO CONTEXT
		//

		ImGui::Dummy(ImVec2(0.0f, 2.0f));

		IMComponents::label("Audio Context: ", EditorUI::getFonts().h4_bold);
		ImGui::SameLine();
		IMComponents::label("Running  " ICON_FA_SIGNAL, EditorUI::getFonts().h4_bold, IM_COL32(145, 255, 145, 200));

		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		//
		// DEVICE
		//

		std::vector<std::string> hardwareDevices = device.hardwareDevices();

		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 12.0f));

		IMComponents::label("Device   " ICON_FA_HEADPHONES, EditorUI::getFonts().h4_bold);
		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		bool deviceOpened = device.opened();
		IMComponents::flagLabel("Device opened", device.opened(), true);
		ImGui::Dummy(ImVec2(0.0f, 5.0f));

		if (deviceOpened) {
			IMComponents::label("Hardware devices:", EditorUI::getFonts().p_bold);
			for (std::string hardwareDevice : hardwareDevices) {
				IMComponents::label("   - " + hardwareDevice);
			}
			ImGui::Dummy(ImVec2(0.0f, 5.0f));

			IMComponents::label("Used device: " + device.usedHardware(), EditorUI::getFonts().p_bold);
		}

		ImGui::Dummy(ImVec2(0.0f, 15.0f));

		//
		// 3D AUDIO
		//

		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 12.0f));

		IMComponents::label("3D Audio   " ICON_FA_CUBE, EditorUI::getFonts().h4_bold);
		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		bool hrtf = audio.hrtfAvailable();
		IMComponents::flagLabel("HRTF", hrtf, true);
		ImGui::Dummy(ImVec2(0.0f, 4.0f));

		if (hrtf) {
			IMComponents::label("HRTF functions:", EditorUI::getFonts().p_bold);
			for (std::string hrtfFunction : audio.hrtfFunctions()) {
				IMComponents::label("   - " + hrtfFunction);
			}
			IMComponents::label(ICON_FA_ARROW_RIGHT " Using hrtf function \"" + audio.usedHrtf() + "\"", EditorUI::getFonts().s);
		}
		else {
			IMComponents::label(ICON_FA_TRIANGLE_EXCLAMATION " HRTF is not available, your system may not provide any hrtf functions.", IM_COL32(255, 255, 0, 180));
		}
		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		IMComponents::flagLabel(ICON_FA_EAR_LISTEN " Stereo Angles", audio.stereoAnglesAvailable(), true);
		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		IMComponents::flagLabel(" Multichannel Formats", audio.multichannelFmtAvailable(), true);
		IMComponents::label(ICON_FA_ARROW_RIGHT " Quad, 5.1 Surround, 6.1 Surround, 7.1 Surround", EditorUI::getFonts().s);
		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		IMComponents::flagLabel(" B-Format", audio.bFmtAvailable(), true);
		IMComponents::label(ICON_FA_ARROW_RIGHT " B-Format 1D, B-Format 2D, B-Format 3D", EditorUI::getFonts().s);
		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		IMComponents::flagLabel(" B-Format Higher Order Ambisonics", audio.bHoaFmtAvailable(), true);
		IMComponents::label(ICON_FA_ARROW_RIGHT " B-Format HOA 4th Order, B-Format HOA 5th Order, B-Format HOA 6th Order", EditorUI::getFonts().s);
		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::Dummy(ImVec2(0.0f, 15.0f));

		//
		// EFFECTS
		//

		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 12.0f));

		IMComponents::label("Effects   " ICON_FA_WAND_MAGIC_SPARKLES, EditorUI::getFonts().h4_bold);
		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		IMComponents::flagLabel("Environmental Effects", audio.effectsAvailable(), true);

		ImGui::Dummy(ImVec2(0.0f, 15.0f));

		//
		// DATA
		//

		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 12.0f));

		IMComponents::label("Data   " ICON_FA_WAVE_SQUARE, EditorUI::getFonts().h4_bold);
		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		IMComponents::flagLabel("Float32 Format", audio.floatFmtAvailable(), true);
		IMComponents::label(ICON_FA_ARROW_RIGHT " 32-bit floating-point audio formats", EditorUI::getFonts().s);

		ImGui::Dummy(ImVec2(0.0f, 15.0f));

	}
	ImGui::End();
}