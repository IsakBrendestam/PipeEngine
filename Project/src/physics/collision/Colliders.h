#pragma once

#include "core/EngineSettings.h"
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <string>
#include <functional>

class Entity;
class Scene;
class Content;

namespace Collisions
{
	struct CollisionData;

	enum ColliderTypes
	{
		RAY_COLLIDER,
		SPHERE_COLLIDER,
		CAPSULE_COLLIDER,
		AABB_COLLIDER,
		OBB_COLLIDER,
		TERRAIN_COLLIDER,

		COLLIDER_TYPES_COUNT,
		NULL_COLLIDER
	};

	enum ColliderTags
	{
		NULL_TAG = 0x0,
		GROUND_TAG = 0x1,
		OBJECT_TAG = 0x2,
		SKIP_TERRAIN_TAG = 0x4,
		STATIC_TAG = 0x8,
		TEMP4_TAG = 0x10,
		TEMP5_TAG = 0x20,
		TEMP6_TAG = 0x40,
		TEMP7_TAG = 0x80
	};

	struct Collider 
	{
		Collider() = default;
		Collider(ColliderTypes type, const std::string &debugMeshName, ColliderTags tag);
		 
		virtual ~Collider();

		virtual void Transform(DirectX::XMMATRIX M, Collider *transformed) = 0;

#ifdef USE_IMGUI
		[[nodiscard]] virtual bool RenderUI();
#endif
#ifdef DEBUG_BUILD
		[[nodiscard]] bool SetDebugEnt(Scene* scene, const Content* content);
		[[nodiscard]] virtual bool RenderDebug(Scene* scene, const RenderQueuer &queuer, const RendererInfo &renderInfo);

		virtual bool TransformDebug(ID3D11DeviceContext *context, Time &time, const Input &input) = 0;
#endif

		void AddIntersectionCallback(std::function<void(const CollisionData&)> callback);
		void AddOnCollisionEnterCallback(std::function<void(const CollisionData&)> callback);
		void AddOnCollisionExitCallback(std::function<void()> callback);

		void Intersection(const CollisionData &data) const;
		void OnCollisionEnter(const CollisionData &data) const;
		void OnCollisionExit() const;

		DirectX::XMFLOAT3 GetMin() const;
		DirectX::XMFLOAT3 GetMax() const;

		void SetTag(ColliderTags tag);
		void RemoveTag(ColliderTags tag);
		bool HasTag(ColliderTags tag) const;
		uint8_t GetTag() const;

		void SetActive(bool status);
		bool GetActive() const;

		void SetDirty();
		bool GetDirty() const;

		void SetIntersecting(bool value);
		bool GetIntersecting() const;

		void CheckDone();
		void SetToCheck();
		bool GetCheck() const;

#ifdef DEBUG_BUILD
		bool debug = true;
		bool haveDebugEnt = false;
#endif
		ColliderTypes colliderType = NULL_COLLIDER;

	protected:
#ifdef DEBUG_BUILD
		Entity* _debugEnt = nullptr;
		std::string _debugMeshName;
#endif
		bool _isDirty;
		bool _active;
		bool m_isIntersecting;
		bool m_toCheck;

		DirectX::XMFLOAT3 _min, _max;

		uint8_t _tagFlag = 0;

		// Serializes the behaviour to a string.
		[[nodiscard]] virtual bool Serialize(std::string *code) const;

		// Deserializes the behaviour from a string.
		[[nodiscard]] virtual bool Deserialize(const std::string &code);

	private:
		std::vector<std::function<void(const CollisionData&)>> _onCollisionEnterCallbacks;
		std::vector<std::function<void()>> _onCollisionExitCallbacks;
		std::vector<std::function<void(const CollisionData&)>> _intersectionCallbacks;
	};

	struct Ray: public Collider
	{
		DirectX::XMFLOAT3 origin;
		DirectX::XMFLOAT3 dir;
		float length;

		Ray() = default;
		Ray(const Ray &other);
		Ray(const DirectX::XMFLOAT3 &o, const DirectX::XMFLOAT3 &d, float l, ColliderTags tag = NULL_TAG);

#ifdef USE_IMGUI
		[[nodiscard]] bool RenderUI() override;
#endif


#ifdef DEBUG_BUILD
		[[nodiscard]] bool RenderDebug(Scene* scene, const RenderQueuer &queuer, const RendererInfo &renderInfo) override;
		void DebugRayRender(Scene *scene, DirectX::XMFLOAT4 color = {0, 1, 0, 1}) const;
#endif

		void Transform(DirectX::XMMATRIX M, Collider *transformed) override;
#ifdef DEBUG_BUILD
		bool TransformDebug(ID3D11DeviceContext* context, Time &time, const Input &input) override;
#endif
	};

	struct Sphere: public Collider 
	{
		DirectX::XMFLOAT3 center;
		float radius;

		Sphere() = default;
		Sphere(const Sphere &other);
		Sphere(const DirectX::XMFLOAT3 &c, float r, ColliderTags tag = NULL_TAG);
		Sphere(const DirectX::BoundingSphere &sphere, ColliderTags tag = NULL_TAG);

#ifdef USE_IMGUI
		[[nodiscard]] bool RenderUI() override;
#endif

		void Transform(DirectX::XMMATRIX M, Collider *transformed) override;
#ifdef DEBUG_BUILD
		bool TransformDebug(ID3D11DeviceContext* context, Time &time, const Input &input) override;
#endif
	};

	struct Capsule: public Collider
	{
		DirectX::XMFLOAT3 center, upDir;
		float radius, height;

		Capsule() = default;
		Capsule(const Capsule &other);
		Capsule(const DirectX::XMFLOAT3 &c, const DirectX::XMFLOAT3 &u, float r, float h, ColliderTags tag = NULL_TAG);

#ifdef USE_IMGUI
		[[nodiscard]] bool RenderUI() override;
#endif

		void Transform(DirectX::XMMATRIX M, Collider *transformed) override;
#ifdef DEBUG_BUILD
		bool TransformDebug(ID3D11DeviceContext* context, Time &time, const Input &input) override;
#endif
	};

	struct AABB: public Collider
	{
		DirectX::XMFLOAT3 center, halfLength;

		AABB() = default;
		AABB(const AABB &other);
		AABB(const DirectX::XMFLOAT3 &c, const DirectX::XMFLOAT3 &hl, ColliderTags tag = NULL_TAG);
		AABB(const DirectX::BoundingBox &aabb, ColliderTags tag = NULL_TAG);
		AABB(const DirectX::BoundingOrientedBox &obb, ColliderTags tag = NULL_TAG);

#ifdef USE_IMGUI
		[[nodiscard]] bool RenderUI() override;
#endif

		void Transform(DirectX::XMMATRIX M, Collider *transformed) override;
#ifdef DEBUG_BUILD
		bool TransformDebug(ID3D11DeviceContext* context, Time &time, const Input &input) override;
#endif
	};

	struct OBB: public Collider 
	{
		DirectX::XMFLOAT3 center, halfLength, axes[3];

		OBB() = default;
		OBB(const OBB &other);
		OBB(const DirectX::XMFLOAT3 &c, const DirectX::XMFLOAT3 &hl, const DirectX::XMFLOAT3 a[3], ColliderTags tag = NULL_TAG);
		OBB(const DirectX::BoundingOrientedBox &obb, ColliderTags tag = NULL_TAG);
		OBB(const AABB &aabb, ColliderTags tag = NULL_TAG);

#ifdef USE_IMGUI
		[[nodiscard]] bool RenderUI() override;
#endif

		void Transform(DirectX::XMMATRIX M, Collider *transformed) override;
#ifdef DEBUG_BUILD
		bool TransformDebug(ID3D11DeviceContext* context, Time &time, const Input &input) override;
#endif
	};
}
