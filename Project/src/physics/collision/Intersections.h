#pragma once

#include <DirectXMath.h>
#include <DirectXCollision.h>

#include "Colliders.h"

namespace Collisions
{
	struct CollisionData {
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 point;
		float depth;					// Intersection Depth
		const Collider* other;
		Entity* otherEntity;
	};

	struct LineSegment
	{
		DirectX::XMFLOAT3 start, end;
	};

	struct Circle
	{
		DirectX::XMFLOAT2 center;
		float radius;
	};

	struct Box
	{
		DirectX::XMFLOAT2 center, halfLength;
	};

	void Initialize();

	bool CheckIntersection(const Collider *c1, const Collider *c2, CollisionData& data);

	DirectX::XMFLOAT3 GetMin(const AABB &obb);
	DirectX::XMFLOAT3 GetMax(const AABB &obb);

	DirectX::XMFLOAT3 ClosestPoint(const LineSegment &segment, const OBB &obb);
	DirectX::XMFLOAT3 ClosestPoint(const LineSegment &segment, const DirectX::XMFLOAT3 &point);
	DirectX::XMFLOAT3 ClosestPoint(const LineSegment &segment1, const LineSegment &segment2);
	DirectX::XMFLOAT3 ClosestPoint(const OBB &obb, const DirectX::XMFLOAT3 &point);
	DirectX::XMFLOAT3 ClosestFacePoint(const OBB &obb, const DirectX::XMFLOAT3 &point, DirectX::XMFLOAT3 &normal, float &depth);

	bool PointInside(const OBB &obb, const DirectX::XMFLOAT3 &point);

	bool CircleBoxIntersection(const Circle &c, const Box &b, float &depth, DirectX::XMFLOAT2 &normal);

	bool RaySphereIntersection(const Ray &ray, const Sphere &s, float &l, DirectX::XMFLOAT3 &p, DirectX::XMFLOAT3 &normal);
	bool RayCapsuleIntersection(const Ray &ray, const Capsule &c, float &l, DirectX::XMFLOAT3 &p, DirectX::XMFLOAT3 &normal);
	bool RayOBBIntersection(const Ray &ray, const OBB &obb, float &l, DirectX::XMFLOAT3 &p, DirectX::XMFLOAT3 &normal);
	bool RayAABBIntersection(const Ray &ray, const AABB &aabb, float &l, DirectX::XMFLOAT3 &p, DirectX::XMFLOAT3 &normal);

	bool SphereRayIntersection(const Sphere &s, const Ray &ray, float &l, DirectX::XMFLOAT3 &p, DirectX::XMFLOAT3 &normal);
	bool SphereSphereIntersection(const Sphere &s1, const Sphere &s2, DirectX::XMFLOAT3& normal, float &depth);
	bool SphereCapsuleIntersection(const Sphere &s, const Capsule &c, DirectX::XMFLOAT3 &normal, float &depth);
	bool SphereAABBIntersection(const Sphere &s, const AABB &aabb, DirectX::XMFLOAT3 &normal, float &depth);
	bool SphereOBBIntersection(const Sphere &s, const OBB &obb, DirectX::XMFLOAT3 &normal, float &depth);

	bool CapsuleRayIntersection(const Capsule &c, const Ray &ray, float &l, DirectX::XMFLOAT3 &p, DirectX::XMFLOAT3 &normal);
	bool CapsuleCapsuleIntersection(const Capsule &c1, const Capsule &c2, DirectX::XMFLOAT3 &normal, float &depth);
	bool CapsuleSphereIntersection(const Capsule &c, const Sphere &s, DirectX::XMFLOAT3 &normal, float &depth);
	bool CapsuleAABBIntersection(const Capsule &c, const AABB &aabb, DirectX::XMFLOAT3 &normal, float &depth);
	bool CapsuleOBBIntersection(const Capsule &c, const OBB &obb, DirectX::XMFLOAT3 &normal, float &depth);

	bool OBBRayIntersection(const OBB &obb, const Ray &ray, float &l, DirectX::XMFLOAT3 &p, DirectX::XMFLOAT3 &normal);
	bool OBBOBBIntersection(const OBB &obb1, const OBB &obb2, DirectX::XMFLOAT3 &normal, float &depth);
	bool OBBSphereIntersection(const OBB &obb, const Sphere &s, DirectX::XMFLOAT3 &normal, float &depth);
	bool OBBCapsuleIntersection(const OBB &obb, const Capsule &c, DirectX::XMFLOAT3 &normal, float &depth);
	bool OBBAABBIntersection(const OBB &obb, const AABB &aabb, DirectX::XMFLOAT3 &normal, float &depth);

	bool AABBRayIntersection(const AABB &aabb, const Ray &ray, float &l, DirectX::XMFLOAT3 &p, DirectX::XMFLOAT3 &normal);
	bool AABBAABBIntersection(const AABB &aabb1, const AABB &aabb2, DirectX::XMFLOAT3 &normal, float &depth);
	bool AABBSphereIntersection(const AABB &aabb, const Sphere &s, DirectX::XMFLOAT3 &normal, float &depth);
	bool AABBCapsuleIntersection(const AABB &aabb, const Capsule &c, DirectX::XMFLOAT3 &normal, float &depth);
	bool AABBOBBIntersection(const AABB &aabb, const OBB &obb, DirectX::XMFLOAT3 &normal, float &depth);
}

