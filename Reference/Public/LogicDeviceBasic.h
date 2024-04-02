#pragma once

#include "Engine_Defines.h"


BEGIN(Engine)

#pragma region 게이지

/// <summary>
/// fCur, fMax값을 저장하고, 관련 기능을 가진 객체
/// </summary>
class FGauge
{
public:
	explicit FGauge() : fMax(_float()), fCur(_float()), fPrevCur(fCur) {}
	explicit FGauge(_float _fMax, _bool bMax = false) : fMax(_fMax), fCur(_float(_float(bMax)* _float(_fMax))), fPrevCur(fCur) {}
	FGauge(const FGauge& rhs) : fMax(rhs.fMax), fCur(rhs.fCur), fPrevCur(rhs.fPrevCur) {}
	~FGauge() = default;

public:
	_float fMax, fCur;
private:
	_float fPrevCur;


public:
	// 값 업데이트 및 맥스값 도달시 반환
	_bool Increase(_float increase)
	{
		fPrevCur = fCur;
		fCur += increase;
		if (fCur >= fMax)
		{
			fCur = fMax;
			return true;
		}
		else if (fCur <= 0.f)
			fCur = 0.f;

		return false;
	}

	_bool Decrease(_float increase)
	{
		fPrevCur = fCur;
		fCur -= increase;
		if (fCur <= 0.f)
		{
			fCur = 0.f;
			return true;
		}
		else if (fCur >= fMax)
			fCur = fMax;

		return false;
	}

	// 특정 포인트(지점)를 넘어가면 반환
	_bool Increase_UpToPoint(_float increase, _float point)
	{
		fPrevCur = fCur;
		fCur += increase;
		if (fCur >= point)
		{
			fCur = point;
			return true;
		}
		else if (fCur <= 0.f)
			fCur = 0.f;

		return false;
	}

	// 특정 포인트(지점)를 넘어가면 반환, 한번 도달해도 끝까지 증가한다.
	_bool Increase_UpToPointOnce(_float increase, _float point)
	{
		fPrevCur = fCur;
		fCur += increase;
		if ((fCur >= point - increase * 0.5f) && (fCur < point + increase * 0.5f))
		{
			if (fCur >= fMax)
			{
				fCur = fMax;
			}
			return true;
		}
		else if (fCur <= 0.f)
			fCur = 0.f;

		return false;
	}

	// 현재값 초기화
	void Reset()
	{
		fCur = _float();
		fPrevCur = fCur;
	}

	void Reset(_float vResetValue)
	{
		fCur = vResetValue;
		fPrevCur = fCur;
	}

	// fMax 값 재설정 및 현재값 초기화
	void Readjust(_float _fMax)
	{
		fMax = _fMax;
		fCur = _float();
	}

	// 특정 포인트(지점)를 넘어가면 계속 트루
	_bool IsUpTo(_float point)
	{
		return (fCur >= point);
	}

	// 증가값과 체크하고자 하는 값으로 한번만 지나갈 때를 체크합니다.
	_bool IsUpTo_Once(_float point, _float increase)
	{
		return ((fCur >= point - increase * 0.5f) && (fCur < point + increase * 0.5f));
	}

	// 맥스일 경우 계속 트루
	_bool IsMax()
	{
		return (fCur >= fMax);
	}

	// 맥스일 경우 한번만 트루
	_bool IsMax_Once()
	{
		return (fCur >= fMax && fPrevCur != fCur);
	}

	// 퍼센트 값 반환
	_float Get_Percent()
	{
		return (static_cast<_float>(fCur) / static_cast<_float>(fMax));
	}
};

using FDelay = FGauge;

#pragma endregion


#pragma region 게이지

/// <summary>
/// fCur, fMin, fMax값을 저장하고
/// 저 사이를 fCur이 왕복하는 형태의 클래스이다.
/// 
/// </summary>
class FCircuitGauge
{
public:
	explicit FCircuitGauge() : fMax(_float()), fCur(_float()), fPrevCur(fCur) {}
	explicit FCircuitGauge(_float _fMax, _bool bMax = false) : fMax(_fMax), fCur(_float(_float(bMax)* _float(_fMax))), fPrevCur(fCur) {}
	explicit FCircuitGauge(const FCircuitGauge& rhs) : fMax(rhs.fMax), fCur(rhs.fCur), fPrevCur(rhs.fPrevCur) {}
	~FCircuitGauge() = default;

public:
	_float fMin, fMax, fCur;
private:
	_float	fPrevCur;
	_byte	bSign;

public:
	// 값 업데이트 및 맥스값 도달시 반환
	_bool Increase(_float increase)
	{
		fPrevCur = fCur;
		fCur += increase * _float(bSign);
		if (fCur >= fMax)
		{
			fCur = fMax;
			return true;
		}

		return false;
	}

	// 현재값 초기화
	void Reset()
	{
		fCur = _float();
		fPrevCur = fCur;
	}

	// fMax 값 재설정 및 현재값 초기화
	void Readjust(_float _fMin, _float _fMax)
	{
		fMin = _fMin;
		fMax = _fMax;
		fCur = _float();
	}

	// 특정 포인트(지점)를 넘어가면 계속 트루
	_bool IsUpTo(_float point)
	{
		return (fCur >= point);
	}

	// 증가값과 체크하고자 하는 값으로 한번만 지나갈 때를 체크합니다.
	_bool IsUpTo_Once(_float point, _float increase)
	{
		return ((fCur >= point - increase * 0.5f) && (fCur < point + increase * 0.5f));
	}

	// 맥스일 경우 계속 트루
	_bool IsMax()
	{
		return (fCur >= fMax);
	}

	// 맥스일 경우 한번만 트루
	_bool IsMax_Once()
	{
		return (fCur >= fMax && fPrevCur != fCur);
	}

	// 퍼센트 값 반환
	_float Get_Percent()
	{
		return (static_cast<_float>(fCur) / static_cast<_float>(fMax));
	}
};

using FDelay = FGauge;

#pragma endregion

END