#include "MapChip3D.h"
#include <map>
#include <fstream>
#include <sstream>
#include <cassert>

namespace {
	std::map<std::string, MapChipType> mapChipTable = {
		{"0", MapChipType::kBlank},
		{"1", MapChipType::kBlock},
		// Add more as needed
	};
}

MapChip3D::Rect MapChip3D::GetRectByIndex(uint32_t xIndex, uint32_t zIndex)
{
	Rect rect = {};
	Vector3 center = GetMapChipPositionByIndex(xIndex, zIndex);

	rect.left = center.x - (kBlockWidth_ / 2.0f);
	rect.right = center.x + (kBlockWidth_ / 2.0f);
	rect.front = center.z - (kBlockDepth_ / 2.0f);
	rect.back = center.z + (kBlockDepth_ / 2.0f);
	return rect;
}

MapChip3D::IndexSet MapChip3D::GetMapChipIndexByPosition(const Vector3& position)
{
	IndexSet indexSet = {};

	indexSet.xIndex = static_cast<uint32_t>(std::floor(position.x + (kBlockWidth_ / 2.0f)) / kBlockWidth_);

	//if inverting Z axis is needed,
	float invertedZ = (kNumBlockZAxis_ * kBlockDepth_) - position.z;
	indexSet.zIndex = static_cast<uint32_t>(std::floor(invertedZ - (kBlockDepth_ / 2.0f)) / kBlockDepth_);

	// if not inverting Z axis, use the following line instead:
	//indexSet.zIndex = static_cast<uint32_t>(std::floor(position.z + (kBlockDepth_ / 2.0f)) / kBlockDepth_);

	//clamp to valid range to prevent crashes
	indexSet.xIndex = std::clamp(indexSet.xIndex, 0u, kNumBlockXAxis_ - 1);
	indexSet.zIndex = std::clamp(indexSet.zIndex, 0u, kNumBlockZAxis_ - 1);

	return indexSet;
}

Vector3 MapChip3D::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t zIndex)
{
	float x = static_cast<float>(xIndex) * kBlockWidth_;
	float y = static_cast<float>(0.0f) * kBlockHeight_; // Assuming y is always 0 for map chips)
	float z = (kNumBlockZAxis_ - 1 - static_cast<float>(zIndex)) * kBlockDepth_;
	return Vector3{ x, y, z };
}

MapChipType MapChip3D::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t zIndex)
{
	if (xIndex < 0 || kNumBlockXAxis_ - 1 < xIndex) {
		return MapChipType::kBlank;
	}
	if (zIndex < 0 || kNumBlockZAxis_ - 1 < zIndex) {
		return MapChipType::kBlank;
	}
	return mapChipData_.mapData[zIndex][xIndex];
}

void MapChip3D::ResetMapChipData()
{
	MapChip3D::mapChipData_.mapData.clear();
	MapChip3D::mapChipData_.mapData.resize(kNumBlockZAxis_);
	for (std::vector<MapChipType>& mapChipDataLine : mapChipData_.mapData) {
		mapChipDataLine.resize(kNumBlockXAxis_/*, MapChipType::Floor*/); // Default to Floor
	}
}

void MapChip3D::LoadMapChipData(const std::string& filePath)
{
	ResetMapChipData();

	std::ifstream file;
	file.open(filePath);
	assert(file.is_open());

	std::stringstream mapChipCsv;
	mapChipCsv << file.rdbuf();
	file.close();

	for (uint32_t i = 0; i < kNumBlockZAxis_; ++i) {
		std::string line;
		std::getline(mapChipCsv, line);

		std::stringstream lineStream(line);

		for (uint32_t j = 0; j < kNumBlockXAxis_; ++j) {
			std::string word;
			std::getline(lineStream, word, ',');

			if (mapChipTable.contains(word)) {
				mapChipData_.mapData[i][j] = mapChipTable[word];
			}
			//else {
			//	mapChipData_.mapData[i][j] = MapChipType::Floor; // Default to Floor if not found
			//}
		}
	}

}
