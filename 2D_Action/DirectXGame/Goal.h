#pragma once
#include "KamataEngine.h"
#include "WorldUpdater.h"

class Goal {
public:
	void Initialize(const Vector3& position);
	void Update();
	void Draw(Camera& camera);

	bool IsActive() const { return isActive_; }
	WorldUpdater::AABB GetAABB() const;

	Goal();
	~Goal();
private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	bool isActive_ = true;
};