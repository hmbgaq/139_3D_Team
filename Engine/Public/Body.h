#pragma once

#include "GameObject.h"
#include "Model.h"

BEGIN(Engine)

class CPhysXCollider;
class CCharacter;
class CCollider;
class CTexture;
class CShader;
class CBone;


class ENGINE_DLL CBody abstract : public CGameObject
{
public:
	typedef struct tagBodyDesc 
	{
		class CTransform* m_pParentTransform = { nullptr };

	}BODY_DESC;
protected:
	CBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CBody(const CBody& rhs);
	virtual ~CBody() = default;

public:
	class CBone* Get_BonePtr(const _char * pBoneName);
	CCollider* Get_Collider() { return m_pColliderCom; }
	void	Collider_Off();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

public:
	_int Get_CurrentAnimIndex();
	void Set_Animation(
		_uint _iNextAnimation
		, CModel::ANIM_STATE _eAnimState = CModel::ANIM_STATE::ANIM_STATE_NORMAL
		, _bool _bIsTransition = true
		, _bool _bUseAnimationPos = true
		, _uint iTargetKeyFrameIndex = 0
	);
	void	Set_AnimState(CModel::ANIM_STATE _eAnimState);

	_bool	Is_Animation_End();
	_bool	Is_UpperAnimation_End();

	_bool	Is_Inputable_Front(_uint _iIndexFront);
	_float	Get_TrackPosition();
	_bool	Compare_TrackPosition_Is_Over(_float fTrackPosition);

	_float3 Get_MovePos() { return m_vMovePos; }
	void Set_TrackPosition(_int iNewTrackPosition);
	void Set_StiffnessRate(_float fStiffnessRate) { m_pModelCom->Set_StiffnessRate(fStiffnessRate); }
	_float Get_StiffnessRate() { return m_pModelCom->Get_StiffnessRate(); }

	void Set_StiffnessRate_Upper(_float fStiffnessRate) {
		m_pModelCom->Set_StiffnessRate_Upper(fStiffnessRate);
	}

public:
	virtual void	OnCollisionEnter(CCollider* other)	override;
	virtual void	OnCollisionStay(CCollider* other)	override;
	virtual void	OnCollisionExit(CCollider* other)	override;


public:
	void Set_MouseMove(_float fTimeDelta, _bool bIsUseMouseMove = true);

public:
	CCharacter* Get_Target_Character(CCollider* other);


public:
	CModel* Get_Model() { return m_pModelCom; }
	

#ifdef _DEBUG
public: //!For.Tool
	virtual _bool Picking(_Out_ _float3* vPickedPos) override;
#endif 


public:	//!For Animation Split
	void Set_Animation_Upper(_uint _iAnimationIndex, CModel::ANIM_STATE _eAnimState = CModel::ANIM_STATE::ANIM_STATE_END, _uint iTargetKeyFrameIndex = 0);
	_bool Is_Splitted() { return m_pModelCom->Is_Splitted(); }
	void Set_Splitted(_bool _bIsSplitted) { m_pModelCom->Set_Splitted(_bIsSplitted); };

	void Set_RotateUpperX(MoveDirection eDirection);

	void Activate_ShootingReaction(_float fHeight = 20.f);
	void Update_ShootingReaction(_float fTimeDelta);

	void Reset_UpperAngle();
	void Set_UseMouseMove(_bool _bIsUseMouseMove) { m_bIsUseMouseMove = _bIsUseMouseMove; };

public:
	_uint Get_CurrentKeyFrames(_uint iIndex = 0);



protected:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	//CTexture* m_pDissolveTexture = { nullptr };

protected:
	class CTransform* m_pParentTransform = { nullptr };
	_float4x4	m_WorldMatrix = {};
	_float3		m_vMovePos = { 0.f, 0.f, 0.f };


	_float		m_fRotateUpperX = { 0.0f };
	_float		m_fRotateUpperY = { 2.6f };

	_float		m_fShootingReaction = { 0.f };
	_float		m_fShootingReactionTarget = { 0.f };


	//_bool		m_bDissolve = { false };
	//_float		m_fDissolveWeight = { 0.f };
	_uint		m_iShaderPass = 0;

protected:
	_bool	m_bIsUseMouseMove = { false };


//protected:
//	_float x = { 1.f };
//	_float y = { 4.8f };



protected:
	CPhysXCollider* m_pPhysXCollider = { nullptr };

protected:
	virtual HRESULT Ready_Components() PURE;
	virtual HRESULT Bind_ShaderResources();


public:
	virtual CGameObject* Clone(void* pArg) PURE;

	virtual void Free() override;
};

END