#include "GameScene.h"
#include "IScene.h"
#include "TitleScene.h"
#include "KamataEngine.h"
#include <Windows.h>

#include "2d/ImGuiManager.h"

using namespace KamataEngine;

enum class SceneId {
	Title,
	Game
};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	
	KamataEngine::Initialize(L"LE2C_25_ファルコン_エブラハム_GameTitle");

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	TitleScene* titleScene = new TitleScene();
	GameScene* gameScene = new GameScene();

	titleScene->Initialize();
	SceneId current = SceneId::Title;


	while (true) {

		if (KamataEngine::Update()) {
			break;
		}

		ImGuiManager* imguiManager = ImGuiManager::GetInstance();

		imguiManager->Begin();

		//-------update current scene---------

		if (current == SceneId::Title) {
			titleScene->Update();

			if (titleScene->GetRequest() == IScene::Request::GoToGame) {
				titleScene->ClearRequest();

				delete gameScene;
				gameScene = new GameScene();
				gameScene->Initialize(); // fresh game / retry
				current = SceneId::Game;
			}
		}
		else if (current == SceneId::Game) {
			gameScene->Update();

			if (gameScene->IsFinished()) {
				gameScene->ClearFinished();
				titleScene->SetShowGameOver(true);

				delete gameScene;
				gameScene = nullptr;

				current = SceneId::Title;
			}
		}

		imguiManager->End();

		/// 描画開始

		dxCommon->PreDraw();

		/// ゲームシーンの描画

		if (current == SceneId::Title) {
			titleScene->Draw();
		}
		else if (current == SceneId::Game && gameScene) {
			gameScene->Draw();
		}

		/// 軸表示の描画

		AxisIndicator::GetInstance()->Draw();

		/// ImGuiの描画
		imguiManager->Draw();

		/// 描画終了

		dxCommon->PostDraw();

		/// 解放処理
	}

	delete gameScene;
	delete titleScene;
	gameScene = nullptr;

	KamataEngine::Finalize();
	return 0;
}
