#pragma once

#include "Component.h"

#include "MyAssimp.h"
#include "MyAIScene.h"

BEGIN(Engine)
class CAnimation;
class CMesh;
class CBone;


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
	_uint					Get_NumMeshes() const {return m_iNumMeshes; }
	//! 모델 인스턴싱 추가
	_uint					Get_NumMaterials() const { return m_iNumMaterials; }
	_uint					Get_NumMeshIndice(_int iMeshIndex);//! 모델 인스턴싱 전용
	vector<class CMesh*>&	Get_Meshes() { return m_Meshes;}
	//! 모델 인스턴싱 앤드
	class CBone*			Get_BonePtr(const _char* pBoneName) const;

	void					Set_StiffnessRate(_float fStiffnessRate);
	//void					Set_Animation(_uint iAnimIndex) { m_iCurrentAnimIndex = iAnimIndex; }

public:
	_bool					Is_AnimEnd() { return m_bIsAnimEnd; };

public:
	virtual HRESULT			Initialize_Prototype(TYPE eType, const string& strModelFilePath, _fmatrix PivotMatrix);
	virtual HRESULT			Initialize(void* pArg);
	virtual HRESULT			Render(_uint iMeshIndex);

public:
	void					Play_Animation(_float fTimeDelta, _bool bIsLoop);
	void					Play_Animation(_float fTimeDelta, _float3& _Pos);

	void					Stop_Animation(_float fTimeDelta,_bool bIsLoop) { Play_Animation(fTimeDelta, bIsLoop); }
	void					Ctrl_Animation(_float fTimeDelta, _bool bIsLoop) { Play_Animation(fTimeDelta, bIsLoop); }

public:
	HRESULT					Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT					Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType);

public:
	void					Set_Animation(_uint _iAnimationIndex, CModel::ANIM_STATE _eAnimState = CModel::ANIM_STATE::ANIM_STATE_END, _bool _bIsTransition = true, _float _fTransitionDuration = 0.2f, _uint iTargetKeyFrameIndex = 0);
	void					Set_Animation_Transition(_uint _iAnimationIndex, _float _fTransitionDuration = 0.2f, _uint iTargetKeyFrameIndex = 0);
	void					Reset_Animation(_int iAnimIndex = -1);

	_float					Get_TickPerSecond();
	void					Set_TickPerSecond(_float _TickPerSecond);
	_bool					Is_Transition();
	void					Set_UseAnimationPos(_bool _bUseAnimationPos) { m_bUseAnimationPos = _bUseAnimationPos; };
	_bool					Is_Inputable_Front(_uint _iIndexFront);

	void					Write_Names(const string& strModelFilePath);
public:
	vector<CAnimation*>*	 Get_Animations();
	_uint&					 Get_AnimationNum() { return m_iNumAnimations; }

public:
	vector<CBone*>*			Get_Bones();
	/*_uint&					Get_BoneNum() {return }*/
private:
	CMyAssimp				m_MyAssimp;
	CMyAIScene				m_pAIScene;

private:
	_float4x4				m_PivotMatrix;
	TYPE					m_eModelType			= { TYPE_END };

	_uint					m_iNumMeshes			= { 0 };
	vector<CMesh*>			m_Meshes;

	_uint					m_iNumMaterials			= { 0 };
	vector<MATERIAL_DESC>	m_Materials;

	/* 내 모델의 전체 뼈들을 부모관계를 포함하여 저장한다. */
	vector<CBone*>			m_Bones;	
	
	_uint					m_iNumAnimations		= { 0 };
	_uint					m_iCurrentAnimIndex		= { 0 };
	vector<CAnimation*>		m_Animations;

	_bool					m_bIsAnimEnd			= { false };
	ANIM_STATE				m_eAnimState			= { CModel::ANIM_STATE::ANIM_STATE_END };
	_bool					m_bUseAnimationPos		= { false };

public:
	typedef vector<CBone*>	BONES;

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