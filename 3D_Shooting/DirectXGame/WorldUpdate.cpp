#include "WorldUpdate.h"

WorldUpdate::WorldUpdate() {}

WorldUpdate::~WorldUpdate() {}

void WorldUpdate::Initialize() {
	//worldTransform_ = new WorldTransform();
	//worldTransform_->Initialize();
	//worldTransform_->scale_ = {1.0f, 1.0f, 1.0f};
	//worldTransform_->rotation_ = {0.0f, 0.0f, 0.0f};
	//worldTransform_->translation_ = {0.0f, 0.0f, 0.0f};
}

void WorldUpdate::Update(KamataEngine::WorldTransform& worldTransform) {
	WorldTransformUpdate(worldTransform);
}
