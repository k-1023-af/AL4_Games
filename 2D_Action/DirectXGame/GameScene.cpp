#include "GameScene.h"

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
	Enemy* enemy1 = new Enemy();
	Vector3 enemy1Pos = mapChipField_->GetMapChipPositionByIndex(10, 10);
	enemy1->Initialize(camera_, enemy1Pos, Enemy::EnemyType::Normal);
	enemy1->SetMapChipField(mapChipField_);
	enemies_.push_back(enemy1);

	//Enemy* enemy2 = new Enemy();
	//Vector3 enemy2Pos = mapChipField_->GetMapChipPositionByIndex(20, 10);
	//enemy2->Initialize(camera_, enemy2Pos, Enemy::EnemyType::Shield);
	//enemy2->SetMapChipField(mapChipField_);
	//enemies_.push_back(enemy2);

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
	for (uint32_t i = 0; i < numBlockVertical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kSlope1L) {
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
	for (uint32_t i = 0; i < numBlockVertical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kSlope1R) {
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

	for (uint32_t i = 0; i < numBlockVertical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kSlope2L) {
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
	for (uint32_t i = 0; i < numBlockVertical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kSlope2R) {
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

	for (uint32_t i = 0; i < numBlockVertical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kSlope3L) {
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
	for (uint32_t i = 0; i < numBlockVertical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kSlope3R) {
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
	modelSlope1L_ = Model::CreateFromOBJ("slope1L", true);
	modelSlope1R_ = Model::CreateFromOBJ("slope1R", true);
	modelSlope2L_ = Model::CreateFromOBJ("slope2L", true);
	modelSlope2R_ = Model::CreateFromOBJ("slope2R", true);
	modelSlope3L_ = Model::CreateFromOBJ("slope3L", true);
	modelSlope3R_ = Model::CreateFromOBJ("slope3R", true);


	//deathModel_ = Model::CreateFromOBJ("particle", true);
	//deathParticles_ = new DeathParticles();
	//deathParticles_->Initialize(deathModel_, camera_, playerPosition);
	//
	//HitEffect::SetModel(Model::CreateFromOBJ("HitEffect", true));
	//HitEffect::SetCamera(camera_);

	//Particles::SetModel(Model::CreateFromOBJ("particle", true));
	//Particles::SetCamera(camera_);


// Create different effects easily:

	Vector3 dummyPos = {};	//replace with real position

// Classic hit spark (old HitEffect)
	//ParticleConfig hitConfig;
	//hitConfig.style = ParticleStyle::HitSpark;
	//hitConfig.partCount = 3;
	//hitConfig.spreadDuration = 12.0f;
	//hitConfig.fadeDuration = 18.0f;
	//auto* hit = Particles::Create(dummyPos, hitConfig);

	//// Death particles (old DeathParticles)
	//ParticleConfig deathConfig;
	//deathConfig.style = ParticleStyle::DeathBurst;
	//deathConfig.partCount = 6;
	//deathConfig.radialSpeed = 0.15f;
	//deathConfig.width = 0.3f;
	//deathConfig.height = 0.3f;
	////auto* death = Particles::Create(dummyPos, deathConfig);
	//
	//// Big explosion
	//ParticleConfig boomConfig;
	//boomConfig.style = ParticleStyle::RadialBurst;
	//boomConfig.partCount = 8;
	//boomConfig.spreadEndScale = 2.5f;
	//boomConfig.radialSpeed = 0.25f;
	////auto* boom = Particles::Create(dummyPos, boomConfig);
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
		targetCamera_->SetPSpeed(player_->IsPSpeed());
	 
		for (size_t i = 0; i < worldTransformBlocks_.size(); ++i) {
		std::vector<WorldTransform*>& worldTransformBlockLine = worldTransformBlocks_[i];
		for (size_t j = 0; j < worldTransformBlockLine.size(); ++j) {
			WorldTransform* worldTransformBlock = worldTransformBlockLine[j];
			if (!worldTransformBlock)
				continue;

			worldUpdater_.Update(*worldTransformBlock);
			}
		}

		for (Enemy* e : enemies_) {
			if (e->IsDead()) continue;
			e->Update();
			e->MapCollision();
		}

		for (auto it = hitEffects_.begin(); it != hitEffects_.end(); ) {
			(*it)->Update();
			if ((*it)->IsDead()) {
				delete* it;
				it = hitEffects_.erase(it);
			}
			else {
				++it;
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

			if (!worldTransformBlock || !isVisible_[i][j]) continue;
			
			MapChipType type = mapChipField_->GetMapChipTypeByIndex(j, i);

			if (type == MapChipType::kBlock && modelBlock_) {
				modelBlock_->Draw(*worldTransformBlock, *camera_);
			}
			if (type == MapChipType::kSlope1L && modelSlope1L_) {
				modelSlope1L_->Draw(*worldTransformBlock, *camera_);
			}
			if (type == MapChipType::kSlope1R && modelSlope1R_) {
				modelSlope1R_->Draw(*worldTransformBlock, *camera_);
			}
			if (type == MapChipType::kSlope2L && modelSlope2L_) {
				modelSlope2L_->Draw(*worldTransformBlock, *camera_);
			}
			if (type == MapChipType::kSlope2R && modelSlope2R_) {
				modelSlope2R_->Draw(*worldTransformBlock, *camera_);
			}
			if (type == MapChipType::kSlope3L && modelSlope3L_) {
				modelSlope3L_->Draw(*worldTransformBlock, *camera_);
			}
			if (type == MapChipType::kSlope3R && modelSlope3R_) {
				modelSlope3R_->Draw(*worldTransformBlock, *camera_);
			}

		}
	}

	for (Enemy* e : enemies_) {
		if (!e->IsDead()) {
			e->Draw(*camera_);
		}
	}
	
	if (player_) player_->Draw();

	for (Particles* p : hitEffects_) {
		p->Draw();
	}

	Model::PostDraw();
	if (fade_) fade_->Draw();
	
}

void GameScene::CheckAllCollisions() {
	//for (Enemy* e : enemies_) {
	//	if (e->IsDead() || e->IsCollisionDisabled()) continue;
	//	if (WorldUpdater::IsCollisionAABB(player_->GetAABB(), e->GetAABB())) {
	//		e->OnCollision(player_, this);
	//	}
	//}

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

void GameScene::CreateHitEffect(const Vector3& position) {
	Particles* hitEffect = Particles::Create(position);
	hitEffects_.push_back(hitEffect);
}

GameScene::GameScene() {}

GameScene::~GameScene() {
	for (Particles* hitEffect : hitEffects_) {
		delete hitEffect;
	}

	for (Enemy* enemy1 : enemies_) {
		delete enemy1;
	}
	enemies_.clear();

	//for (Enemy* enemy2 : enemies_) {
	//	delete enemy2;
	//}
	//enemies_.clear();

	delete player_;
	//player_ = nullptr;

	delete targetCamera_;
	//targetCamera_ = nullptr;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

	//delete mapChipField_;
	//mapChipField_ = nullptr;

	delete modelBlock_;
	//modelBlock_ = nullptr;

	delete modelSlope1L_;
	//modelSlope1L_ = nullptr;

	delete modelSlope1R_;
	//modelSlope1R_ = nullptr;

	delete modelSlope2L_;
	//modelSlope1L_ = nullptr;

	delete modelSlope2R_;
	//modelSlope1R_ = nullptr;
	
	delete fade_;
	//fade_ = nullptr;

	delete debugCamera_;
	//debugCamera_ = nullptr;

	delete camera_;
	//camera_ = nullptr;
}
