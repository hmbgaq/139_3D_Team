#include "..\Public\Easing_Utillity.h"


_vector Engine::Easing::LerpToType(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime, EASING_TYPE eType)
{
	switch (eType)
	{
	case EASING_TYPE::LINEAR:
		return Linear(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::QUAD_IN:
		return QuadIn(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::QUAD_OUT:
		return QuadInOut(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::QUAD_INOUT:
		return QuadInOut(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::CUBIC_IN:
		return CubicIn(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::CUBIC_OUT:
		return CubicOut(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::CUBIC_INOUT:
		return CubicInOut(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::QUART_IN:
		return QuartIn(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::QUART_OUT:
		return QuartOut(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::QUART_INOUT:
		return QuartInOut(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::QUINT_IN:
		return QuintIn(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::QUINT_OUT:
		return QuintOut(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::QUINT_INOUT:
		return QuintInOut(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::SINE_IN:
		return SineIn(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::SINE_OUT:
		return SineOut(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::SINE_INOUT:
		return SineInOut(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::EXPO_IN:
		return ExpoIn(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::EXPO_OUT:
		return ExpoOut(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::EXPO_INOUT:
		return ExpoInOut(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::CIRC_IN:
		return CircIn(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::CIRC_OUT:
		return CircOut(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::CIRC_INOUT:
		return CircInOut(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::ELASTIC_IN:
		return ElasticIn(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::ELASTIC_OUT:
		return ElasticOut(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::ELASTIC_INOUT:
		return ElasticInOut(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::BOUNCE_IN:
		return BounceIn(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::BOUNCE_OUT:
		return BounceOut(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::OUT_BACK:
		return OutBack(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::INOUT_BACK:
		return InOutBack(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::EASING_TYPE_END:
		break;

	default:
		break;
	}

	return Linear(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
}

_float ENGINE_DLL Engine::Easing::LerpToType(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime, EASING_TYPE eType)
{
	switch (eType)
	{
	case EASING_TYPE::LINEAR:
		return Linear(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::QUAD_IN:
		return QuadIn_Float(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::QUAD_OUT:
		return QuadInOut(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::QUAD_INOUT:
		return QuadInOut(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::CUBIC_IN:
		return CubicIn(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::CUBIC_OUT:
		return CubicOut(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::CUBIC_INOUT:
		return CubicInOut(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::QUART_IN:
		return QuartIn(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::QUART_OUT:
		return QuartOut(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::QUART_INOUT:
		return QuartInOut(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::QUINT_IN:
		return QuintIn(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::QUINT_OUT:
		return QuintOut(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::QUINT_INOUT:
		return QuintInOut(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::SINE_IN:
		return SineIn(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::SINE_OUT:
		return SineOut(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::SINE_INOUT:
		return SineInOut(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::EXPO_IN:
		return ExpoIn(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::EXPO_OUT:
		return ExpoOut(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::EXPO_INOUT:
		return ExpoInOut(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::CIRC_IN:
		return CircIn(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::CIRC_OUT:
		return CircOut(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::CIRC_INOUT:
		return CircInOut(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::ELASTIC_IN:
		return ElasticIn(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::ELASTIC_OUT:
		return ElasticOut(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::ELASTIC_INOUT:
		return ElasticInOut(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::BOUNCE_IN:
		return BounceIn(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
		break;

	case EASING_TYPE::BOUNCE_OUT:
		return BounceOut(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
		break;

	//case EASING_TYPE::OUT_BACK:
	//	return OutBack(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
	//	break;

	//case EASING_TYPE::INOUT_BACK:
	//	return InOutBack(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
	//	break;

	case EASING_TYPE::EASING_TYPE_END:
		break;

	default:
		break;
	}

	return Linear(fStartPoint, fTargetPoint, fPassedTime, fTotalTime);
}


_vector Engine::Easing::Linear(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime)
{
	return _vector((vTargetPoint - vStartPoint) * fPassedTime / fTotalTime + vStartPoint);
}

_float Engine::Easing::Linear(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime)
{
	return  _float((fTargetPoint - fStartPoint) * fPassedTime / fTotalTime + fStartPoint);
}

_vector Engine::Easing::QuadIn(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime;
	return _vector((vTargetPoint - vStartPoint) * fPassedTime * fPassedTime + vStartPoint);
}

_vector Engine::Easing::QuadIn(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime;
	_vector	vStartPoint(XMVectorSet(fStartPoint, fStartPoint, fStartPoint, fStartPoint));
	_vector	vTargetPoint(XMVectorSet(fTargetPoint, fTargetPoint, fTargetPoint, fTargetPoint));

	return QuadIn(vStartPoint, vTargetPoint, fPassedTime, fTotalTime);
}

_float Engine::Easing::QuadIn_Float(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime;
	return _float((fTargetPoint - fStartPoint) * fPassedTime * fPassedTime + fStartPoint);
}

_vector Engine::Easing::QuadOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime;
	return _vector(-(vTargetPoint - vStartPoint) * fPassedTime*(fPassedTime - 2.f) + vStartPoint);
}

_float Engine::Easing::QuadOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime;
	return _float(-(fTargetPoint - fStartPoint) * fPassedTime * (fPassedTime - 2.f) + fStartPoint);
}

_vector Engine::Easing::QuadInOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime / 2.f;
	if (fPassedTime < 1.f)
		return _vector((vTargetPoint - vStartPoint) / 2.f * fPassedTime*fPassedTime + vStartPoint);

	fPassedTime--;
	return _vector(-(vTargetPoint - vStartPoint) / 2.f * (fPassedTime*(fPassedTime - 2.f) - 1.f) + vStartPoint);
}

_float Engine::Easing::QuadInOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime / 2.f;
	if (fPassedTime < 1.f)
		return _float((fTargetPoint - fStartPoint) / 2.f * fPassedTime * fPassedTime + fStartPoint);

	fPassedTime--;
	return _float(-(fTargetPoint - fStartPoint) / 2.f * (fPassedTime * (fPassedTime - 2.f) - 1.f) + fStartPoint);
}

_vector Engine::Easing::CubicIn(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime;
	return _vector((vTargetPoint - vStartPoint) * fPassedTime*fPassedTime*fPassedTime + vStartPoint);
}

_float Engine::Easing::CubicIn(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime;
	return _float((fTotalTime - fStartPoint) * fPassedTime * fPassedTime * fPassedTime + fStartPoint);
}

_vector Engine::Easing::CubicOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime;
	fPassedTime--;
	return _vector((vTargetPoint - vStartPoint) * (fPassedTime*fPassedTime*fPassedTime + 1.f) + vStartPoint);
}

_float Engine::Easing::CubicOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime;
	fPassedTime--;
	return _float((fTargetPoint - fStartPoint) * (fPassedTime * fPassedTime * fPassedTime + 1.f) + fStartPoint);
}

_vector Engine::Easing::CubicInOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime / 2.f;
	if (fPassedTime < 1.f) return _vector((vTargetPoint - vStartPoint) / 2.f * fPassedTime*fPassedTime*fPassedTime + vStartPoint);
	fPassedTime -= 2.f;
	return _vector((vTargetPoint - vStartPoint) / 2.f * (fPassedTime*fPassedTime*fPassedTime + 2.f) + vStartPoint);
}

_float Engine::Easing::CubicInOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime / 2.f;
	if (fPassedTime < 1.f) return _float((fTargetPoint - fStartPoint) / 2.f * fPassedTime * fPassedTime * fPassedTime + fStartPoint);
	fPassedTime -= 2.f;
	return _float((fTargetPoint - fStartPoint) / 2.f * (fPassedTime * fPassedTime * fPassedTime + 2.f) + fStartPoint);
}

_vector Engine::Easing::QuartIn(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime;
	return _vector((vTargetPoint - vStartPoint) * fPassedTime*fPassedTime*fPassedTime*fPassedTime + vStartPoint);
}

_float Engine::Easing::QuartIn(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime;
	return _float((fTargetPoint - fStartPoint) * fPassedTime * fPassedTime * fPassedTime * fPassedTime + fStartPoint);
}

_vector Engine::Easing::QuartOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime;
	fPassedTime--;
	return _vector(-(vTargetPoint - vStartPoint) * (fPassedTime*fPassedTime*fPassedTime*fPassedTime - 1.f) + vStartPoint);
}

_float Engine::Easing::QuartOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime;
	fPassedTime--;
	return _float(-(fTargetPoint - fStartPoint) * (fPassedTime * fPassedTime * fPassedTime * fPassedTime - 1.f) + fStartPoint);
}

_vector Engine::Easing::QuartInOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime / 2.f;
	if (fPassedTime < 1.f) return _vector((vTargetPoint - vStartPoint) / 2.f * fPassedTime*fPassedTime*fPassedTime*fPassedTime + vStartPoint);
	fPassedTime -= 2.f;
	return _vector(-(vTargetPoint - vStartPoint) / 2.f * (fPassedTime*fPassedTime*fPassedTime*fPassedTime - 2.f) + vStartPoint);
}

_float Engine::Easing::QuartInOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime / 2.f;
	if (fPassedTime < 1.f) return _float((fTargetPoint - fStartPoint) / 2.f * fPassedTime * fPassedTime * fPassedTime * fPassedTime + fStartPoint);
	fPassedTime -= 2.f;
	return _float(-(fTargetPoint - fStartPoint) / 2.f * (fPassedTime * fPassedTime * fPassedTime * fPassedTime - 2.f) + fStartPoint);
}

_vector Engine::Easing::QuintIn(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime;
	return _vector((vTargetPoint - vStartPoint) * fPassedTime*fPassedTime*fPassedTime*fPassedTime*fPassedTime + vStartPoint);
}

_float Engine::Easing::QuintIn(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime;
	return _float((fTargetPoint - fStartPoint) * fPassedTime * fPassedTime * fPassedTime * fPassedTime * fPassedTime + fStartPoint);
}

_vector Engine::Easing::QuintOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime;
	fPassedTime--;
	return _vector((vTargetPoint - vStartPoint) * (fPassedTime*fPassedTime*fPassedTime*fPassedTime*fPassedTime + 1.f) + vStartPoint);
}

_float Engine::Easing::QuintOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime;
	fPassedTime--;
	return _float((fTargetPoint - fStartPoint) * (fPassedTime * fPassedTime * fPassedTime * fPassedTime * fPassedTime + 1.f) + fStartPoint);
}

_vector Engine::Easing::QuintInOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime / 2.f;
	if (fPassedTime < 1.f) return _vector((vTargetPoint - vStartPoint) / 2.f * fPassedTime*fPassedTime*fPassedTime*fPassedTime*fPassedTime + vStartPoint);
	fPassedTime -= 2.f;
	return _vector((vTargetPoint - vStartPoint) / 2.f * (fPassedTime*fPassedTime*fPassedTime*fPassedTime*fPassedTime + 2.f) + vStartPoint);
}

_float Engine::Easing::QuintInOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime / 2.f;
	if (fPassedTime < 1.f) return _float((fTargetPoint - fStartPoint) / 2.f * fPassedTime * fPassedTime * fPassedTime * fPassedTime * fPassedTime + fStartPoint);
	fPassedTime -= 2.f;
	return _float((fTargetPoint - fStartPoint) / 2.f * (fPassedTime * fPassedTime * fPassedTime * fPassedTime * fPassedTime + 2.f) + fStartPoint);
}

_vector Engine::Easing::SineIn(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime)
{
	return _vector(-(vTargetPoint - vStartPoint) * cosf(fPassedTime / fTotalTime * (PI / 2.f)) + (vTargetPoint - vStartPoint) + vStartPoint);
}

_float Engine::Easing::SineIn(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime)
{
	return _float(-(fTargetPoint - fStartPoint) * cosf(fPassedTime / fTotalTime * (PI / 2.f)) + (fTargetPoint - fStartPoint) + fStartPoint);
}

_vector Engine::Easing::SineOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime)
{
	return _vector((vTargetPoint - vStartPoint) * sinf(fPassedTime / fTotalTime * (PI / 2.f)) + vStartPoint);
}

_float Engine::Easing::SineOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime)
{
	return _float((fTargetPoint - fStartPoint) * sinf(fPassedTime / fTotalTime * (PI / 2.f)) + fStartPoint);
}

_vector Engine::Easing::SineInOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime)
{
	return _vector(-(vTargetPoint - vStartPoint) / 2.f * (cosf(PI*fPassedTime / fTotalTime) - 1.f) + vStartPoint);
}

_float Engine::Easing::SineInOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime)
{
	return _float(-(fTargetPoint - fStartPoint) / 2.f * (cosf(PI * fPassedTime / fTotalTime) - 1.f) + fStartPoint);
}

_vector Engine::Easing::ExpoIn(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime)
{
	return _vector((vTargetPoint - vStartPoint) * pow(2.f, 10.0f * (fPassedTime / fTotalTime - 1.f)) + vStartPoint);
}

_float Engine::Easing::ExpoIn(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime)
{
	return _float((fTargetPoint - fStartPoint) * pow(2.f, 10.0f * (fPassedTime / fTotalTime - 1.f)) + fStartPoint);
}

_vector Engine::Easing::ExpoOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime)
{
	return _vector((vTargetPoint - vStartPoint) * (-pow(2.f, -10.f * fPassedTime / fTotalTime) + 1.f) + vStartPoint);
}

_float Engine::Easing::ExpoOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime)
{
	return _float((fTargetPoint - fStartPoint) * (-pow(2.f, -10.f * fPassedTime / fTotalTime) + 1.f) + fStartPoint);
}

_vector Engine::Easing::ExpoInOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime / 2.f;
	if (fPassedTime < 1.f) return _vector((vTargetPoint - vStartPoint) / 2.f * pow(2.f, 10.f * (fPassedTime - 1.f)) + vStartPoint);
	fPassedTime--;
	return _vector((vTargetPoint - vStartPoint) / 2.f * (-pow(2.f, -10.f * fPassedTime) + 2.f) + vStartPoint);
}

_float Engine::Easing::ExpoInOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime / 2.f;
	if (fPassedTime < 1.f) return _float((fTargetPoint - fStartPoint) / 2.f * pow(2.f, 10.f * (fPassedTime - 1.f)) + fStartPoint);
	fPassedTime--;
	return _float((fTargetPoint - fStartPoint) / 2.f * (-pow(2.f, -10.f * fPassedTime) + 2.f) + fStartPoint);
}

_vector Engine::Easing::CircIn(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime;
	return _vector(-(vTargetPoint - vStartPoint) * (sqrtf(1.f - fPassedTime * fPassedTime) - 1.f) + vStartPoint);
}

_float Engine::Easing::CircIn(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime;
	return _float(-(fTargetPoint - fStartPoint) * (sqrtf(1.f - fPassedTime * fPassedTime) - 1.f) + fStartPoint);
}

_vector Engine::Easing::CircOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime;
	fPassedTime--;
	return _vector((vTargetPoint - vStartPoint) * sqrtf(1.f - fPassedTime * fPassedTime) + vStartPoint);
}

_float Engine::Easing::CircOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime;
	fPassedTime--;
	return _float((fTargetPoint - fStartPoint) * sqrtf(1.f - fPassedTime * fPassedTime) + fStartPoint);
}

_vector Engine::Easing::CircInOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime / 2.f;
	if (fPassedTime < 1.f) return _vector(-(vTargetPoint - vStartPoint) / 2.f * (sqrtf(1.f - fPassedTime * fPassedTime) - 1.f) + vStartPoint);
	fPassedTime -= 2.f;
	return _vector((vTargetPoint - vStartPoint) / 2.f * (sqrtf(1.f - fPassedTime * fPassedTime) + 1.f) + vStartPoint);
}

_float Engine::Easing::CircInOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime)
{
	fPassedTime /= fTotalTime / 2.f;
	if (fPassedTime < 1.f) return _float(-(fTargetPoint - fStartPoint) / 2.f * (sqrtf(1.f - fPassedTime * fPassedTime) - 1.f) + fStartPoint);
	fPassedTime -= 2.f;
	return _float((fTargetPoint - fStartPoint) / 2.f * (sqrtf(1.f - fPassedTime * fPassedTime) + 1.f) + fStartPoint);
}

_vector Engine::Easing::ElasticOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime)
{
	if ((fPassedTime /= fTotalTime) == 1.f)
		return _vector(vStartPoint + (vTargetPoint - vStartPoint));

	_float p(fTotalTime * .3f);
	_float s(p / 4.f);

	return _vector((vTargetPoint - vStartPoint) * pow(2.f, -10.f * fPassedTime) * sinf((fPassedTime * fTotalTime - s) * (2.f * PI) / p) + (vTargetPoint - vStartPoint) + vStartPoint);
}

_float Engine::Easing::ElasticOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime)
{
	if ((fPassedTime /= fTotalTime) == 1.f)
		return _float(fStartPoint + (fTargetPoint - fStartPoint));

	_float p(fTotalTime * .3f);
	_float s(p / 4.f);

	return _float((fTargetPoint - fStartPoint) * pow(2.f, -10.f * fPassedTime) * sinf((fPassedTime * fTotalTime - s) * (2.f * PI) / p) + (fTargetPoint - fStartPoint) + fStartPoint);
}

_vector Engine::Easing::ElasticIn(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime)
{
	if ((fPassedTime /= fTotalTime) == 1.f)
		return _vector(vStartPoint + (vTargetPoint - vStartPoint));

	_float p(fTotalTime * .3f);
	_float s(p / 4.f);

	return _vector(-((vTargetPoint - vStartPoint) * pow(2.f, 10.f * (fPassedTime -= 1.f)) * sinf((fPassedTime * fTotalTime - s) * (2.f * PI) / p)) + vStartPoint);
}

_float Engine::Easing::ElasticIn(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime)
{
	if ((fPassedTime /= fTotalTime) == 1.f)
		return _float(fStartPoint + (fTargetPoint - fStartPoint));

	_float p(fTotalTime * .3f);
	_float s(p / 4.f);

	return _float(-((fTargetPoint - fStartPoint) * pow(2.f, 10.f * (fPassedTime -= 1.f)) * sinf((fPassedTime * fTotalTime - s) * (2.f * PI) / p)) + fStartPoint);
}

_vector Engine::Easing::ElasticInOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime)
{
	if ((fPassedTime /= fTotalTime / 2.f) == 2.f)
		return _vector(vStartPoint + (vTargetPoint - vStartPoint));

	_float p(fTotalTime * (.3f * 1.5f));
	_float s(p / 4.f);

	if (fPassedTime < 1.f)
		return _vector(-.5f * ((vTargetPoint - vStartPoint) * pow(2.f, 10.f * (fPassedTime -= 1.f)) * sinf((fPassedTime * fTotalTime - s) * (2.f * PI) / p)) + vStartPoint);
	return _vector((vTargetPoint - vStartPoint) * pow(2.f, -10.f * (fPassedTime -= 1.f)) * sinf((fPassedTime * fTotalTime - s) * (2.f * PI) / p) * .5f + (vTargetPoint - vStartPoint) + vStartPoint);
}

_float Engine::Easing::ElasticInOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime)
{
	if ((fPassedTime /= fTotalTime / 2.f) == 2.f)
		return _float(fStartPoint + (fTargetPoint - fStartPoint));

	_float p(fTotalTime * (.3f * 1.5f));
	_float s(p / 4.f);

	if (fPassedTime < 1.f)
		return _float(-.5f * ((fTargetPoint - fStartPoint) * pow(2.f, 10.f * (fPassedTime -= 1.f)) * sinf((fPassedTime * fTotalTime - s) * (2.f * PI) / p)) + fStartPoint);
	return _float((fTargetPoint - fStartPoint) * pow(2.f, -10.f * (fPassedTime -= 1.f)) * sinf((fPassedTime * fTotalTime - s) * (2.f * PI) / p) * .5f + (fTargetPoint - fStartPoint) + fStartPoint);
}

_vector Engine::Easing::BounceOut(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime)
{
	if ((fPassedTime /= fTotalTime) < (1 / 2.75f))
		return _vector((vTargetPoint - vStartPoint) * (7.5625f * fPassedTime * fPassedTime) + vStartPoint);
	else if (fPassedTime < (2 / 2.75f))
		return _vector((vTargetPoint - vStartPoint) * (7.5625f * (fPassedTime -= (1.5f / 2.75f)) * fPassedTime + .75f) + vStartPoint);
	else if (fPassedTime < (2.5f / 2.75f))
		return _vector((vTargetPoint - vStartPoint) * (7.5625f * (fPassedTime -= (2.25f / 2.75f)) * fPassedTime + .9375f) + vStartPoint);
	else
		return _vector((vTargetPoint - vStartPoint) * (7.5625f * (fPassedTime -= (2.625f / 2.75f)) * fPassedTime + .984375f) + vStartPoint);
}

_float Engine::Easing::BounceOut(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime)
{
	if ((fPassedTime /= fTotalTime) < (1 / 2.75f))
		return _float((fTargetPoint - fStartPoint) * (7.5625f * fPassedTime * fPassedTime) + fStartPoint);
	else if (fPassedTime < (2 / 2.75f))
		return _float((fTargetPoint - fStartPoint) * (7.5625f * (fPassedTime -= (1.5f / 2.75f)) * fPassedTime + .75f) + fStartPoint);
	else if (fPassedTime < (2.5f / 2.75f))
		return _float((fTargetPoint - fStartPoint) * (7.5625f * (fPassedTime -= (2.25f / 2.75f)) * fPassedTime + .9375f) + fStartPoint);
	else
		return _float((fTargetPoint - fStartPoint) * (7.5625f * (fPassedTime -= (2.625f / 2.75f)) * fPassedTime + .984375f) + fStartPoint);
}

_vector Engine::Easing::OutBack(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime)
{
	const _float c1 = 1.70158f;
	const _float c3 = c1 + 1.f;

	return (vTargetPoint - vStartPoint) * (1.f + c3 * pow(fPassedTime / fTotalTime - 1.f, 3.f) + c1 * pow(fPassedTime / fTotalTime - 1.f, 2.f)) + vStartPoint;
}

_vector Engine::Easing::InOutBack(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime)
{
	const _float c1 = 1.70158f;
	const _float c2 = c1 * 1.525f;

	_float x = fPassedTime / fTotalTime;

	return x < 0.5f
		? (vTargetPoint - vStartPoint) * ((pow(2.f * x, 2.f) * ((c2 + 1.f) * 2.f * x - c2)) / 2.f) + vStartPoint
		: (vTargetPoint - vStartPoint) * ((pow(2.f * x - 2.f, 2.f) * ((c2 + 1.f) * (x * 2.f - 2.f) + c2) + 2.f) / 2.f) + vStartPoint;
	
}


_vector Engine::Easing::BounceIn(_vector vStartPoint, _vector vTargetPoint, _float fPassedTime, _float fTotalTime)
{
	return _vector(BounceOut(vTargetPoint, vStartPoint, fTotalTime - fPassedTime, fTotalTime));
}

_float Engine::Easing::BounceIn(_float fStartPoint, _float fTargetPoint, _float fPassedTime, _float fTotalTime)
{
	return _float(BounceOut(fTargetPoint, fStartPoint, fTotalTime - fPassedTime, fTotalTime));
}
