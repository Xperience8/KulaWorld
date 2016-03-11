#include "stdafx.h"
#include "KulaWorldView.h"
#include "KulaWorldController.h"
#include "KulaWorldCamera.h"
#include "EightEngine\Actor\Components\CharacterPhysicsComponent.h"
#include "EightEngine\Process\Process.h"
#include "EightEngine\Process\ProcessManager.h"
#include "EightEngine\EventManager\EventManager.h"
#include "EightEngine\EventManager\Events\JumpEventData.h"
#include "EightEngine\Actor\Components\TransformComponent.h"
#include "Process\SmoothCameraPitchRotationProcess.h"
#include "Process\SmoothCameraYawRotationProcess.h"
#include "Process\JumpProcess.h"

void KulaWorldView::VOnRestore()
{
	m_pController = std::make_unique<KulaWorldController>();

	m_pCamera = std::make_shared<KulaWorldCamera>(DirectX::XM_PIDIV4);
	m_pCamera->SetOffsetFromActor(DirectX::XMFLOAT3(0.f, -0.35f, 0.35f));
}
void KulaWorldView::VUpdate(float deltaTime)
{
	auto pTransformComponent = m_pActor._Get()->GetActorComponent<TransformComponent>();
	m_pCamera->VUpdate(pTransformComponent->GetWorldMatrix().r[3]);

	static float elapsedTime = 0.f;

	elapsedTime += deltaTime;
	if (elapsedTime >= UPDATE_TIME)
	{
		elapsedTime -= UPDATE_TIME;

		if (reinterpret_cast<KulaWorldController*>(m_pController.get())->CanProcessInput())
		{
			auto pPhysicsComponent = m_pActor._Get()->GetActorComponent<CharacterPhysicsComponent>();

			DirectX::XMFLOAT3 &unitMovDir = m_pController->GetUnitMovingDirection();

			// Player wants to move forward
			if (unitMovDir.z == 1.f)
			{
				MoveForward(pPhysicsComponent);
			}
			// Player wants to rotate
			if (unitMovDir.x != 0.f)
			{
				g_ProcessManager.AttachProcess(std::static_pointer_cast<Process>(
					std::make_shared<SmoothCameraYawRotationProcess>(
					DirectX::XM_PIDIV2 * unitMovDir.x, 0.25f, m_pCamera)));
			}
			// Player wants to jump
			else if (m_pController->Jump())
			{
				PerformJump(pPhysicsComponent);
			}

			ApplyGravity(pPhysicsComponent);
		}
	}
}
void KulaWorldView::MoveForward(const std::shared_ptr<CharacterPhysicsComponent> &pPhysicsComponent)
{
	const float SPEED_SCALE_FACTOR = 0.75f;
	DirectX::XMVECTOR force = DirectX::XMVectorScale(
		m_pCamera->GetLookingDirection(), SPEED_SCALE_FACTOR * UPDATE_TIME);

	pPhysicsComponent->Move(force, UPDATE_TIME);
	pPhysicsComponent->MoveAngular(force, m_pCamera->GetRightDirection());

	// If we have a side collision and we are inside trigger in which we can change gravity
	if (m_CanChangeGravity && pPhysicsComponent->IsSideCollision())
	{
		pPhysicsComponent->ChangeGravity(DirectX::XMVectorScale(m_pCamera->GetLookingDirection(), 9.81f));

		g_ProcessManager.AttachProcess(std::static_pointer_cast<Process>(
			std::make_shared<SmoothCameraPitchRotationProcess>(-DirectX::XM_PIDIV2, 
			0.5f, m_pCamera, pPhysicsComponent)));
	}
}
void KulaWorldView::PerformJump(const std::shared_ptr<CharacterPhysicsComponent> &pPhysicsComponent)
{
	DirectX::XMVECTOR jumpDir = m_pCamera->GetUpDirection();

	// If we move forward and then we want to jump in forward direction,
	// otherwise we jump directly up
	if (m_pController->GetUnitMovingDirection().z == 1.f)
	{
		jumpDir = DirectX::XMVector3TransformNormal(jumpDir,
			DirectX::XMMatrixRotationAxis(m_pCamera->GetRightDirection(),
			DirectX::XMConvertToRadians(8.f)));
	}

	g_ProcessManager.AttachProcess(std::static_pointer_cast<Process>(
		std::make_shared<JumpProcess>(pPhysicsComponent, m_pCamera, jumpDir, 7.f, UPDATE_TIME)));
}
void KulaWorldView::ApplyGravity(const std::shared_ptr<CharacterPhysicsComponent> &pPhysicsComponent)
{
	// We are applying gravity and if this method returns true, applied gravity is too
	// big so we are falling and we create jumping process with zero initial speed
	// what is actually falling
	if (pPhysicsComponent->ApplyGravity(UPDATE_TIME))
	{
		g_ProcessManager.AttachProcess(std::static_pointer_cast<Process>(std::make_shared<
			JumpProcess>(pPhysicsComponent, m_pCamera, m_pCamera->GetUpDirection(), 0.f, UPDATE_TIME)));
	}

	// If we haven't a down side collision and we are inside trigger in which we can change gravity
	if (!pPhysicsComponent->IsDownSideCollision() && m_CanChangeGravity)
	{
		DirectX::XMVECTOR gravity = DirectX::XMVectorScale(m_pCamera->GetLookingDirection(), -9.81f);
		pPhysicsComponent->ChangeGravity(gravity);

		g_ProcessManager.AttachProcess(std::static_pointer_cast<Process>(std::make_shared <
			SmoothCameraPitchRotationProcess >(DirectX::XM_PIDIV2, 0.5f, m_pCamera, pPhysicsComponent)));
	}
}