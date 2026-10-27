#pragma once
#include "KamataEngine.h"
#include "WorldUpdater.h"
//#include "Player.h"
//#include "GameScene.h"
#include "MapChip.h"
#define NOMINMAX
#include <Windows.h>
#include <algorithm>
#include <numbers>

using namespace KamataEngine;

class Player; // forward
class GameScene; //forward


class Enemy {

public:
	enum class EnemyType {
		Normal,
		Shield, 
	};

	enum class Behavior {
		kWalk,
		kUnknown,
		kDeath,
	};

	enum class DeathPhase {
		kBefore,
		kAnimation,
		kDisappear,
	};

	enum class LRDirection {
		kRight,
		kLeft,
	};

	void Initialize(KamataEngine::Camera* camera, const Vector3& position, EnemyType type);
	void Update();
	void Draw(KamataEngine::Camera& camera);

	void OnCollision(const Player* p, GameScene* gamescene);
	void MapCollision(); //call in GameScene
	void SetMapChipField(MapChip* mapChipField) { mapChipField_ = mapChipField; }

	bool IsDead() const { return isDead_; }
	bool IsCollisionDisabled() const { return isCollisionDisabled_; }

	Vector3 GetWorldPosition();
	WorldUpdater::AABB GetAABB();
	EnemyType GetType() const { return enemyType_; }
	void OnStomped(GameScene* gamescene);

	Enemy();
	~Enemy();

private:
	void BehaviorWalkInitialize();
	void BehaviorWalkUpdate();
	void BehaviorDeathInitialize();
	void BehaviorDeathUpdate();

	//=============================
	// Member Variables
	//=============================

	static Model* modelNormal_;
	static Model* modelShield_;

	Model* model_ = nullptr;
	EnemyType enemyType_ = EnemyType::Normal;

	WorldUpdater worldUpdater_;
	WorldTransform worldTransform_;
	Camera* camera_;
	MapChip* mapChipField_ = nullptr;

	Behavior behavior_ = Behavior::kWalk;
	Behavior behaviorRequest_ = Behavior::kUnknown;
	DeathPhase deathPhase_ = DeathPhase::kBefore;

	LRDirection lrDirection_ = LRDirection::kLeft;
	  
	Vector3 velocity_ = { 0.0f, 0.0f, 0.0f };

	float kWalkSpeed_ = 0.05f;
	float walkTimer_ = 0.0f;

	uint32_t deathParameter_ = 0;
	float animDuration_ = 20.0f;
	float lingerDuration_ = 20.0f;

	bool onGround_ = true;
	bool isDead_ = false;
	bool isCollisionDisabled_ = false;

	static inline const float kLimitWalkSpeed = 0.05f;
	static inline const float kGravity = -0.1f;
	static inline const float kWalkMotionAngleStart = 60.0f;
	static inline const float kWalkMotionAngleEnd = 90.0f;
	static inline const float kWalkMotionTime = 1.0f;
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	uint32_t enemyDefeatSoundHandle_ = 0;
};

