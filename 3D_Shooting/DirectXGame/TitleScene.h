#pragma once
//#include "IScene.h"
#include "KamataEngine.h"
#include "Fade.h"
#include "WorldUpdate.h"

using namespace KamataEngine;

class TitleScene /*: public IScene*/ {
public:
	void Initialize();
	void Update();
	void Draw();

	//Request GetRequest() const override { return request_; }

	//void ClearRequest() override { request_ = Request::None; }
	//void SetShowGameover(bool v) { showGameOver_ = v; }

	TitleScene();
	~TitleScene();

	enum class Phase {
		kFadeIn,
		kMain,
		kFadeOut,
	};

	bool IsFinished() const { return isFinished_; }

private:
	Phase phase_ = Phase::kFadeIn;

	Camera camera_;


	Fade* fade_ = nullptr;

	bool isFinished_ = false;

	//Request request_ = Request::None;
	//bool showGameOver_ = false;

	// Resource related

};

