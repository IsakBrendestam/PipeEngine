#include "stdafx.h"
#include "Intersections.h"
#include "debug/Debug.h"

#include <functional>
#include <vector>
#include <float.h>

#ifdef LEAK_DETECTION
#define new			DEBUG_NEW
#endif

using namespace DirectX;
using namespace Collisions;

#define MIN(A, B) (((A) < (B)) ? (A) : (B))
#define MAX(A, B) (((A) > (B)) ? (A) : (B))
#define CLAMP(A, min, max) (MIN(MAX((x), (minVal)), (maxVal)))

using CollisionFunction = std::function<bool(const Collider*, const Collider*, DX::XMFLOAT3& normal, float &depth)>;
static CollisionFunction collisionTable[COLLIDER_TYPES_COUNT][COLLIDER_TYPES_COUNT];

void Collisions::Initialize()
{
	//collisionTable[RAY_COLLIDER][RAY_COLLIDER] = Collisions::OBBOBBIntersection;
	//collisionTable[][]
}

bool Collisions::CheckIntersection(const Collider *c1, const Collider *c2, CollisionData& data)
{
	ZoneScopedC(RandomUniqueColor());

	if (!c1 || !c2)
		return false;

	if (c1->colliderType == NULL_COLLIDER || c2->colliderType == NULL_COLLIDER)
		return false;

	if (!c1->GetActive() || !c2->GetActive())
		return false;

	if (c1->HasTag(STATIC_TAG) && c2->HasTag(STATIC_TAG))
		return false;
	
	// Ray Intersections
	if (c1->colliderType == RAY_COLLIDER && c2->colliderType == SPHERE_COLLIDER)
		return RaySphereIntersection(*static_cast<const Ray *>(c1), *static_cast<const Sphere *>(c2), data.depth, data.point, data.normal);

	if (c1->colliderType == RAY_COLLIDER && c2->colliderType == CAPSULE_COLLIDER)
		return RayCapsuleIntersection(*static_cast<const Ray *>(c1), *static_cast<const Capsule *>(c2), data.depth, data.point, data.normal);

	if (c1->colliderType == RAY_COLLIDER && c2->colliderType == OBB_COLLIDER)
		return RayOBBIntersection(*static_cast<const Ray *>(c1), *static_cast<const OBB *>(c2), data.depth, data.point, data.normal);

	if (c1->colliderType == RAY_COLLIDER && c2->colliderType == AABB_COLLIDER)
		return RayAABBIntersection(*static_cast<const Ray *>(c1), *static_cast<const AABB *>(c2), data.depth, data.point, data.normal);

	// Sphere Intersections
	if (c1->colliderType == SPHERE_COLLIDER && c2->colliderType == RAY_COLLIDER)
		return SphereRayIntersection(*static_cast<const Sphere *>(c1), *static_cast<const Ray *>(c2), data.depth, data.point, data.normal);

	if (c1->colliderType == SPHERE_COLLIDER && c2->colliderType == SPHERE_COLLIDER)
		return SphereSphereIntersection(*static_cast<const Sphere *>(c1), *static_cast<const Sphere *>(c2), data.normal, data.depth);

	if (c1->colliderType == SPHERE_COLLIDER && c2->colliderType == OBB_COLLIDER)
		return SphereOBBIntersection(*static_cast<const Sphere *>(c1), *static_cast<const OBB *>(c2), data.normal, data.depth);

	if (c1->colliderType == SPHERE_COLLIDER && c2->colliderType == AABB_COLLIDER)
		return SphereAABBIntersection(*static_cast<const Sphere *>(c1), *static_cast<const AABB *>(c2), data.normal, data.depth);

	if (c1->colliderType == SPHERE_COLLIDER && c2->colliderType == CAPSULE_COLLIDER)
		return SphereCapsuleIntersection(*static_cast<const Sphere *>(c1), *static_cast<const Capsule *>(c2), data.normal, data.depth);

	// Capsule Intersections
	if (c1->colliderType == CAPSULE_COLLIDER && c2->colliderType == RAY_COLLIDER)
		return CapsuleRayIntersection(*static_cast<const Capsule *>(c1), *static_cast<const Ray *>(c2), data.depth, data.point, data.normal);

	if (c1->colliderType == CAPSULE_COLLIDER && c2->colliderType == CAPSULE_COLLIDER)
		return CapsuleCapsuleIntersection(*static_cast<const Capsule *>(c1), *static_cast<const Capsule *>(c2), data.normal, data.depth);

	if (c1->colliderType == CAPSULE_COLLIDER && c2->colliderType == SPHERE_COLLIDER)
		return CapsuleSphereIntersection(*static_cast<const Capsule *>(c1), *static_cast<const Sphere *>(c2), data.normal, data.depth);

	if (c1->colliderType == CAPSULE_COLLIDER && c2->colliderType == OBB_COLLIDER)
		return CapsuleOBBIntersection(*static_cast<const Capsule *>(c1), *static_cast<const OBB *>(c2), data.normal, data.depth);

	if (c1->colliderType == CAPSULE_COLLIDER && c2->colliderType == AABB_COLLIDER)
		return CapsuleAABBIntersection(*static_cast<const Capsule *>(c1), *static_cast<const AABB *>(c2), data.normal, data.depth);

	// OBB Intersections
	if (c1->colliderType == OBB_COLLIDER && c2->colliderType == RAY_COLLIDER)
		return OBBRayIntersection(*static_cast<const OBB *>(c1), *static_cast<const Ray *>(c2), data.depth, data.point, data.normal);

	if (c1->colliderType == OBB_COLLIDER && c2->colliderType == OBB_COLLIDER)
		return OBBOBBIntersection(*static_cast<const OBB *>(c1), *static_cast<const OBB *>(c2), data.normal, data.depth);

	if (c1->colliderType == OBB_COLLIDER && c2->colliderType == SPHERE_COLLIDER)
		return OBBSphereIntersection(*static_cast<const OBB *>(c1), *static_cast<const Sphere *>(c2), data.normal, data.depth);

	if (c1->colliderType == OBB_COLLIDER && c2->colliderType == CAPSULE_COLLIDER)
		return OBBCapsuleIntersection(*static_cast<const OBB *>(c1), *static_cast<const Capsule *>(c2), data.normal, data.depth);

	if (c1->colliderType == OBB_COLLIDER && c2->colliderType == AABB_COLLIDER)
		return OBBAABBIntersection(*static_cast<const OBB *>(c1), *static_cast<const AABB *>(c2), data.normal, data.depth);

	// AABB Intersections
	if (c1->colliderType == AABB_COLLIDER && c2->colliderType == OBB_COLLIDER)
		return AABBRayIntersection(*static_cast<const AABB *>(c1), *static_cast<const Ray *>(c2), data.depth, data.point, data.normal);

	if (c1->colliderType == AABB_COLLIDER && c2->colliderType == AABB_COLLIDER)
		return AABBAABBIntersection(*static_cast<const AABB *>(c1), *static_cast<const AABB *>(c2), data.normal, data.depth);

	if (c1->colliderType == AABB_COLLIDER && c2->colliderType == OBB_COLLIDER)
		return AABBOBBIntersection(*static_cast<const AABB *>(c1), *static_cast<const OBB *>(c2), data.normal, data.depth);

	if (c1->colliderType == AABB_COLLIDER && c2->colliderType == SPHERE_COLLIDER)
		return AABBSphereIntersection(*static_cast<const AABB *>(c1), *static_cast<const Sphere *>(c2), data.normal, data.depth);

	if (c1->colliderType == AABB_COLLIDER && c2->colliderType == CAPSULE_COLLIDER)
		return AABBCapsuleIntersection(*static_cast<const AABB *>(c1), *static_cast<const Capsule *>(c2), data.normal, data.depth);

	//ErrMsg("Collider pair not supported");
	return false;
}


/********************************************
*			 HELPER FUNCTIONS				*
*********************************************/

void ProjectOntoAxis(const Collisions::OBB &obb, const XMVECTOR &normalizedAxis, float &minProj, float &maxProj)
{
	XMVECTOR axis = XMVector3Normalize(normalizedAxis);
    float centerProjection = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&obb.center), axis));

	// Project orientations to axis
    float r =fabs(obb.halfLength.x * XMVectorGetX(XMVector3Dot(XMLoadFloat3(&obb.axes[0]), axis))) +
			 fabs(obb.halfLength.y * XMVectorGetX(XMVector3Dot(XMLoadFloat3(&obb.axes[1]), axis))) +
			 fabs(obb.halfLength.z * XMVectorGetX(XMVector3Dot(XMLoadFloat3(&obb.axes[2]), axis)));

	maxProj = centerProjection + r;
	minProj = centerProjection - r;
}

bool IsAxisAligned(const OBB& obb)
{
    return fabs(obb.axes[0].x - 1.0f) < GameMath::EPSILON &&
           fabs(obb.axes[1].y - 1.0f) < GameMath::EPSILON &&
           fabs(obb.axes[2].z - 1.0f) < GameMath::EPSILON;
}

XMFLOAT3 Collisions::GetMin(const AABB &obb)
{
	XMVECTOR c = XMLoadFloat3(&obb.center),
			 hl = XMLoadFloat3(&obb.halfLength),
			 p1 = XMVectorAdd(c, hl),
			 p2 = XMVectorSubtract(c, hl);

	return { fminf(XMVectorGetX(p1), XMVectorGetX(p2)),
			 fminf(XMVectorGetY(p1), XMVectorGetY(p2)),
			 fminf(XMVectorGetZ(p1), XMVectorGetZ(p2)) };
}

XMFLOAT3 Collisions::GetMax(const AABB &obb)
{
	XMVECTOR c = XMLoadFloat3(&obb.center),
			 hl = XMLoadFloat3(&obb.halfLength),
			 p1 = XMVectorAdd(XMLoadFloat3(&obb.center), XMLoadFloat3(&obb.halfLength)),
			 p2 = XMVectorSubtract(XMLoadFloat3(&obb.center), XMLoadFloat3(&obb.halfLength));

	return { fmaxf(XMVectorGetX(p1), XMVectorGetX(p2)),
			 fmaxf(XMVectorGetY(p1), XMVectorGetY(p2)),
			 fmaxf(XMVectorGetZ(p1), XMVectorGetZ(p2)) };
}

XMFLOAT3 Collisions::ClosestPoint(const LineSegment &segment, const OBB &obb)
{
	// TODO: Verify
	XMVECTOR closest = XMLoadFloat3(&obb.center);
	XMVECTOR hl = XMLoadFloat3(&obb.halfLength);
    XMVECTOR segmentDir = XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&segment.end), XMLoadFloat3(&segment.start)));

    for (int i = 0; i < 3; i++)
    {
        XMVECTOR axis = XMLoadFloat3(&obb.axes[i]);
        float extent = hl.m128_f32[i];

        float projected = XMVectorGetX(XMVector3Dot(XMVectorSubtract(closest, XMLoadFloat3(&segment.start)), axis));

        if (projected > extent) projected = extent;
        if (projected < -extent) projected = -extent;

        closest = XMVectorAdd(closest, XMVectorScale(axis, projected));
    }

	XMFLOAT3 res;
	XMStoreFloat3(&res, closest);

    return res;
}

XMFLOAT3 Collisions::ClosestPoint(const LineSegment &seg, const XMFLOAT3 &point)
{
	XMVECTOR a = XMLoadFloat3(&seg.start);
	XMVECTOR b = XMLoadFloat3(&seg.end);
	XMVECTOR p = XMLoadFloat3(&point);
	XMVECTOR ba = XMVectorSubtract(b, a);

	float abSquareDist = XMVectorGetX(XMVector3Dot(ba, ba));
	float t = XMVectorGetX(XMVector3Dot(XMVectorSubtract(p, a), ba)) / abSquareDist;
	float tSaturated = MIN(MAX(t, 0), 1);

	XMFLOAT3 res;
	XMStoreFloat3(&res, XMVectorAdd(a, XMVectorScale(ba, tSaturated)));

	return res;
}

XMFLOAT3 Collisions::ClosestPoint(const LineSegment &seg1, const LineSegment &seg2)
{
	XMVECTOR P0 = XMLoadFloat3(&seg1.start),
			 P1 = XMLoadFloat3(&seg1.end),
			 Q0 = XMLoadFloat3(&seg2.start),
			 Q1 = XMLoadFloat3(&seg2.end),
			 u = XMVectorSubtract(P1, P0),
			 v = XMVectorSubtract(Q1, Q0);

	float a = XMVectorGetX(XMVector3Dot(u, u)),
		  b = XMVectorGetX(XMVector3Dot(u, v)),
		  c = XMVectorGetX(XMVector3Dot(v, v)),
		  d = XMVectorGetX(XMVector3Dot(u, XMVectorSubtract(P0, Q0))),
		  e = XMVectorGetX(XMVector3Dot(v, XMVectorSubtract(P0, Q0)));

	XMFLOAT3 res;
	XMStoreFloat3(&res, XMVectorAdd(P0, XMVectorScale(u, (b * e - c * d) / (a * c - b * b))));
	return res;
}

XMFLOAT3 Collisions::ClosestPoint(const OBB &obb, const XMFLOAT3 &point)
{
	XMVECTOR res = XMLoadFloat3(&obb.center);
	XMVECTOR dir = XMVectorSubtract(XMLoadFloat3(&point), res);

	float hl[3] = { obb.halfLength.x, obb.halfLength.y, obb.halfLength.z };

	for (int i = 0; i < 3; i++) 
	{
		XMVECTOR axis = XMVector3Normalize(XMLoadFloat3(&obb.axes[i]));

		// project dir on axis
		float distance = XMVectorGetX(XMVector3Dot(dir, axis));
		
		// Check distance with half-length
		if (distance > hl[i])
			distance = hl[i];
		if (distance < -1*hl[i])
			distance = -1*hl[i];

		// Add result
		res = XMVectorAdd(res, XMVectorScale(axis, distance));
	}

	XMFLOAT3 result;
	XMStoreFloat3(&result, res);
	return result;
}

DirectX::XMFLOAT3 Collisions::ClosestFacePoint(const OBB &obb, const DirectX::XMFLOAT3 &point, DirectX::XMFLOAT3 &outNormal, float &outDepth)
{
    XMVECTOR obbCenter = XMLoadFloat3(&obb.center);
    XMVECTOR dir = XMVectorSubtract(XMLoadFloat3(&point), obbCenter);

    float hl[3] = { obb.halfLength.x, obb.halfLength.y, obb.halfLength.z };
    XMVECTOR res = obbCenter;

    float minPenetration = FLT_MAX;
    XMVECTOR bestNormal = XMVectorZero();
    bool inside = true;

    // Iterate over each axis
    for (int i = 0; i < 3; i++) 
    {
        XMVECTOR axis = XMVector3Normalize(XMLoadFloat3(&obb.axes[i]));
        float distance = XMVectorGetX(XMVector3Dot(dir, axis));

        float overDist = 0.0f;
        if (distance > hl[i]) 
		{
            overDist = distance - hl[i];
            inside = false;
        } 
        else if (distance < -hl[i]) 
		{
            overDist = distance + hl[i];
            inside = false;
        }

        if (inside) 
		{
            float penPos = hl[i] - distance;
            float penNeg = hl[i] + distance;

            if (penPos < minPenetration) 
			{
                minPenetration = penPos;
                bestNormal = axis;
            }

            if (penNeg < minPenetration) 
			{
                minPenetration = penNeg;
                bestNormal = XMVectorNegate(axis);
            }
        } 
		else 
            distance = std::clamp(distance, -hl[i], hl[i]);

        res = XMVectorAdd(res, XMVectorScale(axis, distance));
    }

    if (inside) 
	{
        outDepth = minPenetration;
        XMStoreFloat3(&outNormal, bestNormal);
    } 
	else 
	{
        outDepth = 0.0f;
        outNormal = { 0.0f, 0.0f, 0.0f };
    }

    XMFLOAT3 result;
    XMStoreFloat3(&result, res);
    return result;
}


bool Collisions::PointInside(const OBB &obb, const DirectX::XMFLOAT3 &point)
{
	XMVECTOR axes[3] = { XMLoadFloat3(&obb.axes[0]),
						 XMLoadFloat3(&obb.axes[1]),
						 XMLoadFloat3(&obb.axes[2]) };

	XMVECTOR p = XMVectorSubtract(XMLoadFloat3(&point), XMLoadFloat3(&obb.center)),
			 hl = XMLoadFloat3(&obb.halfLength);


	for (int i = 0; i < 3; i++)
	{
		float len2 = XMVectorGetX(XMVector3Dot(p, axes[i])) / XMVectorGetX(XMVector3Dot(axes[i], axes[i]));
		if (std::fabsf(len2) > hl.m128_f32[i])
			return false;
	}

	return true;
}

bool Collisions::CircleBoxIntersection(const Circle &c, const Box &b, float &depth, XMFLOAT2 &normal)
{
    float closestX = std::max<float>(b.center.x - b.halfLength.x, std::min<float>(c.center.x, b.center.x + b.halfLength.x));
    float closestY = std::max<float>(b.center.y - b.halfLength.y, std::min<float>(c.center.y, b.center.y + b.halfLength.y));
    
    XMFLOAT2 difference = XMFLOAT2(c.center.x - closestX, c.center.y - closestY);
    float distanceSquared = difference.x * difference.x + difference.y * difference.y;
    
    // Check for intersection
    if (distanceSquared < (c.radius * c.radius))
	{
        float distance = sqrtf(distanceSquared);
        
        depth = c.radius - distance;
        
        if (distance > 0.0f)
            normal = XMFLOAT2(difference.x / distance, difference.y / distance);
        else
			normal = XMFLOAT2(1.0f, 0.0f);

		return true; // Found intersection
    }
    
    return false; // No intersection
}



/********************************************
*			 RAY INTERSECTIONS				*
*********************************************/

bool Collisions::RaySphereIntersection(const Ray &ray, const Sphere &s, float &l, DirectX::XMFLOAT3 &p, DirectX::XMFLOAT3 &normal)
{
	XMVECTOR dir = XMLoadFloat3(&ray.dir),
			 ori = XMLoadFloat3(&ray.origin),
		     center = XMLoadFloat3(&s.center);

	XMVECTOR oc = XMVectorSubtract(center, ori);
	float a = XMVectorGetX(XMVector3Dot(dir, dir)),
		  h = XMVectorGetX(XMVector3Dot(dir, oc)),
		  c = XMVectorGetX(XMVector3Dot(oc, oc)) - s.radius * s.radius,
		  d = h * h - a * c;

	if (d < 0) return false; // Missed sphere

	float sqrtd = std::sqrtf(d);

	l = (h - sqrtd) / a;
	if (l <= 0 || l >= ray.length)
	{
		l = (h + sqrtd) / a;
		if (l <= 0 || l >= ray.length) return false;
	}

	XMVECTOR point = XMVectorAdd(ori, XMVectorScale(dir, ray.length));
	XMStoreFloat3(&p, point);
	XMStoreFloat3(&normal, XMVectorScale(XMVectorSubtract(point, center), 1 / s.radius));

	return true;
}

bool Collisions::RayCapsuleIntersection(const Ray &ray, const Capsule &c, float &l, DirectX::XMFLOAT3 &p, DirectX::XMFLOAT3 &normal)
{
	// Capsule
	XMVECTOR normal1 = XMVector3Normalize(XMLoadFloat3(&c.upDir)),
			 lineEndOffset1 = XMVectorScale(normal1, (c.height / 2) - c.radius),
			 A1 = XMVectorAdd(XMLoadFloat3(&c.center), lineEndOffset1),
			 B1 = XMVectorSubtract(XMLoadFloat3(&c.center), lineEndOffset1);

	XMVECTOR A2 = XMLoadFloat3(&ray.origin),
			 B2 =  XMVectorAdd(XMLoadFloat3(&ray.origin), XMVectorScale(XMLoadFloat3(&ray.dir), ray.length));

	XMFLOAT3 tempA1, tempA2, tempB1, tempB2;
	XMStoreFloat3(&tempA1, A1);
	XMStoreFloat3(&tempB1, B1);
	XMStoreFloat3(&tempA2, A2);
	XMStoreFloat3(&tempB2, B2);

	XMFLOAT3 point1 = ClosestPoint({ tempA1, tempB1 }, { tempA2, tempB2 });
	XMFLOAT3 point2 = ClosestPoint({ tempA2, tempB2 }, point1);

	XMVECTOR diff = XMVectorSubtract(XMLoadFloat3(&point1), XMLoadFloat3(&point2));
	float dist2 = XMVectorGetX(XMVector3Dot(diff, diff));

	if (dist2 < c.radius * c.radius)
		return true;
	
	return false;
}

bool Collisions::RayOBBIntersection(const Ray &ray, const OBB &obb, float &l, XMFLOAT3 &p, XMFLOAT3 &normal)
{
	float minV = -1*FLT_MAX,
		  maxV = FLT_MAX;

	XMVECTOR r0 = XMLoadFloat3(&ray.origin),
			 rD = XMLoadFloat3(&ray.dir);

	XMVECTOR halfLength = XMLoadFloat3(&obb.halfLength),
				 center = XMLoadFloat3(&obb.center),
			    axes[3] = { XMLoadFloat3(&obb.axes[0]),
							XMLoadFloat3(&obb.axes[1]),
							XMLoadFloat3(&obb.axes[2]) };

	XMVECTOR rayToCenter = XMVectorSubtract(center, r0),
					nMin = XMVectorSet(0, 0, 0, 0),
					nMax = XMVectorSet(0, 0, 0, 0);

	for (int a = 0; a < 3; a++)
	{
		XMVECTOR axis = axes[a];
		float hl = halfLength.m128_f32[a];

		float distAlongAxis = XMVector3Dot(axis, rayToCenter).m128_f32[0],
			  f = XMVector3Dot(axis, rD).m128_f32[0];

		if (fabsf(f) > FLT_MIN)
		{
			XMVECTOR tnMin = axis,
					  tnMax = XMVectorScale(axis, -1);

			float t0 = (distAlongAxis + hl) / f,
				  t1 = (distAlongAxis - hl) / f;

			if (t0 > t1) // Flip Intersection Order
			{
				float temp = t0;
				t0 = t1;
				t1 = temp;

				tnMin = tnMax;
				tnMax = axis;
			}

			if (t0 > minV) // Keep the longer entry-point
			{
				minV = t0;
				nMin = tnMin;
			}

			if (t1 < maxV) // Keep the shorter exit-point
			{
				maxV = t1;
				nMax = tnMax;
			}

			if (minV > maxV) return false; // Ray misses OBB
			if (maxV < 0.0f) return false; // OBB is behind Ray
		}
		else if (-distAlongAxis - hl > 0.0f || -distAlongAxis + hl < 0.0f)
		{
			return false; // Ray is orthogonal to axis but no located between the axis-planes
		}
	}
	
	// Find closest positive intersection
	if (minV > 0.0f)
	{
		l = minV;
		XMStoreFloat3(&normal, XMVector3Normalize(nMin));
	}
	else
	{
		l = maxV;
		XMStoreFloat3(&normal, XMVectorNegate(XMVector3Normalize(-nMax)));
	}

	XMStoreFloat3(&p, XMVectorAdd(r0, XMVectorScale(rD, l)));
	return true;
}

bool Collisions::RayAABBIntersection(const Ray &ray, const AABB &aabb, float &l, DirectX::XMFLOAT3 &p, DirectX::XMFLOAT3 &normal)
{
	return RayOBBIntersection(ray, OBB(aabb), l, p, normal);
}


/********************************************
*		   SPHERE INTERSECTIONS				*
*********************************************/

bool Collisions::SphereRayIntersection(const Sphere &s, const Ray &ray, float &l, DirectX::XMFLOAT3 &p, DirectX::XMFLOAT3 &normal)
{
	if (RaySphereIntersection(ray, s, l, p, normal))
	{
		XMStoreFloat3(&normal, XMVectorNegate(XMLoadFloat3(&normal)));
		return true;
	}
	return false;
}

bool Collisions::SphereSphereIntersection(const Sphere &s1, const Sphere &s2, XMFLOAT3 &normal, float &depth)
{
	const XMVECTOR c1 = XMLoadFloat3(&s1.center),
				   c2 = XMLoadFloat3(&s2.center);

	const XMVECTOR diff = XMVectorSubtract(c1, c2);
	const float d = XMVectorGetX(XMVector3Length(diff));
	const float rSum = s1.radius + s2.radius;

	// Check intersection
	if (d <= rSum)
	{
		if (d != 0)
			XMStoreFloat3(&normal, XMVectorScale(diff, 1.0f/d));
		depth = rSum - d;
		return true;
	}

	return false;
}

bool Collisions::SphereCapsuleIntersection(const Sphere &s, const Capsule &c, DirectX::XMFLOAT3 &normal, float &depth)
{
	bool res = CapsuleSphereIntersection(c, s, normal, depth);
	if (res)
		XMStoreFloat3(&normal, XMVectorNegate(XMLoadFloat3(&normal)));
	return res;
}

bool Collisions::SphereAABBIntersection(const Sphere &s, const AABB &aabb, DirectX::XMFLOAT3 &normal, float &depth)
{
	bool res = AABBSphereIntersection(aabb, s, normal, depth);
	if (res)
		XMStoreFloat3(&normal, XMVectorNegate(XMLoadFloat3(&normal)));
	return res;
}

bool Collisions::SphereOBBIntersection(const Sphere &s, const OBB &obb, XMFLOAT3 &normal, float &depth)
{
	// Get closest point
	const XMFLOAT3 closestPoint = ClosestPoint(obb, s.center);
	const XMVECTOR diff = XMVectorSubtract(XMLoadFloat3(&s.center), XMLoadFloat3(&closestPoint));
	const float d = XMVectorGetX(XMVector3Length(diff));

	// Check closest point with radius
	if (d <= s.radius)
	{
		if (d != 0)
			XMStoreFloat3(&normal, XMVectorScale(diff, 1.0f/d));
		depth = s.radius - d;
		return true;
	}
	return false;
}


/********************************************
*		   CAPSULE INTERSECTIONS			*
*********************************************/

bool Collisions::CapsuleRayIntersection(const Capsule &c, const Ray &ray, float &l, DirectX::XMFLOAT3 &p, DirectX::XMFLOAT3 &normal)
{
	if (RayCapsuleIntersection(ray, c, l, p, normal))
	{
		XMStoreFloat3(&normal, XMVectorNegate(XMLoadFloat3(&normal)));
		return true;
	}
	return false;
}

bool Collisions::CapsuleCapsuleIntersection(const Capsule &c1, const Capsule &c2, DirectX::XMFLOAT3 &normal, float &depth)
{
	// TODO: Look for early escapes

	// Capsule 1
	XMVECTOR normal1 = XMVector3Normalize(XMLoadFloat3(&c1.upDir)),
			 lineEndOffset1 = XMVectorScale(normal1, (c1.height / 2) - c1.radius),
			 A1 = XMVectorAdd(XMLoadFloat3(&c1.center), lineEndOffset1),
			 B1 = XMVectorSubtract(XMLoadFloat3(&c1.center), lineEndOffset1);

	// Capsule 2
	XMVECTOR normal2 = XMVector3Normalize(XMLoadFloat3(&c2.upDir)),
			 lineEndOffset2 = XMVectorScale(normal2, (c2.height / 2) - c2.radius),
			 A2 = XMVectorAdd(XMLoadFloat3(&c2.center), lineEndOffset2),
			 B2 = XMVectorSubtract(XMLoadFloat3(&c2.center), lineEndOffset2);

	// Vectors between line endpoints
	XMVECTOR v0 = XMVectorSubtract(A2, A1),
			 v1 = XMVectorSubtract(B2, A1),
			 v2 = XMVectorSubtract(A2, B1),
			 v3 = XMVectorSubtract(B2, B1);

	// Square distances
	float d0 = XMVectorGetX(XMVector3Dot(v0, v0)),
		  d1 = XMVectorGetX(XMVector3Dot(v1, v1)),
		  d2 = XMVectorGetX(XMVector3Dot(v2, v2)),
		  d3 = XMVectorGetX(XMVector3Dot(v3, v3));

	XMVECTOR bestA, bestB;
	if (d2 < d0 || d2 < d1 || d3 < d0 || d3 < d1)
		bestA = B1;
	else
		bestA = A1;


	XMFLOAT3 tempA1, tempB1, tempA2, tempB2, tempBestA, tempBestB;

	XMStoreFloat3(&tempA1, A1);
	XMStoreFloat3(&tempB1, B1);
	XMStoreFloat3(&tempA2, A2);
	XMStoreFloat3(&tempB2, B2);
	XMStoreFloat3(&tempBestA, bestA);

	// select point on capsule B line segment nearest to best potential endpoint on A capsule:
	tempBestB = ClosestPoint({ tempA2, tempB2 }, tempBestA);
	bestB = XMLoadFloat3(&tempBestB);

	// now do the same for capsule A segment:
	tempBestA = ClosestPoint({ tempA1, tempB1 }, tempBestB);
	bestA = XMLoadFloat3(&tempBestA);

	// Calculate normal
	XMVECTOR penetration_normal = XMVectorSubtract(bestA, bestB);
	float len = XMVectorGetX(XMVector3Length(penetration_normal));
	if (len != 0)
		XMStoreFloat3(&normal, XMVectorScale(penetration_normal, 1.0f / len));

	// Calculate depth
	depth = c1.radius + c2.radius - len;

	return depth > 0;
}

bool Collisions::CapsuleSphereIntersection(const Capsule &c, const Sphere &s, XMFLOAT3 &normal, float &depth)
{
	// Capsule information
	XMVECTOR capNormal = XMVector3Normalize(XMLoadFloat3(&c.upDir)),
			 lineEndOffset = XMVectorScale(capNormal, (c.height / 2) - c.radius),
			 A = XMVectorAdd(XMLoadFloat3(&c.center), lineEndOffset),
			 B = XMVectorSubtract(XMLoadFloat3(&c.center), lineEndOffset);

	XMFLOAT3 tempA, tempB;
	XMStoreFloat3(&tempA, A);
	XMStoreFloat3(&tempB, B);

	// Get closest point
	XMFLOAT3 temp = ClosestPoint({ tempB, tempA }, s.center);
	XMVECTOR closePoint = XMLoadFloat3(&temp);

	XMVECTOR penNormal = XMVectorSubtract(closePoint, XMLoadFloat3(&s.center));
	float minDist = XMVectorGetX(XMVector3Length(penNormal));

	if (minDist != 0)
		XMStoreFloat3(&normal, XMVectorScale(penNormal, 1.0f / minDist));

	depth = c.radius + s.radius - minDist;

	return depth > 0;
}

bool Collisions::CapsuleAABBIntersection(const Capsule &c, const AABB &aabb, DirectX::XMFLOAT3 &normal, float &depth)
{
	return CapsuleOBBIntersection(c, OBB(aabb), normal, depth);
}

bool Collisions::CapsuleOBBIntersection(const Capsule &c, const OBB &obb, DirectX::XMFLOAT3 &normal, float &depth)
{
	// Capsule information
	XMVECTOR cCenter = XMLoadFloat3(&c.center),
			 capNormal = XMVector3Normalize(XMLoadFloat3(&c.upDir)),
			 lineEndOffset = XMVectorScale(capNormal, (c.height / 2) - c.radius),
			 A = XMVectorAdd(cCenter, lineEndOffset),
			 B = XMVectorSubtract(cCenter, lineEndOffset);

	bool intersection = false;

	// Check lower hemisphere
	XMFLOAT3 tempB;
	XMStoreFloat3(&tempB, B);
	if (SphereOBBIntersection(Sphere(tempB, c.radius), obb, normal, depth))
	{
		float intersectionAngle = XMVectorGetX(XMVector3Dot(capNormal, XMLoadFloat3(&normal)));
		if (intersectionAngle > 0)
			return true;
	}

	// Check upper hemisphere
	XMFLOAT3 tempA;
	XMStoreFloat3(&tempA, A);
	if (SphereOBBIntersection(Sphere(tempA, c.radius), obb, normal, depth))
	{
		float intersectionAngle = XMVectorGetX(XMVector3Dot(capNormal, XMLoadFloat3(&normal)));
		if (intersectionAngle < 0)
			return true;
	}


	// TODO: Make sure the following part is working correctly with rotation
	// Find closest points on OBB and capsule segment
	XMVECTOR distVec;
	float distanceSq = FLT_MAX;

	for (int i = 0; i < 2; i++)
	{
		XMFLOAT3 obbClosest;
		if (i == 0)
			obbClosest = ClosestPoint(obb, tempB);
		else
			obbClosest = ClosestPoint(obb, tempA);

		XMFLOAT3 segmentClosest = ClosestPoint(LineSegment(tempA, tempB), obbClosest);

		XMVECTOR closestOBB = XMLoadFloat3(&obbClosest);
		XMVECTOR closestLine = XMLoadFloat3(&segmentClosest);

		XMVECTOR tempDistVec = XMVectorSubtract(closestOBB, closestLine);
		float tempDistanceSq = XMVectorGetX(XMVector3Dot(tempDistVec, tempDistVec));
		
		// Compute distance vector
		if (tempDistanceSq < distanceSq)
		{
			distVec = tempDistVec;
			distanceSq = tempDistanceSq;
		}
	}

	// Handle issue when distance is 0
	if (distanceSq == 0)
	{
		// Determin normal from OBB
		XMVECTOR bestNormal = XMVectorZero();
		float maxDot = -FLT_MAX;

		for (int i = 0; i < 3; ++i)
		{
			XMVECTOR axis = XMVector3Normalize(XMLoadFloat3(&obb.axes[i]));
			float dot = fabsf(XMVectorGetX(XMVector3Dot(capNormal, axis)));

			if (dot > maxDot)
			{
				maxDot = dot;
				bestNormal = axis;
			}
		}

		// Use the best normal as the collision normal
		XMStoreFloat3(&normal, bestNormal);
		depth = c.radius;
		return true;
	}

	// Check distance
	if (distanceSq <= c.radius * c.radius)
	{
		// Normal calculation
		XMVECTOR intersectionNormal = XMVector3Normalize(distVec);
		intersectionNormal = XMVectorNegate(intersectionNormal);
		XMStoreFloat3(&normal, intersectionNormal);

		// Calculate depth
		depth = c.radius - sqrtf(distanceSq);
		return true;
	}

	return false;
}


/********************************************
*		      OBB INTERSECTIONS				*
*********************************************/

bool Collisions::OBBRayIntersection(const OBB &obb, const Ray &ray, float &l, DirectX::XMFLOAT3 &p, DirectX::XMFLOAT3 &normal)
{
	if (RayOBBIntersection(ray, obb, l, p, normal))
	{
		XMStoreFloat3(&normal, XMVectorNegate(XMLoadFloat3(&normal)));
		return true;
	}
	return false;
}

bool Collisions::OBBOBBIntersection(const OBB &obb1, const OBB &obb2, XMFLOAT3 &normal, float &depth)
{
	ZoneScopedC(RandomUniqueColor());

	// Get axes
	XMVECTOR axes[15]{};
	int count = 6;

	for (int i = 0; i < 3; i++)
	{
		axes[i]	  = XMVector3Normalize(XMLoadFloat3(&obb1.axes[i]));
		axes[i+3] = XMVector3Normalize(XMLoadFloat3(&obb2.axes[i]));
	}
    
	// Cross product axes
	bool axisAligned = IsAxisAligned(obb1) && IsAxisAligned(obb2);
	if (!axisAligned)
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
			{
				XMVECTOR crossAxis = XMVector3Cross(axes[i], axes[j+3]);
				float lenSq = XMVectorGetX(XMVector3LengthSq(crossAxis));
				if (lenSq > GameMath::EPSILON && count < 15)
					axes[count++] = XMVectorScale(crossAxis, 1.0f/sqrtf(lenSq));
			}

	depth = FLT_MAX;
	XMVECTOR tempNormal = XMVectorZero(),
			 center1 = XMLoadFloat3(&obb1.center),
			 center2 = XMLoadFloat3(&obb2.center);

	int tempAxisIndex = -1;

	for (int i = 0; i < count; i++)
	{
		// Project obb to current axis
		float min1, max1, min2, max2;
		ProjectOntoAxis(obb1, axes[i], min1, max1);
		ProjectOntoAxis(obb2, axes[i], min2, max2);

		// Check if there are planes not intersecting
		constexpr float EPS = 1e-5f;
		if (max1 < min2 || max2 < min1)
			return false;

		bool isFaceAxis = (i < 6);
		// Get overlap
		float overlap = MIN(max1, max2) - MAX(min1, min2);
		if (overlap < depth && i < 6)
		{
			depth = overlap;
			tempAxisIndex = i;

			// Project Center to axis
			XMVECTOR dir = XMVectorSubtract(center1, center2);
			tempNormal = axes[i];
			if (XMVectorGetX(XMVector3Dot(dir, axes[i])) < 0.0f)
				tempNormal = XMVectorNegate(axes[i]);
		}
    }

	if (XMVector3Equal(tempNormal, XMVectorZero()))
		return false;

	if (tempAxisIndex >= 6) // TODO: Consider if neccessary
		depth = 0.0f;

	DX::XMStoreFloat3(&normal, DX::XMVector3Normalize(tempNormal));
	return true;
}

bool Collisions::OBBSphereIntersection(const OBB &obb, const Sphere &s, XMFLOAT3 &normal, float &depth)
{
	bool res = SphereOBBIntersection(s, obb, normal, depth);
	if (res)
		XMStoreFloat3(&normal, XMVectorNegate(XMLoadFloat3(&normal)));
	return res;
}

bool Collisions::OBBCapsuleIntersection(const OBB &obb, const Capsule &c, DirectX::XMFLOAT3 &normal, float &depth)
{
	bool res = CapsuleOBBIntersection(c, obb, normal, depth);
	if (res)
		XMStoreFloat3(&normal, XMVectorNegate(XMLoadFloat3(&normal)));
	return res;
}

bool Collisions::OBBAABBIntersection(const OBB &obb, const AABB &aabb, DirectX::XMFLOAT3 &normal, float &depth)
{
	bool res = AABBOBBIntersection(aabb, obb, normal, depth);
	if (res)
		XMStoreFloat3(&normal, XMVectorNegate(XMLoadFloat3(&normal)));
	return res;
}


/********************************************
*		      AABB INTERSECTIONS			*
*********************************************/

bool Collisions::AABBRayIntersection(const AABB &aabb, const Ray &ray, float &l, DirectX::XMFLOAT3 &p, DirectX::XMFLOAT3 &normal)
{
	if (RayOBBIntersection(ray, aabb, l, p, normal))
	{
		XMStoreFloat3(&normal, XMVectorNegate(XMLoadFloat3(&normal)));
		return true;
	}
	return false;
}

bool Collisions::AABBAABBIntersection(const AABB &aabb1, const AABB &aabb2, XMFLOAT3 &normal, float &depth)
{
	// TODO: Optimize this code
	XMFLOAT3 aMin = GetMin(aabb1);
	XMFLOAT3 aMax = GetMax(aabb1);
	XMFLOAT3 bMin = GetMin(aabb2);
	XMFLOAT3 bMax = GetMax(aabb2);

	if ((aMin.x <= bMax.x && aMax.x >= bMin.x) &&
		(aMin.y <= bMax.y && aMax.y >= bMin.y) &&
		(aMin.z <= bMax.z && aMax.z >= bMin.z))
		return OBBOBBIntersection(OBB(aabb1), OBB(aabb2), normal, depth);

	return false;
}

bool Collisions::AABBSphereIntersection(const AABB &aabb, const Sphere &s, DirectX::XMFLOAT3 &normal, float &depth)
{
	// TODO: Optimize this code
	return OBBSphereIntersection(OBB(aabb), s, normal, depth);
}

bool Collisions::AABBCapsuleIntersection(const AABB &aabb, const Capsule &c, DirectX::XMFLOAT3 &normal, float &depth)
{
	// TODO: Optimize this code
	return OBBCapsuleIntersection(OBB(aabb), c, normal, depth);
}

bool Collisions::AABBOBBIntersection(const AABB &aabb, const OBB &obb, DirectX::XMFLOAT3 &normal, float &depth)
{
	// TODO: Optimize this code
	return OBBOBBIntersection(OBB(aabb), obb, normal, depth);
}

