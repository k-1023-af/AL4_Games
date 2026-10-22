#include "GameScene.h"
#include "2d/ImGuiManager.h"

using namespace KamataEngine;

void GameScene::Initialize() {
	camera_ = new Camera();
	camera_->Initialize();

	//camera_->scale_ = {1.0f, 1.0f, 1.0f};
	//camera_->rotation_ = {0.3f, 0.0f, 0.0f};
	//camera_->translation_ = {20.0f, 10.0f, -30.0f};
	camera_->nearZ = 0.1f;
	camera_->farZ = 20000.0f;

	PrimitiveDrawer::GetInstance()->SetCamera(camera_);

	debugCamera_ = new DebugCamera(1280, 720);
	debugCamera_->SetNearZ(0.1f);
	debugCamera_->SetFarZ(20000.0f);

	AxisIndicator::GetInstance()->SetVisible(false);
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());

	//set map chip resources
	mapChipField_ = new MapChip();
	mapChipField_->ResetMapChipData();
	mapChipField_->LoadMapChipCSV("Resources/blocks_test.csv");

	// Fade status
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	//player Initialize
	player_ = new Player();
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 10);
	player_->Initialize(camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);

	targetCamera_ = new CameraController();
	targetCamera_->Initialize(camera_);
	targetCamera_->SetMoveableArea(mapChipField_);
	targetCamera_->SetTarget(player_);
	targetCamera_->Reset();

	//enemy Initialize


	//Block Initialize
	uint32_t numBlockVertical = mapChipField_->GetNumBlockVertical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	worldTransformBlocks_.resize(numBlockVertical);
	isVisible_.resize(numBlockVertical);

	for (uint32_t i = 0; i < numBlockVertical; ++i) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
		isVisible_[i].resize(numBlockHorizontal, false);
	}

	for (uint32_t i = 0; i < numBlockVertical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransform->scale_ = { 1.0f, 1.0f, 1.0f };
				worldTransform->rotation_ = { 0.0f, 0.0f, 0.0f };
				worldTransform->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);

				worldTransformBlocks_[i][j] = worldTransform;
				isVisible_[i][j] = true;
			}
		}
	}

	modelBlock_ = Model::CreateFromOBJ("block", true);

	//deathModel_ = Model::CreateFromOBJ("particle", true);
	//deathParticles_ = new DeathParticles();
	//deathParticles_->Initialize(deathModel_, camera_, playerPosition);
	//
	//HitEffect::SetModel(Model::CreateFromOBJ("HitEffect", true));
	//HitEffect::SetCamera(camera_);

}

void GameScene::Update() {

	ChangePhase();
	
	switch (phase_) {
	
	case Phase::kFadeIn:
		fade_->Update();
		//break;
	case Phase::kPlay:
		player_->Update();
		targetCamera_->Update();
	 
		for (size_t i = 0; i < worldTransformBlocks_.size(); ++i) {
		std::vector<WorldTransform*>& worldTransformBlockLine = worldTransformBlocks_[i];
		for (size_t j = 0; j < worldTransformBlockLine.size(); ++j) {
			WorldTransform* worldTransformBlock = worldTransformBlockLine[j];
			if (!worldTransformBlock)
				continue;

			worldUpdater_.Update(*worldTransformBlock);
			}
		}
		/// collision
		CheckAllCollisions();
	 
		break;
	case Phase::kDeath:
	 
	 targetCamera_->Update();
	 
		break;
	case Phase::kFadeOut:
	 fade_->Update();
		break;
	}

#ifdef _DEBUG
		if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
			isDebugCameraActive_ = true;
		}
		if (isDebugCameraActive_ && Input::GetInstance()->TriggerKey(DIK_UP)) {
			isDebugCameraActive_ = false;
		}
		if (isDebugCameraActive_) {
			debugCamera_->Update();
			AxisIndicator::GetInstance()->SetVisible(true);
			camera_->matView = debugCamera_->GetCamera().matView;
			camera_->matProjection = debugCamera_->GetCamera().matProjection;
			camera_->TransferMatrix();
		}
		else {
			camera_->UpdateMatrix();
		}

#endif
}

void GameScene::Draw() {

	Model::PreDraw();

	//draw blocks
	for (uint32_t i = 0; i < worldTransformBlocks_.size(); ++i) {
		for (uint32_t j = 0; j < worldTransformBlocks_[i].size(); ++j) {
			WorldTransform* worldTransformBlock = worldTransformBlocks_[i][j];

			if (!worldTransformBlock || !isVisible_[i][j]) {
				continue;
			}
			modelBlock_->Draw(*worldTransformBlock, *camera_); // Use camera_, not debugCamera_
		}
	}
	if (player_) player_->Draw();

	Model::PostDraw();
	if (fade_) fade_->Draw();
	
}

void GameScene::CheckAllCollisions() {
#pragma region
	//WorldUpdater::AABB aabb1; //aabb2, aabb3
	//
	//aabb1 = player_->GetAABB();

#pragma endregion
}

void GameScene::ChangePhase() {
	switch (phase_) {
	case Phase::kFadeIn:
		if (fade_ && fade_->IsFinished()) {
			fade_->Stop();
			phase_ = Phase::kPlay;
		}
		break;
	case Phase::kPlay:
		if (player_ && player_->IsDead()) {
			phase_ = Phase::kDeath;

			//const Vector3& deathParticlesPosition = player_->GetWorldPosition();

			//deathParticles_->Initialize(deathModel_, camera_, deathParticlesPosition);
		}
		break;
	case Phase::kDeath:
		//if (deathParticles_ && deathParticles_->IsFinished()) {
			//fade_->Start(Fade::Status::FadeOut, 1.0f);
			//phase_ = Phase::kFadeOut;
		//}
		break;
	case Phase::kFadeOut:
		if (fade_ && fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	}

}

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete fade_;
	fade_ = nullptr;

	delete targetCamera_;
	targetCamera_ = nullptr;

	delete player_;
	player_ = nullptr;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

	delete mapChipField_;
	mapChipField_ = nullptr;

	delete modelBlock_;
	modelBlock_ = nullptr;

	delete debugCamera_;
	debugCamera_ = nullptr;

	delete camera_;
	camera_ = nullptr;
	
	
	
	//delete deathParticles_;
	
	

	//for (HitEffect* newHitEffect : hitEffects_) {
	//	delete newHitEffect;
	//}

	//for (Enemy* newEnemy : enemies_) {
	//	delete newEnemy;
	//}
	//enemies_.clear();
}
