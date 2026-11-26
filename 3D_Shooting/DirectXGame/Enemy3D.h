#pragma once
#include "WorldUpdate.h"
#include "MapChip3D.h"
#include <3d/Camera.h>
#include "Player3D.h"

using namespace KamataEngine;

class Player3D;		// forward
class GameScene;	// forward

class Enemy3D {

public:
	Enemy3D();
	~Enemy3D();

	enum class Behavior {
		kWalk,
		kUnknown,
		kAlert,
		kDeath,
	};

	enum class DeathPhase {
		kBefore,
		kAnimation,
		kLayThere,
		kDisappear,
	};

	enum class LookDirection {
		kRight,
		kLeft,
		kDown,
		kUp,
	};

	LookDirection lookingDirection_ = LookDirection::kLeft;

	void Initialize(Camera* camera, const Vector3& position);
	void Update();
	void Draw(Camera& camera);
	void OnCollision(const Player3D* player, GameScene* gamescene);
	void SetMapChipField(MapChip3D* mapChipField);

	//=========================================================================
	// Behavior
	//=========================================================================
	void BehaviorWalkInitialize();
	void BehaviorWalkUpdate();
	void BehaviorAlertInitialize();
	void BehaviorAlertUpdate();
	void BehaviorDeathInitialize();
	void BehaviorDeathUpdate();

	Behavior behavior_ = Behavior::kWalk;
	Behavior behaviorRequest_ = Behavior::kUnknown;
	DeathPhase deathPhase_ = DeathPhase::kBefore;

	bool IsDead() const { return isDead_; }
	bool IsCollisionDisabled() const { return isCollisionDisabled_; }

	Vector3 GetWorldPosition();
	AABB GetAABB();

private:
	//========================================================================
	// Entities
	//========================================================================
	WorldUpdate worldUpdater_;
	WorldTransform enemyTransform_;
	Camera* camera_;
	MapChip3D* mapChipField_ = nullptr;

	//========================================================================
	// Models 
	//========================================================================
	Model* enemyModel_;

	//=========================================================================
	// Behavior variables
	//=========================================================================
	Vector3 velocity_ = { 0.0f, 0.0f, 0.0f };
	ObjectColor objectColor_;
	Vector4 enemyColor_ = { 0.0f, 0.0f, 0.0f };
	float kWalkSpeed_ = 0.05f;
	float walkTimer_ = 0.0f;
	float animDuration_ = 20.0f;
	float lingerDuration_ = 20.0f;
	float pi = std::numbers::pi_v<float>;

	uint32_t deathParameter_ = 0;

	//=========================================================================
	// Constants
	//=========================================================================
	static inline const float kWalkMotionAngleStart = 60.0f;
	static inline const float kWalkMotionAngleEnd = 90.0f;
	static inline const float kWalkMotionTime = 1.0f;
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	//=========================================================================
	// Flags
	//=========================================================================
	bool isDead_ = false;
	bool isCollisionDisabled_ = false;
	
};

