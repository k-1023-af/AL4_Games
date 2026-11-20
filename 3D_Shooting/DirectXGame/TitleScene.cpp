#include "TitleScene.h"
#include <algorithm>
#include <numbers>

void TitleScene::Initialize() {
	camera_.Initialize();

	// Resource initiate
	//titleModel_ = Model::CreateFromOBJ("title", true);
	//playerModel_ = Model::CreateFromOBJ("player", true);
	//
	//titleWorldTransform.Initialize();
	//titleWorldTransform.rotation_ = { std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float>, 0.0f };
	//titleWorldTransform.translation_ = titlePosition;
	//
	//playerWorldTransform.Initialize();
	//playerWorldTransform.translation_ = playerPosition;
	//playerWorldTransform.rotation_.y = std::numbers::pi_v<float>;

	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);
}

void TitleScene::Update() {
	switch (phase_) {

	case Phase::kFadeIn:
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;
	case Phase::kMain:
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = Phase::kFadeOut;
		}
		break;
	case Phase::kFadeOut:
		if (fade_->IsFinished()) {
			isFinished_ = true;
		}
		break;
	}

	camera_.UpdateMatrix();
	//worldUpdater_.Update(titleWorldTransform);
	//worldUpdater_.Update(playerWorldTransform);
	//
	//titleWorldTransform.translation_.y += titleLevitate;
	//titleWorldTransform.rotation_.x -= 2 * titleLevitate;
	//
	//if (titleWorldTransform.translation_.y > 0.52f) {
	//	titleLevitate *= -1;
	//}
	//if (titleWorldTransform.translation_.y < 0.33f) {
	//	titleLevitate *= -1;
	//}
	fade_->Update();
}

void TitleScene::Draw() {
	//titleModel_->Draw(titleWorldTransform, camera_);
	//playerModel_->Draw(playerWorldTransform, camera_);

	switch (phase_) {

	case Phase::kFadeIn:
		fade_->Draw();
		break;
	case Phase::kMain:
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			isFinished_ = true;
		}
		break;
	case Phase::kFadeOut:
		fade_->Draw();
		break;
	}
}

TitleScene::TitleScene()
{
}

TitleScene::~TitleScene() {
	delete fade_;
}
