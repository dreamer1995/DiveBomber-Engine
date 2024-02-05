#pragma once

namespace DiveBomber::Config
{
// Render Settings
#define MainWindowTitle L"DiveBomber Engine"
#define MainWindowWidth 1600
#define MainWindowHeight 900
#define UseWARP false
#define VSync false;
#define SwapChainBufferCount 3
#define ClearMainRTColor { 1.0f, 0.0f, 1.0f, 1.0f }

#define TimerSpeed 1.0f

#define EnableTAA true

// Memory Settings
#define BindlessRendering true

// Miscellaneous Settings
#define ThrowLanguage LANG_ENGLISH
#define EnableConsole true
#define EditorMode true
#define EnableEditorUI true

#define RefreshAllResources false
}
