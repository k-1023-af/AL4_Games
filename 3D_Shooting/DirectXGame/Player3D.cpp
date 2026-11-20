#include "Player3D.h"
#include "MapChip3D.h"
#include "Enemy3D.h"

Player3D::Player3D() { }

Player3D::~Player3D() { }

void Player3D::Initialize(Camera* camera, const Vector3& position)
{
	//==============================================================================
	//  Camera
	//==============================================================================
	camera_ = camera;

	//==============================================================================
	//  Player Transform + Models
	//==============================================================================
	playerTransform_.Initialize();
	playerTransform_.scale_ = { 0.5f, 0.5f, 0.5f };
	playerTransform_.translation_ = position;

	playerModel_ = Model::CreateFromOBJ("player", true);
	//modelAttack_ = Model::CreateFromOBJ("attack", true);
	//wtAttack_.Initialize();

	//==============================================================================
	//  Color
	//==============================================================================
	objectColor_.Initialize();
	objectColor_.SetColor(normalColor_);

	//==============================================================================
	//  Initial Behavior
	//==============================================================================
	velocity_ = { 0.0f, 0.0f, 0.0f };
	lookDirection_ = LookDirection::kFront;

	isAiming_ = false;



	worldUpdater_.Update(playerTransform_);

}

void Player3D::Update()
{
	//==============================================================================
	//  Initial checks
	//==============================================================================
	

	CollisionMapInfo info;
	info.move = velocity_;
	objectColor_.SetColor(currentColor_);

	//==============================================================================
	//  Floor Check
	//==============================================================================
	if (playerTransform_.translation_.y <= 0.5f) {
		playerTransform_.translation_.y = 0.5f;
		velocity_.y = 0.0f;
		//onGround_ = true;
		//canDash_ = true;
		//canAirDash_ = true;
		//velocity_.x *= (1.0f - kAttenuationLanding);
	}

	//==============================================================================
	//	  [1] movement input	 
	//==============================================================================
	if (Input::GetInstance()->PushKey(DIK_W)) {
		velocity_.z += kAcceleration;
	}

	if (Input::GetInstance()->PushKey(DIK_S)) {
		velocity_.z -= kAcceleration;
	}

	if (Input::GetInstance()->PushKey(DIK_D)) {
		velocity_.x += kAcceleration;
		//if (lrDirection_ != LRDirection::kRight) {
		//	lrDirection_ = LRDirection::kRight;
		//	turnFirstRotationY_ = playerTransform_.rotation_.y;
		//	turnTimer_ = kTimeTurn;
		//}
	}
	if (Input::GetInstance()->PushKey(DIK_A)) {
		velocity_.x -= kAcceleration;
		//if (lrDirection_ != LRDirection::kLeft) {
		//	lrDirection_ = LRDirection::kLeft;
		//	turnFirstRotationY_ = playerTransform_.rotation_.y;
		//	turnTimer_ = kTimeTurn;
		//}
	}

	if (velocity_.x != 0) {
		velocity_.x *= (1.0f - kAttenuation);
	}
	if (velocity_.z != 0) {
		velocity_.z *= (1.0f - kAttenuation);
	}

	velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
	velocity_.z = std::clamp(velocity_.z, -kLimitRunSpeed, kLimitRunSpeed);

	////------------------------------------////
//	[2]collision detection with movement  //
////------------------------------------////

// Apply velocity then collide
	const int zSteps = 8;
	for (int i = 0; i < zSteps; ++i) {
		playerTransform_.translation_.z += velocity_.z / zSteps;
		MapCollisionZ(info);
		if (info.zWallFront || info.zWallBack) {
			velocity_.z = 0.0f;
			break;
		}
	}

	// Horizontal sub-steps
	const int xSteps = 6;
	for (int i = 0; i < xSteps; ++i) {
		playerTransform_.translation_.x += velocity_.x / xSteps;
		MapCollisionX(info);
		if (info.xWallLeft || info.xWallRight) {
			velocity_.x = 0.0f;
			break;
		}
	}

	//if (info.ceilingFlag) {
	//	velocity_.y = 0;
	//	DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
	//}
	//if (info.landingFlag) {
	//	velocity_.y = 0;
	//	onGround_ = true;
	//	canDash_ = true;
	//	DebugText::GetInstance()->ConsolePrintf("landing\n");
	//}

	//==============================================================================
	//  Phase Update
	//==============================================================================


	//==============================================================================
	//  World Transform
	//==============================================================================
	worldUpdater_.Update(playerTransform_);
}

void Player3D::Draw(Camera& camera)
{
	if (isDead_) return;

	playerTransform_.TransferMatrix();

	//==============================================================================
	//  Player Models
	//==============================================================================
	Model::PreDraw();
	playerModel_->Draw(playerTransform_, camera, &objectColor_);
	Model::PostDraw();

	// --- Debug Wireframe Box ---
	Vector3 pos = playerTransform_.translation_;
	float hx = kWidth / 2.0f;
	float hy = kHeight / 2.0f;
	float hz = kDepth / 2.0f;
	Vector4 color = { 0.0f, 1.0f, 0.0f, 1.0f }; // Bright green

	// 8 corners of the box
	Vector3 p[8] = {
		{ pos.x - hx, pos.y - hy, pos.z - hz }, // 0: bottom-left-front
		{ pos.x + hx, pos.y - hy, pos.z - hz }, // 1: bottom-right-front
		{ pos.x + hx, pos.y - hy, pos.z + hz }, // 2: bottom-right-back
		{ pos.x - hx, pos.y - hy, pos.z + hz }, // 3: bottom-left-back
		{ pos.x - hx, pos.y + hy, pos.z - hz }, // 4: top-left-front
		{ pos.x + hx, pos.y + hy, pos.z - hz }, // 5: top-right-front
		{ pos.x + hx, pos.y + hy, pos.z + hz }, // 6: top-right-back
		{ pos.x - hx, pos.y + hy, pos.z + hz }, // 7: top-left-back
	};

	auto* drawer = PrimitiveDrawer::GetInstance();
	for (int i = 0; i < 4; ++i) {
		int next = (i + 1) % 4;
		drawer->DrawLine3d(p[i], p[next], color);         // Bottom face
		drawer->DrawLine3d(p[i + 4], p[next + 4], color); // Top face
		drawer->DrawLine3d(p[i], p[i + 4], color);        // Vertical pillars
	}

	//==============================================================================
	//  Aiming Reticle
	//==============================================================================

	//if (isAiming_) {

	float inputX = 0.0f;
	float inputZ = 0.0f;

	if (Input::GetInstance()->PushKey(DIK_D)) inputX += 1.0f;
	if (Input::GetInstance()->PushKey(DIK_A)) inputX -= 1.0f;
	if (Input::GetInstance()->PushKey(DIK_W)) inputZ += 1.0f;
	if (Input::GetInstance()->PushKey(DIK_S)) inputZ -= 1.0f;

	if (inputX != 0.0f || inputZ != 0.0f) {
		// Normalize so diagonal is not faster
		float len = std::sqrt(inputX * inputX + inputZ * inputZ);
		aimDirection_.x = inputX / len;
		aimDirection_.y = 0.0f;
		aimDirection_.z = inputZ / len;

		//if (aimDirection_.x > 0.01f)
		//	lrDirection_ = LRDirection::kRight;
		//else if (aimDirection_.x < -0.01f)
		//	lrDirection_ = LRDirection::kLeft;
	}
		Vector3 start = playerTransform_.translation_;
		//start.y += 0.3f;
	//
	//	// Full-length faint guide
		Vector3 fullEnd = Add(start, Multiply(aimDirection_, kArrowLength));
		PrimitiveDrawer::GetInstance()->DrawLine3d(
			start, fullEnd, { 0.4f, 0.4f, 0.4f, 0.6f });   // grey guide
	//
	//	// Charge fill (grows with chargePower_)
	//	float chargeT = std::clamp(chargePower_, 0.0f, 1.0f);
	//	Vector3 chargeEnd = Add(start, Multiply(aimDirection_, kArrowLength * chargeT));
	//
	//	// Color goes from yellow → orange → red as it charges
	//	Vector4 chargeColor = {
	//		1.0f,
	//		1.0f - chargeT * 0.7f,   // green channel decreases
	//		0.15f,
	//		1.0f
	//	};
	//
	//	PrimitiveDrawer::GetInstance()->DrawLine3d(start, chargeEnd, chargeColor);
	//
	//	// Arrow head at the tip of the charge line (or full line – your choice)
	//	Vector3 side = { -aimDirection_.y, aimDirection_.x, 0.0f };
	//	side = Multiply(side, 0.35f);
	//
	//	Vector3 back = Multiply(aimDirection_, 0.55f);
	//	Vector3 headBase = Subtract(chargeEnd, back);   // tip of the charged part
	//
	//	Vector3 head1 = Add(headBase, side);
	//	Vector3 head2 = Subtract(headBase, side);
	//
	//	PrimitiveDrawer::GetInstance()->DrawLine3d(chargeEnd, head1, chargeColor);
	//	PrimitiveDrawer::GetInstance()->DrawLine3d(chargeEnd, head2, chargeColor);
	//}

	//==============================================================================
	//  Player Attack
	//==============================================================================



}

AABB Player3D::GetAABB()
{
	Vector3 worldPos = GetWorldPosition();
	AABB aabb;
	aabb.min = { worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f };
	aabb.max = { worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f };
	return aabb;
}

Vector3 Player3D::GetCornerPosition(const Vector3& center, Corner corner)
{
	Vector3 offsetTable[kNumCorner] = {
		//{kWidth / 2.0f,  -kHeight / 2.0f, 0.0f}, // kRightBottom
		//{-kWidth / 2.0f, -kHeight / 2.0f, 0.0f}, // kLeftBottom
		//{kWidth / 2.0f,  kHeight / 2.0f,  0.0f}, // kRightTop
		//{-kWidth / 2.0f, kHeight / 2.0f,  0.0f}  // kLeftTop

		{+kWidth / 2.0f, 0.0f, -kDepth / 2.0f},	//kRightFront
		{-kWidth / 2.0f, 0.0f, -kDepth / 2.0f},	//kleftFront
		{+kWidth / 2.0f, 0.0f, +kDepth / 2.0f},	//kRightBack
		{-kWidth / 2.0f, 0.0f, +kDepth / 2.0f},	//kLeftBack
	};

	Vector3 offset = offsetTable[static_cast<uint32_t>(corner)];

	Vector3 result;
	result.x = center.x + offset.x;
	result.y = center.y + offset.y;
	result.z = center.z + offset.z;
	return result;
}

Vector3 Player3D::GetWorldPosition()
{
	Vector3 worldPos;
	worldPos.x = playerTransform_.matWorld_.m[3][0];
	worldPos.y = playerTransform_.matWorld_.m[3][1];
	worldPos.z = playerTransform_.matWorld_.m[3][2];
	return worldPos;
}

Vector3 Player3D::GetScale()
{
	Vector3 worldScale;
	worldScale.x = playerTransform_.matWorld_.m[0][0];
	worldScale.y = playerTransform_.matWorld_.m[1][1];
	worldScale.z = playerTransform_.matWorld_.m[2][2];
	return worldScale;
}

//Vector3 Player3D::GetRotation()
//{
//	return Vector3();
//}

void Player3D::SetMapChipField(MapChip3D* mapChipField)
{
	mapChipField_ = mapChipField;
}

void Player3D::MapCollisionX(CollisionMapInfo& info)
{
	if (!mapChipField_) return;

	const int xSteps = 8;
	float stepX = velocity_.x / xSteps;

	Vector3 prevCenter = {
		playerTransform_.translation_.x - stepX,
		playerTransform_.translation_.y,
		playerTransform_.translation_.z
	};

	std::array<Vector3, 4> oldPos, newPos;
	for (uint32_t i = 0; i < 4; ++i) {
		oldPos[i] = GetCornerPosition(prevCenter, static_cast<Corner>(i));
		newPos[i] = GetCornerPosition(playerTransform_.translation_, static_cast<Corner>(i));
	}

	info.xWallLeft = info.xWallRight = false;

	// Test every corner – the diagonal ones will now be caught
	bool hit = false;
	hit |= ResolveCornerCollision(oldPos[kLeftFront], newPos[kLeftFront], true, false);
	hit |= ResolveCornerCollision(oldPos[kRightFront], newPos[kRightFront], true, false);
	hit |= ResolveCornerCollision(oldPos[kLeftBack], newPos[kLeftBack], true, false);
	hit |= ResolveCornerCollision(oldPos[kRightBack], newPos[kRightBack], true, false);

	if (hit) {
		if (info.move.x > 0) info.xWallLeft = true;
		else                 info.xWallRight = true;
		velocity_.x = 0.0f;
	}

}

//void Player3D::MapCollisionY(CollisionMapInfo& info) {if (!mapChipField_) { return; }}

void Player3D::MapCollisionZ(CollisionMapInfo& info)
{
	if (!mapChipField_) return;

	const int zSteps = 8;
	float stepZ = velocity_.z / zSteps;

	Vector3 prevCenter = {
		playerTransform_.translation_.x,
		playerTransform_.translation_.y,
		playerTransform_.translation_.z - stepZ
	};

	std::array<Vector3, 4> oldPos, newPos;
	for (uint32_t i = 0; i < 4; ++i) {
		oldPos[i] = GetCornerPosition(prevCenter, static_cast<Corner>(i));
		newPos[i] = GetCornerPosition(playerTransform_.translation_, static_cast<Corner>(i));
	}

	info.zWallFront = info.zWallBack = false;

	bool hit = false;
	hit |= ResolveCornerCollision(oldPos[kLeftFront], newPos[kLeftFront], false, true);
	hit |= ResolveCornerCollision(oldPos[kRightFront], newPos[kRightFront], false, true);
	hit |= ResolveCornerCollision(oldPos[kLeftBack], newPos[kLeftBack], false, true);
	hit |= ResolveCornerCollision(oldPos[kRightBack], newPos[kRightBack], false, true);

	if (hit) {
		if (info.move.z < 0) info.zWallFront = true;
		else                 info.zWallBack = true;
		velocity_.z = 0.0f;
	}
}

bool Player3D::ResolveCornerCollision(const Vector3& oldPos, const Vector3& newPos, bool checkX, bool checkZ)
{
	MapChip3D::IndexSet oldIdx = mapChipField_->GetMapChipIndexByPosition(oldPos);
	MapChip3D::IndexSet newIdx = mapChipField_->GetMapChipIndexByPosition(newPos);

	// Only care when the index actually changed
	if (oldIdx.xIndex == newIdx.xIndex && oldIdx.zIndex == newIdx.zIndex)
		return false;

	// The cell we are entering
	MapChipType entered = mapChipField_->GetMapChipTypeByIndex(newIdx.xIndex, newIdx.zIndex);
	if (entered != MapChipType::kBlock)
		return false;

	// We hit a solid cell → snap back to the edge of the *previous* cell
	MapChip3D::Rect rect = mapChipField_->GetRectByIndex(oldIdx.xIndex, oldIdx.zIndex);

	if (checkX && newIdx.xIndex != oldIdx.xIndex)
	{
		if (newIdx.xIndex > oldIdx.xIndex) // moved right
			playerTransform_.translation_.x = rect.right - (kWidth * 0.5f) - kBlank;
		else                               // moved left
			playerTransform_.translation_.x = rect.left + (kWidth * 0.5f) + kBlank;
	}

	if (checkZ && newIdx.zIndex != oldIdx.zIndex)
	{
		// Remember Z is inverted: higher zIndex = smaller world Z (front)
		if (newIdx.zIndex > oldIdx.zIndex) // moved front (world -Z)
			playerTransform_.translation_.z = rect.front + (kDepth * 0.5f) + kBlank;
		else                               // moved back  (world +Z)
			playerTransform_.translation_.z = rect.back - (kDepth * 0.5f) - kBlank;
	}

	return true;
}

void Player3D::EnemyCollision(Enemy3D* enemy)
{
	if (IsAttack()) { return; }

	DebugText::GetInstance()->ConsolePrintf("HIT!\n");
	(void)enemy;
	playerTransform_.translation_.y += 2.0f;
	isDead_ = true;
}


