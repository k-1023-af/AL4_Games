#include "PlayerHUD.h"

void PlayerHUD::Initialize() {

	barTextureHandle_ = TextureManager::Load("./Resources/pMeter_Blank.png");
	fillTextureHandle_ = TextureManager::Load("./Resources/pMeter_Fill.png");

	const Vector2 barPos = { 30.0f, 20.0f };

	pMeterBar_ = Sprite::Create(barTextureHandle_, { 20.0f, 10.0f, });
	pMeterFill_ = Sprite::Create(fillTextureHandle_, { 20.0f, 10.0f });

	pMeterBar_->SetSize({ kBarWidth, kBarHeight });
	pMeterFill_->SetSize({ 0.0f, kBarHeight });
}

void PlayerHUD::Update(float pMeter, bool canAirDash) {

	float t = std::clamp(pMeter / kMaxPMeter, 0.0f, 1.0f);
	float fillWidth = kBarWidth * t;

	if (canAirDash) {
		pulseTimer_ += 0.15f;
		float pulse = 0.85f + 0.15f * std::sin(pulseTimer_); //0.65 ~ 1.0f
	
		//fillWidth *= pulse;
	
		pMeterFill_->SetColor({ pulse - 0.7f, pulse - 0.7f, pulse - 0.4f, 1.0f });
	}
	else {
		pMeterFill_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		pulseTimer_ = 0.0f;
	}
	pMeterFill_->SetSize({ fillWidth, kBarHeight });
}

void PlayerHUD::Draw() {

	pMeterFill_->Draw();
	pMeterBar_->Draw();
}

PlayerHUD::PlayerHUD(){ }

PlayerHUD::~PlayerHUD() {
	delete pMeterBar_;
	delete pMeterFill_;
}
