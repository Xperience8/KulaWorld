#include "stdafx.h"
#include "JumpProcess.h"
#include "EightEngine\Actor\Components\CharacterPhysicsComponent.h"
#include "EightEngine\EventManager\EventManager.h"
#include "EightEngine\EventManager\Events\JumpEventData.h"
#include "EightEngine\EventManager\Events\PhysCollisionEventData.h"
#include "EightEngine\GameView\GameCamera.h"


JumpProcess::JumpProcess(const std::shared_ptr<CharacterPhysicsComponent> &pPhysicsComponent,
	const std::shared_ptr<GameCamera> &pCamera, DirectX::FXMVECTOR jumpDir, float jumpSpeed, 
	float updateTime) : m_pPhysicsComponent(pPhysicsComponent), m_UpdateTime(updateTime), 
	m_pCamera(pCamera), m_TotalTime(0.f), m_ElapsedTime(0.f)
{
	DirectX::XMStoreFloat3(&m_InitialVelocity, DirectX::XMVectorScale(jumpDir, jumpSpeed));
	DirectX::XMStoreFloat3(&m_Gravity, DirectX::XMVectorScale(pCamera->GetUpDirection(), -9.81f));

	m_UpdateAngularVelocity = !DirectX::XMVector3Equal(jumpDir, pCamera->GetUpDirection());

	g_EventManager.TriggerEvent(EventPtr<EnableInputEventData>(false));
}
void JumpProcess::VUpdate(float deltaTime)
{
	if (m_pPhysicsComponent.expired())
	{
		m_ProcessState = ProcessState::FAILED;
	}
	else
	{
		m_ElapsedTime += deltaTime;
		if (m_ElapsedTime >= m_UpdateTime)
		{
			m_TotalTime += m_UpdateTime;
			m_ElapsedTime -= m_UpdateTime;

			PerformMoving();

			// We recompute initial velocity so player can bounce from walls
			if (m_pPhysicsComponent._Get()->IsSideCollision())
			{
				Bounce();
			}

			// Jump ends
			else if (m_pPhysicsComponent._Get()->IsDownSideCollision())
			{
				m_ProcessState = ProcessState::SUCCEDED;

				g_EventManager.PushEvent(EventPtr<PhysCollisionEventData>(
					m_pPhysicsComponent._Get()->GetActor()._Get(), nullptr));
			}
		}
	}
}
void JumpProcess::VOnSucced()
{
	g_EventManager.TriggerEvent(EventPtr<EnableInputEventData>(true));
}
void JumpProcess::PerformMoving()
{
	// Compute velocity: v = v0t + att
	DirectX::XMVECTOR acceleration = DirectX::XMVectorScale(DirectX::XMLoadFloat3(
		&m_Gravity), m_TotalTime * m_TotalTime);
	DirectX::XMVECTOR velocity = DirectX::XMVectorScale(DirectX::XMLoadFloat3(
		&m_InitialVelocity), m_TotalTime);

	velocity = DirectX::XMVectorScale(DirectX::XMVectorAdd(velocity, acceleration), m_UpdateTime);

	m_pPhysicsComponent._Get()->Move(velocity, m_UpdateTime);
	if (m_UpdateAngularVelocity)
	{
		// Gravity part from angular velocity is removed because if we falling too fast
		// then rotation looks unrealistic
		DirectX::XMVECTOR gravityPart = DirectX::XMVector3Normalize(acceleration);
		gravityPart = DirectX::XMVectorMultiply(gravityPart,
			DirectX::XMVector3Dot(velocity, gravityPart));
		DirectX::XMVECTOR angularVelocity = DirectX::XMVectorSubtract(velocity, gravityPart);

		m_pPhysicsComponent._Get()->MoveAngular(angularVelocity,
			m_pCamera._Get()->GetRightDirection());
	}
}
void JumpProcess::Bounce()
{
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationAxis(
		m_pCamera._Get()->GetRightDirection(), DirectX::XM_PI);

	DirectX::XMStoreFloat3(&m_InitialVelocity, DirectX::XMVector3TransformNormal(
		DirectX::XMLoadFloat3(&m_InitialVelocity), rotationMatrix));

	g_EventManager.PushEvent(EventPtr<PhysCollisionEventData>(
		m_pPhysicsComponent._Get()->GetActor()._Get(), nullptr));

	m_TotalTime = 0.f;
}