#include "Global.h"

namespace Util
{


	D3DXVECTOR3 CalcVibrationOffset(float freq, float amplitude, const D3DXVECTOR3& forward);

	D3DXVECTOR3 CalcUpDownOffset(float dt, float amp, float speed, const D3DXVECTOR3& forward);

}