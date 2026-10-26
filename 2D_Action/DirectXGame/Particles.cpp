#include "Particles.h"

Model* Particles::model_ = nullptr;
Camera* Particles::camera_ = nullptr;

Particles* Particles::Create(const Vector3& position, const ParticleConfig& config) {
	Particles* e = new Particles();
	e->Initialize(position, config);
	return e;
}

void Particles::Initialize(const Vector3& position, const ParticleConfig& config) {

	config_ = config;
	originPos_ = position;

	objectColor_.Initialize();
	color_ = config_.startColor;
	objectColor_.SetColor(color_);

	activePartCount_ = (std::min)(config_.partCount, kMaxParts);

	for (size_t i = 0; i < activePartCount_; ++i) {
		WorldTransform& wt = parts_[i];
		wt.Initialize();
		wt.scale_ = { config_.width, config_.height, 1.0f };
		wt.translation_ = position;

		if (i < config_.rotations.size()) {
			wt.rotation_ = config_.rotations[i];
		}

		// Pre-calculate radial directions
		float angle = (2.0f * std::numbers::pi_v<float> *i) / activePartCount_;
		directions_[i] = { std::cos(angle), std::sin(angle), 0.0f };
	}


	
	





	spreadParameter_ = 0;
	fadeParameter_ = 0;
	phase_ = Phase::Spread;
	isDead_ = false;
	isActive_ = true;
}

void Particles::Update() {
	if (!isActive_ || isDead_) return;
	AnimPhaseUpdate();

	for (size_t i = 0; i < activePartCount_; ++i) {
		parts_[i].TransferMatrix();
	}
	
}

void Particles::Draw() {
	if (!isActive_ || isDead_ || !model_ || !camera_) return;
	
	for (size_t i = 0; i < activePartCount_; ++i) {
		model_->Draw(parts_[i], *camera_, &objectColor_);
	}
}

void Particles::AnimPhaseUpdate() {
	switch (phase_) {
		//------------------------------------------------------------
	case Phase::Spread: {
		spreadParameter_++;
		float t = std::clamp(spreadParameter_ / config_.spreadDuration, 0.0f, 1.0f);

		switch (config_.style) {
		case ParticleStyle::HitSpark:
		case ParticleStyle::EllipsePop:
		case ParticleStyle::Twinkle: {
			float scale = EaseOut(config_.spreadStartScale, config_.spreadEndScale, t);
			for (size_t i = 0; i < activePartCount_; ++i) {
				parts_[i].scale_ = { config_.width * scale, config_.height * scale, 1.0f };
			}
			color_.w = 1.0f;
			break;
		}
		case ParticleStyle::DeathBurst:
		case ParticleStyle::RadialBurst: {
			float scale = EaseOut(config_.spreadStartScale, config_.spreadEndScale, t);
			float dist = t * config_.radialSpeed * 30.0f;
			for (size_t i = 0; i < activePartCount_; ++i) {
				parts_[i].scale_ = { config_.width * scale, config_.height * scale, 1.0f };
				parts_[i].translation_.x = originPos_.x + directions_[i].x * dist;
				parts_[i].translation_.y = originPos_.y + directions_[i].y * dist;
				parts_[i].translation_.z = originPos_.z + directions_[i].z * dist;
			}
			color_.w = 1.0f;
			break;
		}
		case ParticleStyle::SpinAndScale: {
			float scale = EaseOut(config_.spreadStartScale, config_.spreadEndScale, t);
			for (size_t i = 0; i < activePartCount_; ++i) {
				parts_[i].scale_ = { config_.width * scale, config_.height * scale, 1.0f };
				parts_[i].rotation_.z += config_.spinSpeed;
			}
			color_.w = 1.0f;
			break;
		}
		case ParticleStyle::Pulse: {
			float pulse = std::sin(t * std::numbers::pi_v<float>) * config_.pulseStrength;
			float scale = config_.spreadStartScale + (config_.spreadEndScale - config_.spreadStartScale) * t + pulse;
			for (size_t i = 0; i < activePartCount_; ++i) {
				parts_[i].scale_ = { config_.width * scale, config_.height * scale, 1.0f };
			}
			color_.w = 1.0f;
			break;
		}
		}

		objectColor_.SetColor(color_);

		if (spreadParameter_ >= config_.spreadDuration) {
			phase_ = Phase::Fade;
			fadeParameter_ = 0;
		}
		break;
	}

	case Phase::Fade: {
		fadeParameter_++;
		float t = std::clamp(fadeParameter_ / config_.fadeDuration, 0.0f, 1.0f);

		float scale = EaseIn(config_.spreadEndScale, config_.fadeEndScale, t);
		float alpha = EaseOut(1.0f, 0.0f, t);

		for (size_t i = 0; i < activePartCount_; ++i) {
			parts_[i].scale_ = { config_.width * scale, config_.height * scale, 1.0f };

			// Keep moving outward a little for radial styles
			if (config_.style == ParticleStyle::DeathBurst || config_.style == ParticleStyle::RadialBurst) {
				float dist = config_.radialSpeed * 30.0f + t * config_.radialSpeed * 15.0f;
				parts_[i].translation_.x = originPos_.x + directions_[i].x * dist;
				parts_[i].translation_.y = originPos_.y + directions_[i].y * dist;
				parts_[i].translation_.z = originPos_.z + directions_[i].z * dist;
			}
			if (config_.style == ParticleStyle::SpinAndScale) {
				parts_[i].rotation_.z += config_.spinSpeed * 0.5f;
			}
		}

		color_.w = alpha;
		objectColor_.SetColor(color_);

		if (fadeParameter_ >= config_.fadeDuration) {
			phase_ = Phase::Dead;
			isDead_ = true;
		}
		break;
	}
	case Phase::Dead:
		isDead_ = true;
		break;
	}
}

Particles::Particles() { }

Particles::~Particles() {
	//delete model_;
	//delete camera_;
}
