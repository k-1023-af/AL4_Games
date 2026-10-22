#pragma once
#include "KamataEngine.h"
#include <cassert>
#include <algorithm>
#include <numbers>
#include <array>
//#include "WorldUpdater.h"


class Particles {
public:

	void Initialize();
	void Update();
	void Draw();

	void Create();	// might not be needed

	void AnimPhaseInitialize();
	void AnimPhaseUpdate();

	Particles();
	~Particles();

	//static void SetModel(Model* model) { model_ = model; }
	//static void SetCamera(Camera* camera) { camera_ = camera; }

	bool IsDead() const { return isDead_; }
	bool IsFinished() const { return isFinished_; }

	enum class Behavior {	//subject to change
		kInitialize,
		kUnknown,
		kDraw
	};

	enum class AnimPhase {	//subject to change
		kSpread,
		kFade,
		kDelete,
	};

	Behavior behavior_ = Behavior::kInitialize;
	Behavior behaviorRequest_ = Behavior::kUnknown;

	AnimPhase AnimPhase = AnimPhase::kSpread;

private:
	//static Model* model_;	// create model for each effect
	//static Camera* camera_;

	//WorldTransform particleTransform_;	// create for each effect
	//
	//WorldUpdater worldUpdater_;	//create updater for each effect
	//
	//ObjectColor objectColor_;	// create for each effect
	//Vector4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	static inline const float kWidth = 2.0f;
	static inline const float kHeight = 2.0f;

	float spreadDuration_ = 12.0f;	// death particles or hit effect
	float fadeDuration_ = 18.0f;	// 

	uint32_t spreadParameter_ = 0;	// okay to use for multiple effects???
	uint32_t fadeParameter_ = 0;


	bool isDead_ = false;
	bool isFinished_ = false;
};

