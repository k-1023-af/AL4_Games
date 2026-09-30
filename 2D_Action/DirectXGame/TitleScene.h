#pragma once
#include "IScene.h"
#include "KamataEngine.h"

using namespace KamataEngine;

class TitleScene : public IScene {

public:
	void Initialize();
	void Update();
	void Draw();

	Request GetRequest() const override { return request_; }
	void ClearRequest() override { request_ = Request::None; }

	void SetShowGameOver(bool v) { showGameOver_ = v; }

	~TitleScene();

private:
	Request request_ = Request::None;
	bool showGameOver_ = false;

};

