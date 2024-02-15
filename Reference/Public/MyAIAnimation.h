#pragma once
#include "Base.h"
#include "MyAINodeAnimation.h"

BEGIN(Engine)

class ENGINE_DLL CMyAIAnimation
{
public:
	CMyAIAnimation() {};
	CMyAIAnimation(aiAnimation* _pAnimation) { 
		//m_pAnimation = _pAnimation; m_bIsBianry = false; 

		mName = _pAnimation->mName.data;

		mDuration		= (_float)_pAnimation->mDuration;
		mTicksPerSecond = (_float)_pAnimation->mTicksPerSecond;
		mNumChannels	= (_uint)_pAnimation->mNumChannels;

		for (_uint i = 0; i < mNumChannels; ++i) 
		{
			mChannels.push_back(CMyAINodeAnimation(_pAnimation->mChannels[i]));
		}
	};

	CMyAIAnimation(ifstream& is) {
		size_t istringSize;
		read_typed_data(is, istringSize);
		mName.resize(istringSize);
		is.read(&mName[0], istringSize);

		read_typed_data(is, mDuration);
		read_typed_data(is, mTicksPerSecond);
		read_typed_data(is, mNumChannels);

		for (_uint i = 0; i < mNumChannels; ++i)
		{
			mChannels.push_back(CMyAINodeAnimation(is));
		}
	};

private:
	//_bool			m_bIsBianry		= { false };
	//aiAnimation*	m_pAnimation	= { nullptr };

	string	mName;
	_float	mDuration;
	_float	mTicksPerSecond;
	_uint	mNumChannels;

	vector<CMyAINodeAnimation> mChannels;


public:
	void Bake_Binary(ofstream& os)
	{
		write_typed_data(os, mName.size());
		os.write(&mName[0], mName.size());

		write_typed_data(os, mDuration);
		write_typed_data(os, mTicksPerSecond);
		write_typed_data(os, mNumChannels);

		for (_uint i = 0; i < mNumChannels; ++i) 
		{
			mChannels[i].Bake_Binary(os);
		}
	}

	string	Get_Name() { 
		//return m_pAnimation->mName.data; 
		return mName;
	};

	_float	Get_Duration() {
		//return m_pAnimation->mDuration; 
		return mDuration;
	};

	_float	Get_TickPerSecond() { 
		//return m_pAnimation->mTicksPerSecond; 
		return mTicksPerSecond;
	};

	_uint	Get_NumChannels() { 
		//return m_pAnimation->mNumChannels; 
		return mNumChannels;
	};
	
	CMyAINodeAnimation Get_Channel(_uint _iIndex) { 
		//return CMyAINodeAnimation(m_pAnimation->mChannels[_iIndex]); 
		return mChannels[_iIndex];
	};
};

END