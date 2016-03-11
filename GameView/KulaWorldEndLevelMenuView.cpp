#include "stdafx.h"
#include "KulaWorldEndLevelMenuView.h"
#include "EightEngine\EventManager\EventManager.h"
#include "EightEngine\EventManager\Events\MouseMoveEventData.h"
#include "EightEngine\EventManager\Events\DestroyActorEventData.h"
#include "EightEngine\EventManager\Events\StartLevelEventData.h"
#include "EightEngine\Actor\Components\UIRenderComponent.h"
#include "EightEngine\Graphics\UserInterface\BackgroundImage.h"

void KulaWorldEndLevelMenuView::VRegisterEvents()
{
	g_EventManager.AddListener(EventListener(this, &KulaWorldEndLevelMenuView::ActivateGuiElementEvent),
		ActivateGuiElementEventData::s_TYPE);
	g_EventManager.AddListener(EventListener(this, &KulaWorldEndLevelMenuView::ActivateGuiElementEvent),
		ActivateGuiElementByMouseEventData::s_TYPE);

	g_EventManager.PushEvent(EventPtr<ShowMouseCursorEventData>(true));
}
void KulaWorldEndLevelMenuView::VUnregisterEvents()
{
	g_EventManager.RemoveListener(reinterpret_cast<UINT>(this),
		ActivateGuiElementEventData::s_TYPE);
	g_EventManager.RemoveListener(reinterpret_cast<UINT>(this),
		ActivateGuiElementByMouseEventData::s_TYPE);

	if (!m_pActor.expired())
	{
		g_EventManager.TriggerEvent(EventPtr<DestroyActorEventData>(
			m_pActor._Get()->GetActorID()));
	}
}
void KulaWorldEndLevelMenuView::ShowReward(UINT numStars)
{
	auto pUIComponent = m_pActor._Get()->GetActorComponent<UIRenderComponent>();
	auto guiElements = pUIComponent->GetUserInterface()->VGetGuiElements();

	// Trick is that if player completes level than he gets automatically one star
	// and from this one star, which is first, we get proper texture
	auto pStarDoneTexture = std::static_pointer_cast<BackgroundImage>(
		guiElements[2])->GetSprite()->GetTexture();

	for (UINT i = 1; i < numStars; ++i)
	{
		auto pImage = std::static_pointer_cast<BackgroundImage>(guiElements[2 + i]);
		pImage->GetSprite()->ChangeTexture(pStarDoneTexture);
	}
}
void KulaWorldEndLevelMenuView::ActivateGuiElementEvent(const EventPtr<IEventData> &pEvent)
{
	g_EventManager.PushEvent(EventPtr<StartLevelEventData>("MainMenuConfig.xml"));
}