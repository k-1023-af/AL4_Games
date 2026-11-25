#pragma once
#include "WorldUpdate.h"
#include "Player3D.h"
#include "Enemy3D.h"
#include "MapChip3D.h"
#include "CameraController.h"
#include "Fade.h"
#include "Skydome.h"
#include "KamataEngine.h"

using namespace KamataEngine;

class WorldUpdate;

class GameScene {
public:

	GameScene();
	~GameScene();

	void Initialize();
	void Update();
	void Draw();
	void ChangePhase();
	void CheckAllCollisions();
	void GenerateFieldObjects();
	void CreateHitEffect(const Vector3& position);

	bool IsFinished() const { return finished_; }

	enum class Phase {
		kFadeIn,
		kPlay,
		kDeath,
		kFadeOut,
	};

private:
	Phase phase_ = Phase::kFadeIn;

	//==============================================================================
	//  Lists
	//==============================================================================
	std::list<Enemy3D*> enemies_;
	//std::list<HitEffect*> hitEffects_;
	std::vector<std::vector<bool>> isVisible_;
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	//==============================================================================
	//  Entities
	//==============================================================================
	Player3D* player_ = nullptr;
	Enemy3D* enemy1_ = nullptr;
	WorldUpdate worldUpdater_;
	Skydome modelSkyDome_;
	MapChip3D* mapChipField_ = nullptr;
	Fade* fade_ = nullptr;

	//==============================================================================
	//  Models
	//==============================================================================

	Model* modelBlock_ = nullptr;

	//==============================================================================
	//  Camera and Debug
	//==============================================================================
	CameraController* targetCamera_;
	Camera* camera_ = nullptr;
	//DebugCamera* debugCamera_ = nullptr;
	PrimitiveDrawer* primitiveDrawer_ = PrimitiveDrawer::GetInstance();

	//==============================================================================
	//  Flags
	//==============================================================================
	//bool isDebugCameraActive_ = false;
	bool finished_ = false;

};

