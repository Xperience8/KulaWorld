#pragma once

#include "EightEngine\Process\Process.h"

class GameCamera;
class KulaWorldCamera;

class PhysXCharacterPhysicsComponent;
typedef PhysXCharacterPhysicsComponent CharacterPhysicsComponent;


// Brief:
// SmoothCameraPitchRotationProcess is used to smoothly rotate camera to desired angle by desired time
// and slightly offsets character position
// Input is disabled so player can not rotate and move in the same time
// Camera or character can be destroyed(if we quiting level...) and process will be canceled 
// If we attach another process as child process unblocking input event will not be send!
// This way we can merge multiple rotation processes without undesired movements
class SmoothCameraPitchRotationProcess : public Process
{
public:
	// angleInRadians -> how much you want to rotate per time
	// time -> how long should rotation take
	// pCamera -> on which camera will be rotation performed
	// pPhysicsComponent -> which character will be slightly moved to new location
	// Input is disabled(if we rotating, we can not move)
	SmoothCameraPitchRotationProcess(float angleInRadians, float time,
		const std::shared_ptr<GameCamera> &pCamera,
		const std::shared_ptr<CharacterPhysicsComponent> &pPhysicsComponent);

	~SmoothCameraPitchRotationProcess() { }

	// Every frame is bit of movement applied to character
	// Every frame is a bit of rotation applied to camera
	// and if we come to desired rotation(or time -> this is the same moment)
	// then this process is succeded and it ends
	void VUpdate(float deltaTime) override;

	// Enable input and finalize rotation
	void VOnSucced() override;

	// Empty
	void VOnFailed() override { }
private:
	std::weak_ptr<KulaWorldCamera> m_pCamera;
	std::weak_ptr<CharacterPhysicsComponent> m_pPhysicsComponent;

	const float m_DesiredAngle;
	const float m_DesiredTime;

	float m_ActualPitch;
};