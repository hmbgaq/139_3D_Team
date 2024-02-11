#pragma once
#include "Base.h"
#include "MyAIQuatKey.h"
#include "MyAIVectorKey.h"

BEGIN(Engine)

class ENGINE_DLL CMyAINodeAnimation
{
public:
	CMyAINodeAnimation() {};
	CMyAINodeAnimation(aiNodeAnim* _pNodeAnimation) { 
		//m_pNodeAnimation = _pNodeAnimation; 

		mName = _pNodeAnimation->mNodeName.data;

		mNumScalingKeys = _pNodeAnimation->mNumScalingKeys;
		mNumRotationKeys = _pNodeAnimation->mNumRotationKeys;
		mNumPositionKeys = _pNodeAnimation->mNumPositionKeys;

		for (_uint i = 0; i < mNumScalingKeys; ++i) 
		{
			mScalingKeys.push_back(_pNodeAnimation->mScalingKeys[i]);
		}

		for (_uint i = 0; i < mNumRotationKeys; ++i)
		{
			mRotationKeys.push_back(_pNodeAnimation->mRotationKeys[i]);
		}

		for (_uint i = 0; i < mNumPositionKeys; ++i)
		{
			mPositionKeys.push_back(_pNodeAnimation->mPositionKeys[i]);
		}

	};

	CMyAINodeAnimation(ifstream& is) {

		size_t istringSize;
		read_typed_data(is, istringSize);
		mName.resize(istringSize);
		is.read(&mName[0], istringSize);

		read_typed_data(is, mNumScalingKeys);
		read_typed_data(is, mNumRotationKeys);
		read_typed_data(is, mNumPositionKeys);

		for (_uint i = 0; i < mNumScalingKeys; ++i)
		{
			mScalingKeys.push_back(CMyAIVectorKey(is));
		}

		for (_uint i = 0; i < mNumRotationKeys; ++i)
		{
			mRotationKeys.push_back(CMyAIQuatKey(is));
		}

		for (_uint i = 0; i < mNumPositionKeys; ++i)
		{
			mPositionKeys.push_back(CMyAIVectorKey(is));
		}

	}

private:
	//aiNodeAnim*		m_pNodeAnimation = { nullptr };

	string mName = "";

	_uint mNumScalingKeys = 0;
	_uint mNumRotationKeys = 0;
	_uint mNumPositionKeys = 0;

	vector<CMyAIVectorKey>	mScalingKeys;
	vector<CMyAIQuatKey>	mRotationKeys;
	vector<CMyAIVectorKey>	mPositionKeys;



public:
	void Bake_Binary(ofstream& os)
	{
		write_typed_data(os, mName.size());
		os.write(&mName[0], mName.size());

		write_typed_data(os, mNumScalingKeys);
		write_typed_data(os, mNumRotationKeys);
		write_typed_data(os, mNumPositionKeys);

		for (_uint i = 0; i < mNumScalingKeys; ++i)
		{
			mScalingKeys[i].Bake_Binary(os);
		}

		for (_uint i = 0; i < mNumRotationKeys; ++i)
		{
			mRotationKeys[i].Bake_Binary(os);
		}

		for (_uint i = 0; i < mNumPositionKeys; ++i)
		{
			mPositionKeys[i].Bake_Binary(os);
		}
	}

	string	Get_Name() { 
		//return m_pNodeAnimation->mNodeName.data; 
		return mName;
	};

	_uint Get_NumScalingKeys() { 
		//return m_pNodeAnimation->mNumScalingKeys; 
		return mNumScalingKeys;
	};

	_uint Get_NumRotationKeys() { 
		//return m_pNodeAnimation->mNumRotationKeys; 
		return mNumRotationKeys;
	};

	_uint Get_NumPositionKeys() {
		//return m_pNodeAnimation->mNumPositionKeys; 
		return mNumPositionKeys;
	};

	CMyAIVectorKey	Get_ScalingKeys(_uint _iIndex) {
		return CMyAIVectorKey(mScalingKeys[_iIndex]);
	};

	CMyAIQuatKey	Get_RotationKeys(_uint _iIndex) {
		
		return CMyAIQuatKey(mRotationKeys[_iIndex]);
	};


	CMyAIVectorKey Get_PositionKeys(_uint _iIndex) {

		return CMyAIVectorKey(mPositionKeys[_iIndex]);
	};

};

END