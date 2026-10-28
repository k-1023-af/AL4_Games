#include "TitleScene.h"

void TitleScene::Initialize() {
	request_ = Request::None;
	showGameOver_ = false;

	titleSprite_ = TextureManager::Load("./Resources/skydome.png");
	titleBackground_ = Sprite::Create(titleSprite_, { 0,0 });

	//Audio::GetInstance()->StopWave(voiceBGM_);
	
	//voiceBGM_ = Audio::GetInstance()->PlayWave(soundBGM_, true, 0.8f);
	soundBGM_ = Audio::GetInstance()->LoadWave("./Resources/sounds/title.wav");
	levelStartSoundHandle_ = Audio::GetInstance()->LoadWave("./Resources/sounds/levelStart.wav");

	PlayBGM();
	//StopBGM();
}


void TitleScene::Update() {
	if (showGameOver_) {
		if (Input::GetInstance()->TriggerKey(DIK_SPACE) || Input::GetInstance()->TriggerKey(DIK_P)) {
			showGameOver_ = false;
			StopBGM();                     // stop title BGM before going to game
			request_ = Request::GoToGame;
		}
		if (Input::GetInstance()->TriggerKey(DIK_ESCAPE)) {
			showGameOver_ = false;
		}
		return;
	}

	if (Input::GetInstance()->TriggerKey(DIK_SPACE) || Input::GetInstance()->TriggerKey(DIK_P)) {
		StopBGM();                         // stop title BGM
		request_ = Request::GoToGame;
	}
	
}

void TitleScene::Draw() {
	Sprite::PreDraw();
	if (titleBackground_) {
		titleBackground_->Draw();
	}
	Sprite::PostDraw();
}

void TitleScene::PlayBGM() {
	StopBGM();
	if (soundBGM_ != 0) {
		voiceBGM_ = Audio::GetInstance()->PlayWave(soundBGM_, true, 0.7f);
	}
}

void TitleScene::StopBGM() {
	if (voiceBGM_ != 0) {
		Audio::GetInstance()->StopWave(voiceBGM_);
		voiceBGM_ = 0;
	}
}

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {
	delete titleBackground_;
}
