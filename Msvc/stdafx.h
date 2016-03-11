// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <windowsx.h>
#include <wrl/client.h>

// C++ RunTime Header Files
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <memory>
#include <unordered_map>
#include <list>
#include <stack>
#include <array>
#include <queue>
#include <functional>

// Multithreaded
#include <ppl.h>
#include <ppltasks.h>

// XML
#include "EightEngine\3rdParty\XML\rapidxml.hpp"
#include "EightEngine\3rdParty\XML\rapidxml_iterators.hpp"
#include "EightEngine\3rdParty\XML\rapidxml_print.hpp"
#include "EightEngine\3rdParty\XML\rapidxml_utils.hpp"

#pragma comment(lib, "d3d11.lib")

// DirectX
#include <d3d11_2.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <DirectXColors.h>

// XAudio2
#include <xaudio2.h>
#pragma comment(lib, "XAudio2.lib")

// PhysX

#if defined _WIN64
#if defined _DEBUG
#pragma comment(lib, "PhysX3DEBUG_x64.lib")
#pragma comment(lib, "PhysX3CommonDEBUG_x64.lib")
#pragma comment(lib, "PhysX3ExtensionsDEBUG.lib")
#pragma comment(lib, "PhysXVisualDebuggerSDKDEBUG.lib")
#pragma comment(lib, "PhysX3CharacterKinematicDEBUG_x64.lib")
#else
#pragma comment(lib, "PhysX3_x64.lib")
#pragma comment(lib, "PhysX3Common_x64.lib")
#pragma comment(lib, "PhysX3Extensions.lib")
#pragma comment(lib, "PhysXVisualDebuggerSDK.lib")
#pragma comment(lib, "PhysX3CharacterKinematic_x64.lib")
#endif
#elif defined WIN32
#if defined _DEBUG
#pragma comment(lib, "PhysX3DEBUG_x86.lib")
#pragma comment(lib, "PhysX3CommonDEBUG_x86.lib")
#pragma comment(lib, "PhysX3ExtensionsDEBUG.lib")
#pragma comment(lib, "PhysXVisualDebuggerSDKDEBUG.lib")
#pragma comment(lib, "PhysX3CharacterKinematicDEBUG_x86.lib")
#else
#pragma comment(lib, "PhysX3_x86.lib")
#pragma comment(lib, "PhysX3Common_x86.lib")
#pragma comment(lib, "PhysX3Extensions.lib")
#pragma comment(lib, "PhysXVisualDebuggerSDK.lib")
#pragma comment(lib, "PhysX3CharacterKinematic_x86.lib")
#endif
#endif


#include <PhysX/PxPhysicsAPI.h>

// Engine
#pragma comment(lib, "EightEngine.lib")

#include "EightEngine\Debug\Log.h"
#include "EightEngine\GameAppLayer\Helper.h"
