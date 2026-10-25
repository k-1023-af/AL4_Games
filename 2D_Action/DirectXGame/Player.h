#pragma once
#include "MapChip.h"
#include "Function.h"
#include "WorldUpdater.h"
#include "KamataEngine.h"
#define NOMINMAX
#include <Windows.h>
#include <algorithm>
#include <numbers>
#include <array>

using namespace KamataEngine;

class MapChip;
class Enemy;

class Player {

public:
	struct CollisionMapInfo {
		bool ceilingFlag = false;
		bool landingFlag = false;
		bool wallFlagL = false;
		bool wallFlagR = false;
		bool onSlopeL = false;
		bool onSlopeR = false;
		Vector3 move;
	};

	enum Corner {
		kRightBottom, // (+kWidth/2.0f, -kHeight/2.0f, 0.0f)
		kLeftBottom,  // (-kWidth/2.0f, -kHeight/2.0f, 0.0f)
		kRightTop,    // (+kWidth/2.0f, +kHeight/2.0f, 0.0f)
		kLeftTop,     // (-kWidth/2.0f, +kHeight/2.0f, 0.0f)

		kNumCorner, // Offset number
	};

	enum class Behavior {
		kRoot,
		kUnknown,
		kAttack,
	};

	enum class AttackPhase {
		kAim,
		kCoil,
		kRush,
		kLinger,
	};

	enum class LRDirection {
		kLeft,
		kRight,
		kNone
	};

	void Initialize(Camera* camera, const Vector3& position);
	void Update();
	void Draw();

	Vector3 GetCornerPosition(const Vector3& center, Corner corner);
	Vector3 GetWorldPosition(); 
	//Vector3 GetRotation();
	WorldUpdater::AABB GetAABB();

	Vector3 GetScale();
	LRDirection GetLRDirection() const { return lrDirection_; }

	void SetMapChipField(MapChip* mapChipField);
	void MapCollisionX(CollisionMapInfo& info);
	void MapCollisionY(CollisionMapInfo& info);
	void EnemyCollision(Enemy* enemy);

	void BehaviorInitialize();
	void BehaviorUpdate();
	void BehaviorAttackInitialize();
	void BehaviorAttackUpdate();
	
	Player();
	~Player();

	LRDirection lrDirection_ = LRDirection::kRight;

	Vector3 velocity_ = { 0.0f, 0.0f, 0.0f };

	//Vector3 acceleration_ = { 0.0f, kGravityAcceleration, 0.0f };

	Behavior behavior_ = Behavior::kRoot;
	Behavior behaviorRequest_ = Behavior::kUnknown;
	AttackPhase attackPhase_ = AttackPhase::kCoil;

	const KamataEngine::WorldTransform& GetTransform() const { return playerTransform_; }
	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }

	bool IsDead() const { return isDead_; }
	bool IsAttack() const { return isAttack_; }
	bool IsPSpeed() const { return isPSpeed_; }

private:
	WorldUpdater worldUpdater_;
	WorldTransform playerTransform_;
	WorldTransform worldTransformAttack_;
	Model* playerModel_ = nullptr;
	Model* modelAttack_ = nullptr;

	ObjectColor objectColor_;          
	Vector4 normalColor_	= { 1.0f, 1.0f, 1.0f, 1.0f };		// white
	Vector4 pSpeedColor_	= { 1.0f, 0.95f, 0.15f, 1.0f };		// bright yellow
	Vector4 attackColor_	= { 1.0f, 0.15f, 0.15f, 1.0f };		// bright red
	Vector4 aimColor_		= { 0.3f, 1.0f, 1.0f, 1.0f };		// blue

	Camera* camera_ = nullptr;
	MapChip* mapChipField_ = nullptr;

	static inline const float kAttenuation			= 0.008f;
	static inline const float kAttenuationLanding	= 0.0025f;
	static inline const float kLimitRunSpeed		= 0.15f;
	static inline const float kLimitFallSpeed		= 0.2f;
	static inline const float kRunAcceleration		= 0.0025f;
	static inline const float kAcceleration			= 0.0018f;
	static inline const float kJumpAcceleration		= 0.95f;
	static inline const float kGravityAcceleration	= 0.1f;

	static inline const float kLimitPSpeed		= 4.2f;
	static inline const float kPMeterFillRate	= 0.025f;
	static inline const float kPMeterDrainRate	= 0.008f;
	static inline const float kPSpeedDuration	= 180.0f;
	static inline const float kPSpeedThreshold	= 0.98f;

	static inline const float kWidth	= 0.8f;
	static inline const float kHeight	= 0.8f;
	static inline const float kBlank	= { 0.01f }; //numerical value

	static inline const float kTimeTurn = 0.25f;
	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;

	//------- Attack -----------------------------
	
	uint32_t attackParameter_ = 0;
	float coilDuration_		= 1.0f;
	float rushDuration_		= 50.0f;
	float lingerDuration_	= 2.0f;
	float chargePower_		= 0.0f;
	float attackVelocity_	= 0.25f;

	const float attackOffset	= 0.1f; // tweak to taste
	const int vSteps			= 24;
	const int hSteps			= 36;

	float frameCount = 0.0f;
	//float attackTime_ = 10.0f;

	//---------------------------------------------

	bool isPSpeed_		= false;
	bool onGround_		= false;
	bool onSlope_		= false;
	bool canDash_		= false;
	bool canAirDash_	= false;
	bool isAttack_		= false;
	bool isDead_		= false;
	bool isAiming_		= false;

	float pMeter_		= 0.0f;
	float pSpeedTimer_	= 0.0f;
	float aimAngle_		= 0.0f;
	Vector3 aimDirection_{ 1.0f, 0.0f, 0.0f };
	static inline const float kArrowLength = 2.8f;
};

