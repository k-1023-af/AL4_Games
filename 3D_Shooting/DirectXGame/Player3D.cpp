#include "Player3D.h"
#include "MapChip3D.h"
#include "Enemy3D.h"

void Player3D::Initialize(Camera* camera, const Vector3& position)
{
	camera_ = camera;

	playerTransform_.Initialize();
	playerTransform_.translation_ = position;


	//playerModel_ = Model::CreateFromOBJ("player", true);
	//modelAttack_ = Model::CreateFromOBJ("attack", true);
	//wtAttack_.Initialize();

	objectColor_.Initialize();
	objectColor_.SetColor(normalColor_);

	// Initial behavior
	velocity_ = { 0.0f, 0.0f, 0.0f };
	lookDirection_ = LookDirection::kDown;

	isAiming_ = false;

}

void Player3D::Update()
{


}

void Player3D::Draw()
{
	if (isDead_) return;

	playerTransform_.TransferMatrix();
	//playerModel_->Draw(playerTransform_, *camera_, &objectColor_);

	//===============================================================

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

void Player3D::EnemyCollision(Enemy3D* enemy)
{
	//if (IsAttack()) { return; }

	DebugText::GetInstance()->ConsolePrintf("HIT!\n");
	(void)enemy;
	//playerTransform_.translation_.y += 2.0f;
	isDead_ = true;
}


