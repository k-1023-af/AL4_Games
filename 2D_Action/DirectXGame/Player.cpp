#include "Player.h"

void Player::Initialize(Camera* camera, const Vector3& position) {
	//-----------------------------
	camera_ = camera;

	playerTransform_.Initialize();
	playerTransform_.scale_ = { 0.5f, 0.5f, 0.5f };
	playerTransform_.rotation_.y = halfPi; // 180 degrees in radians
	playerTransform_.translation_ = position;

	playerModel_ = Model::CreateFromOBJ("player", true);

	// modelAttack_
	//-----------------------------
	modelAttack_ = Model::CreateFromOBJ("DashAttack", true);
	worldTransformAttack_.Initialize();
	worldTransformAttack_.scale_ = { 1.0f, 0.6f, 1.0f };
	worldTransformAttack_.rotation_.z = std::numbers::pi_v<float>;

	objectColor_.Initialize();
	objectColor_.SetColor(normalColor_);
	
	//Initial behavior
	velocity_ = { 0.0f, 0.0f, 0.0f };
	onGround_ = false;
	lrDirection_ = LRDirection::kRight;
	turnTimer_ = 0.0f;

	attackPhase_ = AttackPhase::kCoil;
	attackParameter_ = 0;

}

void Player::Update() {
	if (behaviorRequest_ != Behavior::kUnknown) {
		behavior_ = behaviorRequest_;
		//behaviorRequest_ = Behavior::kUnknown;

		switch (behavior_) {

		case Behavior::kRoot:
		default:
			BehaviorInitialize();
			break;
		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		}
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {

	case Behavior::kRoot:
	default:
		BehaviorUpdate();
		//isAttack_ = false;
		break;
	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	}
}

void Player::Draw() {
	if (!isDead_) {
		playerTransform_.TransferMatrix();
		playerModel_->Draw(playerTransform_, *camera_, &objectColor_);

		if (attackPhase_ == AttackPhase::kRush || attackPhase_ == AttackPhase::kLinger) {

			//if (lrDirection_ == LRDirection::kRight) {
			//	worldTransformAttack_.translation_.x = +attackVelocity;
			//} else {
			//	worldTransformAttack_.translation_.x = -attackVelocity;
			//}
			worldTransformAttack_.TransferMatrix();
			modelAttack_->Draw(worldTransformAttack_, *camera_);
		}
	}
}

Vector3 Player::GetCornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
		{kWidth / 2.0f,  -kHeight / 2.0f, 0.0f}, // kRightBottom
		{-kWidth / 2.0f, -kHeight / 2.0f, 0.0f}, // kLeftBottom
		{kWidth / 2.0f,  kHeight / 2.0f,  0.0f}, // kRightTop
		{-kWidth / 2.0f, kHeight / 2.0f,  0.0f}  // kLeftTop
	};

	Vector3 offset = offsetTable[static_cast<uint32_t>(corner)];

	Vector3 result;
	result.x = center.x + offset.x;
	result.y = center.y + offset.y;
	result.z = center.z + offset.z;

	return result;
}

Vector3 Player::GetWorldPosition() {
	Vector3 worldPos;

	worldPos.x = playerTransform_.matWorld_.m[3][0];;
	worldPos.y = playerTransform_.matWorld_.m[3][1];;
	worldPos.z = playerTransform_.matWorld_.m[3][2];;

	return worldPos;
}

WorldUpdater::AABB Player::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	WorldUpdater::AABB aabb;

	aabb.min = { worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f };
	aabb.max = { worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f };

	return aabb;
}

//Vector3 Player::GetRotation() {
//	
//}

Vector3 Player::GetScale() {
	Vector3 worldScale;

	worldScale.x = playerTransform_.matWorld_.m[0][0];
	worldScale.y = playerTransform_.matWorld_.m[1][1];
	worldScale.z = playerTransform_.matWorld_.m[2][2];

	return worldScale;
}

void Player::SetMapChipField(MapChip* mapChipField) {
	mapChipField_ = mapChipField;
}

void Player::MapCollisionX(CollisionMapInfo& info) {
	if (!mapChipField_)
		return;

	Vector3 prevCenter = { playerTransform_.translation_.x - velocity_.x, playerTransform_.translation_.y, playerTransform_.translation_.z };

	std::array<Vector3, 4> positionOld;
	std::array<Vector3, 4> positionNew;

	for (uint32_t i = 0; i < 4; ++i) {
		positionOld[i] = GetCornerPosition(prevCenter, static_cast<Corner>(i));
		positionNew[i] = GetCornerPosition(playerTransform_.translation_, static_cast<Corner>(i));
	}

	info.wallFlagL = false;
	info.wallFlagR = false;

	if (info.move.x <= 0.0f) { // moving left
		MapChip::IndexSet indexSet;
		MapChipType mapChipType;
		MapChipType mapChipTypeNext;
		bool hit = false;

		/// left bottom check
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionOld[kLeftBottom]);
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
		if (mapChipType != MapChipType::kBlock && mapChipTypeNext == MapChipType::kBlock) {
			hit = true;
		}
		if (hit) {
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionOld[kLeftBottom]);
			MapChip::IndexSet indexSetNow;
			indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom]);
			if (indexSetNow.xIndex != indexSet.xIndex) {
				MapChip::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
				playerTransform_.translation_.x = rect.left + (kWidth / 2.0f) + kBlank;
				info.wallFlagL = true;
				return;
			}
		}
		/// left top check
		hit = false;
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionOld[kLeftTop]);
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
		if (mapChipType != MapChipType::kBlock && mapChipTypeNext == MapChipType::kBlock) {
			hit = true;
		}
		if (hit) {
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionOld[kLeftTop]);
			MapChip::IndexSet indexSetNow;
			indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
			if (indexSetNow.xIndex != indexSet.xIndex) {
				MapChip::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
				playerTransform_.translation_.x = rect.left + (kWidth / 2.0f) + kBlank;
				info.wallFlagL = true;
				return;
			}
		}
	}

	if (info.move.x >= 0.0f) { // moving right
		MapChip::IndexSet indexSet;
		MapChipType mapChipType;
		MapChipType mapChipTypeNext;
		bool hit = false;

		/// right bottom check
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionOld[kRightBottom]);
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
		if (mapChipType != MapChipType::kBlock && mapChipTypeNext == MapChipType::kBlock) {
			hit = true;
		}
		if (hit) {
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionOld[kRightBottom]);
			MapChip::IndexSet indexSetNow;
			indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom]);
			if (indexSetNow.xIndex != indexSet.xIndex) {
				MapChip::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
				playerTransform_.translation_.x = rect.right - (kWidth / 2.0f) - kBlank;
				info.wallFlagR = true;
				return;
			}
		}
		/// right top check
		hit = false;
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionOld[kRightTop]);
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
		if (mapChipType != MapChipType::kBlock && mapChipTypeNext == MapChipType::kBlock) {
			hit = true;
		}
		if (hit) {
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionOld[kRightTop]);
			MapChip::IndexSet indexSetNow;
			indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
			if (indexSetNow.xIndex != indexSet.xIndex) {
				MapChip::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
				playerTransform_.translation_.x = rect.right - (kWidth / 2.0f) - kBlank;
				info.wallFlagR = true;
				return;
			}
		}
	}
}

void Player::MapCollisionY(CollisionMapInfo& info) {
	if (!mapChipField_) {
		return;
	}

	std::array<Vector3, 4> positionOld;
	std::array<Vector3, 4> positionNew;

	Vector3 prevPosition = { playerTransform_.translation_.x, playerTransform_.translation_.y - velocity_.y, playerTransform_.translation_.z };

	for (uint32_t i = 0; i < 4; ++i) {
		positionNew[i] = GetCornerPosition(playerTransform_.translation_, static_cast<Corner>(i));
		positionOld[i] = GetCornerPosition(prevPosition, static_cast<Corner>(i));
	}

	info.ceilingFlag = false;
	info.landingFlag = false;
	info.onSlopeL = false;
	info.onSlopeR = false;

	if (info.move.y > 0.0f) { // Ceiling check
		MapChip::IndexSet indexSet;
		MapChipType mapChipType;
		MapChipType mapChipTypeNext;
		bool hit = false;

		/// left side
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
		if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
			hit = true;
		}
		if (hit) {
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
			MapChip::IndexSet indexSetNow;
			indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(positionOld[kLeftTop]);
			if (indexSetNow.yIndex != indexSet.yIndex) {
				MapChip::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
				playerTransform_.translation_.y = rect.bottom - (kHeight / 2.0f) - kBlank;
				info.ceilingFlag = true;
				return;
			}
		}
		/// right side
		hit = false;
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
		if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
			hit = true;
		}
		if (hit) {
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
			MapChip::IndexSet indexSetNow;
			indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(positionOld[kRightTop]);
			if (indexSetNow.yIndex != indexSet.yIndex) {
				MapChip::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
				playerTransform_.translation_.y = rect.bottom - (kHeight / 2.0f) - kBlank;
				info.ceilingFlag = true;
				return;
			}
		}
		else {
			info.ceilingFlag = false;
			return;
		}
	}

	//--------------------------------------------
	// Landing Flag Check

	if (info.move.y <= 0.0f) {
		MapChip::IndexSet indexSet;
		MapChipType mapChipType;
		MapChipType mapChipTypeNext;
		bool hitBlock = false;
		bool hitSlopeL = false;
		bool hitSlopeR = false;
		float highestSurfaceY = -100000.0f;
		float surfaceY = -100000.0f;

		/// left side
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom]);
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
		if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
			MapChip::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			highestSurfaceY = (std::max)(highestSurfaceY, rect.top);
			hitBlock = true;
		}
		if (mapChipType == MapChipType::kSlope1L && mapChipTypeNext != MapChipType::kSlope1L || mapChipType == MapChipType::kSlope1R && mapChipTypeNext != MapChipType::kSlope1R) {
			surfaceY = mapChipField_->GetSlopeSurfaceY(
				indexSet.xIndex,
				indexSet.yIndex,
				playerTransform_.translation_.x
			);
			if (surfaceY > highestSurfaceY) {
				highestSurfaceY = surfaceY;
				hitSlopeL = true;

				if (mapChipType == MapChipType::kSlope1L) info.onSlopeL = true;
				if (mapChipType == MapChipType::kSlope1R) info.onSlopeR = true;
			}
		}
		
		if (hitBlock || hitSlopeL) {
			float playerBottom = playerTransform_.translation_.y - (kHeight / 2.0f);
			if (playerBottom <= highestSurfaceY + kBlank) {
				playerTransform_.translation_.y = highestSurfaceY + (kHeight / 2.0f) + kBlank;
				info.landingFlag = true;
				velocity_.y = 0.0f;
				onGround_ = true;
				return;
			}
		}


		/// right side
		hitBlock = false;
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom]);
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
		if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
			MapChip::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			highestSurfaceY = (std::max)(highestSurfaceY, rect.top);
			hitBlock = true;
		}
		if (mapChipType == MapChipType::kSlope1L && mapChipTypeNext != MapChipType::kSlope1L || mapChipType == MapChipType::kSlope1R && mapChipTypeNext != MapChipType::kSlope1R) {
			hitSlopeR = true;
			surfaceY = mapChipField_->GetSlopeSurfaceY(
				indexSet.xIndex,
				indexSet.yIndex,
				playerTransform_.translation_.x
			);
			if (surfaceY > highestSurfaceY) {
				highestSurfaceY = surfaceY;
				hitSlopeR = true;

				if (mapChipType == MapChipType::kSlope1L) info.onSlopeL = true;
				if (mapChipType == MapChipType::kSlope1R) info.onSlopeR = true;
			}
		}
		
		if (hitBlock || hitSlopeR) {
			float playerBottom = playerTransform_.translation_.y - (kHeight / 2.0f);

			if (playerBottom <= highestSurfaceY + kBlank) {
				playerTransform_.translation_.y = highestSurfaceY + (kHeight / 2.0f) + kBlank;
				info.landingFlag = true;
				velocity_.y = 0.0f;
				onGround_ = true;
				return;
			}
		}
		if (velocity_.y < -0.01f) {   // falling
			onGround_ = false;
		}
		
		else {
			info.landingFlag = false;
			info.onSlopeL = false;
			info.onSlopeR = false;
			return;
		}
	}
}

void Player::EnemyCollision(Enemy* enemy) {
	if (IsAttack()) { return; }

	DebugText::GetInstance()->ConsolePrintf("HIT!\n");
	(void)enemy;
	//playerTransform_.translation_.y += 2.0f;
	isDead_ = true;
}

void Player::BehaviorInitialize() {
	objectColor_.SetColor(normalColor_);
	playerTransform_.scale_ = {0.5f, 0.5f, 0.5f};

	worldTransformAttack_.Initialize();
	worldTransformAttack_.translation_ = playerTransform_.translation_;
	attackPhase_ = AttackPhase::kCoil;

}

void Player::BehaviorUpdate() {

	worldUpdater_.Update(playerTransform_);
	CollisionMapInfo info;
	info.move = velocity_;

	// Floor check
	if (playerTransform_.translation_.y <= 1.0f) {
		playerTransform_.translation_.y = 1.0f;
		velocity_.y = 0.0f;
		onGround_ = true;
		velocity_.x *= (1.0f - kAttenuationLanding);
	}
	////-----------------------////
	//	  [1] movement input	 //
	////-----------------------////
	if (Input::GetInstance()->PushKey(DIK_D)) {
		velocity_.x += kAcceleration;
		if (lrDirection_ != LRDirection::kRight) {
			lrDirection_ = LRDirection::kRight;
			turnFirstRotationY_ = playerTransform_.rotation_.y;

			turnTimer_ = kTimeTurn;
		}
	}
	if (Input::GetInstance()->PushKey(DIK_A)) {
		velocity_.x -= kAcceleration;
		if (lrDirection_ != LRDirection::kLeft) {
			lrDirection_ = LRDirection::kLeft;
			turnFirstRotationY_ = playerTransform_.rotation_.y;

			turnTimer_ = kTimeTurn;
		}
	}
	// jump
	if (onGround_ && Input::GetInstance()->TriggerKey(DIK_W)) {
		DebugText::GetInstance()->ConsolePrintf("JUMP!!\n");
		velocity_.y = kJumpAcceleration;
		//info.landingFlag = false;
		onGround_ = false;
	}
	
	// gravity
	if (!info.landingFlag) {
		velocity_.y -= kGravityAcceleration;
		velocity_.y = (std::max)(velocity_.y, -kLimitFallSpeed);

		if (velocity_.x != 0) {
			velocity_.x *= (1.0f - kAttenuation);
		}
	}

	velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f / 60.0f; // 60 FPS

		float destinationRotationYTable[] = {
			std::numbers::pi_v<float> / 2.0f,        // kRight
			std::numbers::pi_v<float> *3.0f / 2.0f, // kLeft
		};
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		playerTransform_.rotation_.y = Lerp(turnFirstRotationY_, destinationRotationY, kTimeTurn);

		if (turnTimer_ <= 0.0f) {
			turnTimer_ = 0.0f;

			playerTransform_.rotation_.y = destinationRotationY;
		}
	}

	if (behaviorRequest_ == Behavior::kUnknown && canDash_ == true && Input::GetInstance()->TriggerKey(DIK_B)) {
		behaviorRequest_ = Behavior::kAttack;
		canDash_ = false;
		isAttack_ = true;
		DebugText::GetInstance()->ConsolePrintf("ATTACK!\n");
	}

	if (isAttack_) {
		timer++;

		if (timer >= attackTime_) {
			isAttack_ = false;
			timer = 0.0f;
		}
	}

	////------------------------------------////
	//	[2]collision detection with movement  //
	////------------------------------------////

	// Apply velocity then collide
	const int vSteps = 8;
	for (int i = 0; i < vSteps; ++i) {
		playerTransform_.translation_.y += velocity_.y / vSteps;
		MapCollisionY(info);
	}

	// Horizontal sub-steps
	const int hSteps = 6;
	for (int i = 0; i < hSteps; ++i) {
		playerTransform_.translation_.x += velocity_.x / hSteps;
		MapCollisionX(info);
	}

	if (info.ceilingFlag) {
		velocity_.y = 0;
		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
	}
	if (info.landingFlag) {
		velocity_.y = 0;
		onGround_ = true;
		canDash_ = true;
		DebugText::GetInstance()->ConsolePrintf("landing\n");
	}
	// jump
	if (onGround_ && Input::GetInstance()->TriggerKey(DIK_W)) {
		DebugText::GetInstance()->ConsolePrintf("JUMP!!\n");
		velocity_.y = kJumpAcceleration;
		info.landingFlag = false;
		onGround_ = false;
	}

}

void Player::BehaviorAttackInitialize() {
	attackParameter_ = 0;
	attackPhase_ = AttackPhase::kCoil;
}

void Player::BehaviorAttackUpdate() {

	if (!modelAttack_) {
		return;
	}

	objectColor_.SetColor(attackColor_);

	attackParameter_++;

	worldTransformAttack_.translation_ = playerTransform_.translation_;
	worldTransformAttack_.rotation_ = playerTransform_.rotation_;

	// Offset so the effect sits clearly in front of the character

	if (lrDirection_ == LRDirection::kRight) {
		worldTransformAttack_.translation_.x += attackOffset;
	}
	else {
		worldTransformAttack_.translation_.x -= attackOffset;
	}

	worldUpdater_.Update(worldTransformAttack_);

	switch (attackPhase_) {

	case AttackPhase::kCoil:
	default: {

		float t = attackParameter_ / coilDuration_; // coil time
		playerTransform_.scale_.z = EaseOut(1.0f, 0.1f, t);
		playerTransform_.scale_.y = EaseOut(1.0f, 1.6f, t);

		if (attackParameter_ >= (coilDuration_)) { // coil time
			attackPhase_ = AttackPhase::kRush;
			attackParameter_ = 0;
		}
		break;
	}
	case AttackPhase::kRush: {
		//isAttack_ = true;
		if (lrDirection_ == LRDirection::kRight) {
			velocity_.x = +attackVelocity;
		}
		else {
			velocity_.x = -attackVelocity;
		}

		CollisionMapInfo info;
		info.move = velocity_;
		const int hSteps = 6;
		for (int i = 0; i < hSteps; ++i) {
			playerTransform_.translation_.x += velocity_.x / hSteps;
			MapCollisionX(info);
		}

		float t = attackParameter_ / rushDuration_;
		playerTransform_.scale_.z = EaseOut(0.1f, 1.6f, t);
		playerTransform_.scale_.y = EaseIn(1.6f, 0.7f, t);

		if (attackParameter_ >= (rushDuration_)) {
			attackPhase_ = AttackPhase::kLinger;
			attackParameter_ = 0;
		}
		break;
	}
	case AttackPhase::kLinger: {
		velocity_.x = 0.0f; // stop after the rush

		float t = attackParameter_ / lingerDuration_;
		playerTransform_.scale_.z = EaseOut(1.6f, 1.0f, t);
		playerTransform_.scale_.y = EaseOut(0.7f, 1.0f, t);

		if (attackParameter_ >= (lingerDuration_)) {
			//isAttack_ = false;
			behaviorRequest_ = Behavior::kRoot;
			// attackPhase_ = AttackPhase::kCoil;
			attackParameter_ = 0;
		}
		break;
	}

	}

	worldUpdater_.Update(playerTransform_);
}

Player::Player(){}

Player::~Player(){
	delete playerModel_;
	delete modelAttack_;
}
