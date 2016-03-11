#include "stdafx.h"
#include "KulaWorldController.h"
#include "EightEngine\EventManager\EventManager.h"
#include "EightEngine\EventManager\Events\MouseMoveEventData.h"
#include "EightEngine\EventManager\Events\JumpEventData.h"

void KulaWorldController::RegisterEvents()
{
	g_EventManager.AddListener(EventListener(this, &KulaWorldController::MovementEvent),
		MovementEventData::s_TYPE);
	g_EventManager.AddListener(EventListener(this, &KulaWorldController::JumpEvent),
		JumpEventData::s_TYPE);
	g_EventManager.AddListener(EventListener(this, &KulaWorldController::ChangeInputState),
		EnableInputEventData::s_TYPE);

	g_EventManager.PushEvent(EventPtr<ShowMouseCursorEventData>(false));
}
void KulaWorldController::UnregisterEvents()
{
	g_EventManager.RemoveListener(reinterpret_cast<UINT>(this), EnableInputEventData::s_TYPE);
	g_EventManager.RemoveListener(reinterpret_cast<UINT>(this), JumpEventData::s_TYPE);
	g_EventManager.RemoveListener(reinterpret_cast<UINT>(this), MovementEventData::s_TYPE);
}
void KulaWorldController::MovementEvent(const EventPtr<IEventData> &pEvent)
{
	auto pEventData = StaticPointerCast<MovementEventData>(pEvent);
	
	m_UnitMovingDirection = pEventData->GetMovementDirection();
}
void KulaWorldController::ChangeInputState(const EventPtr<IEventData> &pEvent)
{
	auto pEventData = StaticPointerCast<EnableInputEventData>(pEvent);

	m_Enabled = pEventData->IsJumpDisabled();
}