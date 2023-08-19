#pragma once
#include <cstdint>

namespace DiveBomber
{
#define EngineDirectory "..\\..\\DiveBomberCore\\"
#define EngineDirectoryW L"..\\..\\DiveBomberCore\\"

#define ProjectDirectory "..\\..\\..\\SampleProject\\"
#define ProjectDirectoryW L"..\\..\\..\\SampleProject\\"
}

namespace DiveBomber::Utility
{
	extern uint64_t g_FrameCounter;
	extern double g_DeltaTime;
	extern double g_EngineTime;
	extern double g_GameTime;
	extern float g_FramePerSnd;
	extern float g_TimerSpeed;
	extern double g_RawDeltaTime;
}