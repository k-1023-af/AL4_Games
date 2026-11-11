#pragma once
#include "KamataEngine.h"
#include "Formula.h"

using namespace KamataEngine;

class WorldUpdate {
public:
	WorldUpdate();
	~WorldUpdate();

	void Initialize();
	void Update(WorldTransform& wt);

private:
	WorldTransform* worldTransform_;

	void WorldTransformUpdate(WorldTransform& wt) {
		Matrix4x4 MatrixS = MakeScale(wt.scale_);
		Matrix4x4 MatrixR = MakeRotatedXYZ(wt.rotation_);
		Matrix4x4 MatrixT = MakeTranslation(wt.translation_);

		wt.matWorld_ = MakeAffineMatrix(wt.scale_, wt.rotation_, wt.translation_);
		wt.TransferMatrix();
	}

};

