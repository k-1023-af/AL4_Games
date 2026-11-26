#include "Enemy3D.h"
#include "Player3D.h"
#include "GameScene.h"

Enemy3D::Enemy3D() {}

Enemy3D::~Enemy3D() { delete enemyModel_; }

void Enemy3D::Initialize(Camera* camera, const Vector3& position)
{
	camera_ = camera;

	enemyTransform_.Initialize();
	enemyTransform_.scale_ = { 1.0f, 1.0f, 1.0f };
	enemyTransform_.rotation_ = { 0.0f, 0.0f, 0.0f };
	enemyTransform_.translation_ = position;
	enemyColor_ = { 1.0f, 0.0f, 0.0f, 1.0f };
	objectColor_.Initialize();
	objectColor_.SetColor(enemyColor_);

	enemyModel_ = Model::CreateFromOBJ("enemy1", true);

	velocity_ = { 0.0f, 0.0f, 0.0f };
	walkTimer_ = 0.0f;
	deathPhase_ = DeathPhase::kBefore;
	deathParameter_ = 0;

}

void Enemy3D::Update()
{
	
	if (behaviorRequest_ != Behavior::kUnknown) {
		behavior_ = behaviorRequest_;

		switch (behavior_) {
		case Behavior::kWalk:
		default:
			BehaviorWalkInitialize();
			break;
		case Behavior::kAlert:
			break;
		case Behavior::kDeath:
			BehaviorDeathInitialize();
			break;
		}

		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {
	case Behavior::kWalk:
	default:
		BehaviorWalkUpdate();
		break;
	case Behavior::kAlert:
		break;
	case Behavior::kDeath:
		BehaviorDeathUpdate();
		deathPhase_ = DeathPhase::kAnimation;
		break;
	}

}

void Enemy3D::Draw(Camera& camera)
{
	Model::PreDraw();
	enemyModel_->Draw(enemyTransform_, camera, &objectColor_);
	Model::PostDraw();
}

void Enemy3D::OnCollision(const Player3D* player, GameScene* gamescene)
{
	if (behavior_ == Behavior::kDeath) { return; }
	//if (!(player->IsAttack() && player->attackPhase_ == Player::LRDirection::kRush)) { return; }

	//const bool playerAttackingRight = (player->GetLRDirection() == Player::LRDirection::kRight);

	bool isFrontHit = false;

	if (isFrontHit) { return; }

	isCollisionDisabled_ = true;
	behaviorRequest_ = Behavior::kDeath;

	Vector3 effectPos = {
			(enemyTransform_.translation_.x + player->GetTransform().translation_.x) * 0.5f,
			(enemyTransform_.translation_.y + player->GetTransform().translation_.y) * 0.5f,
			(enemyTransform_.translation_.z + player->GetTransform().translation_.z) * 0.5f
	};

	gamescene->CreateHitEffect(effectPos);

}

void Enemy3D::SetMapChipField(MapChip3D* mapChipField)
{
	mapChipField_ = mapChipField;
}

void Enemy3D::BehaviorWalkInitialize()
{
	deathPhase_ = DeathPhase::kBefore;
}

void Enemy3D::BehaviorWalkUpdate()
{

	if (velocity_.x < 0.0f) {
		lookingDirection_ = LookDirection::kLeft;
	}
	else if (velocity_.x > 0.0f) {
		lookingDirection_ = LookDirection::kRight;
	}
	if (velocity_.z < 0.0f) {
		lookingDirection_ = LookDirection::kDown;
	}
	else if (velocity_.z > 0.0f) {
		lookingDirection_ = LookDirection::kUp;
	}

	walkTimer_ += 1.0f / 60.0f;

	// rotation animation
	float param = std::cosf((2.0f * pi) * (walkTimer_ / 30.0f));
	float degree = kWalkMotionAngleStart + (kWalkMotionAngleEnd * (param + 1.0f) / 2.0f);
	float ZAxisRotateAngle = std::cosf(degree); //changes radian to degree

	worldUpdater_.Update(enemyTransform_);
	enemyTransform_.rotation_ = { 0.0f, 3.0f, -velocity_.x * ZAxisRotateAngle };

	
	// turn around when hitting wall?
	// stop and look around ????


	enemyTransform_.translation_.x += velocity_.x;
	enemyTransform_.translation_.y += velocity_.y;
	enemyTransform_.translation_.z += velocity_.z;

}

void Enemy3D::BehaviorAlertInitialize()
{
}

void Enemy3D::BehaviorAlertUpdate()
{
}

void Enemy3D::BehaviorDeathInitialize()
{
	deathParameter_ = 0;
}

void Enemy3D::BehaviorDeathUpdate()
{
	switch (deathPhase_) {
		case DeathPhase::kBefore:
		default: {
			//isCollisionDisabled_ = false;
			break;
		}
		case DeathPhase::kAnimation: {
			deathParameter_++;
			//isCollisionDisabled_ = true;

			// spinning??
			enemyTransform_.rotation_.y += pi / 8.0f;
			enemyTransform_.rotation_.x += pi / 8.0f;

			if (deathParameter_ >= (animDuration_)) {
				isDead_ = true;

				deathParameter_ = 0;
				deathPhase_ = DeathPhase::kDisappear;
			}
			break;
		}
		case DeathPhase::kLayThere:
			break;
		case DeathPhase::kDisappear: {
			isCollisionDisabled_ = false;
			deathParameter_++;
			if (deathParameter_ >= (lingerDuration_)) {
				behaviorRequest_ = Behavior::kWalk;
				deathPhase_ = DeathPhase::kBefore;
				deathParameter_ = 0;
			}
			break;
		}
	}


}

Vector3 Enemy3D::GetWorldPosition()
{
	Vector3 worldPos;

	worldPos.x = enemyTransform_.matWorld_.m[3][0];
	worldPos.y = enemyTransform_.matWorld_.m[3][1];
	worldPos.z = enemyTransform_.matWorld_.m[3][2];
	;
	return worldPos;
}

AABB Enemy3D::GetAABB()
{
	Vector3 worldPos = GetWorldPosition();
	AABB aabb;

	aabb.min = { worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f };
	aabb.max = { worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f };

	return aabb;
}
