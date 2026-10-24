#pragma once
#include "KamataEngine.h"
#define NOMINMAX
#include <Windows.h>
#include <algorithm>
#include <numbers>
#include <cstdint>
#include "MapChip.h"
#include "Player.h"
#include "Fade.h"
#include "WorldUpdater.h"
#include "CameraController.h"
#include "Particles.h"
//#include "Sky.h"
#include "Enemy.h"
#include <vector>
#include <memory>

using namespace KamataEngine;

//class WorldUpdater;

const int kWindowWidth = 1280;
const int kWindowHeight = 720;

enum class Phase {
	kFadeIn,
	kPlay,
	kDeath,
	kFadeOut,
};

class GameScene {
public:
	void Initialize();
	void Update();
	void Draw();

	void CheckAllCollisions();
	void ChangePhase();
	void CreateHitEffect(const Vector3& position);

	bool IsFinished() const { return finished_; } // true → game over
	void ClearFinished() { finished_ = false; }

	GameScene();
	~GameScene();

private:
	WorldUpdater worldUpdater_;
	Camera* camera_ = nullptr;
	DebugCamera* debugCamera_ = nullptr;
	bool isDebugCameraActive_ = false;
	std::vector<std::vector<bool>> isVisible_;

	PrimitiveDrawer* primitiveDrawer_ = PrimitiveDrawer::GetInstance();

	Player* player_ = nullptr;
	CameraController* targetCamera_ = nullptr;

	std::list<Enemy*> enemies_;
	std::list<Particles*> hitEffects_;

	Model* enemyModel_ = nullptr;

	Phase phase_ = Phase::kFadeIn;
	Fade* fade_ = nullptr;

	MapChip* mapChipField_ = nullptr;

	Model* modelBlock_ = nullptr;
	Model* modelSlope1L_ = nullptr;	// 45 degrees
	Model* modelSlope1R_ = nullptr;	// 45 degrees
	Model* modelSlope2L_ = nullptr;	// 22.5 full
	Model* modelSlope2R_ = nullptr;	// 22.5 full
	Model* modelSlope3L_ = nullptr;	// 22.5 half
	Model* modelSlope3R_ = nullptr;	// 22.5 half
	Model* modelSlope4L_ = nullptr;	// curved
	Model* modelSlope4R_ = nullptr;	// curved

	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	bool finished_ = false;
};

