#include "stdafx.h"
#include "Colliders.h"

namespace DX = DirectX;
using namespace DirectX;
using namespace Collisions;

Collisions::Collider::Collider(ColliderTypes type, const std::string &name, ColliderTags tag):
	colliderType(type), m_toCheck(true),
#ifdef DEBUG_BUILD
	debug(false), _debugMeshName(name), 
#endif
	_min(0, 0, 0), _max(0, 0, 0), 
	_isDirty(true), _tagFlag((uint8_t)tag), _active(true), m_isIntersecting(false)
#ifdef DEBUG_BUILD
	, haveDebugEnt(false)
#endif
{

}

Collisions::Collider::~Collider()
{
#ifdef DEBUG_BUILD
	delete _debugEnt;
#endif
}

#ifdef USE_IMGUI
bool Collisions::Collider::RenderUI()
{
	return true;
}
#endif

static Scene *_scene;

#ifdef DEBUG_BUILD
bool Collisions::Collider::SetDebugEnt(Scene* scene, const Content* content)
{
	/*
	const UINT
		meshID = content->GetMeshID(_debugMeshName),
		textureID = content->GetTextureID("Tex_Green"),
		normalID = CONTENT_NULL,
		specularID = CONTENT_NULL,
		reflectiveID = CONTENT_NULL,
		ambientID = content->GetTextureID("Tex_Green"),
		heightID = CONTENT_NULL,
		samplerID = CONTENT_NULL;

	if (_debugEnt)
		delete _debugEnt;

	_debugEnt = new Object(-1, BoundingOrientedBox({ 0, 0, 0 }, { 1, 1, 1 }, { 0, 0, 0, 1 }));
	Object *obj = reinterpret_cast<Object *>(_debugEnt);

	if (!obj->Initialize(device, nullptr, "Maxwell Marker", meshID, textureID, normalID, specularID, reflectiveID, ambientID, heightID, samplerID))
	{
		ErrMsg("Failed to initialize Maxwell marker object!");
	}
	*/

	_scene = scene;

	UINT meshID = content->GetMeshID(_debugMeshName);
	Material mat;
	mat.textureID = content->GetTextureID("Tex_Green");
	mat.ambientID = content->GetTextureID("Tex_Green");
	mat.vsID = content->GetShaderID("VS_Geometry");

	if (!scene->CreateGlobalMeshEntity(&_debugEnt, "Collider Wireframe", meshID, mat))
	{
		ErrMsg("Failed to create collider debug object!");
		return false;
	}
	//debug = true;
	haveDebugEnt = true;

	return true;
}

bool Collisions::Collider::RenderDebug(Scene* scene, const RenderQueuer &queuer, const RendererInfo &renderInfo)
{
	if (debug)
	{
		DebugDrawer *debugDraw = scene->GetDebugDrawer();
		debugDraw->DrawLineThreadSafe(_min, _max, 0.01f, { 1, 0, 1, 1 });

		if (_debugEnt)
			return _debugEnt->InitialRender(queuer, renderInfo);
	}
	return true;
}
#endif

void Collisions::Collider::AddIntersectionCallback(std::function<void(const CollisionData &)> callback)
{
	_intersectionCallbacks.push_back(callback);
}

void Collisions::Collider::AddOnCollisionEnterCallback(std::function<void(const CollisionData&)> callback)
{
	_onCollisionEnterCallbacks.push_back(callback);
}

void Collisions::Collider::AddOnCollisionExitCallback(std::function<void()> callback)
{
	_onCollisionExitCallbacks.push_back(callback);
}

void Collisions::Collider::Intersection(const CollisionData &data) const
{
	for (auto &callback : _intersectionCallbacks)
		callback(data);
}

void Collisions::Collider::OnCollisionEnter(const CollisionData &data) const
{
	for (auto &callback : _onCollisionEnterCallbacks)
		callback(data);
}

void Collisions::Collider::OnCollisionExit() const
{
	for (auto &callback : _onCollisionExitCallbacks)
		callback();
}

DirectX::XMFLOAT3 Collisions::Collider::GetMin() const
{
	return _min;
}

DirectX::XMFLOAT3 Collisions::Collider::GetMax() const
{
	return _max;
}

void Collisions::Collider::SetTag(ColliderTags tag)
{
	_tagFlag |= (int)tag;
}

void Collisions::Collider::RemoveTag(ColliderTags tag)
{
	_tagFlag &= ~(int)tag;
}

bool Collisions::Collider::HasTag(ColliderTags tag) const
{
	return (_tagFlag & (int)tag) == (int)tag;
}

uint8_t Collisions::Collider::GetTag() const
{
	return _tagFlag;
}

void Collisions::Collider::SetActive(bool status)
{
	_active = status;
}

bool Collisions::Collider::GetActive() const
{
	return _active;
}

void Collisions::Collider::SetDirty()
{
	_isDirty = true;
	m_toCheck = true;
}

bool Collisions::Collider::GetDirty() const
{
	return _isDirty;
}

void Collisions::Collider::SetIntersecting(bool value)
{
	m_isIntersecting = value;
}

bool Collisions::Collider::GetIntersecting() const
{
	return m_isIntersecting;
}

void Collisions::Collider::CheckDone()
{
	m_toCheck = false;
}

void Collisions::Collider::SetToCheck()
{
	m_toCheck = true;
}

bool Collisions::Collider::GetCheck() const
{
	return _active && m_toCheck;
}

bool Collisions::Collider::Serialize(std::string *code) const
{
	return true;
}

bool Collisions::Collider::Deserialize(const std::string &code)
{
	return true;
}


/********************************************
*			 RAY COLLIDER					*
*********************************************/

Collisions::Ray::Ray(const Ray &other):
	Ray(other.origin, other.dir, other.length, (ColliderTags)other._tagFlag)
{
}

Collisions::Ray::Ray(const DirectX::XMFLOAT3 &o, const DirectX::XMFLOAT3 &d, float l, ColliderTags tag):
	Collider(RAY_COLLIDER, "Mesh_WireframeCube", tag), origin(o), dir(d), length(l)
{
}

#ifdef USE_IMGUI
bool Collisions::Ray::RenderUI()
{
	ImGui::Text("Ray Collider");
	ImGui::Separator();

	// TODO: Fix rotation

	XMVECTOR o = XMLoadFloat3(&origin),
			 d = XMLoadFloat3(&dir);

	float minValues[3] = { -100, -100, -100 },
		  maxValues[3] = { 100, 100, 100 },
		  speed = 0.001f;

	_isDirty |= ImGui::DragScalarN("Origon", ImGuiDataType_Float, o.m128_f32, 3, speed, minValues, maxValues, "%.3f");
	_isDirty |= ImGui::DragScalarN("Direction", ImGuiDataType_Float, d.m128_f32, 3, speed, minValues, maxValues, "%.3f");
	_isDirty |= ImGui::DragScalar("Lengt", ImGuiDataType_Float, &length, speed, minValues, maxValues, "%.3f");

	ImGui::Separator();

	XMStoreFloat3(&origin, o);
	XMStoreFloat3(&dir, XMVector3Normalize(d));

	return true;

}
#endif

#ifdef DEBUG_BUILD
bool Collisions::Ray::RenderDebug(Scene *scene, const RenderQueuer &queuer, const RendererInfo &renderInfo)
{
	if (!debug)
		return true;

	DebugDrawer *debugDraw = scene->GetDebugDrawer();
	XMFLOAT3 max;
	XMStoreFloat3(&max, XMVectorAdd(Load(origin), XMVectorScale(Load(dir), length)));
	debugDraw->DrawLineThreadSafe(origin, max, 0.01f, { 0, 1, 0, 1 });

	return true;
}
void Collisions::Ray::DebugRayRender(Scene *scene, XMFLOAT4 color) const
{
	DebugDrawer *debugDraw = scene->GetDebugDrawer();
	XMFLOAT3 max;
	XMStoreFloat3(&max, XMVectorAdd(Load(origin), XMVectorScale(Load(dir), length)));
	debugDraw->DrawLineThreadSafe(origin, max, 0.01f, color);
}
#endif

void Collisions::Ray::Transform(DirectX::XMMATRIX worldMatrix, Collider *transformed)
{
	if (!transformed)
		return;
	if (transformed->colliderType != RAY_COLLIDER)
		return;

	Ray *transformedRay = dynamic_cast<Ray *>(transformed);

	// Transform center position
	DX::XMStoreFloat3(&transformedRay->origin, XMVector3Transform(XMLoadFloat3(&origin), worldMatrix));

	XMVECTOR axisVec = XMLoadFloat3(&dir);
	axisVec = XMVector3Normalize(XMVector3TransformNormal(axisVec, worldMatrix));
	DX::XMStoreFloat3(&transformedRay->dir, axisVec);

	// Scale Half-Lenghts
	XMFLOAT3 scale;
    scale.x = XMVectorGetX(XMVector3Length(worldMatrix.r[0]));
    scale.y = XMVectorGetX(XMVector3Length(worldMatrix.r[1]));
    scale.z = XMVectorGetX(XMVector3Length(worldMatrix.r[2]));

	// TODO: scale this length
	transformedRay->length = length;
	
	XMVECTOR startPoint = XMLoadFloat3(&transformedRay->origin),
			 endPoint = XMVectorAdd(startPoint, XMVectorScale(axisVec, transformedRay->length));


	DX::XMStoreFloat3(&transformedRay->_min, XMVectorMin(startPoint, endPoint));
	DX::XMStoreFloat3(&transformedRay->_max, XMVectorMax(startPoint, endPoint));

	_isDirty = false;
	m_toCheck = true;
	transformed->SetToCheck();
}

#ifdef DEBUG_BUILD
bool Collisions::Ray::TransformDebug(ID3D11DeviceContext* context, Time &time, const Input &input)
{
	return true;
}
#endif


/********************************************
*		   SPHERE COLLIDER					*
*********************************************/

Collisions::Sphere::Sphere(const Sphere &other):
	Sphere(other.center, other.radius, (ColliderTags)other._tagFlag)
{
}

Collisions::Sphere::Sphere(const DirectX::XMFLOAT3& c, float r, ColliderTags tag):
	Collider(SPHERE_COLLIDER, "Mesh_WireframeSphere", tag), center(c), radius(r)
{
}

Collisions::Sphere::Sphere(const DirectX::BoundingSphere& sphere, ColliderTags tag):
	Collider(SPHERE_COLLIDER, "Mesh_WireframeSphere", tag), center(sphere.Center), radius(sphere.Radius)
{
}

#ifdef USE_IMGUI
bool Collisions::Sphere::RenderUI()
{
	ImGui::Text("Sphere Collider");
	ImGui::Separator();

	XMVECTOR c = XMLoadFloat3(&center);

	float minValues[3] = { -100, -100, -100 },
		  maxValues[3] = { 100, 100, 100 },
		  speed = 0.001f;

	_isDirty |= ImGui::DragScalarN("Center", ImGuiDataType_Float, c.m128_f32, 3, speed, minValues, maxValues, "%.3f");
	_isDirty |= ImGui::DragScalar("Radius", ImGuiDataType_Float, &radius, speed, &minValues[0], &maxValues[0], "%.3f");

	ImGui::Separator();

	XMStoreFloat3(&center, c);

	return true;
}
#endif

void Collisions::Sphere::Transform(DirectX::XMMATRIX worldMatrix, Collider *transformed)
{
	if (!transformed)
		return;
	if (transformed->colliderType != SPHERE_COLLIDER)
		return;

	Sphere *transformedSphere = dynamic_cast<Sphere *>(transformed);

	// Translation
	DX::XMStoreFloat3(&transformedSphere->center, XMVector3Transform(XMLoadFloat3(&center), worldMatrix));

	// Scaling
	XMFLOAT3 scale;
    scale.x = XMVectorGetX(XMVector3Length(worldMatrix.r[0])) * radius;
    scale.y = XMVectorGetX(XMVector3Length(worldMatrix.r[1])) * radius;
    scale.z = XMVectorGetX(XMVector3Length(worldMatrix.r[2])) * radius;

	transformedSphere->radius = fabsf(fmaxf(scale.x, fmaxf(scale.y, scale.z)));

	// Update Min and Max
	transformedSphere->_min = { transformedSphere->center.x - transformedSphere->radius,
							    transformedSphere->center.y - transformedSphere->radius,
							    transformedSphere->center.z - transformedSphere->radius };

	transformedSphere->_max = { transformedSphere->center.x + transformedSphere->radius,
							    transformedSphere->center.y + transformedSphere->radius,
							    transformedSphere->center.z + transformedSphere->radius };

	_isDirty = false;
	m_toCheck = true;
	transformed->SetToCheck();
}

#ifdef DEBUG_BUILD
bool Collisions::Sphere::TransformDebug(ID3D11DeviceContext* context, Time &time, const Input &input)
{
	if (!debug || !_debugEnt)
		return true;

	_debugEnt->GetTransform()->SetPosition(XMFLOAT3A(center.x,
													 center.y, 
													 center.z));

	_debugEnt->GetTransform()->SetScale(XMFLOAT4A(radius, radius, radius, 0));

	if (!_debugEnt->GetTransform()->UpdateConstantBuffer(context))
	{
		ErrMsg("Failed to set world matrix buffer!");
		return false;
	}

	if (!_debugEnt->InitialUpdate(time, input))
	{
		ErrMsg("Failed to set world matrix buffer!");
		return false;
	}
	return true;
}
#endif


/********************************************
*		   CAPSULE COLLIDER					*
*********************************************/

Collisions::Capsule::Capsule(const Capsule &other):
	Capsule(other.center, other.upDir, other.radius, other.height, (ColliderTags)other._tagFlag)
{
}

Collisions::Capsule::Capsule(const DirectX::XMFLOAT3 &c, const DirectX::XMFLOAT3 &u, float r, float h, ColliderTags tag):
	Collider(CAPSULE_COLLIDER, "Mesh_WireframeCapsule", tag), center(c), upDir(u), radius(r), height(h)
{
	if (radius * 2 > height)
		radius = height / 2;
}

#ifdef USE_IMGUI
bool Collisions::Capsule::RenderUI()
{
	ImGui::Text("Capsule Collider");
	ImGui::Separator();

	XMVECTOR c = XMLoadFloat3(&center);
	XMVECTOR u = XMLoadFloat3(&upDir);

	float minValues[3] = { -100, -100, -100 },
		  maxValues[3] = { 100, 100, 100 },
		  speed = 0.001f;

	_isDirty |= ImGui::DragScalarN("Center", ImGuiDataType_Float, c.m128_f32, 3, speed, minValues, maxValues, "%.3f");
	_isDirty |= ImGui::DragScalarN("Up Direction", ImGuiDataType_Float, u.m128_f32, 3, speed, minValues, maxValues, "%.3f");
	_isDirty |= ImGui::DragScalar("Height", ImGuiDataType_Float, &height, speed, 0, &maxValues[0], "%.3f");
	_isDirty |= ImGui::DragScalar("Radius", ImGuiDataType_Float, &radius, speed, 0, &maxValues[0], "%.3f");

	ImGui::Separator();

	u = XMVector3Normalize(u);
	if (XMVectorGetX(XMVector3LengthSq(u)) == 0)
		u = XMVectorSet(0, 1, 0, 0); // Set default up
	XMStoreFloat3(&upDir, u);
	XMStoreFloat3(&center, c);

	return true;
}
#endif

void Collisions::Capsule::Transform(DirectX::XMMATRIX worldMatrix, Collider *transformed)
{
	if (!transformed)
		return;
	if (transformed->colliderType != CAPSULE_COLLIDER)
		return;

	Capsule *transformedCapsule = dynamic_cast<Capsule *>(transformed);

	// Translation
	DX::XMStoreFloat3(&transformedCapsule->center, XMVector3Transform(XMLoadFloat3(&center), worldMatrix));

	// TODO: Transform rotation of up vector
    XMVECTOR axisVec = XMLoadFloat3(&upDir);
	axisVec = XMVector3TransformNormal(axisVec, worldMatrix);
	DX::XMStoreFloat3(&transformedCapsule->upDir, XMVector3Normalize(axisVec));


	// Scaling
	XMFLOAT3 scale;
    scale.x = XMVectorGetX(XMVector3Length(worldMatrix.r[0])) * radius;
    scale.y = XMVectorGetX(XMVector3Length(worldMatrix.r[1])) * height;
    scale.z = XMVectorGetX(XMVector3Length(worldMatrix.r[2])) * radius;

	transformedCapsule->radius = fabsf(fmaxf(scale.x, scale.z));
	transformedCapsule->height = fabsf(scale.y);

	// Update Min and Max
	XMVECTOR normal1 = XMVector3Normalize(XMLoadFloat3(&transformedCapsule->upDir)),
			 lineEndOffset1 = XMVectorScale(normal1, (transformedCapsule->height / 2) - transformedCapsule->radius),
			 A1 = XMVectorAdd(XMLoadFloat3(&transformedCapsule->center), lineEndOffset1),
			 B1 = XMVectorSubtract(XMLoadFloat3(&transformedCapsule->center), lineEndOffset1);

	XMVECTOR p1 = XMVectorAdd(A1, XMVectorScale(XMVectorSet(1, 1, 1, 0), transformedCapsule->radius));
	XMVECTOR p2 = XMVectorSubtract(B1, XMVectorScale(XMVectorSet(-1, -1, -1, 0), transformedCapsule->radius));
	XMVECTOR p3 = XMVectorAdd(A1, XMVectorScale(XMVectorSet(-1, -1, -1, 0), transformedCapsule->radius));
	XMVECTOR p4 = XMVectorSubtract(B1, XMVectorScale(XMVectorSet(1, 1, 1, 0), transformedCapsule->radius));

	DX::XMStoreFloat3(&transformedCapsule->_min, XMVectorMin(XMVectorMin(XMVectorMin(p1, p2), p3), p4));
	DX::XMStoreFloat3(&transformedCapsule->_max, XMVectorMax(XMVectorMax(XMVectorMax(p1, p2), p3), p4));

	_isDirty = false;
	m_toCheck = true;
	transformed->SetToCheck();
}

#ifdef DEBUG_BUILD
bool Collisions::Capsule::TransformDebug(ID3D11DeviceContext *context, Time &time, const Input &input)
{
	if (!_debugEnt || !debug)
		return true;

	// Translation
	_debugEnt->GetTransform()->SetPosition(XMFLOAT4A(center.x,
													 center.y, 
													 center.z, 
													 0));

	// Rotation
	XMVECTOR defaultUp = XMVectorSet(0, 1, 0, 0);
	XMVECTOR transformedUp = XMVector3Normalize(XMLoadFloat3(&upDir));

	// Compute rotation axis
	XMVECTOR rotationAxis = XMVector3Cross(defaultUp, transformedUp);
	float dot = XMVectorGetX(XMVector3Dot(defaultUp, transformedUp));

	// Handle parallel vectors 
	if (XMVector3Equal(defaultUp, transformedUp)) 
	{
		_debugEnt->GetTransform()->SetRotation({ 0, 0, 0, 1 });
	}
	else if (dot <= -1.0f)
	{
		rotationAxis = XMVectorSet(0, 1, 0, 0);
		XMVECTOR rotationQuat = XMQuaternionRotationAxis(rotationAxis, XM_PI);
		XMFLOAT4A quaternion;
		XMStoreFloat4(&quaternion, rotationQuat);
		_debugEnt->GetTransform()->SetRotation(quaternion);
	}
	else
	{
		// Compute rotation angle
		float angle = acosf(dot); // Angle between defaultUp and transformedUp

		// Compute quaternion from axis-angle representation
		XMVECTOR rotationQuat = XMQuaternionRotationAxis(rotationAxis, angle);

		// Store and apply rotation
		XMFLOAT4A quaternion;
		XMStoreFloat4(&quaternion, rotationQuat);
		_debugEnt->GetTransform()->SetRotation(quaternion);
	}

	// Scaling
	_debugEnt->GetTransform()->SetScale(XMFLOAT3A(radius, height/2, radius));

	if (!_debugEnt->GetTransform()->UpdateConstantBuffer(context))
	{
		ErrMsg("Failed to set world matrix buffer!");
		return false;
	}

	if (!_debugEnt->InitialUpdate(time, input))
	{
		ErrMsg("Failed to set world matrix buffer!");
		return false;
	}
	return true;
}
#endif


/********************************************
*		      OBB COLLIDER					*
*********************************************/

Collisions::OBB::OBB(const OBB &other):
	OBB(other.center, other.halfLength, other.axes, (ColliderTags)other._tagFlag)
{
}

Collisions::OBB::OBB(const DirectX::XMFLOAT3 &c, const DirectX::XMFLOAT3 &hl, const DirectX::XMFLOAT3 a[3], ColliderTags tag):
    Collider(OBB_COLLIDER, "Mesh_WireframeCube", tag),	center(c), halfLength(hl)
{
	for (int i = 0; i < 3; i++)
		axes[i] = a[i];
}

Collisions::OBB::OBB(const BoundingOrientedBox &obb, ColliderTags tag):
	Collider(OBB_COLLIDER, "Mesh_WireframeCube", tag), center(obb.Center), halfLength(obb.Extents)
{
	XMVECTOR orientation = XMLoadFloat4(&obb.Orientation);

	DX::XMStoreFloat3(&axes[0], XMVector3Rotate({1, 0, 0}, orientation));
	DX::XMStoreFloat3(&axes[1], XMVector3Rotate({ 0, 1, 0 }, orientation));
	DX::XMStoreFloat3(&axes[2], XMVector3Rotate({0, 0, 1}, orientation));
}

Collisions::OBB::OBB(const AABB &aabb, ColliderTags tag):
	Collider(OBB_COLLIDER, "Mesh_WireframeCube", tag), center(aabb.center), halfLength(aabb.halfLength)
{
	axes[0] = { 1, 0, 0 };
	axes[1] = { 0, 1, 0 };
	axes[2] = { 0, 0, 1 };

	_min = { center.x - halfLength.x, center.y - halfLength.y, center.z - halfLength.z };
	_max = { center.x + halfLength.x, center.y + halfLength.y, center.z + halfLength.z };
}

#ifdef USE_IMGUI
bool Collisions::OBB::RenderUI()
{
	ImGui::Text("OBB Collider");
	ImGui::Separator();

	// TODO: Fix rotation

	XMVECTOR c = XMLoadFloat3(&center),
			 hl = XMLoadFloat3(&halfLength),
			 a[3] = { XMLoadFloat3(&axes[0]),
					  XMLoadFloat3(&axes[1]),
				      XMLoadFloat3(&axes[2]) };

	float minValues[3] = { -100, -100, -100 },
		  maxValues[3] = { 100, 100, 100 },
		  speed = 0.001f;

	_isDirty |= ImGui::DragScalarN("Center", ImGuiDataType_Float, c.m128_f32, 3, speed, minValues, maxValues, "%.3f");
	_isDirty |= ImGui::DragScalarN("Half Lengths", ImGuiDataType_Float, hl.m128_f32, 3, speed, minValues, maxValues, "%.3f");
	ImGui::Text("Axes: ");
	_isDirty |= ImGui::DragScalarN("x-axes", ImGuiDataType_Float, a[0].m128_f32, 3, speed, minValues, maxValues, "%.3f");
	_isDirty |= ImGui::DragScalarN("y-axes", ImGuiDataType_Float, a[1].m128_f32, 3, speed, minValues, maxValues, "%.3f");
	_isDirty |= ImGui::DragScalarN("z-axes", ImGuiDataType_Float, a[2].m128_f32, 3, speed, minValues, maxValues, "%.3f");

	ImGui::Separator();

	XMStoreFloat3(&center, c);
	XMStoreFloat3(&halfLength, hl);
	for (int i = 0; i < 3; i++)
	{
		a[i] = XMVector3Normalize(a[i]);
		XMStoreFloat3(&axes[i], a[i]);
	}

	return true;
}
#endif

void Collisions::OBB::Transform(XMMATRIX M, Collider* transformed)
{
    if (!transformed || transformed->colliderType != OBB_COLLIDER)
        return;

    OBB* out = static_cast<OBB*>(transformed);

    // Center
    XMStoreFloat3(
        &out->center,
        XMVector3Transform(XMLoadFloat3(&center), M));

    // Axes (orthonormal)
    XMVECTOR x = XMVector3TransformNormal(XMLoadFloat3(&axes[0]), M);
    XMVECTOR y = XMVector3TransformNormal(XMLoadFloat3(&axes[1]), M);

    float sx = XMVectorGetX(XMVector3Length(x));
    float sy = XMVectorGetX(XMVector3Length(y));

    x = XMVector3Normalize(x);
    y = XMVector3Normalize(y);
    XMVECTOR z = XMVector3Normalize(XMVector3Cross(x, y));
    y = XMVector3Normalize(XMVector3Cross(z, x));

    XMStoreFloat3(&out->axes[0], x);
    XMStoreFloat3(&out->axes[1], y);
    XMStoreFloat3(&out->axes[2], z);

    // Half extents (local-axis scaling)
    out->halfLength = {
        halfLength.x * sx,
        halfLength.y * sy,
        halfLength.z * XMVectorGetX(
            XMVector3Length(XMVector3TransformNormal(
                XMLoadFloat3(&axes[2]), M)))
    };

    // World AABB for broadphase
    XMVECTOR c = XMLoadFloat3(&out->center);
    XMVECTOR r =
        XMVectorAbs(x) * out->halfLength.x +
        XMVectorAbs(y) * out->halfLength.y +
        XMVectorAbs(z) * out->halfLength.z;

    XMStoreFloat3(&out->_min, c - r);
    XMStoreFloat3(&out->_max, c + r);

	_isDirty = false;
	m_toCheck = true;
    out->SetToCheck();
}

/* Legacy
void Collisions::OBB::Transform(DX::XMMATRIX worldMatrixT, Collider *transformed)
{
	if (!transformed)
		return;
	if (transformed->colliderType != OBB_COLLIDER)
		return;

	OBB *transformedOBB = dynamic_cast<OBB *>(transformed);

	DX::XMMATRIX worldMatrix = DX::XMMatrixTranspose(worldMatrixT);

	// Transform center position
	DX::XMStoreFloat3(&transformedOBB->center, XMVector3Transform(XMLoadFloat3(&center), worldMatrix));

	// Rotate axes
    for (int i = 0; i < 3; i++)
    {
        XMVECTOR axisVec = XMLoadFloat3(&axes[i]);
        axisVec = XMVector3TransformNormal(axisVec, worldMatrix);
        DX::XMStoreFloat3(&transformedOBB->axes[i], XMVector3Normalize(axisVec));
    }

	// Scale Half-Lenghts
	XMFLOAT3 scale;
    scale.x = XMVectorGetX(XMVector3Length(worldMatrix.r[0]));
    scale.y = XMVectorGetX(XMVector3Length(worldMatrix.r[1]));
    scale.z = XMVectorGetX(XMVector3Length(worldMatrix.r[2]));

    DX::XMStoreFloat3(&transformedOBB->halfLength, XMVectorAbs(XMVectorMultiply(XMLoadFloat3(&halfLength), XMLoadFloat3(&scale))));

	// Update min max
	// TODO: Verify this part
    XMVECTOR corners[8] = {
        XMVectorSet(-transformedOBB->halfLength.x, -transformedOBB->halfLength.y, -transformedOBB->halfLength.z, 0),
        XMVectorSet( transformedOBB->halfLength.x, -transformedOBB->halfLength.y, -transformedOBB->halfLength.z, 0),
        XMVectorSet(-transformedOBB->halfLength.x,  transformedOBB->halfLength.y, -transformedOBB->halfLength.z, 0),
        XMVectorSet( transformedOBB->halfLength.x,  transformedOBB->halfLength.y, -transformedOBB->halfLength.z, 0),
        XMVectorSet(-transformedOBB->halfLength.x, -transformedOBB->halfLength.y,  transformedOBB->halfLength.z, 0),
        XMVectorSet( transformedOBB->halfLength.x, -transformedOBB->halfLength.y,  transformedOBB->halfLength.z, 0),
        XMVectorSet(-transformedOBB->halfLength.x,  transformedOBB->halfLength.y,  transformedOBB->halfLength.z, 0),
        XMVectorSet( transformedOBB->halfLength.x,  transformedOBB->halfLength.y,  transformedOBB->halfLength.z, 0)
    };
	
    XMVECTOR minPoint = XMVectorSet(INFINITY, INFINITY, INFINITY, 0);
    XMVECTOR maxPoint = XMVectorSet(-INFINITY, -INFINITY, -INFINITY, 0);

    for (int i = 0; i < 8; ++i)
    {
		XMVECTOR ex = XMVectorScale(XMLoadFloat3(&transformedOBB->axes[0]), corners[i].m128_f32[0]);
		XMVECTOR ey = XMVectorScale(XMLoadFloat3(&transformedOBB->axes[1]), corners[i].m128_f32[1]);
		XMVECTOR ez = XMVectorScale(XMLoadFloat3(&transformedOBB->axes[2]), corners[i].m128_f32[2]);

		XMVECTOR sum = XMVectorAdd(XMVectorAdd(ex, ey), ez);

		minPoint = XMVectorMin(minPoint, sum);
		maxPoint = XMVectorMax(maxPoint, sum);
	}

	minPoint = XMVectorAdd(minPoint, XMLoadFloat3(&transformedOBB->center));
	maxPoint = XMVectorAdd(maxPoint, XMLoadFloat3(&transformedOBB->center));

	DX::XMStoreFloat3(&transformedOBB->_min, minPoint);
	DX::XMStoreFloat3(&transformedOBB->_max, maxPoint);

	_isDirty = false;
	m_toCheck = true;
	transformed->SetToCheck();
}
*/

#ifdef DEBUG_BUILD
bool Collisions::OBB::TransformDebug(ID3D11DeviceContext *context, Time &time, const Input &input)
{
	if (!_debugEnt || !debug)
		return true;

	_debugEnt->GetTransform()->SetPosition(XMFLOAT4A(center.x,
		center.y,
		center.z,
		0));

	XMFLOAT4A rot;
	XMStoreFloat4A(&rot, XMQuaternionRotationMatrix(XMMatrixSet(axes[0].x, axes[0].y, axes[0].z, 0,
																axes[1].x, axes[1].y, axes[1].z, 0,
																axes[2].x, axes[2].y, axes[2].z, 0,
																0, 0, 0, 1)));

	_debugEnt->GetTransform()->SetRotation(rot);

	_debugEnt->GetTransform()->SetScale(XMFLOAT4A(halfLength.x, 
												  halfLength.y, 
												  halfLength.z, 
												  0));

	if (!_debugEnt->GetTransform()->UpdateConstantBuffer(context))
	{
		ErrMsg("Failed to set world matrix buffer!");
		return false;
	}

	if (!_debugEnt->InitialUpdate(time, input))
	{
		ErrMsg("Failed to set world matrix buffer!");
		return false;
	}
	return true;
}
#endif


/********************************************
*		      AABB COLLIDER					*
*********************************************/

Collisions::AABB::AABB(const AABB &other):
	AABB(other.center, other.halfLength, (ColliderTags)other._tagFlag)
{
}

Collisions::AABB::AABB(const DirectX::XMFLOAT3& c, const DirectX::XMFLOAT3& hl, ColliderTags tag):
	Collider(AABB_COLLIDER, "Mesh_WireframeCube", tag), center(c), halfLength(hl)
{
}

Collisions::AABB::AABB(const DirectX::BoundingBox& aabb, ColliderTags tag):
	Collider(AABB_COLLIDER, "Mesh_WireframeCube", tag), center(aabb.Center), halfLength(aabb.Extents)
{
}

Collisions::AABB::AABB(const DirectX::BoundingOrientedBox &obb, ColliderTags tag):
	Collider(AABB_COLLIDER, "Mesh_WireframeCube", tag), center(obb.Center), halfLength(obb.Extents)
{
}

#ifdef USE_IMGUI
bool Collisions::AABB::RenderUI()
{
	ImGui::Text("AABB Collider");
	ImGui::Separator();

	XMVECTOR c = XMLoadFloat3(&center),
			 hl = XMLoadFloat3(&halfLength);

	float minValues[3] = { -100, -100, -100 },
		  maxValues[3] = { 100, 100, 100 },
		  speed = 0.001f;

	_isDirty |= ImGui::DragScalarN("Center", ImGuiDataType_Float, c.m128_f32, 3, speed, minValues, maxValues, "%.3f");

	// TODO: Fix half length scale in x-z axes
	_isDirty |= ImGui::DragScalarN("Half Lengths", ImGuiDataType_Float, hl.m128_f32, 3, speed, minValues, maxValues, "%.3f");

	ImGui::Separator();

	XMStoreFloat3(&center, c);
	XMStoreFloat3(&halfLength, hl);

	return true;
}
#endif

void Collisions::AABB::Transform(DirectX::XMMATRIX worldMatrix, Collider *transformed)
{
	if (!transformed)
		return;
	if (transformed->colliderType != AABB_COLLIDER)
		return;

	AABB *transformedAABB = dynamic_cast<AABB *>(transformed);

	// Translation
	XMVECTOR worldTranslation = worldMatrix.r[3];
	XMVECTOR localCenter = XMLoadFloat3(&center);
	XMVECTOR worldCenter = XMVectorAdd(localCenter, worldTranslation);

	DX::XMStoreFloat3(&transformedAABB->center, worldCenter);
	//DX::XMStoreFloat3(&transformedAABB->center, XMVector3Transform(XMLoadFloat3(&center), worldMatrix));

	// Rotation
	XMVECTOR maxAxis = XMVectorSet(0, 0, 0, 0);
	XMVECTOR hl = XMLoadFloat3(&halfLength);
	XMVECTOR tempAxes[3] = { XMVectorSet(1, 0, 0, 0), 
							 XMVectorSet(0, 1, 0, 0), 
							 XMVectorSet(0, 0, 1, 0) };

    for (int i = 0; i < 3; i++)
    {
        XMVECTOR axisVec = tempAxes[i];
        axisVec = XMVector3Normalize(XMVector3TransformNormal(axisVec, worldMatrix));
		XMVECTOR temp = XMVectorSet(0, 0, 0, 0);
		temp = XMVectorMax(temp, XMVectorScale(axisVec,		 hl.m128_f32[i] * XMVectorGetX(XMVector3Length(worldMatrix.r[i]))));
		temp = XMVectorMax(temp, XMVectorScale(axisVec, -1 * hl.m128_f32[i] * XMVectorGetX(XMVector3Length(worldMatrix.r[i]))));
		maxAxis = XMVectorAdd(maxAxis, temp);
    }

	DX::XMStoreFloat3(&transformedAABB->halfLength, maxAxis);

	// Update min max
	transformedAABB->_min = { transformedAABB->center.x - transformedAABB->halfLength.x,
							  transformedAABB->center.y - transformedAABB->halfLength.y,
							  transformedAABB->center.z - transformedAABB->halfLength.z };

	transformedAABB->_max = { transformedAABB->center.x + transformedAABB->halfLength.x,
							  transformedAABB->center.y + transformedAABB->halfLength.y,
							  transformedAABB->center.z + transformedAABB->halfLength.z };

	_isDirty = false;
	m_toCheck = true;
	transformed->SetToCheck();
}

#ifdef DEBUG_BUILD
bool Collisions::AABB::TransformDebug(ID3D11DeviceContext* context, Time &time, const Input &input)
{
	if (!_debugEnt || !debug)
		return true;

	_debugEnt->GetTransform()->SetPosition(XMFLOAT4A(center.x,
													 center.y, 
													 center.z, 
													 0));

	_debugEnt->GetTransform()->SetScale(XMFLOAT4A(halfLength.x, 
												  halfLength.y, 
												  halfLength.z, 
												  0));

	if (!_debugEnt->GetTransform()->UpdateConstantBuffer(context))
	{
		ErrMsg("Failed to set world matrix buffer!");
		return false;
	}

	if (!_debugEnt->InitialUpdate(time, input))
	{
		ErrMsg("Failed to set world matrix buffer!");
		return false;
	}
	return true;
}
#endif

