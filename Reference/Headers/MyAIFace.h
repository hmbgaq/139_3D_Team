#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CMyAIFace
{
public:
	CMyAIFace() {};
	CMyAIFace(aiFace _pFace) {
		//m_pFace = _pFace;

		mNumIndices = _pFace.mNumIndices;

		for (_uint i = 0; i < _pFace.mNumIndices; ++i)
		{
			mIndices.push_back(_pFace.mIndices[i]);
		}
	};

	CMyAIFace(ifstream& is) {

		read_typed_data(is, mNumIndices);

		for (_uint i = 0; i < mNumIndices; ++i)
		{
			_uint iIndex;
			read_typed_data(is, iIndex);

			mIndices.push_back(iIndex);
		}
	};

private:
	//aiFace m_pFace;
	_uint mNumIndices = 0;
	vector<_uint> mIndices;

public:
	void Bake_Binary(ofstream& os)
	{
		write_typed_data(os, mNumIndices);

		for (_uint i = 0; i < mIndices.size(); ++i) 
		{
			write_typed_data(os, mIndices[i]);
		}
	}

	vector<_uint> Get_Indices() 
	{
		return mIndices;
	}

};

END