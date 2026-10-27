#include "Player.h"
#include "MapChip.h"
#include "Enemy.h"

void Player::Initialize(Camera* camera, const Vector3& position) {
	//-----------------------------
	camera_ = camera;

	playerTransform_.Initialize();
	playerTransform_.scale_ = { 0.5f, 0.5f, 0.5f };
	playerTransform_.rotation_.y = -halfPi; // 180 degrees in radians
	playerTransform_.translation_ = position;

	headWT_.Initialize();
	headWT_.scale_ = playerTransform_.scale_;
	headWT_.rotation_ = playerTransform_.rotation_;
	headWT_.translation_ = playerTransform_.translation_;

	rightArmWT_.Initialize();
	rightArmWT_.scale_ = playerTransform_.scale_;
	rightArmWT_.rotation_ = playerTransform_.rotation_;
	rightArmWT_.translation_ = playerTransform_.translation_;

	leftArmWT_.Initialize();
	leftArmWT_.scale_ = playerTransform_.scale_;
	leftArmWT_.rotation_ = playerTransform_.rotation_;
	leftArmWT_.translation_ = playerTransform_.translation_;

	rightLegWT_.Initialize();
	rightLegWT_.scale_ = playerTransform_.scale_;
	rightLegWT_.rotation_ = playerTransform_.rotation_;
	rightLegWT_.translation_ = playerTransform_.translation_;

	leftLegWT_.Initialize();
	leftLegWT_.scale_ = playerTransform_.scale_;
	leftLegWT_.rotation_ = playerTransform_.rotation_;
	leftLegWT_.translation_ = playerTransform_.translation_;

	playerModel_ = Model::CreateFromOBJ("player", true);
	headModel_ = Model::CreateFromOBJ("playerHead", true);
	rightArmModel_ = Model::CreateFromOBJ("rightArm", true);
	leftArmModel_ = Model::CreateFromOBJ("leftArm", true);
	rightLegModel_ = Model::CreateFromOBJ("rightLeg", true);
	leftLegModel_ = Model::CreateFromOBJ("leftLeg", true);

	airDashReadySoundHandle_ = Audio::GetInstance()->LoadWave("./Resources/sounds/canAirDash.wav");
	bounceSoundHandle_ = Audio::GetInstance()->LoadWave("./Resources/sounds/bounce.wav");
	hitSoundHandle_ = Audio::GetInstance()->LoadWave("./Resources/sounds/isHit.wav");
	jumpSoundHandle_ = Audio::GetInstance()->LoadWave("./Resources/sounds/jump.wav");
	dashAttackSoundHandle_ = Audio::GetInstance()->LoadWave("./Resources/sounds/airDash.wav");

	// modelAttack_
	//-----------------------------
	modelAttack_ = Model::CreateFromOBJ("DashAttack", true);
	worldTransformAttack_.Initialize();
	worldTransformAttack_.scale_ = { 0.8f, 0.6f, 0.8f };
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
	isAiming_ = false;
	canAirDash_ = false;
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
		break;
	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	}
}

void Player::Draw() {
	if (isDead_) return;

	playerTransform_.TransferMatrix();
	playerModel_->Draw(playerTransform_, *camera_, &objectColor_);
	//headModel_->Draw(playerTransform_, *camera_, &objectColor_);

	//auto DrawPart = [&](Model* model, WorldTransform& wt,
	//	const Vector3& localOffset, float localRotZ)
	//	{
	//		wt.scale_ = playerTransform_.scale_;
	//		wt.rotation_ = playerTransform_.rotation_;
	//		wt.translation_ = playerTransform_.translation_;
	//
	//		// Apply local offset in the character's local space
	//		// (This is the key fix)
	//		Matrix4x4 rotY = MakeRotatedY(playerTransform_.rotation_.y);
	//		Vector3 offset = Transform(localOffset, rotY);   // rotate the offset
	//		wt.translation_ = {
	//			wt.translation_.x + offset.x,
	//			wt.translation_.y + offset.y,
	//			wt.translation_.z + offset.z,
	//		};
	//
	//		// Add the running swing on Z (local)
	//		wt.rotation_.z += localRotZ;
	//
	//		wt.TransferMatrix();
	//		model->Draw(wt, *camera_, &objectColor_);
	//	};
	//
	//// ===== Head =====
	////DrawPart(headModel_, headWT_, { 0.0f, 0.38f, 0.0f }, 0.0f);
	//
	//// ===== Arms =====
	//DrawPart(rightArmModel_, rightArmWT_, { 0.2f, 0.2f, 0.0f }, armAngle);
	//DrawPart(leftArmModel_, leftArmWT_, { -0.2f, 0.2f, 0.0f }, armAngleOpp);
	//
	//// ===== Legs =====
	//DrawPart(rightLegModel_, rightLegWT_, { 0.12f, -0.3f, 0.0f }, legAngle);
	//DrawPart(leftLegModel_, leftLegWT_, { -0.12f, -0.3f, 0.0f }, legAngleOpp);

	//=================================================================
	//	Draw Running Animation
	//=================================================================

	headWT_.scale_ = playerTransform_.scale_;
	headWT_.rotation_ = playerTransform_.rotation_;
	headWT_.translation_ = playerTransform_.translation_;
	headWT_.translation_.y += 0.0f;               // adjust to your model
	headWT_.TransferMatrix();
	headModel_->Draw(headWT_, *camera_, &objectColor_);
	
	rightArmWT_.scale_ = playerTransform_.scale_;
	rightArmWT_.rotation_ = playerTransform_.rotation_;
	rightArmWT_.translation_ = playerTransform_.translation_;
	rightArmWT_.translation_ = {						
		rightArmWT_.translation_.x + 0.05f,		// adjust
		rightArmWT_.translation_.y + 0.0f,		// adjust
		rightArmWT_.translation_.z + 0.0f		// adjust
	}; 
	rightArmWT_.rotation_.z = armAngle;              // from animation
	rightArmWT_.TransferMatrix();
	rightArmModel_->Draw(rightArmWT_, *camera_, &objectColor_);
	
	leftArmWT_.scale_ = playerTransform_.scale_;
	leftArmWT_.rotation_ = playerTransform_.rotation_;
	leftArmWT_.translation_ = playerTransform_.translation_;
	leftArmWT_.translation_ = {
		leftArmWT_.translation_.x - 0.05f,		// adjust
		leftArmWT_.translation_.y + 0.0f,		// adjust
		leftArmWT_.translation_.z + 0.0f		// adjust
	};
	leftArmWT_.rotation_.z = armAngleOpp;
	leftArmWT_.TransferMatrix();
	leftArmModel_->Draw(leftArmWT_, *camera_, &objectColor_);
	
	rightLegWT_.scale_ = playerTransform_.scale_;
	rightLegWT_.rotation_ = playerTransform_.rotation_;
	rightLegWT_.translation_ = playerTransform_.translation_;
	rightLegWT_.translation_ = {
		rightLegWT_.translation_.x + 0.05f,		// adjust
		rightLegWT_.translation_.y - 0.0f,		// adjust
		rightLegWT_.translation_.z + 0.0f		// adjust
	};
	rightLegWT_.rotation_.z = legAngle;
	rightLegWT_.TransferMatrix();
	rightLegModel_->Draw(rightLegWT_, *camera_, &objectColor_);
	
	leftLegWT_.scale_ = playerTransform_.scale_;
	leftLegWT_.rotation_ = playerTransform_.rotation_;
	leftLegWT_.translation_ = playerTransform_.translation_;
	leftLegWT_.translation_ = {
		leftLegWT_.translation_.x - 0.05f,		// adjust
		leftLegWT_.translation_.y - 0.0f,		// adjust
		leftLegWT_.translation_.z + 0.0f		// adjust
	};
	leftLegWT_.rotation_.z = legAngleOpp;
	leftLegWT_.TransferMatrix();
	leftLegModel_->Draw(leftLegWT_, *camera_, &objectColor_);



	//===============================================================================

	if (attackPhase_ == AttackPhase::kRush || attackPhase_ == AttackPhase::kLinger) {
		worldTransformAttack_.TransferMatrix();
		modelAttack_->Draw(worldTransformAttack_, *camera_);
	}

	if (isAiming_) {
		Vector3 start = playerTransform_.translation_;
		start.y += 0.3f;

		// Full-length faint guide
		Vector3 fullEnd = Add(start, MultiplyByS(aimDirection_, kArrowLength));
		PrimitiveDrawer::GetInstance()->DrawLine3d(
			start, fullEnd, { 0.4f, 0.4f, 0.4f, 0.6f });   // grey guide

		// Charge fill (grows with chargePower_)
		float chargeT = std::clamp(chargePower_, 0.0f, 1.0f);
		Vector3 chargeEnd = Add(start, MultiplyByS(aimDirection_, kArrowLength * chargeT));

		// Color goes from yellow → orange → red as it charges
		Vector4 chargeColor = {
			1.0f,
			1.0f - chargeT * 0.7f,   // green channel decreases
			0.15f,
			1.0f
		};

		PrimitiveDrawer::GetInstance()->DrawLine3d(start, chargeEnd, chargeColor);

		// Arrow head at the tip of the charge line (or full line – your choice)
		Vector3 side = { -aimDirection_.y, aimDirection_.x, 0.0f };
		side = MultiplyByS(side, 0.35f);

		Vector3 back = MultiplyByS(aimDirection_, 0.55f);
		Vector3 headBase = Subtract(chargeEnd, back);   // tip of the charged part

		Vector3 head1 = Add(headBase, side);
		Vector3 head2 = Subtract(headBase, side);

		PrimitiveDrawer::GetInstance()->DrawLine3d(chargeEnd, head1, chargeColor);
		PrimitiveDrawer::GetInstance()->DrawLine3d(chargeEnd, head2, chargeColor);
	}

}

Vector3 Player::GetCornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
		{kWidth	/ 2.0f,  -kHeight / 2.0f, 0.0f}, // kRightBottom
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
	worldPos.x = playerTransform_.matWorld_.m[3][0];
	worldPos.y = playerTransform_.matWorld_.m[3][1];
	worldPos.z = playerTransform_.matWorld_.m[3][2];
	return worldPos;
}

WorldUpdater::AABB Player::GetAABB() {
	Vector3 worldPos = GetWorldPosition();
	WorldUpdater::AABB aabb;
	aabb.min = { worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f };
	aabb.max = { worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f };
	return aabb;
}

Vector3 Player::GetScale() {
	Vector3 worldScale;
	worldScale.x = playerTransform_.matWorld_.m[0][0];
	worldScale.y = playerTransform_.matWorld_.m[1][1];
	worldScale.z = playerTransform_.matWorld_.m[2][2];
	return worldScale;
}

void Player::OnDamaged(const Vector3& hitPos) {
	if (isInvincible_ || isDead_ || isAttack_) return;

	// Hit sound here
	Audio::GetInstance()->PlayWave(hitSoundHandle_);

	// Knockback direction (away from enemy)
	float dir = (playerTransform_.translation_.x > hitPos.x) ? 1.0f : -1.0f;

	velocity_.x = dir * 0.35f;   // horizontal knockback
	velocity_.y = 0.25f;         // small upward boost

	// Start invincibility
	isInvincible_ = true;
	invincibleTimer_ = kInvincibleTime;

	// Optional: cancel attack if currently attacking
	behaviorRequest_ = Behavior::kRoot;
	isAttack_ = false;
	isAiming_ = false;
}

void Player::SetMapChipField(MapChip* mapChipField) {
	mapChipField_ = mapChipField;
}

void Player::MapCollisionX(CollisionMapInfo& info) {
	if (!mapChipField_) return;
	
	Vector3 prevCenter = {
		playerTransform_.translation_.x - velocity_.x,
		playerTransform_.translation_.y,
		playerTransform_.translation_.z
	};
	
	std::array<Vector3, 4> positionOld;
	std::array<Vector3, 4> positionNew;
	for (uint32_t i = 0; i < 4; ++i) {
		positionOld[i] = GetCornerPosition(prevCenter, static_cast<Corner>(i));
		positionNew[i] = GetCornerPosition(playerTransform_.translation_, static_cast<Corner>(i));
	}
	
	//info.wallFlagL = false;
	//info.wallFlagR = false;
	
	//------------------------------------------------------------
	// Moving LEFT
	//------------------------------------------------------------
	if (info.move.x <= 0.0f) {
	
		auto CheckLeft = [&](Corner corner) -> bool {
			MapChip::IndexSet idx = mapChipField_->GetMapChipIndexSetByPosition(positionOld[corner]);
			MapChipType cur = mapChipField_->GetMapChipTypeByIndex(idx.xIndex, idx.yIndex);
			MapChipType next = mapChipField_->GetMapChipTypeByIndex(idx.xIndex - 1, idx.yIndex);
	
			// classic block wall
			bool blockWall = (cur != MapChipType::kBlock && next == MapChipType::kBlock);
			bool slopeWall = false; 

			if (next == MapChipType::kSlope1R || next == MapChipType::kSlope2R) {
				slopeWall = true;
			}
			if (next == MapChipType::kSlope3R || next == MapChipType::kSlope2L || 
				(lrDirection_ == LRDirection::kLeft && next == MapChipType::kSlope3R) ||
				(lrDirection_ == LRDirection::kLeft && next == MapChipType::kSlope2L)) {
				float checkY = positionNew[corner].y;
				slopeWall = mapChipField_->IsInBottomHalf(idx.xIndex - 1, idx.yIndex, checkY);
			}
	
			if (!blockWall && !slopeWall) return false;
	
			MapChip::IndexSet now = mapChipField_->GetMapChipIndexSetByPosition(positionNew[corner]);
			if (now.xIndex == idx.xIndex) return false;   // still in same cell
	
			MapChip::Rect rect = mapChipField_->GetRectByIndex(idx.xIndex, idx.yIndex);
			playerTransform_.translation_.x = rect.left + (kWidth * 0.5f) + kBlank;
			info.wallFlagL = true;
			return true;
		};
	
		if (CheckLeft(kLeftBottom) || CheckLeft(kLeftTop))
			return;
	}

	//------------------------------------------------------------
	// Moving RIGHT
	//------------------------------------------------------------
	if (info.move.x >= 0.0f) {

		auto CheckRight = [&](Corner corner) {
			MapChip::IndexSet idx = mapChipField_->GetMapChipIndexSetByPosition(positionOld[corner]);
			MapChipType cur = mapChipField_->GetMapChipTypeByIndex(idx.xIndex, idx.yIndex);
			MapChipType next = mapChipField_->GetMapChipTypeByIndex(idx.xIndex + 1, idx.yIndex);

			bool blockWall = (cur != MapChipType::kBlock && next == MapChipType::kBlock);
			bool slopeWall = false;

			if (next == MapChipType::kSlope1L || next == MapChipType::kSlope2L) {
				slopeWall = true;
			}
			if (next == MapChipType::kSlope3L || next == MapChipType::kSlope2R || 
				(lrDirection_ == LRDirection::kRight && next == MapChipType::kSlope3L) || 
				(lrDirection_ == LRDirection::kRight && next == MapChipType::kSlope2R)) {
				float checkY = positionNew[corner].y;
				slopeWall = mapChipField_->IsInBottomHalf(idx.xIndex + 1, idx.yIndex, checkY);
			}

			if (!blockWall && !slopeWall) return false;

			MapChip::IndexSet now = mapChipField_->GetMapChipIndexSetByPosition(positionNew[corner]);
			if (now.xIndex == idx.xIndex) return false;

			MapChip::Rect rect = mapChipField_->GetRectByIndex(idx.xIndex, idx.yIndex);
			playerTransform_.translation_.x = rect.right - (kWidth * 0.5f) - kBlank;
			info.wallFlagR = true;
			return true;
		};

		if (CheckRight(kRightBottom) || CheckRight(kRightTop)) return;
	}

	//if (!mapChipField_) return;
	//
	//Vector3 prevCenter = {
	//	playerTransform_.translation_.x - velocity_.x,
	//	playerTransform_.translation_.y,
	//	playerTransform_.translation_.z
	//};
	//
	//std::array<Vector3, 4> positionOld;
	//std::array<Vector3, 4> positionNew;
	//for (uint32_t i = 0; i < 4; ++i) {
	//	positionOld[i] = GetCornerPosition(prevCenter, static_cast<Corner>(i));
	//	positionNew[i] = GetCornerPosition(playerTransform_.translation_, static_cast<Corner>(i));
	//}
	//
	//info.wallFlagL = false;
	//info.wallFlagR = false;
	//
	////------------------------------------------------------------
	//// Moving LEFT
	////------------------------------------------------------------
	//if (info.move.x <= 0.0f) {
	//
	//	auto CheckLeft = [&](Corner corner) -> bool {
	//		MapChip::IndexSet idx = mapChipField_->GetMapChipIndexSetByPosition(positionOld[corner]);
	//		MapChipType cur = mapChipField_->GetMapChipTypeByIndex(idx.xIndex, idx.yIndex);
	//		MapChipType next = mapChipField_->GetMapChipTypeByIndex(idx.xIndex - 1, idx.yIndex);
	//
	//		// classic block wall
	//		bool blockWall = (cur != MapChipType::kBlock && next == MapChipType::kBlock);
	//		bool slopeWall = false;
	//
	//		if (next == MapChipType::kSlope1R || next == MapChipType::kSlope2R) {
	//			slopeWall = true;
	//		}
	//		if (next == MapChipType::kSlope3R || next == MapChipType::kSlope2L ||
	//			(lrDirection_ == LRDirection::kLeft && next == MapChipType::kSlope3R) ||
	//			(lrDirection_ == LRDirection::kLeft && next == MapChipType::kSlope2L)) {
	//			float checkY = positionNew[corner].y;
	//			slopeWall = mapChipField_->IsInBottomHalf(idx.xIndex - 1, idx.yIndex, checkY);
	//		}
	//
	//		if (!blockWall && !slopeWall) return false;
	//
	//		MapChip::IndexSet now = mapChipField_->GetMapChipIndexSetByPosition(positionNew[corner]);
	//		if (now.xIndex == idx.xIndex) return false;   // still in same cell
	//
	//		MapChip::Rect rect = mapChipField_->GetRectByIndex(idx.xIndex, idx.yIndex);
	//		playerTransform_.translation_.x = rect.left + (kWidth * 0.5f) + kBlank;
	//		info.wallFlagL = true;
	//		return true;
	//		};
	//
	//	if (CheckLeft(kLeftBottom) || CheckLeft(kLeftTop))
	//		return;
	//}
	//
	////------------------------------------------------------------
	//// Moving RIGHT
	////------------------------------------------------------------
	//if (info.move.x >= 0.0f) {
	//
	//	auto CheckRight = [&](Corner corner) {
	//		MapChip::IndexSet idx = mapChipField_->GetMapChipIndexSetByPosition(positionOld[corner]);
	//		MapChipType cur = mapChipField_->GetMapChipTypeByIndex(idx.xIndex, idx.yIndex);
	//		MapChipType next = mapChipField_->GetMapChipTypeByIndex(idx.xIndex + 1, idx.yIndex);
	//
	//		bool blockWall = (cur != MapChipType::kBlock && next == MapChipType::kBlock);
	//		bool slopeWall = false;
	//
	//		if (next == MapChipType::kSlope1L || next == MapChipType::kSlope2L) {
	//			slopeWall = true;
	//		}
	//		if (next == MapChipType::kSlope3L || next == MapChipType::kSlope2R ||
	//			(lrDirection_ == LRDirection::kRight && next == MapChipType::kSlope3L) ||
	//			(lrDirection_ == LRDirection::kRight && next == MapChipType::kSlope2R)) {
	//			float checkY = positionNew[corner].y;
	//			slopeWall = mapChipField_->IsInBottomHalf(idx.xIndex + 1, idx.yIndex, checkY);
	//		}
	//
	//		if (!blockWall && !slopeWall) return false;
	//
	//		MapChip::IndexSet now = mapChipField_->GetMapChipIndexSetByPosition(positionNew[corner]);
	//		if (now.xIndex == idx.xIndex) return false;
	//
	//		MapChip::Rect rect = mapChipField_->GetRectByIndex(idx.xIndex, idx.yIndex);
	//		playerTransform_.translation_.x = rect.right - (kWidth * 0.5f) - kBlank;
	//		info.wallFlagR = true;
	//		return true;
	//		};
	//
	//	if (CheckRight(kRightBottom) || CheckRight(kRightTop)) return;
	//}


}

void Player::MapCollisionY(CollisionMapInfo& info) {
	if (!mapChipField_) return;

	std::array<Vector3, 4> positionOld;
	std::array<Vector3, 4> positionNew;

	Vector3 prevPosition = { 
		playerTransform_.translation_.x, 
		playerTransform_.translation_.y - velocity_.y, 
		playerTransform_.translation_.z 
	};

	for (uint32_t i = 0; i < 4; ++i) {
		positionNew[i] = GetCornerPosition(playerTransform_.translation_, static_cast<Corner>(i));
		positionOld[i] = GetCornerPosition(prevPosition, static_cast<Corner>(i));
	}

	info.ceilingFlag = false;
	info.landingFlag = false;
	info.onSlopeL = false;
	info.onSlopeR = false;

	if (info.move.y > 0.0f) {		// Ceiling check
		MapChip::IndexSet indexSet;
		MapChipType mapChipType;
		MapChipType mapChipTypeNext;
		bool hit = false;

		/// left side
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
		if ((mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) || 
			(mapChipType == MapChipType::kSlope1L && mapChipTypeNext != MapChipType::kSlope1L) ||
			(mapChipType == MapChipType::kSlope1R && mapChipTypeNext != MapChipType::kSlope1R) ||
			(mapChipType == MapChipType::kSlope2L && mapChipTypeNext != MapChipType::kSlope2L) ||
			(mapChipType == MapChipType::kSlope2R && mapChipTypeNext != MapChipType::kSlope2R) ||
			(mapChipType == MapChipType::kSlope3L && mapChipTypeNext != MapChipType::kSlope3L) ||
			(mapChipType == MapChipType::kSlope3R && mapChipTypeNext != MapChipType::kSlope3R) ){
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
		if ((mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) ||
			(mapChipType == MapChipType::kSlope1L && mapChipTypeNext != MapChipType::kSlope1L) ||
			(mapChipType == MapChipType::kSlope1R && mapChipTypeNext != MapChipType::kSlope1R) ||
			(mapChipType == MapChipType::kSlope2L && mapChipTypeNext != MapChipType::kSlope2L) ||
			(mapChipType == MapChipType::kSlope2R && mapChipTypeNext != MapChipType::kSlope2R) ||
			(mapChipType == MapChipType::kSlope3L && mapChipTypeNext != MapChipType::kSlope3L) ||
			(mapChipType == MapChipType::kSlope3R && mapChipTypeNext != MapChipType::kSlope3R) ){
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
		float highestSurfaceY = -100000.0f;
		bool foundSurface = false;
		
		float fallSpeed = std::abs(velocity_.y);
		const float kBaseTolerance = 0.20f;
		const float kSpeedTolerance = 0.45f;	// high speed
		float snapTolerance = kBaseTolerance + fallSpeed * kSpeedTolerance;
		snapTolerance = (std::min)(snapTolerance, 0.85f);	
		if (behavior_ == Behavior::kAttack) {
			snapTolerance *= 2.0f;               // or even 2.0f
		}

		auto ConsiderSurface = [&](float surfaceY) {
			if (surfaceY < -9999.0f) return;

			float playerBottom = playerTransform_.translation_.y - (kHeight * 0.4f);

			// Accept the surface if we are below it or only slightly above it
			if (playerBottom <= surfaceY + kBlank + 0.02f &&
				playerBottom >= surfaceY - snapTolerance)
			{
				if (surfaceY > highestSurfaceY)
				{
					highestSurfaceY = surfaceY;
					foundSurface = true;
				}
			}
		};

		// Left foot
		{
			MapChip::IndexSet idx = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom]);
			MapChipType type = mapChipField_->GetMapChipTypeByIndex(idx.xIndex, idx.yIndex);
			MapChipType next = mapChipField_->GetMapChipTypeByIndex(idx.xIndex, idx.yIndex - 1);

			if (type == MapChipType::kBlock && next != MapChipType::kBlock) {
				ConsiderSurface(mapChipField_->GetRectByIndex(idx.xIndex, idx.yIndex).top + 0.08f);
			}
			else if (type == MapChipType::kSlope1L || type == MapChipType::kSlope1R ||
				type == MapChipType::kSlope2L || type == MapChipType::kSlope2R ||
				type == MapChipType::kSlope3L || type == MapChipType::kSlope3R) {
				float sy = mapChipField_->GetSlopeSurfaceY(idx.xIndex, idx.yIndex, playerTransform_.translation_.x);
				ConsiderSurface(sy);
				if (type == MapChipType::kSlope1L || type == MapChipType::kSlope2L || type == MapChipType::kSlope3L)
					info.onSlopeL = true;
				else
					info.onSlopeR = true;
			}
		}

		/// right side
		{
			MapChip::IndexSet idx = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom]);
			MapChipType type = mapChipField_->GetMapChipTypeByIndex(idx.xIndex, idx.yIndex);
			MapChipType next = mapChipField_->GetMapChipTypeByIndex(idx.xIndex, idx.yIndex - 1);

			if (type == MapChipType::kBlock && next != MapChipType::kBlock) {
				ConsiderSurface(mapChipField_->GetRectByIndex(idx.xIndex, idx.yIndex).top + 0.08f);
			}
			else if (type == MapChipType::kSlope1L || type == MapChipType::kSlope1R ||
				type == MapChipType::kSlope2L || type == MapChipType::kSlope2R ||
				type == MapChipType::kSlope3L || type == MapChipType::kSlope3R) {
				float sy = mapChipField_->GetSlopeSurfaceY(idx.xIndex, idx.yIndex, playerTransform_.translation_.x);
				ConsiderSurface(sy);
				if (type == MapChipType::kSlope1L || type == MapChipType::kSlope2L || type == MapChipType::kSlope3L)
					info.onSlopeL = true;
				else
					info.onSlopeR = true;
			}
		}

		if (foundSurface) {
			if (info.onSlopeL || info.onSlopeR) {
				velocity_.x *= 0.98f;
			}
			playerTransform_.translation_.y = highestSurfaceY + (kHeight * 0.5f) + kBlank;
			info.landingFlag = true;
			velocity_.y = 0.0f;
			onGround_ = true;
			return;
		}

		if (velocity_.y < -0.01f) onGround_ = false;
	}

//if (!mapChipField_) return;
//
//std::array<Vector3, 4> positionOld;
//std::array<Vector3, 4> positionNew;
//
//Vector3 prevPosition = {
//	playerTransform_.translation_.x,
//	playerTransform_.translation_.y - velocity_.y,
//	playerTransform_.translation_.z
//};
//
//for (uint32_t i = 0; i < 4; ++i) {
//	positionNew[i] = GetCornerPosition(playerTransform_.translation_, static_cast<Corner>(i));
//	positionOld[i] = GetCornerPosition(prevPosition, static_cast<Corner>(i));
//}
//
//info.ceilingFlag = false;
//info.landingFlag = false;
//info.onSlopeL = false;
//info.onSlopeR = false;
//
//if (info.move.y > 0.0f) {		// Ceiling check
//	MapChip::IndexSet indexSet;
//	MapChipType mapChipType;
//	MapChipType mapChipTypeNext;
//	bool hit = false;
//
//	/// left side
//	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
//	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
//	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
//	if ((mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) ||
//		(mapChipType == MapChipType::kSlope1L && mapChipTypeNext != MapChipType::kSlope1L) ||
//		(mapChipType == MapChipType::kSlope1R && mapChipTypeNext != MapChipType::kSlope1R) ||
//		(mapChipType == MapChipType::kSlope2L && mapChipTypeNext != MapChipType::kSlope2L) ||
//		(mapChipType == MapChipType::kSlope2R && mapChipTypeNext != MapChipType::kSlope2R) ||
//		(mapChipType == MapChipType::kSlope3L && mapChipTypeNext != MapChipType::kSlope3L) ||
//		(mapChipType == MapChipType::kSlope3R && mapChipTypeNext != MapChipType::kSlope3R)) {
//		hit = true;
//	}
//	if (hit) {
//		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
//		MapChip::IndexSet indexSetNow;
//		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(positionOld[kLeftTop]);
//		if (indexSetNow.yIndex != indexSet.yIndex) {
//			MapChip::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
//			playerTransform_.translation_.y = rect.bottom - (kHeight / 2.0f) - kBlank;
//			info.ceilingFlag = true;
//			return;
//		}
//	}
//	/// right side
//	hit = false;
//	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
//	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
//	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
//	if ((mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) ||
//		(mapChipType == MapChipType::kSlope1L && mapChipTypeNext != MapChipType::kSlope1L) ||
//		(mapChipType == MapChipType::kSlope1R && mapChipTypeNext != MapChipType::kSlope1R) ||
//		(mapChipType == MapChipType::kSlope2L && mapChipTypeNext != MapChipType::kSlope2L) ||
//		(mapChipType == MapChipType::kSlope2R && mapChipTypeNext != MapChipType::kSlope2R) ||
//		(mapChipType == MapChipType::kSlope3L && mapChipTypeNext != MapChipType::kSlope3L) ||
//		(mapChipType == MapChipType::kSlope3R && mapChipTypeNext != MapChipType::kSlope3R)) {
//		hit = true;
//	}
//	if (hit) {
//		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
//		MapChip::IndexSet indexSetNow;
//		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(positionOld[kRightTop]);
//		if (indexSetNow.yIndex != indexSet.yIndex) {
//			MapChip::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
//			playerTransform_.translation_.y = rect.bottom - (kHeight / 2.0f) - kBlank;
//			info.ceilingFlag = true;
//			return;
//		}
//	}
//	else {
//		info.ceilingFlag = false;
//		return;
//	}
//}
//
////--------------------------------------------
//// Landing Flag Check
//
//if (info.move.y <= 0.0f) {
//	float highestSurfaceY = -100000.0f;
//	bool foundSurface = false;
//
//	float fallSpeed = std::abs(velocity_.y);
//	const float kBaseTolerance = 0.20f;
//	const float kSpeedTolerance = 0.55f;	// high speed
//	float snapTolerance = kBaseTolerance + fallSpeed * kSpeedTolerance;
//	snapTolerance = (std::min)(snapTolerance, 0.85f);
//
//	auto ConsiderSurface = [&](float surfaceY) {
//		if (surfaceY < -9999.0f) return;
//
//		float playerBottom = playerTransform_.translation_.y - (kHeight * 0.4f);
//
//		// Accept the surface if we are below it or only slightly above it
//		if (playerBottom <= surfaceY + kBlank + 0.02f &&
//			playerBottom >= surfaceY - snapTolerance)
//		{
//			if (surfaceY > highestSurfaceY)
//			{
//				highestSurfaceY = surfaceY;
//				foundSurface = true;
//			}
//		}
//		};
//
//	// Left foot
//	{
//		MapChip::IndexSet idx = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom]);
//		MapChipType type = mapChipField_->GetMapChipTypeByIndex(idx.xIndex, idx.yIndex);
//		MapChipType next = mapChipField_->GetMapChipTypeByIndex(idx.xIndex, idx.yIndex - 1);
//
//		if (type == MapChipType::kBlock && next != MapChipType::kBlock) {
//			ConsiderSurface(mapChipField_->GetRectByIndex(idx.xIndex, idx.yIndex).top);
//		}
//		else if (type == MapChipType::kSlope1L || type == MapChipType::kSlope1R ||
//			type == MapChipType::kSlope2L || type == MapChipType::kSlope2R ||
//			type == MapChipType::kSlope3L || type == MapChipType::kSlope3R) {
//			float sy = mapChipField_->GetSurfaceY(idx.xIndex, idx.yIndex, playerTransform_.translation_.x);
//			ConsiderSurface(sy);
//			if (type == MapChipType::kSlope1L || type == MapChipType::kSlope2L || type == MapChipType::kSlope3L)
//				info.onSlopeL = true;
//			else
//				info.onSlopeR = true;
//		}
//	}
//
//	/// right side
//	{
//		MapChip::IndexSet idx = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom]);
//		MapChipType type = mapChipField_->GetMapChipTypeByIndex(idx.xIndex, idx.yIndex);
//		MapChipType next = mapChipField_->GetMapChipTypeByIndex(idx.xIndex, idx.yIndex - 1);
//
//		if (type == MapChipType::kBlock && next != MapChipType::kBlock) {
//			ConsiderSurface(mapChipField_->GetRectByIndex(idx.xIndex, idx.yIndex).top);
//		}
//		else if (type == MapChipType::kSlope1L || type == MapChipType::kSlope1R ||
//			type == MapChipType::kSlope2L || type == MapChipType::kSlope2R ||
//			type == MapChipType::kSlope3L || type == MapChipType::kSlope3R) {
//			float sy = mapChipField_->GetSurfaceY(idx.xIndex, idx.yIndex, playerTransform_.translation_.x);
//			ConsiderSurface(sy);
//			if (type == MapChipType::kSlope1L || type == MapChipType::kSlope2L || type == MapChipType::kSlope3L)
//				info.onSlopeL = true;
//			else
//				info.onSlopeR = true;
//		}
//	}
//
//	if (foundSurface) {
//		if (info.onSlopeL || info.onSlopeR) {
//			velocity_.x *= 0.98f;
//		}
//		playerTransform_.translation_.y = highestSurfaceY + (kHeight * 0.5f) + kBlank;
//		info.landingFlag = true;
//		velocity_.y = 0.0f;
//		onGround_ = true;
//		return;
//	}
//
//	if (velocity_.y < -0.01f) onGround_ = false;
//}

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

	if (!isAiming_) {
		playerTransform_.rotation_.x = 0.0f;
		playerTransform_.rotation_.z = 0.0f;

		if (lrDirection_ == LRDirection::kRight) {
			playerTransform_.rotation_.y = std::numbers::pi_v<float> *0.5f;
		}
		else {
			playerTransform_.rotation_.y = std::numbers::pi_v<float> *1.5f;
		}
	}

	worldTransformAttack_.Initialize();
	worldTransformAttack_.translation_ = playerTransform_.translation_;
	attackPhase_ = AttackPhase::kCoil;
	//isAiming_ = false;
}

void Player::BehaviorUpdate() {

	worldUpdater_.Update(playerTransform_);

	CollisionMapInfo info;
	info.move = velocity_;
	objectColor_.SetColor(currentColor_);

	// Floor check
	if (playerTransform_.translation_.y <= 1.0f) {
		playerTransform_.translation_.y = 1.0f;
		velocity_.y = 0.0f;
		onGround_ = true;
		canDash_ = true;
		//canAirDash_ = true;
		velocity_.x *= (1.0f - kAttenuationLanding);
	}
	//===========================================================
	//	  [1] movement input	 
	//============================================================

	float speedLimit = isPSpeed_ ? kLimitPSpeed : kLimitRunSpeed;
	float accel		 = isBoost_ ? kBoostAcceleration : 
						isPSpeed_ ? kRunAcceleration : kAcceleration;

	if (Input::GetInstance()->PushKey(DIK_D)) {
		velocity_.x += accel;
		if (lrDirection_ != LRDirection::kRight) {
			lrDirection_ = LRDirection::kRight;
			turnFirstRotationY_ = playerTransform_.rotation_.y;
			turnTimer_ = kTimeTurn;
		}
	}
	if (Input::GetInstance()->PushKey(DIK_A)) {
		velocity_.x -= accel;
		if (lrDirection_ != LRDirection::kLeft) {
			lrDirection_ = LRDirection::kLeft;
			turnFirstRotationY_ = playerTransform_.rotation_.y;
			turnTimer_ = kTimeTurn;
		}
	}

	// clamp
	velocity_.x = std::clamp(velocity_.x, -speedLimit, speedLimit);

	if (isPSpeed_ && onGround_) {
		// One extra ground sample at high speed
		MapCollisionY(info);
		if (info.landingFlag || info.onSlopeL || info.onSlopeR) {
			onGround_ = true;
			velocity_.y = 0.0f;
		}
	}

	//================================================================
	// P Speed
	//================================================================
	bool hittingWall = info.wallFlagL || info.wallFlagR;

	bool atFullRunSpeed = false;

	if (!hittingWall) {
		if (lrDirection_ == LRDirection::kRight) {
			// Running right → need high positive speed
			atFullRunSpeed = velocity_.x >= (kLimitRunSpeed * 0.90f);
		}
		else if (lrDirection_ == LRDirection::kLeft) {
			// Running left → need high negative speed
			atFullRunSpeed = velocity_.x <= -(kLimitRunSpeed * 0.90f);
		}
	}

	if (atFullRunSpeed) {
		pMeter_ = (std::min)(2.0f, pMeter_ + kPMeterFillRate);
	}
	else {
		// Drain faster against walls
		float drain = hittingWall ? kPMeterDrainRate * 2.5f
			: (std::abs(velocity_.x) > 0.5f ? kPMeterDrainRate * 0.6f
				: kPMeterDrainRate);
		pMeter_ = (std::max)(0.0f, pMeter_ - drain);
	}

	if (pMeter_ >= kBoostThreshold) {
		isBoost_ = true;
		isPSpeed_ = true;          // Boost implies P-Speed
		pSpeedTimer_ = kPSpeedDuration;
	}
	if (pMeter_ >= kPSpeedThreshold) {
		isBoost_ = false;
		isPSpeed_ = true;
		pSpeedTimer_ = kPSpeedDuration;
	}
	else if (isPSpeed_) {
		pSpeedTimer_ -= 1.0f;
		if (pSpeedTimer_ <= 0.0f) {
			isPSpeed_ = false;
			pMeter_ = 0.4f;
		}
	}

	if (isBoost_) {
		// Strong red + slight pulse
		float pulse = 0.85f + 0.15f * std::sin(pSpeedTimer_ * 0.3f);
		currentColor_ = { 
			attackColor_.x * pulse,
			attackColor_.y,
			attackColor_.z,
			1.0f };
	}
	else if (isPSpeed_) {
		float pulse = 0.82f + 0.18f * std::sin(pSpeedTimer_ * 0.25f);
		currentColor_ = {
			pSpeedColor_.x * pulse,
			pSpeedColor_.y * pulse,
			pSpeedColor_.z * pulse,
			1.0f
		};
	}
	else {
		// normal → yellow lerp based on pMeter_ (0 ~ 1)
		float t = std::clamp(pMeter_, 0.0f, 1.0f);
		currentColor_ = {
			Lerp(normalColor_.x, pSpeedColor_.x, t),
			Lerp(normalColor_.y, pSpeedColor_.y, t),
			Lerp(normalColor_.z, pSpeedColor_.z, t),
			1.0f
		};
	}

	objectColor_.SetColor(currentColor_);

	//===============================================================================
	// Jump
	//==============================================================================
	if (info.landingFlag || info.onSlopeL || info.onSlopeR) {
		onGround_ = true;


		playerTransform_.rotation_.x = 0.0f;
		playerTransform_.rotation_.z = 0.0f;
		if (velocity_.x < -0.01f) {
			lrDirection_ = LRDirection::kLeft;
			playerTransform_.rotation_.y = std::numbers::pi_v<float> *0.5f;
		}
		else if (velocity_.x > 0.01f) {
			lrDirection_ = LRDirection::kRight;
			playerTransform_.rotation_.y = std::numbers::pi_v<float> *1.5f;
		}



	}
	if (onGround_ || info.onSlopeL || info.onSlopeR) {
		canAirDash_ = false;
		frameCount = 0.0f;
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			DebugText::GetInstance()->ConsolePrintf("JUMP!!\n");
			// Jump sound here
			Audio::GetInstance()->PlayWave(jumpSoundHandle_);

			velocity_.y = kJumpAcceleration;
			if (lrDirection_ == LRDirection::kLeft) {
				velocity_.x -= 0.2f * accel;
			}
			if (lrDirection_ == LRDirection::kRight) {
				velocity_.x += 0.2f * accel;
			}
			onGround_ = false;
			frameCount = 0.0f;
		}
	}
	else {
		if (behavior_ == Behavior::kRoot && !Input::GetInstance()->PushKey(DIK_SPACE)) {
			frameCount++;
			if (frameCount >= 2.0f) {
				if (!canAirDash_ && isPSpeed_) {
					Audio::GetInstance()->PlayWave(airDashReadySoundHandle_);
				}
				canAirDash_ = isPSpeed_;
			}
		}
	}
	
	if (canAirDash_ && canDash_) {
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			//start aiming
			behaviorRequest_ = Behavior::kAttack;
			attackPhase_ = AttackPhase::kAim;
			isAiming_ = true;
			canAirDash_ = false;
			canDash_ = false;
			isAttack_ = true;
			frameCount = 0.0f;
			aimDirection_ = (lrDirection_ == LRDirection::kRight)
				? Vector3{ 1.0f, 0.0f, 0.0f }
				: Vector3{ -1.0f, 0.0f, 0.0f };
		}
	}
	
	// gravity
	if (!info.landingFlag) {
		velocity_.y -= kGravityAcceleration;
		velocity_.y = (std::max)(velocity_.y, -kLimitFallSpeed);

		if (velocity_.x != 0) velocity_.x *= (1.0f - kAttenuation);
	}

	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f / 60.0f; // 60 FPS

		float destinationRotationYTable[] = {
			std::numbers::pi_v<float> / 2.0f,       // kRight
			std::numbers::pi_v<float> *3.0f / 2.0f, // kLeft
		};
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		playerTransform_.rotation_.y = Lerp(turnFirstRotationY_, destinationRotationY, kTimeTurn);

		if (turnTimer_ <= 0.0f) {
			turnTimer_ = 0.0f;

			playerTransform_.rotation_.y = destinationRotationY;
		}
	}


	////------------------------------------////
	//	[2]collision detection with movement  //
	////------------------------------------////

	// Apply velocity then collide
	for (int i = 0; i < vSteps; ++i) {
		playerTransform_.translation_.y += velocity_.y / vSteps;
		MapCollisionY(info);
	}

	// Horizontal sub-steps
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
		canAirDash_ = false;

		DebugText::GetInstance()->ConsolePrintf("landing\n");
	}


	if (isInvincible_) {
		invincibleTimer_ -= 1.0f;

		// Flash (blink every few frames)
		bool visible = (static_cast<int>(invincibleTimer_) / 4) % 2 == 0;
		objectColor_.SetColor({
			currentColor_.x,
			currentColor_.y,
			currentColor_.z,
			visible ? 1.0f : 0.2f
			});

		if (invincibleTimer_ <= 0.0f) {
			isInvincible_ = false;



			objectColor_.SetColor(currentColor_); // restore normal color
		}
	}
	
		if (lrDirection_ != LRDirection::kRight && velocity_.x > 0) {
			lrDirection_ = LRDirection::kRight;
			turnFirstRotationY_ = playerTransform_.rotation_.y;
			turnTimer_ = kTimeTurn;
		}
	
	
		if (lrDirection_ != LRDirection::kLeft && velocity_.x < 0) {
			lrDirection_ = LRDirection::kLeft;
			turnFirstRotationY_ = playerTransform_.rotation_.y;
			turnTimer_ = kTimeTurn;
		}
	

	UpdateRunningAnimation();
	worldUpdater_.Update(headWT_);
	worldUpdater_.Update(rightArmWT_);
	worldUpdater_.Update(leftArmWT_);
	worldUpdater_.Update(rightLegWT_);
	worldUpdater_.Update(leftLegWT_);


}

void Player::BehaviorAttackInitialize() {
	attackParameter_ = 0;
}

void Player::BehaviorAttackUpdate() {
	if (!modelAttack_) return;

	if (!isPSpeed_) {
		behaviorRequest_ = Behavior::kRoot;
		isAiming_ = false;
		isAttack_ = false;
		return;
	}

	//objectColor_.SetColor(attackColor_);

	attackParameter_++;

	worldTransformAttack_.translation_ = playerTransform_.translation_;
	worldTransformAttack_.rotation_ = playerTransform_.rotation_;

	if (lrDirection_ == LRDirection::kRight) {
		worldTransformAttack_.translation_.x += attackOffset;
	}
	else {
		worldTransformAttack_.translation_.x -= attackOffset;
	}
	worldUpdater_.Update(worldTransformAttack_);

	switch (attackPhase_) {
	//------------------------------------------------------------
	case AttackPhase::kAim:
	{
		objectColor_.SetColor(aimColor_);
		chargePower_ = std::clamp(chargePower_, 0.5f, 1.0f);
		chargePower_ += 1.0f / 60.0f;
		// ----- Read input -----
		float inputX = 0.0f;
		float inputY = 0.0f;

		if (Input::GetInstance()->PushKey(DIK_D)) inputX += 1.0f;
		if (Input::GetInstance()->PushKey(DIK_A)) inputX -= 1.0f;
		if (Input::GetInstance()->PushKey(DIK_W)) inputY += 1.0f;
		if (Input::GetInstance()->PushKey(DIK_S)) inputY -= 1.0f;

		if (inputX != 0.0f || inputY != 0.0f) {
			// Normalize so diagonal is not faster
			float len = std::sqrt(inputX * inputX + inputY * inputY);
			aimDirection_.x = inputX / len;
			aimDirection_.y = inputY / len;
			aimDirection_.z = 0.0f;

			if (aimDirection_.x > 0.01f)
				lrDirection_ = LRDirection::kRight;
			else if (aimDirection_.x < -0.01f)
				lrDirection_ = LRDirection::kLeft;

			playerTransform_.rotation_.z += (lrDirection_ == LRDirection::kRight)
				? -0.3f
				: 0.3f;

			headWT_.rotation_.z = playerTransform_.rotation_.z;
			rightArmWT_.rotation_.z = playerTransform_.rotation_.z;
			leftArmWT_.rotation_.z = playerTransform_.rotation_.z;
			rightLegWT_.rotation_.z = playerTransform_.rotation_.z;
			leftLegWT_.rotation_.z = playerTransform_.rotation_.z;

		}

		// slight float while aiming
		velocity_.y *= 0.85f;
		velocity_.y = (std::max)(velocity_.y, -0.04f);

		// Confirm dash with Jump
		if (!Input::GetInstance()->PushKey(DIK_SPACE) && (inputX != 0.0f || inputY != 0.0f || true)) {
			
			// ===== Dash Attack sound here =====
			Audio::GetInstance()->PlayWave(dashAttackSoundHandle_);
			
			velocity_ = MultiplyByS(aimDirection_, attackVelocity_ * chargePower_);

			pMeter_ = 0.5f;
			//isPSpeed_ = false;
			isBoost_ = false;
			pSpeedTimer_ = 30.0f;

			attackPhase_ = AttackPhase::kCoil;
			attackParameter_ = 0;
			chargePower_ = 0.0f;
			isAiming_ = false;
		}

		// cancel if landed
		if (onGround_) {
			behaviorRequest_ = Behavior::kRoot;
			isAiming_ = false;
			isAttack_ = false;
		}
		break;
	}
	//-----------------------------------------------------------------------
	case AttackPhase::kCoil:
	 {
		objectColor_.SetColor(attackColor_);
		float t = attackParameter_ / coilDuration_; // coil time
		playerTransform_.scale_.z = EaseOut(1.0f, 0.1f, t);
		playerTransform_.scale_.y = EaseOut(1.0f, 1.6f, t);

		
		{
			CollisionMapInfo info;
			info.move = velocity_;
			MapCollisionY(info);
			MapCollisionX(info);
		}

		if (attackParameter_ >= (coilDuration_)) { // coil time
			attackPhase_ = AttackPhase::kRush;
			attackParameter_ = 0;
		}
		break;
	}
	 //----------------------------------------------------------------------
	case AttackPhase::kRush: 
	{

		objectColor_.SetColor(attackColor_);

		float angle = std::atan2(aimDirection_.y, aimDirection_.x);
		playerTransform_.rotation_.z = angle - std::numbers::pi_v<float> *0.5f;

		headWT_.rotation_.z = playerTransform_.rotation_.z;
		rightArmWT_.rotation_.z = playerTransform_.rotation_.z;
		leftArmWT_.rotation_.z = playerTransform_.rotation_.z;
		rightLegWT_.rotation_.z = playerTransform_.rotation_.z;
		leftLegWT_.rotation_.z = playerTransform_.rotation_.z;

		const float kSpinSpeed = 0.45f;
		playerTransform_.rotation_.y += kSpinSpeed;
		
		if (playerTransform_.rotation_.y > std::numbers::pi_v<float> *2.0f) {
			playerTransform_.rotation_.y -= std::numbers::pi_v<float> *2.0f;
		}

		headWT_.rotation_.y = playerTransform_.rotation_.y;
		rightArmWT_.rotation_.y = playerTransform_.rotation_.y;
		leftArmWT_.rotation_.y = playerTransform_.rotation_.y;
		rightLegWT_.rotation_.y = playerTransform_.rotation_.y;
		leftLegWT_.rotation_.y = playerTransform_.rotation_.y;

		CollisionMapInfo info;
		info.move = velocity_;
		info.wallFlagL = false;
		info.wallFlagR = false;
		//float incomingVelocityY = velocity_.y;

		for (int i = 0; i < hSteps; ++i) {
			playerTransform_.translation_.x += velocity_.x / hSteps;
			MapCollisionX(info);
		}
		for (int i = 0; i < vSteps; ++i) {
			playerTransform_.translation_.y += velocity_.y / vSteps;
			MapCollisionY(info);
			if (info.landingFlag || info.onSlopeL || info.onSlopeR) {
			}
		}
		
		if (info.wallFlagL || info.wallFlagR) {
			velocity_.x *= -0.8f;
			Audio::GetInstance()->PlayWave(bounceSoundHandle_);
		}
		if (info.ceilingFlag) { 
			velocity_.y *= -0.5f; 
			Audio::GetInstance()->PlayWave(bounceSoundHandle_);
		}
		
		Vector3 newDir = velocity_;
		float length = std::sqrt(newDir.x * newDir.x + newDir.y * newDir.y);
		if (length > 0.01f) {
			newDir.x /= length;
			newDir.y /= length;

			angle = std::atan2(newDir.y, newDir.x);
			playerTransform_.rotation_.z = angle - std::numbers::pi_v<float> *0.5f;

			headWT_.rotation_.z = playerTransform_.rotation_.z;
			rightArmWT_.rotation_.z = playerTransform_.rotation_.z;
			leftArmWT_.rotation_.z = playerTransform_.rotation_.z;
			rightLegWT_.rotation_.z = playerTransform_.rotation_.z;
			leftLegWT_.rotation_.z = playerTransform_.rotation_.z;

			if (newDir.x < -0.01f)
				lrDirection_ = LRDirection::kLeft;
			else if (newDir.x > 0.01f)
				lrDirection_ = LRDirection::kRight;
		}


		if (info.onSlopeL) {
			info.landingFlag = true;
			velocity_.x -= velocity_.y * 0.8f;
		}
		if (info.onSlopeR) {
			info.landingFlag = true;
			velocity_.x += velocity_.y * 0.8f;
		}
		if (info.landingFlag || onGround_) {
			velocity_.y = 0.0f;
			onGround_ = true;
			canDash_ = true;

			//playerTransform_.rotation_.x = 0.0f;
			//playerTransform_.rotation_.z = 0.0f;
			//if (velocity_.x < -0.01f) {
			//	lrDirection_ = LRDirection::kLeft;
			//	playerTransform_.rotation_.y = std::numbers::pi_v<float> *1.5f;
			//}
			//else if (velocity_.x > 0.01f) {
			//	lrDirection_ = LRDirection::kRight;
			//	playerTransform_.rotation_.y = std::numbers::pi_v<float> *0.5f;
			//}

			attackPhase_ = AttackPhase::kCoil;
			behaviorRequest_ = Behavior::kRoot;
			attackParameter_ = 0;
		}
		
		float t = attackParameter_ / rushDuration_;
		playerTransform_.scale_.z = EaseOut(0.1f, 0.8f, t);
		playerTransform_.scale_.y = EaseIn(0.8f, 0.5f, t);

		if (attackParameter_ >= (rushDuration_)) {
			attackPhase_ = AttackPhase::kLinger;
			attackParameter_ = 0;
		}
		break;
	}
	//-----------------------------------------------------
	case AttackPhase::kLinger: 
	{
		objectColor_.SetColor(attackColor_);
		velocity_.x *= 0.85f;
		velocity_.y *= 0.85f;

		CollisionMapInfo info;
		info.move = velocity_;
		info.wallFlagL = false;
		info.wallFlagR = false;

		for (int i = 0; i < hSteps; ++i) {
			playerTransform_.translation_.x += velocity_.x / hSteps;
			MapCollisionX(info);
		}
		for (int i = 0; i < vSteps; ++i) {
			playerTransform_.translation_.y += velocity_.y / vSteps;
			MapCollisionY(info);
			if (info.landingFlag || info.onSlopeL || info.onSlopeR) {
			}
		}

		if (info.wallFlagL || info.wallFlagR) velocity_.x *= -0.5f;
		if (info.ceilingFlag)               velocity_.y *= -0.25f;

		if (info.onSlopeL) {
			info.landingFlag = true;
			velocity_.x -= velocity_.y * 0.5f;
		}
		if (info.onSlopeR) {
			info.landingFlag = true;
			velocity_.x += velocity_.y * 0.5f;
		}
		if (info.landingFlag || onGround_) {
			velocity_.y = 0.0f;
			onGround_ = true;
			canDash_ = true;

			attackPhase_ = AttackPhase::kCoil;
			behaviorRequest_ = Behavior::kRoot;
			attackParameter_ = 0;
		}

		float t = attackParameter_ / lingerDuration_;
		playerTransform_.scale_.z = EaseOut(0.8f, 0.5f, t);
		playerTransform_.scale_.y = EaseOut(0.5f, 0.8f, t);

		if (attackParameter_ >= (lingerDuration_)) {
			behaviorRequest_ = Behavior::kRoot;
			isAttack_ = false;
			attackParameter_ = 0;
		}
		break;
	}

	}

	if (isInvincible_) {
		invincibleTimer_ -= 1.0f;

		// Flash (blink every few frames)
		bool visible = (static_cast<int>(invincibleTimer_) / 4) % 2 == 0;
		objectColor_.SetColor({
			currentColor_.x,
			currentColor_.y,
			currentColor_.z,
			visible ? 1.0f : 0.2f
			});

		if (invincibleTimer_ <= 0.0f) {
			isInvincible_ = false;
			objectColor_.SetColor(currentColor_); // restore normal color
		}
	}

	worldUpdater_.Update(playerTransform_);
	worldUpdater_.Update(headWT_);
	worldUpdater_.Update(rightArmWT_);
	worldUpdater_.Update(leftArmWT_);
	worldUpdater_.Update(rightLegWT_);
	worldUpdater_.Update(leftLegWT_);
}

void Player::UpdateRunningAnimation()
{
	bool isRunning = onGround_ && std::abs(velocity_.x) > 0.02f;

	if (isRunning) {
		// Advance the run cycle (faster when P-Speed)
		float speedFactor = isPSpeed_ ? 1.8f : 1.0f;
		runTimer_ += runCycleSpeed_ * speedFactor;
	}
	else {
		// Smoothly return to idle
		runTimer_ *= 0.85f;
	}

	float swing = std::sin(runTimer_);
	float swingOpposite = std::sin(runTimer_ + std::numbers::pi_v<float>);

	// ===== Leg animation =====
	legAngle = swing * 0.6f;          // max swing angle
	legAngleOpp = swingOpposite * 0.6f;

	// ===== Arm animation (opposite to legs) =====
	armAngle = -swing * 0.45f;
	armAngleOpp = -swingOpposite * 0.45f;

	// Slight body bob
	bodyBob = std::abs(swing) * 0.04f;

	// Apply to the local transforms (example)
	// You will use these when drawing
}

void Player::BounceFromStomp() {
	velocity_.y = 0.55f;   // adjust

	isInvincible_ = true;
	invincibleTimer_ = 20.0f;
}


Player::Player(){}

Player::~Player(){
	delete playerModel_;
	delete headModel_;
	delete rightArmModel_;
	delete leftArmModel_;
	delete rightLegModel_;
	delete leftLegModel_;
	delete modelAttack_;

	//delete airDashReadySoundHandle_;
	//delete bounceSoundHandle_;
	//delete hitSoundHandle_;
	//delete jumpSoundHandle_;
	//delete dashAttackSoundHandle_;
}

