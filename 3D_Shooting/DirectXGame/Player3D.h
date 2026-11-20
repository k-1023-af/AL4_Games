#pragma once
#include "Formula.h"
#include "WorldUpdate.h"
#include "KamataEngine.h"
#define NOMINMAX
#include "Windows.h"
#include <algorithm>
#include <numbers>
#include <array>
#include <vector>

using namespace KamataEngine;

class MapChip3D;
class Enemy3D;

class Player3D {
public:
	Player3D();
	~Player3D();

	struct CollisionMapInfo {
		//bool landingFlag = false;
		bool xWallLeft = false;
		bool xWallRight = false;
		bool zWallFront = false;
		bool zWallBack = false;
		Vector3 move;
	};

	enum Corner {
		//kRightBottom,	// (+kWidth/2.0f, -kHeight/2.0f, 0.0f)
		//kLeftBottom,	// (-kWidth/2.0f, -kHeight/2.0f, 0.0f)
		//kRightTop,	// (+kWidth/2.0f, +kHeight/2.0f, 0.0f)
		//kLeftTop,		// (-kWidth/2.0f, +kHeight/2.0f, 0.0f)

		kRightFront,	// (+kWidth/2.0f, 0.0f, -kDepth/2.0f)
		kLeftFront,		// (-kWidth/2.0f, 0.0f, -kDepth/2.0f)
		kRightBack,		// (+kWidth/2.0f, 0.0f, +kDepth/2.0f)
		kLeftBack,		// (-kWidth/2.0f, 0.0f, +kDepth/2.0f)

		//kBottomFront,	// (0.0f, -kHeight/2.0f, -kDepth/2.0f)
		//kTopFront,	// (0.0f, +kHeight/2.0f, -kDepth/2.0f)
		//kBottomBack,	// (0.0f, -kHeight/2.0f, +kDepth/2.0f)
		//kTopBack,		// (0.0f, +kHeight/2.0f, +kDepth/2.0f)

		kNumCorner,		// Offset number
	};

	enum class LookDirection {
		kLeft,
		kRight,
		kFront,
		kBack,
		kNone
	};

	void Initialize(Camera* camera, const Vector3& position);
	void Update();
	void Draw(Camera& camera);

	//==============================================================================
	//  Getters
	//==============================================================================
	AABB GetAABB();
	Vector3 GetCornerPosition(const Vector3& center, Corner corner);
	Vector3 GetWorldPosition();
	Vector3 GetScale();
	Vector3 GetRotation();
	LookDirection GetLookDirection() const { return lookDirection_;}
	bool IsInvinvible() const { return isInvincible_; }
	const WorldTransform& GetTransform() const { return playerTransform_; }
	WorldTransform& GetTransform() { return playerTransform_; }
	const Vector3& GetVelocity() const { return velocity_; }

	bool IsDead() const { return isDead_; }
	bool IsAttack() const { return isAttack_; }

	//==============================================================================
	//  Helper Functions
	//==============================================================================
	void SetMapChipField(MapChip3D* mapChipField);
	void MapCollisionX(CollisionMapInfo& info);
	//void MapCollisionY(CollisionMapInfo& info);
	void MapCollisionZ(CollisionMapInfo& info);
	bool ResolveCornerCollision(const Vector3& oldPos, const Vector3& newPos, bool checkX, bool checkZ);
	void EnemyCollision(Enemy3D* enemy);

	//void BehaviorInitialize();
	//void BehaviorUpdate();
	//void BehaviorAttackInitialize();
	//void BehaviorAttackUpdate();

	//==============================================================================
	//  Variables
	//==============================================================================

	LookDirection lookDirection_ = LookDirection::kFront;

	Vector3 velocity_ = { 0.0f, 0.0f, 0.0f };
	//Vector3 acceleration_ = { 0.0f, kGravityAcceleration, 0.0f };

private:
	//==============================================================================
	//  Entities
	//==============================================================================
	WorldUpdate worldUpdater_;
	MapChip3D* mapChipField_ = nullptr;
	Camera* camera_ = nullptr;
	WorldTransform playerTransform_;
	WorldTransform wtAttack_;

	//==============================================================================
	//  Body Transforms + Models
	//==============================================================================
	Model* playerModel_ = nullptr;
	Model* modelAttack_ = nullptr;

	//==============================================================================
	//  Color
	//==============================================================================
	ObjectColor objectColor_;
	Vector4 currentColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	Vector4 normalColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	Vector4 aimColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	//==============================================================================
	//  Constants
	//==============================================================================
	static inline const float kAcceleration = 0.01f;
	static inline const float kLimitRunSpeed = 3.0f;

	static inline const float kAttenuation = 0.15f;

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	static inline const float kDepth = 0.8f;
	static inline const float kBlank = { 0.01f };

	static inline const float kArrowLength = 2.8f;

	//==============================================================================
	//  Flags
	//==============================================================================
	bool isDead_ = false;
	bool isAttack_ = false;
	bool isInvincible_ = false;
	bool isAiming_ = false;

	//==============================================================================
	//  Variables
	//==============================================================================

	float invincibleTimer_ = 0.0f;

	Vector3 aimDirection_ = { 0.0f, 0.0f, -1.0f };
	
};

