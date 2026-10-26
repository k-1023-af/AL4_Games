#include "Skydome.h"

void Skydome::Initialize() {

	skyTransform_.Initialize();
	skyTransform_.scale_ = { 1.0f, 1.0f, 1.0f };
	skyTransform_.rotation_ = { 0.0f, 0.0f, 0.0f };
	skyTransform_.translation_ = { 0.0f, 0.0f, 0.0f };

	//modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	planeTransform_.Initialize();
	planeTransform_.scale_ = { 1.0f, 1.0f, 1.0f };
	planeTransform_.rotation_ = {0.0f, 0.0f, 0.0f };
	planeTransform_.translation_ = { 0.0f, -2.0f, 0.0f };


	planeGround_ = Model::CreateFromOBJ("field", true);
}

void Skydome::Update(const KamataEngine::Camera& camera) {
	skyTransform_.translation_ = camera.translation_;
	

	//const float kPlaneSize = 100.0f;   // size of your field model
	//
	//// Follow camera but wrap so it never goes too far
	//planeTransform_.translation_.x = camera.translation_.x;
	//planeTransform_.translation_.z = camera.translation_.z;
	//
	//// Optional: snap to grid so texture stays seamless
	//planeTransform_.translation_.x = std::fmod(planeTransform_.translation_.x, kPlaneSize);
	//planeTransform_.translation_.z = std::fmod(planeTransform_.translation_.z, kPlaneSize);
	//
	//// Keep Y fixed
	//planeTransform_.translation_.y = -2.0f;

	worldUpdater_.Update(skyTransform_);
	worldUpdater_.Update(planeTransform_);
}

void Skydome::Draw(KamataEngine::Camera& camera) {
	
	if (modelSkydome_) {
		modelSkydome_->Draw(skyTransform_, camera);
	}
	if (planeGround_) {
		planeGround_->Draw(planeTransform_, camera);
	}
	
	
}

Skydome::Skydome() {
}
Skydome::~Skydome() {
	delete modelSkydome_;
	delete planeGround_;
}