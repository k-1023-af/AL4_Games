#include "Skydome.h"

Skydome::Skydome() {}

Skydome::~Skydome()
{
	delete modelSkydome_;
}

void Skydome::Initialize()
{
	skyTransform_.Initialize();
	skyTransform_.scale_ = { 1.0f, 1.0f, 1.0f };
	skyTransform_.rotation_ = { 0.0f, 0.0f, 0.0f };
	skyTransform_.translation_ = { 0.0f, 0.0f, 0.0f };

	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
}

void Skydome::Update()
{
	worldUpdater_.Update(skyTransform_);
}

void Skydome::Draw(KamataEngine::Camera& camera)
{
	Model::PreDraw();
	modelSkydome_->Draw(skyTransform_, camera);
	Model::PostDraw();
}
