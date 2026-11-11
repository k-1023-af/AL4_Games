#include "TitleScene.h"

void TitleScene::Initialize() {
	request_ = Request::None;
	showGameOver_ = false;

	// Resource initiate
}

void TitleScene::Update() {
	if (showGameOver_) {
		if (Input::GetInstance()->TriggerKey(DIK_SPACE) || Input::GetInstance()->TriggerKey(DIK_R)) {
			showGameOver_ = false;
			request_ = Request::GoToGame; // retry
		}
		if (Input::GetInstance()->TriggerKey(DIK_ESCAPE)) {
			showGameOver_ = false;
		}
		return;
	}

	// Title → start game
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) || Input::GetInstance()->TriggerKey(DIK_RETURN)) {
		request_ = Request::GoToGame;
	}
}

void TitleScene::Draw() {

}

TitleScene::~TitleScene() {
	// destructor
}
