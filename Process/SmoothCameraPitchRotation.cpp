#include "stdafx.h"
#include "SmoothCameraPitchRotationProcess.h"
#include "EightEngine\EventManager\EventManager.h"
#include "EightEngine\EventManager\Events\JumpEventData.h"

#include "GameView\KulaWorldCamera.h"
#include "GameView\KulaWorldController.h"

#include "EightEngine\Actor\Components\CharacterPhysicsComponent.h"

SmoothCameraPitchRotationProcess::SmoothCameraPitchRotationProcess(float angleInRadians, 
	float time, const std::shared_ptr<GameCamera> &pCamera,
	const std::shared_ptr<CharacterPhysicsComponent> &pPhysicsComponent) 
	: m_pCamera(std::static_pointer_cast<KulaWorldCamera>(pCamera)),
	m_pPhysicsComponent(pPhysicsComponent), m_ActualPitch(0.f), 
	m_DesiredAngle(angleInRadians), m_DesiredTime(time)
{
	g_EventManager.TriggerEvent(EventPtr<EnableInputEventData>(false));
}

void SmoothCameraPitchRotationProcess::VUpdate(float deltaTime)
{
	if (m_pCamera.expired() || m_pPhysicsComponent.expired())
	{
		m_ProcessState = ProcessState::FAILED;
	}
	else
	{
		// Convert delta time from "per sec" to "per desired time"
		deltaTime /= m_DesiredTime;

		float angle = m_DesiredAngle * deltaTime;
		m_ActualPitch += angle;

		if (abs(m_ActualPitch) > abs(m_DesiredAngle))
		{
			// Because delta time doesn't stay every frame the same, it is a big chance,
			// that we rotate more than we want, so we have to offset last rotation 
			// so we can get desired angle
			angle = m_DesiredAngle - (m_ActualPitch - angle);

			m_ProcessState = ProcessState::SUCCEDED;
		}

		m_pCamera._Get()->Pitch(angle);

		const float movingSpeed = 0.18f;
		const float gravityFactor = -movingSpeed * 1.5f;

		DirectX::XMVECTOR disp = DirectX::XMVectorScale(
			m_pCamera._Get()->GetLookingDirection(), movingSpeed * deltaTime);
		disp = DirectX::XMVectorAdd(disp, DirectX::XMVectorScale(
			m_pCamera._Get()->GetUpDirection(), gravityFactor * deltaTime));

		m_pPhysicsComponent._Get()->Move(disp, deltaTime);
		m_pPhysicsComponent._Get()->MoveAngular(disp, m_pCamera._Get()->GetRightDirection());
	}
}

void SmoothCameraPitchRotationProcess::VOnSucced()
{
	if (!m_pChildProcess)
	{
		g_EventManager.TriggerEvent(EventPtr<EnableInputEventData>(true));
	}
}