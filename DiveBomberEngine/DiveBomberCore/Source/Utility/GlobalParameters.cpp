#include "GlobalParameters.h"

#include "..\..\Config\SystemConfig.h"

namespace DiveBomber::Utility
{
	uint64_t g_FrameCounter = 0u;
	double g_RawDeltaTime = 0.0f;
	double g_DeltaTime = 0.0;
	double g_EngineTime = 0.0f;
	double g_GameTime = 0.0;
	float g_FramePerSnd = 0.0f;
	float g_TimerSpeed = TimerSpeed;
	bool g_EnableEditorUI = EnableEditorUI;
}