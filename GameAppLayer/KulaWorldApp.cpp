#include "stdafx.h"
#include "KulaWorldApp.h"

#include "EightEngine\EventManager\EventManager.h"
#include "EightEngine\Process\ProcessManager.h"
#include "EightEngine\ResourceCache\ResourceCache.h"
#include "EightEngine\String\LocalizationMap.h"
#include "EightEngine\Graphics\ApiWrapper\Renderer.h"
#include "EightEngine\GameAppLayer\Screen.h"
#include "EightEngine\Audio\AudioManager.h"
#include "EightEngine\GameLogic\GamePhysics.h"
#include "EightEngine\GameAppLayer\InputManager.h"
#include "EightEngine\Memory\MemoryManager.h"

#include "EightEngine\EventManager\Events\ExitGameEventData.h"

bool KulaWorldApp::StartUp()
{
	if (!CheckOneInstance("KulaWorld"))
	{
		EIGHT_WARNING("Instance of this game is allready running, therefore this instance will be closed");
		return false;
	}
	// Starts independent subsystems
	g_EventManager.StartUp();
	g_ProcessManager.StartUp();
	g_ResourceCache.StartUp();

	// Load Config file for configuring game and other subsystems
	if (!g_ResourceCache.LoadResource("KulaWorldConfig.xml", XmlResourceLoader::s_SIGNATURE))
	{
		EIGHT_ERROR("There is no basic config: KulaWorldConfig.xml which is needed for configuring game");
		return false;
	}
	m_pConfig = g_ResourceCache.RequestResource<XmlDocument>("KulaWorldConfig.xml");
	auto configNode = m_pConfig->GetRootNode();

	// Load and initialize string map
	if (!g_LocalizationMap.Init(configNode.GetChildrenNode("Language")))
	{
		return false;
	}

	// Check whether this PC can run this game without problems with performance
	if (!CheckRequirements(configNode.GetChildrenNode("Requirements")))
	{
		return false;
	}

	// Create and initialize screen and link it with renderer as render target(back buffer)
	auto pScreen = std::make_shared<Screen>();
	if (!pScreen->Init(configNode.GetChildrenNode("Renderer")))
	{
		return false;
	}

	// Initialize renderer and register events
	if (!g_Renderer.StartUp(configNode.GetChildrenNode("Renderer"), pScreen))
	{
		return false;
	}

	// Initialize audio
	if (!g_AudioManager.StartUp())
	{
		return false;
	}

	// Initialize physics
	if (!g_Physics.StartUp())
	{
		return false;
	}

	// Initialize InputManager
	g_InputManager.StartUp(pScreen);

	// Start game logic
	m_GameLogic.StartUp();

	// Register event listener for exit request
	g_EventManager.AddListener(EventListener(this, &KulaWorldApp::ExitGameRequest),
		ExitGameEventData::s_TYPE);

	m_Running = true;

	return true;
}
void KulaWorldApp::ShutDown()
{
	g_EventManager.RemoveListener(reinterpret_cast<UINT>(this),
		ExitGameEventData::s_TYPE);

	m_pConfig->SaveToFile("KulaWorldConfig.xml");
	m_pConfig.reset();

	m_GameLogic.ShutDown();
	g_InputManager.ShutDown();
	g_Physics.ShutDown();
	g_AudioManager.ShutDown();
	g_Renderer.ShutDown();
	g_ResourceCache.ShutDown();
	g_ProcessManager.ShutDown();
	g_EventManager.ShutDown();
}

void KulaWorldApp::Run()
{
	while (m_Running)
	{
		g_InputManager.Update();

		g_EventManager.Update();

		m_GameLogic.Update();

		g_Renderer.Render();
	}
}


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
#if defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	{
		KulaWorldApp gameApp;
		if (!gameApp.StartUp())
		{
			return 1;
		}
		gameApp.Run();

		gameApp.ShutDown();
	}

#if defined(_DEBUG)
	s_MemoryManager.PrintStatistics();
#endif
	return 0;
}