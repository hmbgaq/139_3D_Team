#include "stdafx.h"
#include "MyAIScene.h"

CMyAIScene::CMyAIScene(const aiScene* _pAIScene)
{
	mRootNode = _pAIScene->mRootNode;

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

CMyAIScene::CMyAIScene(const string& strModelFilePath)
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


CMyAINode CMyAIScene::Get_RootNode() {
	//if (false == m_bIsBianry)
	//	return CMyAINode(m_pAIScene->mRootNode);
	//return m_pAIScene->mRootNode;
	return mRootNode;
};

void CMyAIScene::Bake_Binary(const string& strModelFilePath)
{
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

_uint CMyAIScene::Get_NumMeshes() {
	//if (false == m_bIsBianry)
	//	return m_pAIScene->mNumMeshes;
	return mNumMeshes;
};

CMyAIMesh CMyAIScene::Get_Mesh(_uint _iIndex) {
	//if (false == m_bIsBianry)
	//	return CMyAIMesh(m_pAIScene->mMeshes[_iIndex]);
	return mMeshes[_iIndex];
};

_uint CMyAIScene::Get_NumMaterials() {
	//if (false == m_bIsBianry)
	//	return m_pAIScene->mNumMaterials;
	return mNumMaterials;
};

CMyAIMaterial CMyAIScene::Get_Material(_uint _iIndex) {
	//if (false == m_bIsBianry)
	//	return CMyAIMaterial(m_pAIScene->mMaterials[_iIndex]);
	return mMaterials[_iIndex];
};

_uint CMyAIScene::Get_NumAnimations() {
	//if (false == m_bIsBianry)
	//	return m_pAIScene->mNumAnimations;
	return mNumAnimations;
};

CMyAIAnimation CMyAIScene::Get_Animation(_uint _iIndex) {
	//if (false == m_bIsBianry)
	//	return CMyAIAnimation(m_pAIScene->mAnimations[_iIndex]);
	return mAnimations[_iIndex];
};