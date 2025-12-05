#pragma once

namespace Util
{

	// Calculates a vibration offset based on time, amplitude, frequency, and forward direction.
	// The offset is a small jitter in local space, oscillating sideways and vertically.
	// The forward vector is used to derive the right vector for the sideways oscillation.
	// Returns the calculated offset as a D3DXVECTOR3.
	// Parameters:
	// - time: Current time in seconds.
	// - amplitude: Amplitude of the vibration.
	// - frequency: Frequency of the vibration.
	// - forward: Forward direction vector.
	// Returns: D3DXVECTOR3 representing the vibration offset.
static D3DXVECTOR3 CalcVibrationOffset(float time, float amplitude, float frequency, const D3DXVECTOR3& forward);


}
