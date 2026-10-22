#include "Particles.h"

void Particles::Initialize() {

	behavior_ = Behavior::kInitialize;
	behaviorRequest_ = Behavior::kUnknown;
	AnimPhase = AnimPhase::kSpread;

	// set counters to 0


	isDead_ = false;
	isFinished_ = false;
}

void Particles::Update() {
	if (isFinished_) return;

	if (behaviorRequest_ != Behavior::kUnknown) {
		behavior_ = behaviorRequest_;

		switch (behavior_) {
		case Behavior::kInitialize:
		default:
			AnimPhaseInitialize();
			break;
		case Behavior::kDraw:
			break;
		}
		behaviorRequest_ = Behavior::kUnknown;
	}
	switch (behavior_) {
	case Behavior::kInitialize:
	default:
		AnimPhaseUpdate();
		break;
	case Behavior::kDraw:
		AnimPhaseUpdate();
		break;
	}
	//worldTransform Update goes here
}

void Particles::Draw() {
	//if (isFinished_ || camera_ == nullptr) return;

	//Model::PreDraw();
	//
	//Model::PostDraw();
}

void Particles::Create() {
}

void Particles::AnimPhaseInitialize() {
}

void Particles::AnimPhaseUpdate() {
}

Particles::Particles() { }

Particles::~Particles() { }
