#pragma once
#include "IScene.h"
#include "KamataEngine.h"

using namespace KamataEngine;

class TitleScene : public IScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;

	Request GetRequest() const override { return request_; }

	void ClearRequest() override { request_ = Request::None; }
	void SetShowGameover(bool v) { showGameOver_ = v; }

	~TitleScene();

private:
	Request request_ = Request::None;
	bool showGameOver_ = false;

	// Resource related

};

