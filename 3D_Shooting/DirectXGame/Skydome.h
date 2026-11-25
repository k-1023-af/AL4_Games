#pragma once
#include "WorldUpdate.h"
class Skydome
{
public:
	Skydome();
	~Skydome();

	void Initialize();
	void Update();
	void Draw(KamataEngine::Camera& camera);

private:
	WorldUpdate worldUpdater_;
	KamataEngine::WorldTransform skyTransform_;
	KamataEngine::Model* modelSkydome_ = nullptr;
};

