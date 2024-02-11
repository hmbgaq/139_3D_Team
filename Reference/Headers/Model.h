#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	class CBone* Get_BonePtr(const _char* pBoneName) const;

	void Set_Animation(_uint iAnimIndex) {
		m_iCurrentAnimIndex = iAnimIndex;
	}


public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const string& strModelFilePath, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render(_uint iMeshIndex);

public:
	void Play_Animation(_float fTimeDelta, _bool isLoop = true);

public:
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType);

private:
	const aiScene*			m_pAIScene = { nullptr };
	Assimp::Importer		m_Importer;

private:
	_float4x4				m_PivotMatrix;
	TYPE					m_eModelType = { TYPE_END };

	_uint					m_iNumMeshes = { 0 };
	vector<class CMesh*>	m_Meshes;

	_uint					m_iNumMaterials = { 0 };
	vector<MATERIAL_DESC>	m_Materials;

	/* 내 모델의 전체 뼈들을 부모관계를 포함하여 저장한다. */
	vector<class CBone*>	m_Bones;	
	
	_uint							m_iNumAnimations = { 0 };
	_uint							m_iCurrentAnimIndex = { 0 };
	vector<class CAnimation*>		m_Animations;

public:
	typedef vector<class CBone*>	BONES;



private:
	HRESULT	Ready_Meshes(_fmatrix PivotMatrix);	
	HRESULT Ready_Materials(const string& strModelFilePath);
	HRESULT Ready_Bones(aiNode* pAINode, _int iParentIndex);
	HRESULT Ready_Animations();

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const string& strModelFilePath, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END