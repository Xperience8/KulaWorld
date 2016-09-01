#pragma once

#include "EightEngine\GameView\GameCamera.h"

class KulaWorldCamera : public GameCamera
{
public:
	explicit KulaWorldCamera(float offsetAngle) : m_OffsetFromLook(offsetAngle)
	{
		SetOffsetFromActor(DirectX::XMVectorSet(0.f, 0.7f, -0.7f, 0.f));
	}
	~KulaWorldCamera() { }

	// Compute new camera position and rotate it by yaw and pitch
	// Compute view matrix and frustum
	void VUpdate(DirectX::FXMVECTOR actorPosition) override
	{
		DirectX::XMFLOAT3 oldUp = m_Up;
		DirectX::XMFLOAT3 oldLook = m_Look;

		DirectX::XMMATRIX rotationMatrixPitch = DirectX::XMMatrixRotationAxis(
			DirectX::XMLoadFloat3(&m_Right), m_OffsetFromLook);

		DirectX::XMStoreFloat3(&m_Look, DirectX::XMVector3TransformNormal(
			DirectX::XMLoadFloat3(&m_Look), rotationMatrixPitch));
		DirectX::XMStoreFloat3(&m_Up, DirectX::XMVector3TransformNormal(
			DirectX::XMLoadFloat3(&m_Up), rotationMatrixPitch));




		DirectX::XMStoreFloat3(&m_Position, DirectX::XMVectorSubtract(actorPosition,
			DirectX::XMLoadFloat3(&m_OffsetFromActor)));

		GameCamera::Update();




		DirectX::XMStoreFloat3(&m_Up, DirectX::XMLoadFloat3(&oldUp));
		DirectX::XMStoreFloat3(&m_Look, DirectX::XMLoadFloat3(&oldLook));
	}

	void Yaw(float angleInRadians)
	{
		DirectX::XMMATRIX rotationMatrixYaw = DirectX::XMMatrixRotationAxis(
			DirectX::XMLoadFloat3(&m_Up), angleInRadians);

		DirectX::XMStoreFloat3(&m_Right, DirectX::XMVector3TransformNormal(
			DirectX::XMLoadFloat3(&m_Right), rotationMatrixYaw));
		DirectX::XMStoreFloat3(&m_Look, DirectX::XMVector3TransformNormal(
			DirectX::XMLoadFloat3(&m_Look), rotationMatrixYaw));

		DirectX::XMStoreFloat3(&m_OffsetFromActor, DirectX::XMVector3TransformCoord(
			DirectX::XMLoadFloat3(&m_OffsetFromActor), rotationMatrixYaw));
	}
	void Pitch(float angleInRadians)
	{
		DirectX::XMMATRIX rotationMatrixPitch = DirectX::XMMatrixRotationAxis(
			DirectX::XMLoadFloat3(&m_Right), angleInRadians);

		DirectX::XMStoreFloat3(&m_Look, DirectX::XMVector3TransformNormal(
			DirectX::XMLoadFloat3(&m_Look), rotationMatrixPitch));
		DirectX::XMStoreFloat3(&m_Up, DirectX::XMVector3TransformNormal(
			DirectX::XMLoadFloat3(&m_Up), rotationMatrixPitch));

		DirectX::XMStoreFloat3(&m_OffsetFromActor, DirectX::XMVector3TransformCoord(
			DirectX::XMLoadFloat3(&m_OffsetFromActor), rotationMatrixPitch));
	}
private:
	const float m_OffsetFromLook;
};

