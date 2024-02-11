#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CMyAIVectorKey
{
public:
	CMyAIVectorKey() {};
	CMyAIVectorKey(aiVectorKey _pVectorKey) { 
		//m_pVectorKey = _pVectorKey; 

		memcpy(&mValue, &_pVectorKey.mValue, sizeof(_float3));
		mTime = _pVectorKey.mTime;
	};

	CMyAIVectorKey(ifstream& is)
	{
		read_typed_data(is, mValue);
		read_typed_data(is, mTime);
	};

private:
	//aiVectorKey m_pVectorKey;
	_float3 mValue = { 0.f, 0.f, 0.f };
	_float	mTime = 0.f;

public:
	void Bake_Binary(ofstream& os)
	{
		write_typed_data(os, mValue);
		write_typed_data(os, mTime);
	}

	_float3 Get_Value() { return mValue; };
	_float	Get_Time()	{ return mTime; };

};

END