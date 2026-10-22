#include "Fade.h"
using namespace KamataEngine;

void Fade::Initialize() {
	if (sprite_) {
		delete sprite_;
		sprite_ = nullptr;
	}

	textureHandle_ = TextureManager::Load("./Resources/uvChecker.png");

	sprite_ = Sprite::Create(textureHandle_, { 0.0f, 0.0f });
	sprite_->SetSize(Vector2{ 1280.0f, 720.0f });
	sprite_->SetColor(Vector4(0, 0, 0, 1));

}

void Fade::Update() {
	switch (status_) {
	case Status::None:
		break;
	case Status::FadeIn:
		counter_ += 1.0f / 60.0f;

		if (counter_ >= duration_) {
			counter_ = duration_;
		}
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp(1.0f - (counter_ / duration_), 0.0f, 1.0f)));
		// decreases alpha value as the counter reaches the duration value
		break;
	case Status::FadeOut:
		counter_ += 1.0f / 60.0f;

		if (counter_ >= duration_) {
			counter_ = duration_;
		}
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp(counter_ / duration_, 0.0f, 1.0f)));
		//increases alpha value as the counter reaches the duration value
		break;
	}
}

void Fade::Draw() {
	if (status_ == Status::None) { return; }

	Sprite::PreDraw();
	sprite_->Draw();
	Sprite::PostDraw();
}

void Fade::Start(Status status, float duration) {
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
}

void Fade::Stop() {
	status_ = Status::None;
}

bool Fade::IsFinished() const {
	switch (status_) {
	case Status::FadeIn:
	case Status::FadeOut:
		if (counter_ >= duration_) {
			return true;
		}
		else {
			return false;
		}
		break;
	}
	return true;
}

Fade::Fade() {}

Fade::~Fade() {
	delete sprite_;
	sprite_ = nullptr;
}
