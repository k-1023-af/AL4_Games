#include "CameraController.h"
//#include "Player3D.h"

void CameraController::Initialize(Camera* mainCamera)
{
	camera_ = mainCamera;
	if (camera_) {
		camera_->rotation_ = { 0.0f, 0.0f, 0.0f };
	}
}

void CameraController::SetTarget(Player3D* target)
{
	target_ = target;
}

void CameraController::SetMoveableArea(MapChip3D* mapChipField)
{
	if (!mapChipField) return;

	const uint32_t width =  mapChipField->GetNumBlockXAxis();
	const uint32_t height = mapChipField->GetNumBlockYAxis();
	const uint32_t depth =  mapChipField->GetNumBlockZAxis();
	const float blockSize = 1.0f; // current block size

	// Camera can move from near left edge to near right edge of the map
	moveableArea_.left = 0.0f + margin.left;
	moveableArea_.right = width * blockSize + margin.right;
	moveableArea_.bottom = 0.0f + margin.bottom;
	moveableArea_.top = height * blockSize + margin.top;
	moveableArea_.in = 0.0f, +margin.in;
	moveableArea_.out = depth * blockSize + margin.out;
}

void CameraController::Update()
{
	const WorldTransform& targetWorldTransform = target_->GetTransform();
	const Vector3& targetVelocity = target_->GetVelocity();

	camera_->translation_.x = (targetWorldTransform.translation_.x + targetOffset_.x) + (targetVelocity.x * kVelocityBias);
	camera_->translation_.y = (targetWorldTransform.translation_.y + targetOffset_.y) + (targetVelocity.y * kVelocityBias);
	camera_->translation_.z = (targetWorldTransform.translation_.z + targetOffset_.z) + (targetVelocity.z * kVelocityBias);

	camera_->translation_ = Lerp(camera_->translation_, targetWorldTransform.translation_, kInterpolationRate);

	camera_->translation_.x = std::clamp(camera_->translation_.x, moveableArea_.left, moveableArea_.right);
	camera_->translation_.y = std::clamp(camera_->translation_.y, moveableArea_.bottom, moveableArea_.top);
	camera_->translation_.z = std::clamp(camera_->translation_.z, moveableArea_.in, moveableArea_.out);

	camera_->UpdateMatrix();
}

void CameraController::Reset()
{
	const WorldTransform& targetWorldTransform = target_->GetTransform();

	camera_->translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;
	camera_->translation_.y = targetWorldTransform.translation_.y + targetOffset_.y;
	camera_->translation_.z = targetWorldTransform.translation_.z + targetOffset_.z;
	camera_->UpdateMatrix();
}


CameraController::CameraController() = default;

CameraController::~CameraController() {}