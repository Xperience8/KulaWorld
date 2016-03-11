#pragma once

#include "EightEngine\GameAppLayer\GameApp.h"
#include "GameLogic\KulaWorldLogic.h"
#include "EightEngine\EventManager\IEventData.h"

#include "EightEngine\GameLogic\GameTime.h"

class KulaWorldApp : public GameApp
{
public:
	KulaWorldApp() { }
	~KulaWorldApp() { }

	bool StartUp();
	void ShutDown();

	void Run();
private:

	void ExitGameRequest(const EventPtr<IEventData> &pEvent)
	{
		m_Running = false;
	}

	KulaWorldLogic m_GameLogic;

};