#pragma once

#include "EightEngine\GameView\MenuView.h"

class KulaWorldEndLevelMenuView : public MenuView
{
public:
	explicit KulaWorldEndLevelMenuView(std::weak_ptr<Actor> &pActor) : MenuView(pActor) { }
	~KulaWorldEndLevelMenuView() { }

	// Only concerned in Enter and LTM
	void VRegisterEvents() override;
	void VUnregisterEvents() override;

	// numStars of stars is converted from NotDone texture to done
	void ShowReward(UINT numStars);

private:
	// Going to menu
	void ActivateGuiElementEvent(const EventPtr<IEventData> &pEvent);

};
