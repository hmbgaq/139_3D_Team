#include "..\Public\Base.h"

CBase::CBase()
{
}

_ulong CBase::AddRef()
{
	return ++m_dwRefCnt;
}

/* 삭제하거나 감소시키거나. */
_ulong CBase::Release()
{
	if (0 == m_dwRefCnt)
	{
		/* 소멸자. */
		Free();

		delete this;

		return 0;
	}
	else
		return m_dwRefCnt--;
}


