#pragma once

#include "EightEngine\GameLogic\GameLogic.h"
#include "EightEngine\EventManager\IEventData.h"

class KulaWorldLogic : public BaseGameLogic
{
public:
	KulaWorldLogic() { }
	~KulaWorldLogic() { }

	// Event listeners are registered and level database is created
	void StartUp();
	
	void ShutDown();

	// Based on set state perform action
	void Update();

	// Actor is created and registered inside actor map
	// If actor has defined view(e.g player, menu etc.), then view is created,
	// initialized and linked with actor
	std::shared_ptr<IGameView> CreateGameView(
		UINT viewType, const std::shared_ptr<Actor> &pActor) override;

private:
	void StartNewGameEvent(const EventPtr<IEventData> &pEvent);

	void GoToMainMenuEvent(const EventPtr<IEventData> &pEvent);

	// Existing level is destroyed, audio is stopped, new level is loaded and started
	void StartLevelEvent(const EventPtr<IEventData> &pEvent);
	
	// New results are saved inside level database, new view with results is created,
	// so player can see his results, current level is paused, input is blocked,
	// and game is waitting for command to continue
	void WinLevelEvent(const EventPtr<IEventData> &eventData);

	// Current level is restarted
	void LooseLevelEvent(const EventPtr<IEventData> &eventData);

	// Collected actor is destroyed
	void CollectItemEvent(const EventPtr<IEventData> &eventData);
	
	// KulaWorldView is informed that it can change gravity
	void ChangeGravityEvent(const EventPtr<IEventData> &eventData);

	// Play collision sound
	void CollisionEvent(const EventPtr<IEventData> &eventData);
};