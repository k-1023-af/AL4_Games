#pragma once
#define NOMINMAX
#include "KamataEngine.h"
#include <Windows.h>
#include <algorithm>
//#include "Player.h"
#include "WorldUpdater.h"
#include "MapChip.h"
#include <3d/Camera.h>

class Player; //forward

class CameraController {
public:
	void Initialize(KamataEngine::Camera* mainCamera);
	void Update();
	
	void SetTarget(Player* target);
	void SetMoveableArea(MapChip* mapChipStage);
	void Reset();

	struct Rect {
		float left = 0.0f;		// min X
		float right = 1.0f;		// max X
		float bottom = 0.0f;	// min Y
		float top = 1.0f;		// max Y
	};


	CameraController();
	~CameraController();
private:
	KamataEngine::Camera* camera_ = nullptr;

	Player* target_ = nullptr;

	Rect moveableArea_ = { 0.0, 100.0f, 0.0f, 100.0f };

	//WorldUpdater* worldUpdater_;

	Vector3 targetOffset_ = { 0.0f, 0.0f, -20.f };

	static inline const float kInterpolationRate = 0.8f;

	static inline const float kVelocityBias = { 2.0f };

	static inline const Rect margin = { -8.0f, 8.0f, -6.0f, 6.0f };

	//Vector3 Lerp

	// Horizontal elastic
	static inline const float kHorizontalBaseLerp = 0.12f;   // minimum follow speed
	static inline const float kHorizontalSpringGain = 0.045f;  // how much stronger it gets with distance
	static inline const float kHorizontalMaxLerp = 0.55f;   // clamp so it never becomes instant

	// Vertical dead-zone
	static inline const float kDeadZoneHalfHeight = 3.4f;    // world units the player can move freely
	static inline const float kVerticalSoftLerp = 0.06f;   // very gentle while inside the zone
	static inline const float kVerticalHardLerp = 0.28f;   // stronger once outside the zone
};

