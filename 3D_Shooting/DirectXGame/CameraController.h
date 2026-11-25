#pragma once
#define NOMINMAX
#include "KamataEngine.h"
#include "Windows.h"
#include <algorithm>
#include "WorldUpdate.h"
#include "MapChip3D.h"
#include "Player3D.h"
#include "Formula.h"

class Player; // forward


enum class CameraMode {
	kFollow,
	kAim,
	kObjective,
	kDebug,
};

class CameraController {
public:
	CameraController();
	~CameraController();

	void Initialize(Camera* mainCamera);

	CameraMode GetMode() const { return mode_; }
	Vector3& GetTargetOffset() { return targetOffset_; }
	const Vector3& GetTargetOffset() const { return targetOffset_; }
	Vector3& GetTargetRotation() { return targetRotation_; }
	const Vector3& GetTargetRotation() const { return targetRotation_; }

	void SetMoveableArea(MapChip3D* mapChipField);
	void SetTarget(Player3D* target) { target_ = target; }
	void SetMode(CameraMode mode);

	void Update();
	void Reset();
	void ApplyShake();


	struct Rect {
		float left = 0.0f;		//min X
		float right = 1.0f;		//max X
		float bottom = 0.0f;	//min Y
		float top = 1.0f;		//max Y
		float in = 0.0f;		//min Z
		float out = 1.0f;		//max Z
	};

private:
	Camera* camera_ = nullptr;
	DebugCamera* debugCamera_ = nullptr;
	Player3D* target_ = nullptr;
	WorldUpdate* worldUpdater_;
	CameraMode mode_ = CameraMode::kFollow;
	CameraMode prevMode_ = CameraMode::kFollow;

	Rect moveableArea_ = { 0.0f, 100.0f, 0.0f, 100.0f, 0.0f, 100.0f }; // these values don't matter
	Vector3 targetOffset_ = { 0.0f, 50.0f, 0.0f };
	Vector3 targetRotation_ = { 1.57f, 0.0f, 0.0f };
	Vector3 targetCameraPosition_ = {};
	Vector2 GetMousePosition() const;
	Vector2 WorldToScreen(const Vector3& worldPos, const Camera& camera, float screenW, float screenH);

	void UpdateFollow();
	void UpdateAim();
	void UpdateObjective();
	void UpdateDebugCamera();

	float aimMaxDistance_ = 2.0f;
	float aimSensitivity_ = 0.035f;
	float aimDeadZone_ = 30.0f;
	float aimLerpRate_ = 0.18f;

	float screenWidth_ = 1280.0f;
	float screenHeight_ = 720.0f;

	
	static inline const float kInterpolationRate = 0.65f;
	static inline const float kVelocityBias = { 2.0f };
	static inline const Rect margin = { -8.0f, 8.0f, -6.0f, 20.0f, -6.0f, 6.0f }; // left, right, down, up, in, out
};

