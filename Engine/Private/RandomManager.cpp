#include "..\Public\RandomManager.h"


CRandom_Manager::CRandom_Manager()
{
}

HRESULT CRandom_Manager::Initialize()
{
	return S_OK;
}

const _float& CRandom_Manager::Random_Float(_float fMin, _float fMax)
{
	std::mt19937_64 gen(m_Random());

	std::uniform_real_distribution<float> xfloatProbality(fMin, fMax);

	m_fRandom_float = xfloatProbality(gen);

	return m_fRandom_float;
}

const _int& CRandom_Manager::Random_Int(_int iMin, _int iMax)
{
	std::mt19937_64 gen(m_Random());

	std::uniform_int_distribution<int> xintProbality(iMin, iMax);

	m_iRandom_int = xintProbality(gen);

	return m_iRandom_int;
}

const _bool& CRandom_Manager::Random_Coin(_float fProbality)
{
	if (1.f <= fProbality)
		fProbality = 1.f;
	else if (0.f >= fProbality)
		fProbality = 0.f;

	std::mt19937_64 gen(m_Random());

	std::bernoulli_distribution coinFlip(fProbality);

	m_bRandom_Bool = coinFlip(gen);

	return m_bRandom_Bool;
}

long long CRandom_Manager::GenerateUniqueID()
{
	std::mt19937_64 gen(m_Random());

	std::uniform_int_distribution<int64_t> xIDProbality(1LL, LLONG_MAX);

	return xIDProbality(gen);
}

CRandom_Manager* CRandom_Manager::Create()
{
	CRandom_Manager* pInstance = new CRandom_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CRandom_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRandom_Manager::Free()
{
	__super::Free();
}
