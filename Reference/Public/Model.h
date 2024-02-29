#pragma once

#include "Component.h"

#include "MyAssimp.h"
#include "MyAIScene.h"

BEGIN(Engine)
class CAnimation;
class CMesh;
class CBone;
class CShader;


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
	
	class CBone*			Get_BonePtr(const _char* pBoneName) const;

	void					Set_StiffnessRate(_float fStiffnessRate);
	//void					Set_Animation(_uint iAnimIndex) { m_iCurrentAnimIndex = iAnimIndex; }

	_matrix					Get_PivotMatrix() { return m_PivotMatrix; }
	_matrix					Get_CombinedMatrix(_uint iBoneIndex);


public:						
	//! 모델 인스턴싱 추가
	_uint					Get_NumMaterials() const { return m_iNumMaterials; }
	_uint					Get_MaterialIndex(_uint iMeshIndex);
	_uint					Get_NumMeshIndice(_int iMeshIndex);//! 모델 인스턴싱 전용
	vector<class CMesh*>&	Get_Meshes() { return m_Meshes; }
	class CMesh*			Get_Mesh_For_Index(_int iMeshIndex);


	CAnimation*				Get_Animation_For_Index(_uint iAnimIndex);
	_float4x4*				Get_Combined_For_AnimationIndex(_uint iAnimationIndex, _float fTrackPosition);

	_float4x4*				Calc_OffsetMatrice(_uint iAnimationIndex, _float fTrackPosition, _float4x4* pMatrix);
	_float4x4*				Get_OffsetMatrices();

	//! 모델 인스턴싱 앤드
	
	//! 맵툴 전용 콜라이더 사이즈 계산 추가
	_float3&				Calculate_AABB_Extents_From_Model(); //! 모델 사이즈에 맞게 AABB 사이즈 잡아줌. 
	void					Calculate_Sphere_Radius(_float3* vOutCenter, _float* fOutRadius); //! 모델 사이즈게 맞게 SPHERE 사이즈잡아줌.  
	//! 맵툴 전용 콜라이더 사이즈 계산 앤드


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
	HRESULT					Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, _float4x4* BoneMatrices = nullptr);
	HRESULT					Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType);
	HRESULT					Bind_ShaderCascade(CShader* pShader);

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
	CMyAIScene*				Get_AIScene();

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

	/* Cascade */
	vector<_matrix>			m_matCurrTransforms;
	vector<KEYFRAME>		m_CurrKeyFrameDatas;
	vector<KEYFRAME>		m_PrevKeyFrameDatas;

public:
	typedef vector<CBone*>	BONES;

private:
	HRESULT	Ready_Meshes(_fmatrix PivotMatrix);	
	HRESULT Ready_Materials(const string& strModelFilePath);
	HRESULT Ready_Bones(CMyAINode pAINode, _int iParentIndex);
	HRESULT Ready_Animations();

	/* Cascade */
public:
	HRESULT SetUp_OnShader(CShader* pShader, _uint iMaterialIndex, aiTextureType eTextureType, const char* strConstantName);
	HRESULT Render(CShader*& pShader, const _uint& iMeshIndex, const _uint& strPassName);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const string& strModelFilePath, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END