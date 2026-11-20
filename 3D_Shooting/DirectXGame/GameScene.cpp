#include "GameScene.h"



void GameScene::Initialize()
{
	//==============================================================================
	//  Camera
	//==============================================================================
	camera_ = new Camera();
	camera_->Initialize();
	//camera_->rotation_ = { -18.0f, 0.0f, 0.0f };
	//camera_->translation_ = { 12.0f, 10.0f, -5.0f };
	camera_->nearZ = 0.1f;
	camera_->farZ = 20000.0f;

	PrimitiveDrawer::GetInstance()->SetCamera(camera_);
	debugCamera_ = new DebugCamera(1280, 720);
	debugCamera_->SetNearZ(0.1f);
	debugCamera_->SetFarZ(20000.0f);
	AxisIndicator::GetInstance()->SetVisible(false);
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());

	//targetCamera_ = new CameraController();
	//targetCamera_->Initialize(camera_);
	//targetCamera_->SetTarget(player_);
	//targetCamera_->Reset();
	 
	//==============================================================================
	//  Fade
	//==============================================================================
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	phase_ = Phase::kFadeIn;

	//==============================================================================
	//  MapChip Data
	//==============================================================================
	mapChipField_ = new MapChip3D();
	mapChipField_->ResetMapChipData(); // Reset the map chip data to default values
	mapChipField_->LoadMapChipData("./Resources/stage.csv");

	uint32_t numBlockZAxis = mapChipField_->GetNumBlockZAxis();
	//uint32_t numBlockYAxis = mapChipField_->GetNumBlockYAxis();
	uint32_t numBlockXAxis = mapChipField_->GetNumBlockXAxis();

	worldTransformBlocks_.resize(numBlockZAxis);
	isVisible_.resize(numBlockZAxis);

	for (uint32_t i = 0; i < numBlockZAxis; ++i) {
		worldTransformBlocks_[i].resize(numBlockXAxis);
		isVisible_[i].resize(numBlockXAxis, false);
	}

	for (uint32_t i = 0; i < numBlockZAxis; ++i) {
		for (uint32_t j = 0; j < numBlockXAxis; ++j) {
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

	//==============================================================================
	//  Player
	//==============================================================================
	player_ = new Player3D();
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 10);
	player_->Initialize(camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);

	//==============================================================================
	//  Enemy Data
	//==============================================================================

	

	//CenterCameraOnPlayer(); or
	//Initiate Camera target

}

void GameScene::Update()
{
	//==============================================================================
	//  Update MapChip Data
	//==============================================================================

	//==============================================================================
	//  Update Sky + Background
	//==============================================================================

	//==============================================================================
	//  Update HUD
	//==============================================================================

	//==============================================================================
	//  Update Player
	//==============================================================================

	//==============================================================================
	//  Update Enemies
	//==============================================================================
	// 
	//==============================================================================
	//  Update Phase
	//==============================================================================
	ChangePhase();

	switch (phase_) {
	case GameScene::Phase::kFadeIn:
			fade_->Update();
			break;
	case GameScene::Phase::kPlay:
		fade_->Update();
		player_->Update();
		//modelSkydome_.Update();
		//for (Enemy3D* newEnemy : enemies_) {
		//	newEnemy->Update();
		//}
		//targetCamera_->Update();
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
	case GameScene::Phase::kDeath:
		//modelSkydome_.Update();
		//for (Enemy3D* newEnemy : enemies_) {
		//	newEnemy->Update();
		//}
		//targetCamera_->Update();
		for (size_t i = 0; i < worldTransformBlocks_.size(); ++i) {
			std::vector<WorldTransform*>& worldTransformBlockLine = worldTransformBlocks_[i];
			for (size_t j = 0; j < worldTransformBlockLine.size(); ++j) {
				WorldTransform* worldTransformBlock = worldTransformBlockLine[j];
				if (!worldTransformBlock)
					continue;
		
				worldUpdater_.Update(*worldTransformBlock);
			}
		}
		break;
	case GameScene::Phase::kFadeOut:
		fade_->Update();
		break;
	}

#ifdef _DEBUG
	//==============================================================================
	//  Update ImGui
	//==============================================================================
	if (Input::GetInstance()->TriggerKey(DIK_RSHIFT)) {
		isDebugCameraActive_ = true;
	}
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		AxisIndicator::GetInstance()->SetVisible(true);
		camera_->matView = debugCamera_->GetCamera().matView;
		camera_->matProjection = debugCamera_->GetCamera().matProjection;
		camera_->TransferMatrix();

		if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
			isDebugCameraActive_ = false;
			//PrimitiveDrawer::GetInstance()->SetCamera(camera_);
		}
	}
	else {
		camera_->UpdateMatrix();
	}

	ImGui::Begin("Camera");
	ImGui::DragFloat3("Camera Rotate", &camera_->rotation_.x, 0.01f, -10.0f, 10.0f);
	ImGui::DragFloat3("Camera Translate", &camera_->translation_.x, 0.01f, -100.0f, 100.0f);
	if (ImGui::Button("Reset Camera")) {
		camera_->rotation_ = { 1.57f, 0.0f, 0.0f };
		camera_->translation_ = { 0.0f, 25.0f, 0.0f };
	}
	
	ImGui::End();

	ImGui::Begin("Player");
	WorldTransform& playerTransform = player_->GetTransform();
	ImGui::DragFloat3("Player Scale", &playerTransform.scale_.x, 0.01f, 0.01f, 10.0f);
	ImGui::DragFloat3("Player Rotate", &playerTransform.rotation_.x, 0.01f, -10.0f, 10.0f);
	ImGui::DragFloat3("Player Translate", &playerTransform.translation_.x, 0.05f, -100.0f, 100.0f);
	if (ImGui::Button("Reset Transform")) {
		playerTransform.scale_ = { 0.5f, 0.5f, 0.5f };
		playerTransform.rotation_ = { 0.0f, 0.0f, 0.0f };
		playerTransform.translation_ = { 0.0f, 0.0f ,0.0f };
	}

	ImGui::End();

	ImGui::Render();

#endif
	//==============================================================================
	//  Update Camera
	//==============================================================================

}

void GameScene::Draw()
{

	Model::PreDraw();

	//==============================================================================
	//  Draw Sky + Backgrounds
	//==============================================================================
	//modelSkydome_.Draw(*camera_);
	//==============================================================================
	//  Draw MapChip Data
	//==============================================================================
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

	for (int i = 0; i < 50; i++) {
		PrimitiveDrawer::GetInstance()->DrawLine3d({ (-25.0f + (float)i * 1.0f), 0, -25.0f }, { (-25.0f + (float)i * 1.0f), 0, 25.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }); // start, end, color

		PrimitiveDrawer::GetInstance()->DrawLine3d({ -25, 0, (-25.0f + (float)i * 1.0f) }, { 25, 0, (-25.0f + (float)i * 1.0f) }, { 1.0f, 0.0f, 0.0f, 1.0f }); // start, end, color
	}
	//==============================================================================
	//  Draw Player + Enemies
	//==============================================================================
	
	//for (Enemy3D* newEnemy : enemies_) {
	//	newEnemy->Draw(*camera_);
	//}

	Model::PostDraw();

	player_->Draw(*camera_);

	Sprite::PreDraw();
	//==============================================================================
	//  Draw HUD
	//==============================================================================

	fade_->Draw();
	Sprite::PostDraw();
}

void GameScene::ChangePhase()
{
	switch (phase_) {
	case GameScene::Phase::kFadeIn:
		if (fade_->IsFinished()) {
			phase_ = GameScene::Phase::kPlay;
		}
		break;
	case Phase::kPlay:
		if (player_->IsDead()) {
			phase_ = Phase::kDeath;

			//const Vector3& deathParticlesPosition = player_->GetWorldPosition();
			//
			//deathParticles_->Initialize(deathModel_, camera_, deathParticlesPosition);
		}
		break;
	case Phase::kDeath:
		//if (deathParticles_ && deathParticles_->IsFinished()) {
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = Phase::kFadeOut;
		//}
		break;
	case Phase::kFadeOut:
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	}
}

void GameScene::CheckAllCollisions()
{
}

//void GameScene::CreateHitEffect(const Vector3& position)
//{
//	//HitEffect* newHitEffect = HitEffect::Create(position);
//	//hitEffects_.push_back(newHitEffect);
//}

GameScene::GameScene() { }

GameScene::~GameScene()
{
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
	//
	////for (HitEffect* newHitEffect : hitEffects_) {
	////	delete newHitEffect;
	////}
	//
	//for (Enemy3D* newEnemy : enemies_) {
	//	delete newEnemy;
	//}
	//
	delete player_;
	delete modelBlock_;
	//
	delete mapChipField_;
	//delete targetCamera_;
	delete debugCamera_;
	delete camera_;
	delete fade_;
}