#pragma once

#include "EightEngine\GameView\PlayerView.h"
#include "EightEngine\EventManager\IEventData.h"


class PhysXCharacterPhysicsComponent;
typedef PhysXCharacterPhysicsComponent CharacterPhysicsComponent;

class KulaWorldView : public PlayerView
{
public:
	KulaWorldView(std::weak_ptr<Actor> &pActor) : PlayerView(pActor), m_CanChangeGravity(false) { }
	~KulaWorldView() { }

	// Create controller and camera
	void VOnRestore() override;

	// Update player position based on input controller, solve jumping requests, falling 
	// and changing gravity
	void VUpdate(float deltaTime) override;

	void ChangeGravity(bool status)
	{
		m_CanChangeGravity = status;
	}

	std::shared_ptr<GameCamera> &GetCamera()
	{
		return m_pCamera;
	}
private:
	// Move player forward, if player hits obstacle and changing gravity is enabled,
	// then new gravity is computed, set and camera rotation process is created
	void MoveForward(const std::shared_ptr<CharacterPhysicsComponent> &pPhysicsComponent);

	// We can jump in two ways, only up or forward jumping
	// Jumping direction is computed and new jumping process is created
	void PerformJump(const std::shared_ptr<CharacterPhysicsComponent> &pPhysicsComponent);

	// Applies gravity on player and if player is falling and he can change gravity, 
	// then new gravity is computed, set and new camera rotation process is created,
	// otherwise jumping process is created with zero initial velocity -> falling
	void ApplyGravity(const std::shared_ptr<CharacterPhysicsComponent> &pPhysicsComponent);

	const float UPDATE_TIME = 1.f / 60.f;
	bool m_CanChangeGravity;
};