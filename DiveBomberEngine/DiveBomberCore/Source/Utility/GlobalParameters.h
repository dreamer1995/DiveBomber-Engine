#pragma once
#include <cstdint>

namespace DiveBomber
{
#define EngineDirectory "..\\..\\DiveBomberCore\\"
#define EngineDirectoryW L"..\\..\\DiveBomberCore\\"

#define EngineTextureDirectory "..\\..\\DiveBomberCore\\EngineResource\\Texture\\"
#define EngineTextureDirectoryW L"..\\..\\DiveBomberCore\\EngineResource\\Texture\\"

#define EngineMaterialDirectory "..\\..\\DiveBomberCore\\EngineResource\\Material\\"
#define EngineMaterialDirectoryW L"..\\..\\DiveBomberCore\\EngineResource\\Material\\"

#define EngineShaderDirectory "..\\..\\DiveBomberCore\\EngineResource\\Shader\\"
#define EngineShaderDirectoryW L"..\\..\\DiveBomberCore\\EngineResource\\Shader\\"

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
	extern bool g_EnableEditorUI;
}