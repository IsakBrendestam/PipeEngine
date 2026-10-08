#pragma once

#include "core/UniqueID.h"

#include "core/rendering/d3d11/camera/CameraD3D11.h"
#include "core/rendering/d3d11/camera/CameraOrthographicD3D11.h"

#include "core/rendering/PipelineManager.h"

#include "WindowManager.h"
#include "InputManager.h"

#include <vector>

class CameraManager 
{
public:
	CameraManager() = default;
	~CameraManager() = default;

	void Initialize();
	void Deconstruct();
	void Update();

	CameraD3D11* GenerateDefaultCamera() const;

	ID AddCamera(DX::XMFLOAT3 position, DX::XMFLOAT3 rotation, ProjectionInfo projection);
	ID AddCamera(CameraD3D11* camera);
	ID AddOrthographicCamera(DX::XMFLOAT3 position, DX::XMFLOAT3 rotation, float width, float height, ProjectionInfo projInfo);

	void RemoveCamera(ID id);

	void SetCurrentCamera(ID id);
	void SetControllCamera(ID id);

	size_t CameraCount();
	ID CurrentCameraId();
	ID ControllCameraId();

	std::vector<ID> GetCameraIds();
	CameraD3D11* GetCamera();
	ID GetCameraId();
	CameraD3D11* GetCamera(ID index);
	CameraD3D11* GetControllCamera();
	CameraD3D11* GetDefaultCamera();
	ID GetDefaultCameraId();

private:
	std::unordered_map <ID, CameraD3D11* > m_cameras;
	ID m_controllCameraId = INVALID_ID;
	ID m_currentCameraId = INVALID_ID;
	ID m_defaultCameraId = INVALID_ID;

	bool m_wasLocked = false;
	float m_mouseSensitivity = 2.0f;
};

extern CameraManager g_cameraManager;