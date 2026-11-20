#pragma once
#define NOMINMAX
#include "KamataEngine.h"
#include "Windows.h"
#include <algorithm>
#include "WorldUpdate.h"
#include "MapChip3D.h"
#include "Player3D.h"

//class Player; // forward

class CameraController {
public:
	CameraController();
	~CameraController();

	void Initialize(Camera* mainCamera);
	void SetTarget(Player3D* target);
	void SetMoveableArea(MapChip3D* mapChipField);

	void Update();
	void Reset();

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
	Player3D* target_ = nullptr;
	WorldUpdate* worldUpdater_;

	Rect moveableArea_ = { 0.0f, 100.0f, 0.0f, 100.0f }; // these values don't matter
	Vector3 targetOffset_ = { 0.0f, 0.0f, -15.0f };
	
	static inline const float kInterpolationRate = 0.8f;
	static inline const float kVelocityBias = { 2.0f };
	static inline const Rect margin = { -8.0f, 8.0f, -6.0f, 6.0f, -6.0f, 6.0f }; // left, right, down, up, in, out
};

