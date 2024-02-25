#pragma once
#include "Base.h"

BEGIN(Engine)

class CRandom_Manager final : public CBase
{

private:
	CRandom_Manager();
	virtual ~CRandom_Manager() = default;

public:
	HRESULT			Initialize();
	const _float&	Random_Float(_float fMin, _float fMax);
	const _int&		Random_Int(_int iMin, _int iMax);
	const _bool&	Random_Coin(_float fProbality);
	int64_t			GenerateUniqueID();

private:
	random_device m_Random;

	_float m_fRandom_float = { 0.f };
	_int   m_iRandom_int = { 0 };
	_bool m_bRandom_Bool = { false };

public:
	static CRandom_Manager* Create();
	virtual void Free() override;
};

END