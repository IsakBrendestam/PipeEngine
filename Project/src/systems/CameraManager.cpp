#include "stdafx.h"

#include "CameraManager.h"
#include "core/EngineSettings.h"

CameraManager g_cameraManager;

void CameraManager::Initialize()
{
	// Default Camera
	m_defaultCameraId = UniqueID::GenerateID();
	m_controllCameraId = m_defaultCameraId;
	m_currentCameraId = m_defaultCameraId;
	m_cameras[m_defaultCameraId] = GenerateDefaultCamera();
}

void CameraManager::Deconstruct()
{
	for (auto& it : m_cameras)
		delete it.second;
}

void CameraManager::Update()
{
	ZoneScopedNC("Camera Update", RandomUniqueColor());

	float moveSpeed = 10.0f;
	const float rotSpeed = 1.5f;

	float deltaTime = g_time.DeltaTime<float>();

	if (InputManager::GetKeyState(SDLK_LSHIFT))
		moveSpeed *= 4;

	if (InputManager::GetKeyState(SDLK_LALT))
		moveSpeed /= 4;

	if (InputManager::GetKeyState(SDLK_a))
		m_cameras[m_controllCameraId]->MoveRight(-moveSpeed * deltaTime);

	if (InputManager::GetKeyState(SDLK_d))
		m_cameras[m_controllCameraId]->MoveRight(moveSpeed * deltaTime);

	if (InputManager::GetKeyState(SDLK_w))
		m_cameras[m_controllCameraId]->MoveForward(moveSpeed * deltaTime);

	if (InputManager::GetKeyState(SDLK_s))
		m_cameras[m_controllCameraId]->MoveForward(-moveSpeed * deltaTime);

	if (InputManager::GetKeyState(SDLK_SPACE))
		m_cameras[m_controllCameraId]->MoveUp(moveSpeed * deltaTime);

	if (InputManager::GetKeyState(SDLK_LCTRL))
		m_cameras[m_controllCameraId]->MoveUp(-moveSpeed * deltaTime);


	MouseState ms = InputManager::GetMouseState();
	if (g_engineSettings.cameraMouseMovement || ms.rightButtonDown)
	{
		InputManager::LockMouse();
		m_cameras[m_controllCameraId]->RotateRight(ms.deltaY * deltaTime * m_mouseSensitivity);
		m_cameras[m_controllCameraId]->RotateUp(ms.deltaX * deltaTime * m_mouseSensitivity);
		m_wasLocked = true;
	}
	else
	{
		if (m_wasLocked)
		{
			InputManager::UnlockMouse();
			m_wasLocked = false;
		}
	}

	if (InputManager::GetKeyState(SDLK_UP))
		m_cameras[m_controllCameraId]->RotateRight(-rotSpeed * deltaTime);

	if (InputManager::GetKeyState(SDLK_DOWN))
		m_cameras[m_controllCameraId]->RotateRight(rotSpeed * deltaTime);

	if (InputManager::GetKeyState(SDLK_LEFT))
		m_cameras[m_controllCameraId]->RotateUp(-rotSpeed * deltaTime);

	if (InputManager::GetKeyState(SDLK_RIGHT))
		m_cameras[m_controllCameraId]->RotateUp(rotSpeed * deltaTime);

	if (InputManager::GetKeyState(SDLK_e))
		m_cameras[m_controllCameraId]->RotateForward(-rotSpeed * deltaTime);

	if (InputManager::GetKeyState(SDLK_q))
		m_cameras[m_controllCameraId]->RotateForward(rotSpeed * deltaTime);

	m_cameras[m_controllCameraId]->UpdateInternalConstantBuffer();
}

CameraD3D11* CameraManager::GenerateDefaultCamera() const
{
	const ProjectionInfo defaultProjection {
		.fovAngleY = DX::XM_PIDIV4,
		.aspectRatio = float(g_engineSettings.renderResolution[0]) / float(g_engineSettings.renderResolution[1]),
		.nearZ = 0.01f,
		.farZ = 175.0f
	};

	return new CameraD3D11(defaultProjection, { 0, 4, -10 });
}

ID CameraManager::AddCamera(DX::XMFLOAT3 position, DX::XMFLOAT3 rotation, ProjectionInfo projection)
{
	ID id = UniqueID::GenerateID();
	m_cameras[id] = new CameraD3D11(projection, position, rotation);
	return id;
}

ID CameraManager::AddCamera(CameraD3D11* camera)
{
	ID id = UniqueID::GenerateID();
	m_cameras[id] = camera;
	return id;
}

ID CameraManager::AddOrthographicCamera(DX::XMFLOAT3 position, DX::XMFLOAT3 rotation, float width, float height, ProjectionInfo projInfo)
{
	ID id = UniqueID::GenerateID();
	m_cameras[id] = new CameraOrthographicD3D11(projInfo, width, height, position, rotation);
	return id;
}

void CameraManager::RemoveCamera(ID id)
{
	if (m_defaultCameraId == id)
		return;

	// Fallback if deleting current camera
	if (m_currentCameraId == id)
		m_currentCameraId = m_defaultCameraId;
	if (m_controllCameraId == id)
		m_controllCameraId = m_defaultCameraId;

	delete m_cameras[id];
	m_cameras.erase(id);
}

void CameraManager::SetCurrentCamera(ID id)
{
	if (m_cameras.find(id) != m_cameras.end())
		m_currentCameraId = id;
}

void CameraManager::SetControllCamera(ID id)
{
	if (m_cameras.find(id) != m_cameras.end())
		m_controllCameraId = id;
}

size_t CameraManager::CameraCount()
{
	return m_cameras.size();
}

ID CameraManager::CurrentCameraId()
{
	return m_currentCameraId;
}

ID CameraManager::ControllCameraId()
{
	return m_controllCameraId;
}

std::vector<ID> CameraManager::GetCameraIds()
{
    std::vector<ID> values;
    values.reserve(m_cameras.size());
    for (auto kv : m_cameras)
        values.push_back(kv.first);
    return values;
}

CameraD3D11* CameraManager::GetCamera()
{
	return m_cameras[m_currentCameraId];
}

ID CameraManager::GetCameraId()
{
	return m_currentCameraId;
}

CameraD3D11* CameraManager::GetCamera(ID id)
{
	return m_cameras[id];
}

CameraD3D11* CameraManager::GetControllCamera()
{
	return m_cameras[m_controllCameraId];
}

CameraD3D11* CameraManager::GetDefaultCamera()
{
	return m_cameras[m_defaultCameraId];
}

ID CameraManager::GetDefaultCameraId()
{
	return m_defaultCameraId;
}
