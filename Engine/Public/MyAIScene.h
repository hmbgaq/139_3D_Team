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
	CMyAIScene(const aiScene* _pAIScene);
	CMyAIScene(const string& strModelFilePath);


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
	void Bake_Binary(const string& strModelFilePath);

	//const aiScene*	Get_AIScene() { return m_pAIScene; };
	CMyAINode		Get_RootNode();

	_uint			Get_NumMeshes();
	_uint			Get_NumMaterials();
	_uint			Get_NumAnimations();
	
	CMyAIMesh		Get_Mesh(_uint _iIndex);
	CMyAIMaterial	Get_Material(_uint _iIndex);
	CMyAIAnimation	Get_Animation(_uint _iIndex);



};

END