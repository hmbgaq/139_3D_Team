#pragma once
#include "Engine_Defines.h"

#define PI 3.141592f

namespace Engine
{
	namespace Easing
	{
		 _vector ENGINE_DLL LerpToType(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime, EASING_TYPE eType);
		 _float	 ENGINE_DLL LerpToType(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime, EASING_TYPE eType);

		 _vector ENGINE_DLL Linear(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
		 _float  ENGINE_DLL Linear(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

		 _vector ENGINE_DLL QuadIn(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
		 _vector ENGINE_DLL QuadIn(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
		 _float  ENGINE_DLL QuadIn_Float(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

		 _vector ENGINE_DLL QuadOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
		 _float  ENGINE_DLL QuadOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

		 _vector ENGINE_DLL QuadInOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
		 _float  ENGINE_DLL QuadInOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

		 _vector ENGINE_DLL CubicIn(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
		 _float  ENGINE_DLL CubicIn(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
		 _vector ENGINE_DLL CubicOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
		 _float  ENGINE_DLL CubicOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
		 _vector ENGINE_DLL CubicInOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
		 _float  ENGINE_DLL CubicInOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

		 _vector ENGINE_DLL QuartIn(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
		 _float  ENGINE_DLL QuartIn(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
		 _vector ENGINE_DLL QuartOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
		 _float  ENGINE_DLL QuartOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
		 _vector ENGINE_DLL QuartInOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
		 _float  ENGINE_DLL QuartInOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

		 _vector ENGINE_DLL QuintIn(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
		 _float  ENGINE_DLL QuintIn(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
		 _vector ENGINE_DLL QuintOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
		 _float  ENGINE_DLL QuintOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
		 _vector ENGINE_DLL QuintInOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
		 _float  ENGINE_DLL QuintInOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

		 _vector ENGINE_DLL SineIn(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
		 _float  ENGINE_DLL SineIn(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
		 _vector ENGINE_DLL SineOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
		 _float  ENGINE_DLL SineOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
		 _vector ENGINE_DLL SineInOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
		 _float  ENGINE_DLL SineInOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

		 _vector ENGINE_DLL ExpoIn(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
		 _float  ENGINE_DLL ExpoIn(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
		 _vector ENGINE_DLL ExpoOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
		 _float  ENGINE_DLL ExpoOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
		 _vector ENGINE_DLL ExpoInOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
		 _float  ENGINE_DLL ExpoInOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	
		 _vector ENGINE_DLL CircIn(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
		 _float  ENGINE_DLL CircIn(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
		 _vector ENGINE_DLL CircOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
		 _float  ENGINE_DLL CircOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
		 _vector ENGINE_DLL CircInOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
		 _float  ENGINE_DLL CircInOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	
		 _vector ENGINE_DLL ElasticIn(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
		 _float  ENGINE_DLL ElasticIn(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
		 _vector ENGINE_DLL ElasticOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
		 _float  ENGINE_DLL ElasticOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
		 _vector ENGINE_DLL ElasticInOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
		 _float  ENGINE_DLL ElasticInOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);

		 _vector ENGINE_DLL BounceIn(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
		 _float  ENGINE_DLL BounceIn(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
		 _vector ENGINE_DLL BounceOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
		 _float  ENGINE_DLL BounceOut(_float	fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime);
	
		 _vector ENGINE_DLL OutBack(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
		 _vector ENGINE_DLL InOutBack(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime);
	}
}

// Reference : https://gizma.com/easing/