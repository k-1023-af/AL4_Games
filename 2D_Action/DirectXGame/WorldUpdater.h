#pragma once
#include "Function.h"
#include "KamataEngine.h"

using namespace KamataEngine;

class WorldUpdater {
public:
	//void Initialize();
	void Update(WorldTransform& worldTransform);
	//void Draw();

	struct AABB {
		KamataEngine::Vector3 min;
		KamataEngine::Vector3 max;
	};

	AABB GetAABB();

	static bool IsCollisionAABB(const AABB& a, const AABB& b);

	WorldUpdater();
	~WorldUpdater();
private:

};

