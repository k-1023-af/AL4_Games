#pragma once
#include "KamataEngine.h"
#include <algorithm>
#include <numbers>
#include <array>
#include <vector>
#include "WorldUpdater.h"
//#include "Player.h"

using namespace KamataEngine;

//-----------------------------
// Animation styles
//-----------------------------
enum class ParticleStyle {
	HitSpark,		// slash
	DeathBurst,		// mega man death
	EllipsePop,		// bubble gum
	RadialBurst,	// particles scatter
	SpinAndScale,	// tumble and scale down/up
	Pulse,			// 
	Twinkle,		// plus shaped (hit spark)
};

enum class BlendMode {
	Normal,
	Add,
	Multiply,
};

struct ParticleConfig {
	ParticleStyle style = ParticleStyle::HitSpark;

	// Visual
	Vector4 startColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	float width = 2.0f;
	float height = 2.0f;
	size_t partCount = 3;           // how many pieces

	// Timing (frames)
	float spreadDuration = 12.0f;
	float fadeDuration = 18.0f;

	// Scale curve
	float spreadStartScale = 0.2f;
	float spreadEndScale = 1.8f;
	float fadeEndScale = 1.4f;

	// Style-specific
	float radialSpeed = 0.15f;      // DeathBurst / RadialBurst
	float spinSpeed = 0.25f;        // SpinAndScale
	float pulseStrength = 0.6f;     // Pulse

	BlendMode blendMode = BlendMode::Add;

	// Optional per-part initial rotations
	std::array<Vector3, 8> rotations{};
};




class Particles {
public:
	void Initialize(const Vector3& position, const ParticleConfig& config = ParticleConfig{});
	void Update();
	void Draw();

	static Particles* Create(const Vector3& position, const ParticleConfig& config = ParticleConfig{});

	static void SetModel(Model* model) { model_ = model; }
	static void SetCamera(Camera* camera) { camera_ = camera; }

	bool IsDead() const { return isDead_; }

	void Start() { isActive_ = true; }

	Particles();
	~Particles();

	

private:
	void AnimPhaseUpdate();

	static Model* model_;
	static Camera* camera_;

	ParticleConfig config_;
	Vector3 originPos_{};

	static constexpr size_t kMaxParts = 8;
	std::array<WorldTransform, kMaxParts> parts_{};
	std::array<Vector3, kMaxParts> directions_{};
	size_t activePartCount_ = 0;
	
	ObjectColor objectColor_;
	Vector4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	uint32_t spreadParameter_ = 0;	// okay to use for multiple effects???
	uint32_t fadeParameter_ = 0;

	enum class Phase {Spread, Fade, Dead};
	Phase phase_ = Phase::Spread;

	bool isActive_ = false;
	bool isDead_ = false;

	// Helpers
	float EaseOut(float a, float b, float t) {
		t = std::clamp(t, 0.0f, 1.0f);
		return a + (1.0f - (1.0f - t) * (1.0f - t)) * (b - a);
	}
	float EaseIn(float a, float b, float t) {
		t = std::clamp(t, 0.0f, 1.0f);
		return a + t * t * (b - a);
	}
};

