#include "stdafx.h"
#include "SmoothCameraYawRotationProcess.h"
#include "GameView\KulaWorldCamera.h"

#include "EightEngine\EventManager\EventManager.h"
#include "EightEngine\EventManager\Events\JumpEventData.h"

SmoothCameraYawRotationProcess::SmoothCameraYawRotationProcess(float angleInRadians, 
	float time, const std::shared_ptr<GameCamera> &pCamera) : 
	m_pCamera(std::static_pointer_cast<KulaWorldCamera>(pCamera)),
	m_ActualYaw(0.f), m_DesiredAngle(angleInRadians), m_DesiredTime(time)
{
	g_EventManager.TriggerEvent(EventPtr<EnableInputEventData>(false));
}
void SmoothCameraYawRotationProcess::VUpdate(float deltaTime)
{
	if (m_pCamera.expired())
	{
		m_ProcessState = ProcessState::FAILED;
	}
	else
	{
		// Convert delta time from "per sec" to "per desired time"
		deltaTime /= m_DesiredTime;

		float angle = m_DesiredAngle * deltaTime;
		m_ActualYaw += angle;

		if (abs(m_ActualYaw) > abs(m_DesiredAngle))
		{
			// Because delta time doesn't stay every frame the same, it is a big chance,
			// that we rotate more than we want, so we have to offset last rotation 
			// so we can get desired angle
			angle = m_DesiredAngle - (m_ActualYaw - angle);

			m_ProcessState = ProcessState::SUCCEDED;
		}

		m_pCamera._Get()->Yaw(angle);
	}
}
void SmoothCameraYawRotationProcess::VOnSucced()
{
	if (!m_pChildProcess)
	{
		g_EventManager.TriggerEvent(EventPtr<EnableInputEventData>(true));
	}
}