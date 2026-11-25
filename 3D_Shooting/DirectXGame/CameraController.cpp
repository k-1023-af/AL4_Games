#include "CameraController.h"
//#include "Player3D.h"

void CameraController::Initialize(Camera* mainCamera)
{
	camera_ = mainCamera;
	if (camera_) {
		camera_->rotation_ = { 0.0f, 0.0f, 0.0f };
		camera_->nearZ = 0.1f;
		camera_->farZ = 20000.0f;
	}

	debugCamera_ = new DebugCamera(1280, 720);
	debugCamera_->SetNearZ(0.1f);
	debugCamera_->SetFarZ(20000.0f);
	AxisIndicator::GetInstance()->SetVisible(false);
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());
}

//void CameraController::SetTarget(Player3D* target)
//{
//	target_ = target;
//}

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

void CameraController::SetMode(CameraMode mode)
{
	if (mode_ == mode) return;

	prevMode_ = mode_;
	mode_ = mode;

	if (mode_ == CameraMode::kDebug) {
		AxisIndicator::GetInstance()->SetVisible(true);
	}
	else if (prevMode_ == CameraMode::kDebug) {
		AxisIndicator::GetInstance()->SetVisible(false);
	}
}

void CameraController::Update()
{

	if (mode_ == CameraMode::kFollow && Input::GetInstance()->IsPressMouse(1)) {
		mode_ = CameraMode::kAim;
	}
	else if (mode_ == CameraMode::kAim && !Input::GetInstance()->IsPressMouse(1)) {
		mode_ = CameraMode::kFollow;
	}

	switch (mode_) {
	case CameraMode::kFollow:
	default:
		UpdateFollow();
		break;
	case CameraMode::kAim:
		UpdateAim();
		break;
	case CameraMode::kObjective:
		UpdateObjective();
		break;
	case CameraMode::kDebug:
		UpdateDebugCamera();
		break;
	}

}

void CameraController::Reset()
{
	const WorldTransform& targetWorldTransform = target_->GetTransform();

	targetOffset_ = { 0.0f, 50.0f, 0.0f };
	targetRotation_ = { 1.57f, 0.0f, 0.0f };

	camera_->translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;
	camera_->translation_.y = targetWorldTransform.translation_.y + targetOffset_.y;
	camera_->translation_.z = targetWorldTransform.translation_.z + targetOffset_.z;
	camera_->UpdateMatrix();
}

void CameraController::ApplyShake()
{


}

Vector2 CameraController::GetMousePosition() const
{
	POINT pt;
	GetCursorPos(&pt);

	HWND hwnd = GetActiveWindow();
	if (hwnd) {
		ScreenToClient(hwnd, &pt);
	}

	return { static_cast<float>(pt.x), static_cast<float>(pt.y) };
}

Vector2 CameraController::WorldToScreen(const Vector3& worldPos, const Camera& camera, float screenW, float screenH)
{
	// World -> View -> Projection
	Matrix4x4 matVP = Multiply(camera.matView, camera.matProjection);

	Vector3 clip;
	clip.x = (worldPos.x * matVP.m[0][0]) + (worldPos.y * matVP.m[1][0]) + (worldPos.z * matVP.m[2][0]) + matVP.m[3][0];
	clip.y = (worldPos.x * matVP.m[0][1]) + (worldPos.y * matVP.m[1][1]) + (worldPos.z * matVP.m[2][1]) + matVP.m[3][1];
	clip.z = (worldPos.x * matVP.m[0][2]) + (worldPos.y * matVP.m[1][2]) + (worldPos.z * matVP.m[2][2]) + matVP.m[3][2];
	float w = (worldPos.x * matVP.m[0][3]) + (worldPos.y * matVP.m[1][3]) + (worldPos.z * matVP.m[2][3]) + matVP.m[3][3];

	if (fabsf(w) < 1e-5f) w = 1e-5f;
	
	// NDC
	float ndcX = clip.x / w;
	float ndcY = clip.y / w;

	// Screen (top-left origin)
	Vector2 screen;
	screen.x = (ndcX * 0.5f + 0.5f) * screenW;
	screen.y = (1.0f - (ndcY * 0.5f + 0.5f)) * screenH;	// flip Y for typical window coordinates

	return screen;
}

void CameraController::UpdateFollow()
{
	const WorldTransform& targetWorldTransform = target_->GetTransform();
	const Vector3& targetVelocity = target_->GetVelocity();

	camera_->rotation_ = targetRotation_;

	camera_->translation_.x = (targetWorldTransform.translation_.x + targetOffset_.x) + (targetVelocity.x * kVelocityBias);
	camera_->translation_.y = (targetWorldTransform.translation_.y + targetOffset_.y) + (targetVelocity.y * kVelocityBias);
	camera_->translation_.z = (targetWorldTransform.translation_.z + targetOffset_.z) + (targetVelocity.z * kVelocityBias);

	camera_->translation_ = EaseIn(camera_->translation_, targetWorldTransform.translation_, kInterpolationRate);

	camera_->translation_.x = std::clamp(camera_->translation_.x, moveableArea_.left, moveableArea_.right);
	camera_->translation_.y = std::clamp(camera_->translation_.y, moveableArea_.bottom, moveableArea_.top);
	camera_->translation_.z = std::clamp(camera_->translation_.z, moveableArea_.in, moveableArea_.out);

	camera_->UpdateMatrix();
}

void CameraController::UpdateAim()
{
	if (!target_ || !camera_) return;

	const Vector3 playerPos = target_->GetTransform().translation_;

	// Where is the player on the screen
	Vector2 playerScreen = WorldToScreen(playerPos, *camera_, screenWidth_, screenHeight_);

	Vector2 mouse = GetMousePosition();
	//Vector2 center = { screenWidth_ * 0.5f, screenHeight_ * 0.5f };
	Vector2 screenOffset = { mouse.x - playerScreen.x, mouse.y - playerScreen.y };

	// Dead zone
	float len = std::sqrt(screenOffset.x * screenOffset.x + screenOffset.y * screenOffset.y);
	if (len < aimDeadZone_) {
		screenOffset = { 0.0f, 0.0f };
	}
	else {
		float scale = (len - aimDeadZone_) / len;
		screenOffset.x *= scale;
		screenOffset.y *= scale;
	}

	Vector3 aimOffset;
	aimOffset.x = screenOffset.x * aimSensitivity_;
	aimOffset.y = 0.0f;
	aimOffset.z = -screenOffset.y * aimSensitivity_;

	// Player stays on screen
	float dist = std::sqrt(aimOffset.x * aimOffset.x + aimOffset.z * aimOffset.z);
	if (dist > aimMaxDistance_ && dist > 0.00001f) {
		float s = aimMaxDistance_ / dist;
		aimOffset.x *= s;
		aimOffset.z *= s;
	}

	Vector3 desired;
	desired.x = playerPos.x + targetOffset_.x + aimOffset.x;
	desired.y = camera_->translation_.y;
	desired.z = playerPos.z + targetOffset_.z + aimOffset.z;

	camera_->rotation_ = targetRotation_;
	camera_->translation_ = EaseIn(camera_->translation_, desired, aimLerpRate_);

	// map bounds
	camera_->translation_.x = std::clamp(camera_->translation_.x, moveableArea_.left, moveableArea_.right);
	camera_->translation_.y = std::clamp(camera_->translation_.y, moveableArea_.bottom, moveableArea_.top);
	camera_->translation_.z = std::clamp(camera_->translation_.z, moveableArea_.in, moveableArea_.out);

	camera_->UpdateMatrix();
}

void CameraController::UpdateObjective()
{


}

void CameraController::UpdateDebugCamera()
{
	if (!debugCamera_ || !camera_) return;

	debugCamera_->Update();

	camera_->matView = debugCamera_->GetCamera().matView;
	camera_->matProjection = debugCamera_->GetCamera().matProjection;
	camera_->TransferMatrix();
}


CameraController::CameraController() = default;

CameraController::~CameraController() {
	delete debugCamera_;
	debugCamera_ = nullptr;
}