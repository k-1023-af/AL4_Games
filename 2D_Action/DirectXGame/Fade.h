#pragma once
#include "KamataEngine.h"
#include <algorithm>

using namespace KamataEngine;

class Fade {
public:
	void Initialize();
	void Update();
	void Draw();

	enum class Status {
		None,
		FadeIn,
		FadeOut,
	};

	void Start(Status status, float duration);
	void Stop();

	bool IsFinished() const;

	Fade();
	~Fade();

private:
	Sprite* sprite_ = nullptr;

	uint32_t textureHandle_ = 0;

	Status status_ = Status::None;

	float duration_ = 0.0f;
	float counter_ = 0.0f;
};
