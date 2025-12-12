#include "Util.h"

// 震動オフセット計算
// time: 経過時間
// amplitude: 振幅
// frequency: 周波数
// forward: 前方向ベクトル
D3DXVECTOR3 Util::CalcVibrationOffset(float time, float amplitude, float frequency, const D3DXVECTOR3& forward)
{
	// Build a small jitter in local space: side + up oscillation
	float s = sinf(time * frequency * 6.2831853f); // 2πf t
	float c = cosf(time * frequency * 6.2831853f);

	// Derive a right vector from forward projected on XZ.
	D3DXVECTOR3 f = forward;
	f.y = 0.f;
	if (D3DXVec3LengthSq(&f) > 0.f)
	{
		D3DXVec3Normalize(&f, &f);
	}
	else
	{
		f = D3DXVECTOR3(0.f, 0.f, 1.f);
	}
	D3DXVECTOR3 up(0.f, 1.f, 0.f);
	D3DXVECTOR3 right;
	D3DXVec3Cross(&right, &up, &f);
	if (D3DXVec3LengthSq(&right) > 0.f)
	{
		D3DXVec3Normalize(&right, &right);
	}
	else
	{
		right = D3DXVECTOR3(1.f, 0.f, 0.f);
	}

	// Jitter slightly sideways and vertically
	D3DXVECTOR3 offset = right * (amplitude * s) + up * (amplitude * 0.5f * c);
	return offset;
}
