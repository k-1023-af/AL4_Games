#pragma once
#include "WorldUpdater.h"
#include "Particles.h"

class Skydome {
public:
	Skydome();
	~Skydome();
	void Initialize();
	void Update(const KamataEngine::Camera& camera);
	void Draw(KamataEngine::Camera& camera);

private:
	WorldUpdater worldUpdater_;

	KamataEngine::WorldTransform skyTransform_;
	KamataEngine::Model* modelSkydome_ = nullptr;

	KamataEngine::WorldTransform planeTransform_;
	KamataEngine::Model* planeGround_ = nullptr;
};
