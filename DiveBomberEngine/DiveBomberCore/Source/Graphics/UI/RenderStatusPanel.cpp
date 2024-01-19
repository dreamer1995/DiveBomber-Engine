#include "RenderStatusPanel.h"

#include "..\..\Utility\GlobalParameters.h"

#include <..\imgui\imgui.h>
#include <chrono>

namespace DiveBomber::UI
{
	using namespace Utility;
	using namespace std::chrono;
	void RenderStatusPanel::DrawUI()
	{
		if (isShown)
		{
			std::string captionChar = GetCaption() + (id == 1 ? "" : " " + std::to_string(id));
			ImGui::Begin(captionChar.c_str(), &isShown);
			ImGui::Text("Frame Rate: %.2f fps", g_FramePerSnd);
			ImGui::Text("Frame Time: %.2f ms", g_RawDeltaTime * 1000);
			ImGui::Text("Total Frames: %lu", g_FrameCounter);
			long long llTime = (long long)g_GameTime;
			auto hou = duration_cast<hours>((seconds)llTime).count();
			auto min = duration_cast<minutes>((seconds)llTime).count() % 60;
			auto sec = llTime % 60;
			ImGui::Text("Total Time: %d h : %d m : %d s", hou, min, sec);
			ImGui::End();
		}
	}
}