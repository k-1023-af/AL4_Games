#include "Goal.h"

void Goal::Initialize(const Vector3& position) {
	model_ = Model::CreateFromOBJ("goal", true);  // your goal model name

	worldTransform_.Initialize();
	worldTransform_.scale_ = { 1.0f, 1.0f, 1.0f };
	worldTransform_.translation_ = position;
	worldTransform_.TransferMatrix();
}

void Goal::Update() {
	// rotate or animate the goal
	worldTransform_.rotation_.y += 0.02f;

	worldTransform_.TransferMatrix();
}

void Goal::Draw(Camera& camera) {
	if (model_ && isActive_) {
		model_->Draw(worldTransform_, camera);
	}
}

WorldUpdater::AABB Goal::GetAABB() const {
	Vector3 pos = worldTransform_.translation_;
	WorldUpdater::AABB aabb;
	aabb.min = { pos.x - 0.8f, pos.y - 0.8f, pos.z - 0.8f };
	aabb.max = { pos.x + 0.8f, pos.y + 1.5f, pos.z + 0.8f };
	return aabb;
}
Goal::Goal(){}
Goal::~Goal() {
	delete model_;
	model_ = nullptr;
}