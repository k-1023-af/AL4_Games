#include "TitleScene.h"

void TitleScene::Initialize() {
	request_ = Request::None;
	showGameOver_ = false;

	titleSprite_ = TextureManager::Load("./Resources/skydome.png");
	titleBackground_ = Sprite::Create(titleSprite_, { 0,0 });

	Audio::GetInstance()->StopWave(voiceBGM_);
	soundBGM_ = Audio::GetInstance()->LoadWave("./Resources/sounds/title.wav");
	//voiceBGM_ = Audio::GetInstance()->PlayWave(soundBGM_, true, 0.8f);

	levelStartSoundHandle_ = Audio::GetInstance()->LoadWave("./Resources/sounds/levelStart.wav");
}


void TitleScene::Update() {
	if (showGameOver_) {
		//option retry
		if (Input::GetInstance()->TriggerKey(DIK_SPACE) || Input::GetInstance()->TriggerKey(DIK_R)) {
			showGameOver_ = false;
			request_ = Request::GoToGame;
		}
		//option return to title
		if (Input::GetInstance()->TriggerKey(DIK_ESCAPE)) {
			showGameOver_ = false;
		}
		return;
	}
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) || Input::GetInstance()->TriggerKey(DIK_R)){
		Audio::GetInstance()->StopWave(voiceBGM_);

		//Audio::GetInstance()->PlayWave(levelStartSoundHandle_, false, 0.6f);

		request_ = Request::GoToGame;
	}
	
	
}

void TitleScene::Draw() {
	Sprite::PreDraw();
	titleBackground_->Draw();
	Sprite::PostDraw();
}

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {
	delete titleBackground_;
}
