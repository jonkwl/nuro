#pragma once

enum class RB_Interpolation {
	NONE,
	INTERPOLATE,
	EXTRAPOLATE
};

enum class RB_CollisionDetection {
	DISCRETE,
	CONTINUOUS,
	CONTINUOUS_SPECULATIVE
};

enum class RB_ForceMode {
	LINEAR,
	IMPULSE,
	VELOCITY_CHANGE,
	ACCELERATION
};