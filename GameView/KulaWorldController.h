#pragma once

#include "EightEngine\GameView\PlayerController.h"
#include "EightEngine\EventManager\IEventData.h"

// Brief:
// KulaWorldController is especially created for Kula World
// It is concerned only in keyboard input, only in moving in one direction(z component),
// rotating(x component) and jumping
// We can disable input if we are rotating or jumping
class KulaWorldController : public PlayerController
{
public:
	KulaWorldController() : m_Enabled(true) { }
	~KulaWorldController() { }

	// Only concerned in:
	// Movement event
	// Jump event
	// Change input state event
	void RegisterEvents() override;

	void UnregisterEvents() override;

	// If we are jumping or rotating then input is blocked
	bool CanProcessInput() const
	{
		return m_Enabled;
	}
private:
	void MovementEvent(const EventPtr<IEventData> &pEvent);
	void ChangeInputState(const EventPtr<IEventData> &pEvent);

	bool m_Enabled;
};