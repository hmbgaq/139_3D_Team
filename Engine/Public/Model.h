#pragma once

#include "Component.h"

#include "MyAssimp.h"
#include "MyAIScene.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

public:
	enum ANIM_STATE { ANIM_STATE_NORMAL, ANIM_STATE_LOOP, ANIM_STATE_REVERSE, ANIM_STATE_STOP, ANIM_STATE_END };

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	class CBone* Get_BonePtr(const _char* pBoneName) const;

	//void Set_Animation(_uint iAnimIndex) {
	//	m_iCurrentAnimIndex = iAnimIndex;
	//}

	_bool Is_AnimEnd() { return m_bIsAnimEnd; };

	void Set_StiffnessRate(_float fStiffnessRate);


public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const string& strModelFilePath, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render(_uint iMeshIndex);

public:
	void Play_Animation(_float fTimeDelta, _bool isLoop = true);

public:
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType);

public:
	void	Set_Animation(_uint _iAnimationIndex, CModel::ANIM_STATE _eAnimState = CModel::ANIM_STATE::ANIM_STATE_END, _bool _bIsTransition = true, _float _fTransitionDuration = 0.2f, _uint iTargetKeyFrameIndex = 0);
	void	Set_Animation_Transition(_uint _iAnimationIndex, _float _fTransitionDuration = 0.2f, _uint iTargetKeyFrameIndex = 0);
	void	Reset_Animation(_int iAnimIndex = -1);

	_float	Get_TickPerSecond();
	_bool	Is_Transition();
	void	Set_UseAnimationPos(_bool _bUseAnimationPos) { m_bUseAnimationPos = _bUseAnimationPos; };
	_bool	Is_Inputable_Front(_uint _iIndexFront);

	void Write_Names(const string& strModelFilePath);

private:
	CMyAssimp					m_MyAssimp;
	CMyAIScene					m_pAIScene;

private:
	_float4x4				m_PivotMatrix;
	TYPE					m_eModelType = { TYPE_END };

	_uint					m_iNumMeshes = { 0 };
	vector<class CMesh*>	m_Meshes;

	_uint					m_iNumMaterials = { 0 };
	vector<MATERIAL_DESC>	m_Materials;

	/* �� ���� ��ü ������ �θ���踦 �����Ͽ� �����Ѵ�. */
	vector<class CBone*>	m_Bones;	
	
	_uint							m_iNumAnimations = { 0 };
	_uint							m_iCurrentAnimIndex = { 0 };
	vector<class CAnimation*>		m_Animations;

	_bool							m_bIsAnimEnd = { false };
	ANIM_STATE						m_eAnimState = { CModel::ANIM_STATE::ANIM_STATE_END };
	_bool							m_bUseAnimationPos = { false };

public:
	typedef vector<class CBone*>	BONES;



private:
	HRESULT	Ready_Meshes(_fmatrix PivotMatrix);	
	HRESULT Ready_Materials(const string& strModelFilePath);
	HRESULT Ready_Bones(CMyAINode pAINode, _int iParentIndex);
	HRESULT Ready_Animations();

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const string& strModelFilePath, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END