#pragma once

#include "EightEngine\Process\Process.h"

class GameCamera;
class KulaWorldCamera;

// Brief:
// SmoothCameraYawRotationProcess is used to smoothly rotate camera to desired angle by desired time
// Input is disabled so player can not rotate and move in the same time
// Camera can be destroyed(if we quiting level...) and process will be canceled 
// If we attach another process as child process unblocking input event will not be send!
// This way we can merge multiple rotation processes without undesired movements
class SmoothCameraYawRotationProcess : public Process
{
public:
	// angleInRadians -> how much you want to rotate per time
	// time -> how long should rotation take
	// pCamera -> on which camera will be rotation performed
	// Input is disabled(if we rotating, we can not move)
	SmoothCameraYawRotationProcess(float angleInRadians, float time,
		const std::shared_ptr<GameCamera> &pCamera);

	~SmoothCameraYawRotationProcess() { }

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

	const float m_DesiredAngle;
	const float m_DesiredTime;

	float m_ActualYaw;
};