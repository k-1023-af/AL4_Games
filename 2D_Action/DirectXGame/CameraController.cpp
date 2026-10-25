#include "CameraController.h"
#include "Player.h"

void CameraController::Initialize(KamataEngine::Camera* mainCamera) {
	camera_ = mainCamera;
	if (camera_) {
		camera_->rotation_ = { 0.0f, 0.0f, 0.0f };
	}
}

void CameraController::Update() {
	if (!camera_ || !target_) return;

	const WorldTransform& targetWorldTransform = target_->GetTransform();
	const Vector3& targetVelocity = target_->GetVelocity();

	// Desired camera position (offset + looking ahead)
	Vector3 desired = {
		targetWorldTransform.translation_.x + targetOffset_.x, 
		targetWorldTransform.translation_.y + targetOffset_.y,
		targetWorldTransform.translation_.z + targetOffset_.z,
	};
	//desired.x += targetVelocity.x * (kVelocityBias * 3);

	float lookAheadMin = isPSpeed_ ? kPLookAheadMin : kLookAheadMin;
	float lookAheadPer = isPSpeed_ ? kPLookAheadPerSpeed : kLookAheadPerSpeed;
	float lookAheadMax = isPSpeed_ ? kPLookAheadMax : kLookAheadMax;

	float speedX = std::abs(targetVelocity.x);
	float lookAhead = lookAheadMin + speedX * lookAheadPer;
	lookAhead = std::clamp(lookAhead, lookAheadMin, lookAheadMax);

	if (targetVelocity.x > 0.1f) {			//looking right
		desired.x += lookAhead;
	}
	else if (targetVelocity.x < -0.01f) {	//looking left
		desired.x -= lookAhead;
	}

	// ---------- Horizontal (elastic movement)

	//float diffX = desired.x - camera_->translation_.x;
	//
	//
	//if (std::abs(diffX) > kDeadZoneHalfWidth) {		// dead zone
	//	float tX = kHorizontalBaseLerp + std::abs(diffX) * kHorizontalSpringGain;
	//	tX = std::clamp(tX, kHorizontalBaseLerp, kHorizontalMaxLerp);
	//
	//	camera_->translation_.x = Lerp(camera_->translation_.x, desired.x, tX);
	//}

	// ----------- Vertical (dead zone)

	float diffY = desired.y - camera_->translation_.y;

	if (std::abs(diffY) > kDeadZoneHalfHeight) {
		// Outside the dead zone → pull harder
		camera_->translation_.y = Lerp(camera_->translation_.y, desired.y, kVerticalHardLerp);
	}
	else {
		// Inside the dead zone → very soft (or completely ignore)
		camera_->translation_.y = Lerp(camera_->translation_.y, desired.y, kVerticalSoftLerp);
		// Uncomment the next line if you want the camera to stay perfectly still
		// while the player is inside the dead zone:
		// (do nothing)
	}

	// old (backup)
	camera_->translation_.x = (targetWorldTransform.translation_.x + targetOffset_.x) + (targetVelocity.x * kVelocityBias);
	//camera_->translation_.y = (targetWorldTransform.translation_.y + targetOffset_.y) + (targetVelocity.y * kVelocityBias);
	//camera_->translation_.z = (targetWorldTransform.translation_.z + targetOffset_.z) + (targetVelocity.z * kVelocityBias);
	//if (abs(targetVelocity.x) > 0.1f) {
		camera_->translation_.z = desired.z - abs(targetVelocity.x) * kVelocityBias ;
	//}
	//else {
	//	camera_->translation_.z = desired.z;
	//}

	camera_->translation_.z = Lerp(camera_->translation_.z, targetWorldTransform.translation_.z, kInterpolationRate);

	// clamp
	camera_->translation_.x = std::clamp(camera_->translation_.x, moveableArea_.left, moveableArea_.right);
	camera_->translation_.y = std::clamp(camera_->translation_.y, moveableArea_.bottom, moveableArea_.top);

	camera_->UpdateMatrix();
}

void CameraController::SetTarget(Player* target) {
	target_ = target;
}

void CameraController::SetMoveableArea(MapChip* mapChipStage) {
	if (!mapChipStage) return;

	const uint32_t width = mapChipStage->GetNumBlockHorizontal();
	const uint32_t height = mapChipStage->GetNumBlockVertical();
	const float blockSize = 1.0f;	// current block size

	// the border box for the area the camera can move
	moveableArea_.left = 0.0f + margin.left;
	moveableArea_.right = width * blockSize + margin.right;
	moveableArea_.bottom = 0.0f + margin.bottom;
	moveableArea_.top = height * blockSize + margin.top;
}

void CameraController::Reset() {
	if (!camera_ || !target_) return;

	const WorldTransform& targetWorldTransform = target_->GetTransform();

	camera_->translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;
	camera_->translation_.y = targetWorldTransform.translation_.y + targetOffset_.y;
	camera_->translation_.z = targetWorldTransform.translation_.z + targetOffset_.z;
	camera_->UpdateMatrix();
}

CameraController::CameraController() = default;

CameraController::~CameraController() { }
