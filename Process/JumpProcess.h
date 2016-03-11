#pragma once

#include "EightEngine\Process\Process.h"

class GameCamera;

class PhysXCharacterPhysicsComponent;
typedef PhysXCharacterPhysicsComponent CharacterPhysicsComponent;

// Brief:
// Player can jump in any direction, if we jump in other direction than directly up,
// then angular movement is performed too, so player is rotating
// Input is disabled so player can not jump and move in the same time
// Camera or character can be destroyed(if we quiting level...) and process will be canceled 
class JumpProcess : public Process
{
public:
	JumpProcess(const std::shared_ptr<CharacterPhysicsComponent> &pPhysicsComponent,
		const std::shared_ptr<GameCamera> &pCamera, DirectX::FXMVECTOR jumpDir, 
		float jumpSpeed, float updateTime);
	~JumpProcess() { }

	// Every frame is physically computed new position,
	// Jump ends if we touch ground
	// We can bounce from walls
	void VUpdate(float deltaTime) override;

	// Enable input
	void VOnSucced() override;
	
	// Empty
	void VOnFailed() override { }
private:
	// Performs linear movement and if we jumped in less than 90 degrees then
	// angular movement is performed too(angular movement is without gravity part)
	void PerformMoving();

	// Player is bounced from wall, new falling path is computed
	void Bounce();

	std::weak_ptr<CharacterPhysicsComponent> m_pPhysicsComponent;
	std::weak_ptr<GameCamera> m_pCamera;

	DirectX::XMFLOAT3 m_InitialVelocity;
	DirectX::XMFLOAT3 m_Gravity;

	const float m_UpdateTime;
	float m_ElapsedTime;
	float m_TotalTime;

	bool m_UpdateAngularVelocity;
};