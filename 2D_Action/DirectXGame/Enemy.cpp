#include "Enemy.h"
//#include "Player.h"
#include "GameScene.h"

Model* Enemy::modelNormal_ = nullptr;
Model* Enemy::modelShield_ = nullptr;

void Enemy::Initialize(KamataEngine::Camera* camera, const Vector3& position, EnemyType type) {
	camera_ = camera;
	enemyType_ = type;

	enemyDefeatSoundHandle_ = Audio::GetInstance()->LoadWave("./Resources/sounds/hit.wav");

	if (!modelNormal_) {
		modelNormal_ = Model::CreateFromOBJ("enemy1", true);
	}
	if (!modelShield_) {
		modelShield_ = Model::CreateFromOBJ("enemy2", true);  
	}

	model_ = (enemyType_ == EnemyType::Shield) ? modelShield_ : modelNormal_;

	worldTransform_.Initialize();
	worldTransform_.scale_ = { 0.6f, 0.6f, 0.6f };
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f; // 90度をラジアンに変換
	worldTransform_.translation_ = position;

	//worldTransform_.rotation_ = { 0.0f, 3.0f, 0.0f };
	//worldTransform_.translation_ = position;
	//model_ = Model::CreateFromOBJ("enemy1", true);

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
	if (!model_) return;

	model_->Draw(worldTransform_, camera);
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
		Audio::GetInstance()->PlayWave(enemyDefeatSoundHandle_);

		isCollisionDisabled_ = true;
		behaviorRequest_ = Behavior::kDeath;

		Vector3 effectPos = {
			(worldTransform_.translation_.x + player->GetTransform().translation_.x) * 0.5f,
			(worldTransform_.translation_.y + player->GetTransform().translation_.y) * 0.5f,
			(worldTransform_.translation_.z + player->GetTransform().translation_.z) * 0.5f
		};
		gamescene->CreateHitEffect(effectPos);
	}
}

void Enemy::MapCollision() {
	if (!mapChipField_) return;

	const float blank = 0.01f;
	onGround_ = false;

	//===== Ground / Slope Check =====
	MapChip::IndexSet index = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_);
	MapChipType type = mapChipField_->GetMapChipTypeByIndex(index.xIndex, index.yIndex);

	float surfaceY = -10000.0f;

	if (type == MapChipType::kBlock) {
		surfaceY = mapChipField_->GetRectByIndex(index.xIndex, index.yIndex).top + 0.2f;
	}
	else if (type == MapChipType::kSlope1L || type == MapChipType::kSlope1R ||
		type == MapChipType::kSlope2L || type == MapChipType::kSlope2R ||
		type == MapChipType::kSlope3L || type == MapChipType::kSlope3R) {
		surfaceY = mapChipField_->GetSlopeSurfaceY(index.xIndex, index.yIndex, worldTransform_.translation_.x) + 0.2f;
	}

	float bottom = worldTransform_.translation_.y - kHeight * 0.5f + 0.325f;

	if (surfaceY > -9999.0f && bottom <= surfaceY + blank) {
		worldTransform_.translation_.y = surfaceY + kHeight * 0.5f - 0.4f + blank;
		velocity_.y = 0.0f;
		onGround_ = true;
	}

	//===== Simple Wall Check (left / right) =====
	// Left
	MapChip::IndexSet leftIndex = mapChipField_->GetMapChipIndexSetByPosition(
		{ worldTransform_.translation_.x - kWidth * 0.5f, worldTransform_.translation_.y, 0.0f });
	MapChipType leftType = mapChipField_->GetMapChipTypeByIndex(leftIndex.xIndex, leftIndex.yIndex);

	if (leftType == MapChipType::kBlock) {
		MapChip::Rect rect = mapChipField_->GetRectByIndex(leftIndex.xIndex, leftIndex.yIndex);
		worldTransform_.translation_.x = rect.right + kWidth * 0.5f + blank;
		velocity_.x *= -1.0f;          // turn around
		lrDirection_ = LRDirection::kRight;
	}

	// Right
	MapChip::IndexSet rightIndex = mapChipField_->GetMapChipIndexSetByPosition(
		{ worldTransform_.translation_.x + kWidth * 0.5f, worldTransform_.translation_.y, 0.0f });
	MapChipType rightType = mapChipField_->GetMapChipTypeByIndex(rightIndex.xIndex, rightIndex.yIndex);

	if (rightType == MapChipType::kBlock) {
		MapChip::Rect rect = mapChipField_->GetRectByIndex(rightIndex.xIndex, rightIndex.yIndex);
		worldTransform_.translation_.x = rect.left - kWidth * 0.5f - blank;
		velocity_.x *= -1.0f;          // turn around
		lrDirection_ = LRDirection::kLeft;
	}
}

void Enemy::BehaviorWalkInitialize() {
	deathPhase_ = DeathPhase::kBefore;
	isCollisionDisabled_ = false;
	onGround_ = false;
}

void Enemy::BehaviorWalkUpdate() {
	//===== 1. Update facing direction =====
	if (velocity_.x < -0.001f) {
		lrDirection_ = LRDirection::kLeft;
	}
	else if (velocity_.x > 0.001f) {
		lrDirection_ = LRDirection::kRight;
	}

	//===== 2. Set model rotation (turn around) =====
	if (lrDirection_ == LRDirection::kRight) {
		worldTransform_.rotation_.y = std::numbers::pi_v<float> *1.5f;   // face right
	}
	else {
		worldTransform_.rotation_.y = std::numbers::pi_v<float> *0.5f;   // face left
	}

	// walking tilt (Z axis)
	walkTimer_ += 1.0f / 60.0f;
	float param = std::cosf(2.0f * std::numbers::pi_v<float> *(walkTimer_ / 30.0f));
	float angle = 60.0f + 30.0f * (param + 1.0f) * 0.5f;
	float zTilt = std::cosf(angle * std::numbers::pi_v<float> / 180.0f);
	worldTransform_.rotation_.z = -velocity_.x * zTilt * 1.5f;

	//===== 3. Gravity =====
	if (!onGround_) {
		velocity_.y += kGravity;
		velocity_.y = (std::max)(velocity_.y, -0.5f); // terminal velocity
	}
	else {
		velocity_.y = 0.0f;
	}

	//===== 4. Horizontal speed =====
	velocity_.x = (lrDirection_ == LRDirection::kLeft) ? -kWalkSpeed_ : kWalkSpeed_;

	//===== 5. Sub-stepping movement + collision =====
	const int hSteps = 4;   // horizontal sub-steps
	const int vSteps = 4;   // vertical sub-steps

	// Horizontal
	for (int i = 0; i < hSteps; ++i) {
		worldTransform_.translation_.x += velocity_.x / hSteps;
		MapCollision();                     // check walls every sub-step
	}

	// Vertical
	for (int i = 0; i < vSteps; ++i) {
		worldTransform_.translation_.y += velocity_.y / vSteps;
		MapCollision();                     // check ground every sub-step
	}

	isCollisionDisabled_ = false;
	worldUpdater_.Update(worldTransform_);
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
		worldTransform_.rotation_.y += std::numbers::pi_v<float> / 8.0f;
		worldTransform_.rotation_.x += std::numbers::pi_v<float> / 8.0f;

		// shrink
		float t = static_cast<float>(deathParameter_) / animDuration_;
		float scale = 0.25f * (1.0f - t * 0.5f);
		worldTransform_.scale_ = { scale, scale, scale };

		worldUpdater_.Update(worldTransform_);

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

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	;
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	;
	worldPos.z = worldTransform_.matWorld_.m[3][2];
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

void Enemy::OnStomped(GameScene* gamescene) {
	if (behavior_ == Behavior::kDeath || isCollisionDisabled_) return;

	Audio::GetInstance()->PlayWave(enemyDefeatSoundHandle_);

	isCollisionDisabled_ = true;
	behaviorRequest_ = Behavior::kDeath;

	// Optional hit effect
	Vector3 effectPos = worldTransform_.translation_;
	gamescene->CreateHitEffect(effectPos);
}

Enemy::Enemy() {}

Enemy::~Enemy() {
	//delete enemyDefeatSoundHandle_;
}
