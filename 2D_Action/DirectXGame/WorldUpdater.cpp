#include "WorldUpdater.h"

void WorldUpdater::Update(WorldTransform& worldTransform) {
	//Matrix4x4 MatrixS = MakeAffineMatrix(worldTransform.scale_);
	//Matrix4x4 MatrixR = MakeRotatedXYZ(worldTransform.rotation_);
	//Matrix4x4 MatrixT = MakeTranslation(worldTransform.translation_);

	//worldTransform.matWorld_ = Multiply(MatrixS, Multiply(MatrixR, MatrixT));

	worldTransform.matWorld_ = MakeAffineMatrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);
	worldTransform.TransferMatrix();
}

WorldUpdater::AABB WorldUpdater::GetAABB() {
	AABB aabb;
	return aabb;
}

bool WorldUpdater::IsCollisionAABB(const AABB& a, const AABB& b) {
	if (a.min.x <= b.max.x && a.max.x >= b.min.x && a.min.y <= b.max.y && a.max.y >= b.min.y && a.min.z <= b.max.z && a.max.z >= b.min.z) {
		return true;
	}
	return false;
}

WorldUpdater::WorldUpdater() {}

WorldUpdater::~WorldUpdater() {}
