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

	//jumpSoundHandle_ = Audio::GetInstance()->LoadWave("./Resources/sounds/jump.wav");
	//hitSoundHandle_ = Audio::GetInstance()->LoadWave("sounds/hit.wav");
	//enemyDefeatSoundHandle_ = Audio::GetInstance()->LoadWave("./Resources/sounds/hit.wav");
	///airDashReadySoundHandle_ = Audio::GetInstance()->LoadWave("sounds/airdash_ready.wav");
	//bounceSoundHandle_ = Audio::GetInstance()->LoadWave("sounds/bounce.wav");
	//levelStartSoundHandle_ = Audio::GetInstance()->LoadWave("./Resources/sounds/levelStart.wav");
	levelClearSoundHandle_ = Audio::GetInstance()->LoadWave("./Resources/sounds/clear.wav");
	
	//
	skyDome_ = new Skydome();
	skyDome_->Initialize();

	//set map chip resources
	mapChipField_ = new MapChip();
	mapChipField_->ResetMapChipData();
	mapChipField_->LoadMapChipCSV("Resources/stage1.csv");

	// Fade status
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	playerHUD_ = new PlayerHUD();
	playerHUD_->Initialize();


	//player Initialize
	player_ = new Player();
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(5, 10);
	player_->Initialize(camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);

	targetCamera_ = new CameraController();
	targetCamera_->Initialize(camera_);
	targetCamera_->SetMoveableArea(mapChipField_);
	targetCamera_->SetTarget(player_);
	targetCamera_->Reset();


	// ===== Block & Slope Initialize =====
	uint32_t numBlockVertical = mapChipField_->GetNumBlockVertical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	worldTransformBlocks_.resize(numBlockVertical);
	isVisible_.resize(numBlockVertical);

	for (uint32_t i = 0; i < numBlockVertical; ++i) {
		worldTransformBlocks_[i].resize(numBlockHorizontal, nullptr);
		isVisible_[i].resize(numBlockHorizontal, false);
	}

	for (uint32_t y = 0; y < numBlockVertical; ++y) {
		for (uint32_t x = 0; x < numBlockHorizontal; ++x) {

			MapChipType type = mapChipField_->GetMapChipTypeByIndex(x, y);

			// Only create a WorldTransform for solid tiles
			bool isSolid =
				type == MapChipType::kBlock ||
				type == MapChipType::kSlope1L ||
				type == MapChipType::kSlope1R ||
				type == MapChipType::kSlope2L ||
				type == MapChipType::kSlope2R ||
				type == MapChipType::kSlope3L ||
				type == MapChipType::kSlope3R ||
				type == MapChipType::kSlope4L ||
				type == MapChipType::kSlope4R;

			if (!isSolid) continue;

			WorldTransform* wt = new WorldTransform();
			wt->Initialize();
			wt->scale_ = { 1.0f, 1.0f, 1.0f };
			wt->rotation_ = { 0.0f, 0.0f, 0.0f };
			wt->translation_ = mapChipField_->GetMapChipPositionByIndex(x, y);

			worldTransformBlocks_[y][x] = wt;
			isVisible_[y][x] = true;
		}
	}

	modelBlock_ = Model::CreateFromOBJ("block", true);
	modelSlope1L_ = Model::CreateFromOBJ("slope1L", true);
	modelSlope1R_ = Model::CreateFromOBJ("slope1R", true);
	modelSlope2L_ = Model::CreateFromOBJ("slope2L", true);
	modelSlope2R_ = Model::CreateFromOBJ("slope2R", true);
	modelSlope3L_ = Model::CreateFromOBJ("slope3L", true);
	modelSlope3R_ = Model::CreateFromOBJ("slope3R", true);


	//enemy Initialize
	for (uint32_t y = 0; y < numBlockVertical; ++y) {
		for (uint32_t x = 0; x < numBlockHorizontal; ++x) {

			MapChipType type = mapChipField_->GetMapChipTypeByIndex(x, y);

			if (type == MapChipType::kEnemy1) {
				Enemy* enemy = new Enemy();
				Vector3 pos = mapChipField_->GetMapChipPositionByIndex(x, y);
				enemy->Initialize(camera_, pos, Enemy::EnemyType::Normal);
				enemy->SetMapChipField(mapChipField_);
				enemies_.push_back(enemy);
			}
			else if (type == MapChipType::kEnemy2) {
				Enemy* enemy = new Enemy();
				Vector3 pos = mapChipField_->GetMapChipPositionByIndex(x, y);
				enemy->Initialize(camera_, pos, Enemy::EnemyType::Shield);
				enemy->SetMapChipField(mapChipField_);
				enemies_.push_back(enemy);
			}
		}
	}

	//// ===== Goal =====
	//for (uint32_t y = 0; y < numBlockVertical; ++y) {
	//	for (uint32_t x = 0; x < numBlockHorizontal; ++x) {
	//		if (mapChipField_->GetMapChipTypeByIndex(x, y) == MapChipType::kGoal) {
	//			goal_ = new Goal();
	//			Vector3 pos = mapChipField_->GetMapChipPositionByIndex(x, y);
	//			goal_->Initialize(pos);
	//			// Only one goal expected
	//			break;
	//		}
	//	}
	//}

	//const uint32_t goalX = 10;   // example: near the end of the stage
	//const uint32_t goalY = 15;   // example: middle height
	//
	//goal_ = new Goal();
	//Vector3 pos = mapChipField_->GetMapChipPositionByIndex(goalX, goalY);
	//goal_->Initialize(pos);



	//deathModel_ = Model::CreateFromOBJ("particle", true);
	//deathParticles_ = new DeathParticles();
	//deathParticles_->Initialize(deathModel_, camera_, playerPosition);
	//
	//HitEffect::SetModel(Model::CreateFromOBJ("HitEffect", true));
	//HitEffect::SetCamera(camera_);

	//Particles::SetModel(Model::CreateFromOBJ("particle", true));
	//Particles::SetCamera(camera_);


// Create different effects easily:

	//Vector3 dummyPos = {};	//replace with real position

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
	skyDome_->Update(*camera_);
	fireflySpawnTimer_ += 1.0f / 60.0f;
	if (goal_) goal_->Update();

	if (fireflySpawnTimer_ >= kFireflySpawnInterval && fireflies_.size() < kMaxFireflies) {
		fireflySpawnTimer_ = 0.0f;

		// Spawn around the camera / player area
		Vector3 camPos = camera_->translation_;
		Vector3 spawnPos;
		spawnPos.x = camPos.x + (RandomFloat(-18.0f, 18.0f));
		spawnPos.y = RandomFloat(0.5f, 6.5f);          // floating above the ground
		spawnPos.z = camPos.z + (RandomFloat(-12.0f, 8.0f));  // some closer, some farther

		Particles* firefly = Particles::Create(spawnPos, MakeFireflyConfig());
		fireflies_.push_back(firefly);
	}

	ChangePhase();
	
	switch (phase_) {
	
	case Phase::kFadeIn:
		fade_->Update();
		//break;
	case Phase::kPlay:
		
		targetCamera_->Update();
		targetCamera_->SetPSpeed(player_->IsPSpeed());
		
		playerHUD_->Update(player_->GetPMeter(), player_->CanAirDash());
		player_->Update();
		
	 
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
	if (skyDome_) {
		skyDome_->Draw(*camera_);
	}

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

	for (Particles* p : fireflies_) {
		p->Draw();
	}

	if (goal_) goal_->Draw(*camera_);


	Model::PostDraw();
	if (fade_) fade_->Draw();

	Sprite::PreDraw();
	if (playerHUD_) playerHUD_->Draw();
	Sprite::PostDraw();
}

void GameScene::CheckAllCollisions() {
	if (!player_ || player_->IsDead()) return;

	for (Enemy* e : enemies_) {
		if (e->IsDead() || e->IsCollisionDisabled()) continue;
		if (player_->IsInvincible()) continue;

		if (WorldUpdater::IsCollisionAABB(player_->GetAABB(), e->GetAABB())) {

			// ===== 1. Player is dashing → existing attack logic =====
			if (player_->IsAttack() && player_->attackPhase_ == Player::AttackPhase::kRush) {
				e->OnCollision(player_, this);
				continue;
			}

			// ===== 2. Stomp check (jump on enemy) =====
			bool isStomping = false;

			// Player is falling and is above the enemy
			if (player_->GetVelocity().y < 0.0f) {
				float playerBottom = player_->GetWorldPosition().y - 0.4f;
				float enemyTop = e->GetWorldPosition().y + 0.3f;

				if (playerBottom > enemyTop - 0.2f) {  // coming from above
					isStomping = true;
				}
			}

			if (isStomping) {
				// Only Normal enemies can be stomped (Enemy1)
				// Shielded enemies (Enemy2) still hurt the player
				if (e->GetType() == Enemy::EnemyType::Normal) {
					// Defeat the enemy
					e->OnStomped(this); 

					// Bounce the player up
					player_->BounceFromStomp();
				}
				else {
					// Shielded enemy → player takes damage
					player_->OnDamaged(e->GetWorldPosition());
				}
			}
			// ===== 3. Side collision → player takes damage =====
			else {
				player_->OnDamaged(e->GetWorldPosition());
			}
		}
	}

	// Goal collision (keep your existing code)
	if (goal_ && phase_ == Phase::kPlay) {
		if (WorldUpdater::IsCollisionAABB(player_->GetAABB(), goal_->GetAABB())) {
			Audio::GetInstance()->PlayWave(levelClearSoundHandle_);
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}
	}
}

void GameScene::ChangePhase() {
	switch (phase_) {
	case Phase::kFadeIn:
		if (fade_ && fade_->IsFinished()) {
			fade_->Stop();
			phase_ = Phase::kPlay;

			// Level start sound
			Audio::GetInstance()->PlayWave(levelStartSoundHandle_);
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
			isCleared_ = true;
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

	for (auto it = fireflies_.begin(); it != fireflies_.end(); ) {
		(*it)->Update();
		if ((*it)->IsDead()) {
			delete* it;
			it = fireflies_.erase(it);
		}
		else {
			++it;
		}
	}

	for (Enemy* enemy1 : enemies_) {
		delete enemy1;
	}
	enemies_.clear();

	//for (Enemy* enemy2 : enemies_) {
	//	delete enemy2;
	//}
	//enemies_.clear();

	delete playerHUD_;

	delete player_;
	//player_ = nullptr;

	delete targetCamera_;
	//targetCamera_ = nullptr;

	delete goal_;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

	delete mapChipField_;
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

	delete skyDome_;
	
	delete fade_;
	//fade_ = nullptr;

	delete debugCamera_;
	//debugCamera_ = nullptr;

	delete camera_;
	//camera_ = nullptr;

	///delete levelStartSoundHandle_;
	//delete levelClearSoundHandle_;
	
}
