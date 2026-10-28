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
	float GetPMeter() const { return pMeter_; }
	void OnDamaged(const Vector3& hitPos);
	bool IsInvincible() const { return isInvincible_; }

	void SetMapChipField(MapChip* mapChipField);
	void MapCollisionX(CollisionMapInfo& info);
	void MapCollisionY(CollisionMapInfo& info);
	void EnemyCollision(Enemy* enemy);

	void BehaviorInitialize();
	void BehaviorUpdate();
	void BehaviorAttackInitialize();
	void BehaviorAttackUpdate();

	void UpdateRunningAnimation();
	void BounceFromStomp();


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
	bool CanAirDash() const { return canAirDash_ && canDash_ && isPSpeed_; }
	

private:
	WorldUpdater worldUpdater_;
	WorldTransform playerTransform_;
	WorldTransform worldTransformAttack_;

	WorldTransform headWT_;
	WorldTransform rightArmWT_;
	WorldTransform leftArmWT_;
	WorldTransform rightLegWT_;
	WorldTransform leftLegWT_;

	Model* playerModel_ = nullptr;
	Model* headModel_ = nullptr;
	Model* rightArmModel_ = nullptr;
	Model* leftArmModel_ = nullptr;
	Model* rightLegModel_ = nullptr;
	Model* leftLegModel_ = nullptr;
	Model* modelAttack_ = nullptr;

	ObjectColor objectColor_;       
	Vector4 currentColor_	= { 1.0f, 1.0f, 1.0f, 1.0f };
	Vector4 normalColor_	= { 1.0f, 1.0f, 1.0f, 1.0f };		// white
	Vector4 pSpeedColor_	= { 1.0f, 0.95f, 0.15f, 1.0f };		// bright yellow
	Vector4 attackColor_	= { 1.0f, 0.15f, 0.15f, 1.0f };		// bright red
	Vector4 aimColor_		= { 0.3f, 1.0f, 1.0f, 1.0f };		// blue

	Camera* camera_ = nullptr;
	MapChip* mapChipField_ = nullptr;

	static inline const float kAttenuation			= 0.015f;
	static inline const float kAttenuationLanding	= 0.0055f;
	static inline const float kLimitRunSpeed		= 0.15f;
	static inline const float kLimitFallSpeed		= 0.2f;
	static inline const float kBoostAcceleration	= 0.00325f;
	static inline const float kRunAcceleration		= 0.0028f;
	static inline const float kAcceleration			= 0.0026f;
	static inline const float kJumpAcceleration		= 0.95f;
	static inline const float kGravityAcceleration	= 0.1f;

	static inline const float kLimitPSpeed		= 4.2f;
	static inline const float kPMeterFillRate	= 0.012f;
	static inline const float kPMeterDrainRate	= 0.008f;
	static inline const float kPSpeedDuration	= 180.0f;
	static inline const float kPSpeedThreshold	= 0.98f;
	static inline const float kBoostThreshold = kPSpeedThreshold * 2.0f;

	static inline const float kWidth	= 0.8f;
	static inline const float kHeight	= 0.8f;
	static inline const float kBlank	= { 0.01f }; //numerical value

	static inline const float kTimeTurn = 0.2f;
	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;

	//------- Attack -----------------------------
	
	uint32_t attackParameter_ = 0;
	float coilDuration_		= 1.0f;
	float rushDuration_		= 40.0f;
	float lingerDuration_	= 2.0f;
	float chargePower_		= 0.0f;
	float attackVelocity_	= 0.3f;

	const float attackOffset	= 0.1f; // tweak to taste
	const int vSteps			= 16;
	const int hSteps			= 24;

	float frameCount = 0.0f;
	//float attackTime_ = 10.0f;

	//---------------------------------------------

	bool isBoost_		= false;
	bool isPSpeed_		= false;
	bool onGround_		= false;
	bool onSlope_		= false;
	bool canDash_		= false;
	bool canAirDash_	= false;
	bool isAttack_		= false;
	bool isDead_		= false;
	bool isAiming_		= false;
	bool isInvincible_	= false;
	bool hasPlayedAirDashReadySound_ = false;

	//--- Running animation -----------------

	float runTimer_			= 0.0f;
	float runCycleSpeed_	= 0.35f;
	float invincibleTimer_	= 0.0f;

	const float kInvincibleTime = 90.0f;

	float armAngle			= 0.0f;
	float armAngleOpp		= 0.0f;
	float legAngle			= 0.0f;
	float legAngleOpp		= 0.0f;
	float bodyBob			= 0.0f;
	float pMeter_			= 0.0f;
	float pSpeedTimer_		= 0.0f;
	float aimAngle_			= 0.0f;
	Vector3 aimDirection_{ 1.0f, 0.0f, 0.0f };
	static inline const float kArrowLength = 2.8f;

	uint32_t airDashReadySoundHandle_ = 0;
	uint32_t bounceSoundHandle_ = 0;
	uint32_t hitSoundHandle_ = 0;
	uint32_t jumpSoundHandle_ = 0;
	uint32_t dashAttackSoundHandle_ = 0;
};

