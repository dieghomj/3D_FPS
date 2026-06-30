#include "Util.h"

// 震動オフセット計算
// time: 経過時間
// amplitude: 振幅
// frequency: 周波数
// forward: 前方向ベクトル
D3DXVECTOR3 Util::CalcVibrationOffset(float freq, float amplitude, const D3DXVECTOR3& forward)
{
	// ローカル空間で小さな揺れを生成する: 横方向 + 上方向の振動
	float s = sinf(freq * 6.2831853f); // 2πf t
	float c = cosf(freq * 6.2831853f);

	// 前方向ベクトルをXZ平面に投影して右方向ベクトルを求める.
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

	// 横方向と縦方向にわずかに揺らす
	D3DXVECTOR3 offset = right * (amplitude * s) + up * (amplitude * 0.5f * c);
	return offset;
}

D3DXVECTOR3 Util::CalcUpDownOffset(float dt, float amp, float speed, const D3DXVECTOR3& forward)
{

	return D3DXVECTOR3();
}
