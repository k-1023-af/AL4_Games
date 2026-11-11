#include "WorldUpdate.h"

WorldUpdate::WorldUpdate() {}

WorldUpdate::~WorldUpdate() {}

void WorldUpdate::Update(KamataEngine::WorldTransform& wt) {
	wt.matWorld_ = MakeAffineMatrix(wt.scale_, wt.rotation_, wt.translation_);
	wt.TransferMatrix();
}
