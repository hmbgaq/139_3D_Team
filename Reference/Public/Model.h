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
	enum class ADD_TEXTURE_TYPE { TYPE_ORM, TYPE_METALIC, TYPE_ROUGHNESS, TYPE_OPACITY, TYPE_EMISSIVE, TYPE_END };

public:
	enum ANIM_STATE { ANIM_STATE_NORMAL, ANIM_STATE_LOOP, ANIM_STATE_REVERSE, ANIM_STATE_LOOP_REVERSE, ANIM_STATE_STOP, ANIM_STATE_END };

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_bool	Is_Splitted() { return m_bIsSplitted; }
	void	Set_Splitted(_bool _bIsSplitted) { m_bIsSplitted = _bIsSplitted; };

	void	Set_MouseMove(_float2 vMouseMove);

	_int	Get_CurrentAnimIndex() { return m_iCurrentAnimIndex; };


	_uint					Get_NumMeshes() const {return m_iNumMeshes; }
	class CBone*			Get_BonePtr(const _char* pBoneName) const;


	void					Set_StiffnessRate(_float fStiffnessRate);
	void					Set_StiffnessRate_Upper(_float fStiffnessRate);
	_float					Get_StiffnessRate();

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
	void					Calculate_ModelSize(_float* fOutWidth, _float* fOutHeight);

	_float					Get_ModelWidth_WithModel() { return m_fModelWidth; }
	_float					Get_ModelHeight_WithModel() { return m_fModelHeight; }
	//! 맵툴 전용 콜라이더 사이즈 계산 앤드


public:
	_bool					Is_AnimEnd() { return m_bIsAnimEnd; };
	_bool					Is_UpperAnimEnd() { return m_bIsUpperAnimEnd; };
	_bool					Is_Upper_Inputable_Front(_uint _iIndexFront);

	void					Set_AnimEnd(_bool _bIsAnimEnd) { m_bIsAnimEnd = _bIsAnimEnd; };

public:
	virtual HRESULT			Initialize_Prototype(TYPE eType, const string& strModelFilePath, _fmatrix PivotMatrix);
	virtual HRESULT			Initialize(void* pArg);
	virtual HRESULT			Render(_uint iMeshIndex);
	
public:
	void					Play_Animation(_float fTimeDelta, _bool bIsLoop);
	void					Play_Animation(_float fTimeDelta, _float3& _Pos);

	void Play_Animation(_float fTimeDelta, _float3& _Pos, _float3 pPlayerPos);

	void					Stop_Animation(_float fTimeDelta,_bool bIsLoop) { Play_Animation(fTimeDelta, bIsLoop); }
	void					Ctrl_Animation(_float fTimeDelta, _bool bIsLoop) { Play_Animation(fTimeDelta, bIsLoop); }

public:
	HRESULT					Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, _float4x4* BoneMatrices = nullptr);
	HRESULT					Bind_MaterialResource(class CShader* pShader, _uint iMeshIndex, _bool* bORM, _bool* bEmissive);
	HRESULT					Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType);
	HRESULT					Bind_ShaderCascade(CShader* pShader);

public:
	void					Set_Animation(_uint _iAnimationIndex, CModel::ANIM_STATE _eAnimState = CModel::ANIM_STATE::ANIM_STATE_END, _bool _bIsTransition = true, _float _fTransitionDuration = 0.2f, _uint iTargetKeyFrameIndex = 0);
	void					Set_Animation_Transition(_uint _iAnimationIndex, _float _fTransitionDuration = 0.2f, _uint iTargetKeyFrameIndex = 0);
	void					Reset_Animation(_int iAnimIndex = -1);
	void					Set_AnimState(CModel::ANIM_STATE _eAnimState);
	CModel::ANIM_STATE		Get_AnimState() { return m_eAnimState; };


	void					Set_Animation_Upper(_uint _iAnimationIndex, CModel::ANIM_STATE _eAnimState = CModel::ANIM_STATE::ANIM_STATE_END, _float _fTransitionDuration = 0.2f, _uint iTargetKeyFrameIndex = 0);
	void					Reset_UpperAnimation(_int iAnimIndex = -1);

	_float					Get_TickPerSecond();
	void					Set_TickPerSecond(_float _TickPerSecond);
	_bool					Is_Transition();
	void					Set_UseAnimationPos(_bool _bUseAnimationPos) { m_bUseAnimationPos = _bUseAnimationPos; };
	_bool					Is_Inputable_Front(_uint _iIndexFront);
	_bool					Compare_TrackPosition_Is_Over(_float fTrackPosition);
	_float					Get_TrackPosition();
	void					Set_TrackPosition(_int iNewTrackPosition);
	void					Write_Names(const string& strModelFilePath);

	_uint					Get_EndKeyFrameIndex(_uint iIndex);
	_float					Calc_Cooltime_Percent(_uint iAnimIndex);
	_float					Calc_Cooltime_Percent();
	_float					Calc_Upper_Cooltime_Percent();


	void					Set_Speed(_int iSpeed);

public:
	vector<CAnimation*>*	 Get_Animations();
	_uint&					 Get_AnimationNum() { return m_iNumAnimations; }
	
public:
	CMyAIScene*				Get_AIScene();
	TYPE					Get_ModelType() { return m_eModelType; }

public:
	vector<CBone*>*			Get_Bones();
	_uint					Get_BoneNum(const _char* szName);

public:
	_uint					Get_CurrentKeyFrames(_uint iIndex = 0);

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


	// 상 하체 분리
	_bool					m_bIsSplitted			= { false };
	_uint					m_iUpperAnimIndex		= { 0 };
	_bool					m_bIsUpperAnimEnd		= { false };
	ANIM_STATE				m_eUpperAnimState		= { CModel::ANIM_STATE::ANIM_STATE_LOOP };

	_float2					m_vMouseMove			= { 0.f, 0.f };

	/* Cascade */
	vector<_matrix>			m_matCurrTransforms;
	vector<KEYFRAME>		m_CurrKeyFrameDatas;
	vector<KEYFRAME>		m_PrevKeyFrameDatas;

	_float					m_fModelWidth = 0.f;
	_float					m_fModelHeight = 0.f;

	// PBR

public:
	typedef vector<CBone*>	BONES;

private:
	HRESULT	Ready_Meshes(_fmatrix PivotMatrix);	
	HRESULT Ready_Materials(const string& strModelFilePath);
	HRESULT Ready_Bones(CMyAINode pAINode, _int iParentIndex);
	HRESULT Ready_Animations();

public: 
	/* Monster */
	_bool IsModelPicking(RAY _Ray, _matrix _WorldMatrix, _float4* pOut);
	
	/* Shader Tool */
	//void Collect_ModelTag(string FilePath);
	//
	//vector<string> ModelTag = {};
	//_bool m_bFillMode = true;

public: /* Cascade */
	HRESULT SetUp_OnShader(CShader* pShader, _uint iMaterialIndex, aiTextureType eTextureType, const char* strConstantName);
	HRESULT Render(CShader*& pShader, const _uint& iMeshIndex, const _uint& strPassName);

	/* ORM Texture */
	CTexture*	Add_NotIncludedTexture(ADD_TEXTURE_TYPE eType, const char* strOriginFileName, const char* strOriginDrive, const char* strOriginDirectory, const char* strOriginExt, _int iCnt = 1);
	void		Update_EmissiveTextures(const string& fileName, MATERIAL_DESC& materialDesc, const char* szFileName, const char* szDrive, const char* szDirectory, const char* szEXT);
	_char		szTempDiffuseFileName[MAX_PATH] = {};
	_char		szTempNormalFileName[MAX_PATH] = {};

	_bool					m_bDiffuseExist = { false };
	_bool					m_bSpecularExist = { false };
	_bool					m_bEmissiveExist = { false };


public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const string& strModelFilePath, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END