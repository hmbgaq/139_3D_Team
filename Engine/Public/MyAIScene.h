#pragma once
#include "Base.h"
#include "MyAINode.h"
#include "MyAIMesh.h"
#include "MyAIMaterial.h"
#include "MyAIAnimation.h"

BEGIN(Engine)


class ENGINE_DLL CMyAIScene
{
public:
	CMyAIScene() = default;
	CMyAIScene(const aiScene* _pAIScene)
	{
		mRootNode = _pAIScene->mRootNode;
		
		//aiProcess_LimitBoneWeights
		

		mNumMeshes = _pAIScene->mNumMeshes;
		mNumMaterials = _pAIScene->mNumMaterials;
		mNumAnimations = _pAIScene->mNumAnimations;

		mMeshes.reserve(mNumMeshes);
		for (_uint i = 0; i < mNumMeshes; ++i)
		{
			mMeshes.push_back(CMyAIMesh(_pAIScene->mMeshes[i]));
		}

		mMaterials.reserve(mNumMaterials);
		for (_uint i = 0; i < mNumMaterials; ++i)
		{
			mMaterials.push_back(CMyAIMaterial(_pAIScene->mMaterials[i]));
		}

		mAnimations.reserve(mNumAnimations);
		for (_uint i = 0; i < mNumAnimations; ++i)
		{
			mAnimations.push_back(CMyAIAnimation(_pAIScene->mAnimations[i]));
		}
	};
	CMyAIScene(const string& strModelFilePath)
	{
		ifstream is(strModelFilePath, ios::binary);

		mRootNode = CMyAINode(is);

		read_typed_data(is, mNumMeshes);
		read_typed_data(is, mNumMaterials);
		read_typed_data(is, mNumAnimations);

		mMeshes.reserve(mNumMeshes);
		for (_uint i = 0; i < mNumMeshes; ++i)
		{
			mMeshes.push_back(CMyAIMesh(is));
		}

		mMaterials.reserve(mNumMaterials);
		for (_uint i = 0; i < mNumMaterials; ++i)
		{
			mMaterials.push_back(CMyAIMaterial(is));
		}

		mAnimations.reserve(mNumAnimations);
		for (_uint i = 0; i < mNumAnimations; ++i)
		{
			mAnimations.push_back(CMyAIAnimation(is));
		}

		is.close();
	};


private:
	//const aiScene*			m_pAIScene = { nullptr };

	CMyAINode	mRootNode;

	_uint	mNumMeshes = 0;
	_uint	mNumMaterials = 0;
	_uint	mNumAnimations = 0;

	vector<CMyAIMesh>		mMeshes;
	vector<CMyAIMaterial>	mMaterials;
	vector<CMyAIAnimation>	mAnimations;


public:
	void Bake_Binary(const string& strModelFilePath) {
		string filePath = strModelFilePath + ".bin";

		ofstream os(filePath, ios::binary);


		mRootNode.Bake_Binary(os);

		write_typed_data(os, mNumMeshes);
		write_typed_data(os, mNumMaterials);
		write_typed_data(os, mNumAnimations);

		for (_uint i = 0; i < mNumMeshes; ++i)
		{
			mMeshes[i].Bake_Binary(os);
		}
		

		for (_uint i = 0; i < mNumMaterials; ++i)
		{
			mMaterials[i].Bake_Binary(os);
		}

		
		for (_uint i = 0; i < mNumAnimations; ++i)
		{
			mAnimations[i].Bake_Binary(os);
		}
		

		os.close();
	};

	//const aiScene*	Get_AIScene() { return m_pAIScene; };
	CMyAINode		Get_RootNode() {
		return mRootNode;
	};

	_uint			Get_NumMeshes() {
		return mNumMeshes;
	};
	_uint			Get_NumMaterials() {
		return mNumMaterials;
	};
	_uint			Get_NumAnimations() {
		return mNumAnimations;
	};
	
	CMyAIMesh		Get_Mesh(_uint _iIndex) {
		return mMeshes[_iIndex];
	};
	CMyAIMaterial	Get_Material(_uint _iIndex) {
		return mMaterials[_iIndex];
	};
	CMyAIAnimation	Get_Animation(_uint _iIndex) {
		return mAnimations[_iIndex];
	};



};

END