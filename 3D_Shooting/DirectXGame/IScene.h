#pragma once

class IScene {
public:
	virtual ~IScene() = default;
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	enum class Request {
		None,
		GoToGame,
		GoToTitle,
		Quit
	};
	virtual Request GetRequest() const { return Request::None; }
	virtual void ClearRequest() {}
};