#pragma once
#include "Base.h"
#include "MyAIVertexWeight.h"

BEGIN(Engine)

class ENGINE_DLL CMyAIBone
{
public:
	CMyAIBone() {};
	CMyAIBone(aiBone* _pBone) {
		//m_pBone = _pBone; 

		mName = _pBone->mName.data;
		mNumWeights = _pBone->mNumWeights;
		memcpy(&mOffsetMatrix, &_pBone->mOffsetMatrix, sizeof(_float4x4));

		for (_uint i = 0; i < mNumWeights; ++i)
		{
			mWeights.push_back(CMyAIVertexWeight(_pBone->mWeights[i]));
		}
	};
	
	CMyAIBone(ifstream& is) {
		size_t istringSize;
		read_typed_data(is, istringSize);
		mName.resize(istringSize);
		is.read(&mName[0], istringSize);

		read_typed_data(is, mOffsetMatrix);
		read_typed_data(is, mNumWeights);

		for (_uint i = 0; i < mNumWeights; ++i)
		{
			mWeights.push_back(CMyAIVertexWeight(is));
		}
	};


private:
	//aiBone*					m_pBone = { nullptr };
	_float4x4					mOffsetMatrix;
	string						mName;
	_uint						mNumWeights;

	vector<CMyAIVertexWeight>	mWeights;

public:
	void Bake_Binary(ofstream& os)
	{
		write_typed_data(os, mName.size());
		os.write(&mName[0], mName.size());

		write_typed_data(os, mOffsetMatrix);
		write_typed_data(os, mNumWeights);

		for (_uint i = 0; i < mNumWeights; ++i) 
		{
			mWeights[i].Bake_Binary(os);
		}
		
	}

	_float4x4	Get_OffsetMatrix() 
	{
		//memcpy(&mOffsetMatrix, &m_pBone->mOffsetMatrix, sizeof(_float4x4));
		return mOffsetMatrix;
	}

	string		Get_Name() 
	{
		//return m_pBone->mName.data;
		return mName;
	}

	_uint		Get_NumWeights() 
	{
		//return m_pBone->mNumWeights;
		return mNumWeights;
	}
	
	CMyAIVertexWeight Get_Weights(_uint _iIndex)
	{
		//return m_pBone->mWeights[_iIndex];
		return mWeights[_iIndex];
	}
	



};

END