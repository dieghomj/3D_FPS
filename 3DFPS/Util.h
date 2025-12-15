#include "Global.h"

namespace Util
{


	D3DXVECTOR3 CalcVibrationOffset(float time, float amplitude, float frequency, const D3DXVECTOR3& forward);

	D3DXVECTOR3 CalcUpDownOffset(float dt, float amp, float speed, const D3DXVECTOR3& forward);

}