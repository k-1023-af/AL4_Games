#include "Player.h"
#include "MapChip.h"
#include "Enemy.h"

void Player::Initialize(Camera* camera, const Vector3& position) {
	//-----------------------------
	camera_ = camera;

	playerTransform_.Initialize();
	playerTransform_.scale_ = { 0.7f, 0.7f, 0.7f };
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

	if (attackPhase_ == AttackPhase::kRush || attackPhase_ == AttackPhase::kLinger) {
		//if (lrDirection_ == LRDirection::kRight) {
		//	worldTransformAttack_.translation_.x = +attackVelocity;
		//} else {
		//	worldTransformAttack_.translation_.x = -attackVelocity;
		//}
		worldTransformAttack_.TransferMatrix();
		modelAttack_->Draw(worldTransformAttack_, *camera_);
	}

	if (isAiming_) {
		Vector3 start = playerTransform_.translation_;
		start.y += 0.3f;
		Vector3 end = Add(start, MultiplyByS(aimDirection_, kArrowLength));

		PrimitiveDrawer::GetInstance()->DrawLine3d(start, end, { 1.0f, 0.9f, 0.2f, 1.0f });

		Vector3 side = { -aimDirection_.y, aimDirection_.x, 0.0f };
		side = MultiplyByS(side, 0.35f);	//side = side * 0.35f;

		Vector3 back = MultiplyByS(aimDirection_, 0.55f);
		Vector3 headBase = Subtract(end, back);

		Vector3 head1 = Add(headBase, side)	;
		Vector3 head2 = Subtract(headBase, side);

		PrimitiveDrawer::GetInstance()->DrawLine3d(end, head1, { 1.0f, 0.9f, 0.2f, 1.0f });
		PrimitiveDrawer::GetInstance()->DrawLine3d(end, head1, { 1.0f, 0.9f, 0.2f, 1.0f });
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
	
	info.wallFlagL = false;
	info.wallFlagR = false;
	
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
		//MapChip::IndexSet indexSet;
		//MapChipType mapChipType;
		//MapChipType mapChipTypeNext;
		//bool hitBlock = false;
		//bool hitSlopeL = false;
		//bool hitSlopeR = false;
		//float highestSurfaceY = -100000.0f;
		//float surfaceY = -100000.0f;

		float highestSurfaceY = -100000.0f;
		bool foundSurface = false;
		
		float fallSpeed = std::abs(velocity_.y);
		const float kBaseTolerance = 0.20f;
		const float kSpeedTolerance = 0.55f;	// high speed
		float snapTolerance = kBaseTolerance + fallSpeed * kSpeedTolerance;
		snapTolerance = (std::min)(snapTolerance, 0.85f);	

		auto ConsiderSurface = [&](float surfaceY) {
			if (surfaceY < -9999.0f) return;

			float playerBottom = playerTransform_.translation_.y - (kHeight * 0.5f);

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

		///// left side
		//indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom]);
		//mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		//mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
		//if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		//	MapChip::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		//	highestSurfaceY = (std::max)(highestSurfaceY, rect.top);
		//	hitBlock = true;
		//}
		//if ((mapChipType == MapChipType::kSlope1L && mapChipTypeNext != MapChipType::kSlope1L) || 
		//	(mapChipType == MapChipType::kSlope1R && mapChipTypeNext != MapChipType::kSlope1R) ||
		//	(mapChipType == MapChipType::kSlope2L && mapChipTypeNext != MapChipType::kSlope2L) ||
		//	(mapChipType == MapChipType::kSlope2R && mapChipTypeNext != MapChipType::kSlope2R) ||
		//	(mapChipType == MapChipType::kSlope3L && mapChipTypeNext != MapChipType::kSlope3L) ||
		//	(mapChipType == MapChipType::kSlope3R && mapChipTypeNext != MapChipType::kSlope3R) ){
		//	surfaceY = mapChipField_->GetSlopeSurfaceY(
		//		indexSet.xIndex,
		//		indexSet.yIndex,
		//		playerTransform_.translation_.x
		//	);
		//	if (surfaceY > highestSurfaceY) {
		//		highestSurfaceY = surfaceY;
		//		hitSlopeL = true;
		//
		//		if (mapChipType == MapChipType::kSlope1L) info.onSlopeL = true;
		//		if (mapChipType == MapChipType::kSlope1R) info.onSlopeR = true;
		//		if (mapChipType == MapChipType::kSlope2L) info.onSlopeL = true;
		//		if (mapChipType == MapChipType::kSlope2R) info.onSlopeR = true;
		//		if (mapChipType == MapChipType::kSlope3L) info.onSlopeL = true;
		//		if (mapChipType == MapChipType::kSlope3R) info.onSlopeR = true;
		//	}
		//}
		//
		//if (hitBlock || hitSlopeL) {
		//	float playerBottom = playerTransform_.translation_.y - (kHeight / 2.0f);
		//	if (playerBottom <= highestSurfaceY + kBlank) {
		//		playerTransform_.translation_.y = highestSurfaceY + (kHeight / 2.0f) + kBlank;
		//		info.landingFlag = true;
		//		velocity_.y = 0.0f;
		//		onGround_ = true;
		//		return;
		//	}
		//}
		// Left foot
		{
			MapChip::IndexSet idx = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom]);
			MapChipType type = mapChipField_->GetMapChipTypeByIndex(idx.xIndex, idx.yIndex);
			MapChipType next = mapChipField_->GetMapChipTypeByIndex(idx.xIndex, idx.yIndex - 1);

			if (type == MapChipType::kBlock && next != MapChipType::kBlock) {
				ConsiderSurface(mapChipField_->GetRectByIndex(idx.xIndex, idx.yIndex).top);
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
				ConsiderSurface(mapChipField_->GetRectByIndex(idx.xIndex, idx.yIndex).top);
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
			playerTransform_.translation_.y = highestSurfaceY + (kHeight * 0.5f) + kBlank;
			info.landingFlag = true;
			velocity_.y = 0.0f;
			onGround_ = true;
			//canDash_ = true;
			//canAirDash_ = true;
			return;
		}

		if (velocity_.y < -0.01f) onGround_ = false;

		//hitBlock = false;
		//indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom]);
		//mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		//mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
		//if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		//	MapChip::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		//	highestSurfaceY = (std::max)(highestSurfaceY, rect.top);
		//	hitBlock = true;
		//}
		//if ((mapChipType == MapChipType::kSlope1L && mapChipTypeNext != MapChipType::kSlope1L) ||
		//	(mapChipType == MapChipType::kSlope1R && mapChipTypeNext != MapChipType::kSlope1R) ||
		//	(mapChipType == MapChipType::kSlope2L && mapChipTypeNext != MapChipType::kSlope2L) ||
		//	(mapChipType == MapChipType::kSlope2R && mapChipTypeNext != MapChipType::kSlope2R) ||
		//	(mapChipType == MapChipType::kSlope3L && mapChipTypeNext != MapChipType::kSlope3L) ||
		//	(mapChipType == MapChipType::kSlope3R && mapChipTypeNext != MapChipType::kSlope3R) ){
		//	hitSlopeR = true;
		//	surfaceY = mapChipField_->GetSlopeSurfaceY(
		//		indexSet.xIndex,
		//		indexSet.yIndex,
		//		playerTransform_.translation_.x
		//	);
		//	if (surfaceY > highestSurfaceY) {
		//		highestSurfaceY = surfaceY;
		//		hitSlopeR = true;
		//
		//		if (mapChipType == MapChipType::kSlope1L) info.onSlopeL = true;
		//		if (mapChipType == MapChipType::kSlope1R) info.onSlopeR = true;
		//		if (mapChipType == MapChipType::kSlope2L) info.onSlopeL = true;
		//		if (mapChipType == MapChipType::kSlope2R) info.onSlopeR = true;
		//		if (mapChipType == MapChipType::kSlope3L) info.onSlopeL = true;
		//		if (mapChipType == MapChipType::kSlope3R) info.onSlopeR = true;
		//	}
		//}
		//
		//if (hitBlock || hitSlopeR) {
		//	float playerBottom = playerTransform_.translation_.y - (kHeight / 2.0f);
		//
		//	if (playerBottom <= highestSurfaceY + kBlank) {
		//		playerTransform_.translation_.y = highestSurfaceY + (kHeight / 2.0f) + kBlank;
		//		info.landingFlag = true;
		//		velocity_.y = 0.0f;
		//		onGround_ = true;
		//		return;
		//	}
		//}
		//if (velocity_.y < -0.01f) {   // falling
		//	onGround_ = false;
		//}
		//
		//else {
		//	info.landingFlag = false;
		//	info.onSlopeL = false;
		//	info.onSlopeR = false;
		//	return;
		//}
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
	playerTransform_.scale_ = {0.7f, 0.7f, 0.7f};

	worldTransformAttack_.Initialize();
	worldTransformAttack_.translation_ = playerTransform_.translation_;
	attackPhase_ = AttackPhase::kCoil;
	isAiming_ = false;
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
		canDash_ = true;
		//canAirDash_ = true;
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
	if (onGround_) {
		canAirDash_ = false;
		frameCount = 0.0f;
		if (Input::GetInstance()->TriggerKey(DIK_B)) {
			DebugText::GetInstance()->ConsolePrintf("JUMP!!\n");
			velocity_.y = kJumpAcceleration;
			onGround_ = false;
			frameCount = 0.0f;
		}
	}
	if (!onGround_ && behavior_ == Behavior::kRoot && !Input::GetInstance()->PushKey(DIK_B)) {
		frameCount++;
		if (frameCount >= 4.0f) {
			canAirDash_ = true;
		}
	}
	if (canAirDash_) {
		if (Input::GetInstance()->PushKey(DIK_B)) {
			//start aiming
			behaviorRequest_ = Behavior::kAttack;
			attackPhase_ = AttackPhase::kAim;
			isAiming_ = true;
			canAirDash_ = false;
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


	//// ----- Slope sliding -----
	//if (onGround_ && mapChipField_)
	//{
	//	MapChip::IndexSet idx = mapChipField_->GetMapChipIndexSetByPosition(playerTransform_.translation_);
	//	MapChipType type = mapChipField_->GetMapChipTypeByIndex(idx.xIndex, idx.yIndex);

	//	float slopeDir = 0.0f;		// +1 = downhill is right, -1 = downhill is left

	//	switch (type)
	//	{
	//	case MapChipType::kSlope1L:
	//	case MapChipType::kSlope2L:
	//	case MapChipType::kSlope3L:
	//		slopeDir = +1.0f;
	//		break;

	//	case MapChipType::kSlope1R:
	//	case MapChipType::kSlope2R:
	//	case MapChipType::kSlope3R:
	//		slopeDir = -1.0f;
	//		break;

	//	default:
	//		break;
	//	}

	//	if (slopeDir != 0.0f)
	//	{
	//		// 1. Normal speed-scaled slide force (from previous version)
	//		float speedAlongSlope = velocity_.x * slopeDir;
	//		float force = kSlopeSlideForce;

	//		if (speedAlongSlope > 0.0f)
	//			force += speedAlongSlope * 0.12f;		// downhill help
	//		else
	//			force += speedAlongSlope * 0.18f;		// uphill resistance

	//		velocity_.x += force * slopeDir;

	//		// 2. Convert some downward velocity into downhill speed
	//		if (velocity_.y < 0.0f)		// only when falling / sliding down
	//		{
	//			// How much of the fall speed we turn into horizontal speed
	//			const float conversionRate = 0.05f;		// 0.4 ~ 0.7 feels good

	//			float converted = -velocity_.y * conversionRate;
	//			velocity_.x += converted * slopeDir;

	//			// Optional: reduce the remaining vertical speed a little
	//			// so the player doesn’t keep bouncing
	//			velocity_.y *= 0.7f;
	//		}

	//		// Safety clamp
	//		velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed * 1.5f, kLimitRunSpeed * 1.5f);
	//	}
	//}

	//velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

	//if (turnTimer_ > 0.0f) {
	//	turnTimer_ -= 1.0f / 60.0f; // 60 FPS

	//	float destinationRotationYTable[] = {
	//		std::numbers::pi_v<float> / 2.0f,        // kRight
	//		std::numbers::pi_v<float> * 3.0f / 2.0f, // kLeft
	//	};
	//	float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
	//	playerTransform_.rotation_.y = Lerp(turnFirstRotationY_, destinationRotationY, 8.0f - (turnTimer_ / kTimeTurn));
	//	if (turnTimer_ <= 0.0f) playerTransform_.rotation_.y = destinationRotationY;
	//}

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
		//canDash_ = true;
		canAirDash_ = false;
		DebugText::GetInstance()->ConsolePrintf("landing\n");
	}
}

void Player::BehaviorAttackInitialize() {
	attackParameter_ = 0;
}

void Player::BehaviorAttackUpdate() {
	if (!modelAttack_) return;

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
		chargePower_ = std::clamp(chargePower_, 0.0f, 1.0f);
		chargePower_ += 1.0f / 30.0f;
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

			playerTransform_.rotation_.y = (lrDirection_ == LRDirection::kRight)
				? std::numbers::pi_v<float> *0.5f
				: std::numbers::pi_v<float> *1.5f;
		}

		// slight float while aiming
		velocity_.y *= 0.85f;
		velocity_.y = (std::max)(velocity_.y, -0.04f);

		// Confirm dash with Jump
		if (!Input::GetInstance()->PushKey(DIK_B) && (inputX != 0.0f || inputY != 0.0f || true)) {
			// even if no key is held we still allow the dash in the last direction
			velocity_ = MultiplyByS(aimDirection_, attackVelocity_ * chargePower_);

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
		CollisionMapInfo info;
		info.move = velocity_;

		const int vSteps = 6;
		for (int i = 0; i < vSteps; ++i) {
			playerTransform_.translation_.y += velocity_.y * 0.6f;
			MapCollisionY(info);
		}
		const int hSteps = 12;
		for (int i = 0; i < hSteps; ++i) {
			playerTransform_.translation_.x += velocity_.x / hSteps;
			MapCollisionX(info);
		}
		
		// if we hit a wall or ceiling, kill the corresponding velocity
		if (info.wallFlagL || info.wallFlagR) velocity_.x = 0.0f;
		if (info.ceilingFlag)               velocity_.y = 0.0f;
		if (info.landingFlag) {
			velocity_.y = 0.0f;
			onGround_ = true;
			attackPhase_ = AttackPhase::kCoil;
			behaviorRequest_ = Behavior::kRoot;
			attackParameter_ = 0;
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
	//-----------------------------------------------------
	case AttackPhase::kLinger: 
	{
		objectColor_.SetColor(attackColor_);
		velocity_.x *= 0.85f;
		velocity_.y *= 0.85f;

		{
			CollisionMapInfo info;
			info.move = velocity_;

			playerTransform_.translation_.y += velocity_.y;
			MapCollisionY(info);
			playerTransform_.translation_.x += velocity_.x;
			MapCollisionX(info);

			if (info.landingFlag) {
				velocity_.y = 0.0f;
				onGround_ = true;
				attackPhase_ = AttackPhase::kCoil;
				behaviorRequest_ = Behavior::kRoot;
				attackParameter_ = 0;
			}
		}

		float t = attackParameter_ / lingerDuration_;
		playerTransform_.scale_.z = EaseOut(1.6f, 1.0f, t);
		playerTransform_.scale_.y = EaseOut(0.7f, 1.0f, t);

		if (attackParameter_ >= (lingerDuration_)) {
			behaviorRequest_ = Behavior::kRoot;
			isAttack_ = false;
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
