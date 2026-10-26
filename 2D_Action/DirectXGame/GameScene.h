#pragma once
#include "KamataEngine.h"
#define NOMINMAX
#include <Windows.h>
#include <algorithm>
#include <numbers>
#include <cstdint>
#include "SkyDome.h"
#include "MapChip.h"
#include "Player.h"
#include "Fade.h"
#include "WorldUpdater.h"
#include "CameraController.h"
#include "Particles.h"
#include "Function.h"
//#include "Sky.h"
#include "Enemy.h"
#include <vector>
#include <cstdlib>
#include <memory>

using namespace KamataEngine;

//class WorldUpdater;
//class SkyDome;

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

	inline ParticleConfig MakeFireflyConfig() {
		ParticleConfig cfg;
		cfg.style = ParticleStyle::Twinkle;   // or Pulse
		cfg.partCount = 1;                        // one particle per spawn
		cfg.width = 0.25f;
		cfg.height = 0.25f;
		cfg.spreadDuration = 40.0f;                    // how long it stays visible
		cfg.fadeDuration = 25.0f;
		cfg.spreadStartScale = 0.4f;
		cfg.spreadEndScale = 1.1f;
		cfg.fadeEndScale = 0.2f;
		cfg.startColor = { 1.0f, 0.95f, 0.4f, 0.9f }; // warm yellow
		cfg.blendMode = BlendMode::Add;           // if your engine supports it
		return cfg;
	}

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
	std::list<Particles*> fireflies_;
	float fireflySpawnTimer_ = 0.0f;
	const float kFireflySpawnInterval = 0.12f;   // spawn every ~7 frames
	const int   kMaxFireflies = 45;

	Model* enemyModel_ = nullptr;

	Phase phase_ = Phase::kFadeIn;
	Fade* fade_ = nullptr;
	
	Skydome* skyDome_ = nullptr;
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

