#include "Enemy.h"
//#include "Player.h"
#include "GameScene.h"

void Enemy::Initialize(KamataEngine::Camera* camera, const Vector3& position, EnemyType type) {
	camera_ = camera;
	enemyType_ = type;

	enemyTransform_.Initialize();

	enemyTransform_.scale_ = { 0.25f, 0.25f, 0.25f };
	//enemyTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f; // 90度をラジアンに変換
	enemyTransform_.rotation_ = { 0.0f, 3.0f, 0.0f };
	enemyTransform_.translation_ = position;

	//enemy1Model_ = Model::CreateFromOBJ("enemy1", true);

	velocity_ = { 0.0f, 0.0f, 0.0f };
	onGround_ = false;
	walkTimer_ = 0.0f;
	behavior_ = Behavior::kWalk;
	deathPhase_ = DeathPhase::kBefore;
	isDead_ = false;
	isCollisionDisabled_ = false;
}

void Enemy::Update() {
	if (behaviorRequest_ != Behavior::kUnknown) {
		behavior_ = behaviorRequest_;
		behaviorRequest_ = Behavior::kUnknown;

		switch (behavior_) {
		case Behavior::kWalk:
			BehaviorWalkInitialize();
			break;
		case Behavior::kDeath:
			BehaviorDeathInitialize();
			break;
		default:
			break;
		}
	}

	switch (behavior_) {
	case Behavior::kWalk:
		BehaviorWalkUpdate();
		break;
	case Behavior::kDeath:
		BehaviorDeathUpdate();
		break;
	default:
		break;
	}
}

void Enemy::Draw(KamataEngine::Camera& camera) {
	if (!enemy1Model_) return;

	enemy1Model_->Draw(enemyTransform_, camera);
}

void Enemy::OnCollision(const Player* player, GameScene* gamescene) {
	if (behavior_ == Behavior::kDeath || isCollisionDisabled_) { return; }
	if (!player->IsAttack() || player->attackPhase_ != Player::AttackPhase::kRush) return;

	bool canKill = true;

	if (enemyType_ == EnemyType::Shield) {
		bool playerAttackingRight = (player->GetLRDirection() == Player::LRDirection::kRight);
		bool isFrontHit = false;

		if (lrDirection_ == LRDirection::kRight){
			isFrontHit = !playerAttackingRight; // front is right side
		}
		else {
			isFrontHit = playerAttackingRight;	// front is left side
		}
		if (isFrontHit) {
			canKill = false;	//blocked
		}
	}

	if (canKill) {
		isCollisionDisabled_ = true;
		behaviorRequest_ = Behavior::kDeath;

		Vector3 effectPos = {
			(enemyTransform_.translation_.x + player->GetTransform().translation_.x) * 0.5f,
			(enemyTransform_.translation_.y + player->GetTransform().translation_.y) * 0.5f,
			(enemyTransform_.translation_.z + player->GetTransform().translation_.z) * 0.5f
		};
		gamescene->CreateHitEffect(effectPos);
	}
}

void Enemy::MapCollision() {
	if (!mapChipField_) return;

	float blank = 0.01f;

	// replace with similar code as Player class
	MapChip::IndexSet index = mapChipField_->GetMapChipIndexSetByPosition(enemyTransform_.translation_);
	MapChipType type = mapChipField_->GetMapChipTypeByIndex(index.xIndex, index.yIndex);

	// Floor check
	if (type == MapChipType::kBlock || type == MapChipType::kSlope1L || type == MapChipType::kSlope1R) {
		float surfaceY = (type == MapChipType::kBlock)
			? mapChipField_->GetRectByIndex(index.xIndex, index.yIndex).top
			//: mapChipField_->GetSurfaceY(index.xIndex, index.yIndex, enemyTransform_.translation_.x);
			: mapChipField_->GetSlopeSurfaceY(index.xIndex, index.yIndex, enemyTransform_.translation_.x);
		float bottom = enemyTransform_.translation_.y - kHeight * 0.5f;
		if (bottom <= surfaceY + blank) {
			enemyTransform_.translation_.y = surfaceY + kHeight * 0.5f + blank;
			velocity_.y = 0.0f;
			onGround_ = true;
		}
	}

	// Wall check
}

void Enemy::BehaviorWalkInitialize() {
	deathPhase_ = DeathPhase::kBefore;
	isCollisionDisabled_ = false;
	onGround_ = false;
}

void Enemy::BehaviorWalkUpdate() {
	if (velocity_.x < -0.001f) {
		lrDirection_ = LRDirection::kLeft;
	}
	else if (velocity_.x > 0.001f) {
		lrDirection_ = LRDirection::kRight;
	}

	walkTimer_ += 1.0f / 60.0f;

	float param = std::cosf(2.0f * std::numbers::pi_v<float> *(walkTimer_ / 30.0f));
	float angle = 60.0f + 30.0f * (param + 1.0f) * 0.5f;						// 60° → 90°
	float zTilt = std::cosf(angle * std::numbers::pi_v<float> / 180.0f);

	enemyTransform_.rotation_ = { 0.0f, 3.0f, -velocity_.x * zTilt * 2.0f };

	if (!onGround_) {
		velocity_.y -= kGravity;
	}
	else {
		velocity_.y = 0.0f;
	}

	velocity_.x = (lrDirection_ == LRDirection::kLeft) ? -kWalkSpeed_ : kWalkSpeed_;

	enemyTransform_.translation_.x += velocity_.x;
	enemyTransform_.translation_.y += velocity_.y;
	enemyTransform_.translation_.z += velocity_.z;

	worldUpdater_.Update(enemyTransform_);
}

void Enemy::BehaviorDeathInitialize() {
	deathParameter_ = 0;
	deathPhase_ = DeathPhase::kBefore;
	velocity_ = {};
}

void Enemy::BehaviorDeathUpdate() {

	switch (deathPhase_) {

	case DeathPhase::kBefore:
	default:
		deathPhase_ = DeathPhase::kAnimation;
		deathParameter_ = 0;
		break;
	case DeathPhase::kAnimation: {
		deathParameter_++;

		// tumble animation
		enemyTransform_.rotation_.y += std::numbers::pi_v<float> / 8.0f;
		enemyTransform_.rotation_.x += std::numbers::pi_v<float> / 8.0f;

		// shrink
		float t = static_cast<float>(deathParameter_) / animDuration_;
		float scale = 0.25f * (1.0f - t * 0.5f);
		enemyTransform_.scale_ = { scale, scale, scale };

		worldUpdater_.Update(enemyTransform_);

		if (deathParameter_ >= static_cast<uint32_t>(animDuration_)) {
			isDead_ = true;
			deathParameter_ = 0;
			deathPhase_ = DeathPhase::kDisappear;
		}
		break;
	}

	case DeathPhase::kDisappear: {
		deathParameter_++;

		isCollisionDisabled_ = false;
		if (deathParameter_ >= (lingerDuration_)) {
			deathParameter_ = 0;
		}
		break;
	}
	}
}

Vector3 Enemy::GetWorldPosition() {

	Vector3 worldPos;

	worldPos.x = enemyTransform_.matWorld_.m[3][0];
	;
	worldPos.y = enemyTransform_.matWorld_.m[3][1];
	;
	worldPos.z = enemyTransform_.matWorld_.m[3][2];
	;

	return worldPos;
}

WorldUpdater::AABB Enemy::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	WorldUpdater::AABB aabb;

	aabb.min = { worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f };
	aabb.max = { worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f };

	return aabb;
}

Enemy::Enemy() {}

Enemy::~Enemy() {
	delete enemy1Model_;
}
