#pragma once

#include "GameObject.h"
#include "Model.h"

#include "Body.h"
#include "Weapon.h"

#include "Engine_Enum.h"

#define MAX_SEARCH 20.f

BEGIN(Engine)

class CNavigation;
class CRigidBody;

class CPhysXCharacterController;

class ENGINE_DLL CCharacter abstract : public CGameObject
{
public:
	typedef struct tagGameCharacterDesc
	{
		string	Body = "";
		string	BoneName = "";

		_float  AnimationIndex = { 0.f };
		_float	Duration = { 0.f };
		_float  EventTrackposition = {0.f};
		_float  AnimationSpeed = { 0.f };
		_float	BoneIndex = { 0.f };

		_float	ColliderSize = { 0.f };
		_float	ColliderTrackPositionOn = { 0.f };
		_float	ColliderTrackPositionOff = { 0.f };
		//Weapon
		_float	EventWeaponTrackPosition = { 0.f };
		_float	ColliderWeaponSize = { 0.f };
		_float	ColliderWeaponTrackPositionOn = { 0.f };
		_float	ColliderWeaponTrackPositionOff = { 0.f };

		_float3 GuizmoTranslatrion = {};
		_float3 GuizmoRotation = {};
		_float3 GuizmoScale = {};
		//Effect
		string	EffectFileName = "";
		_float	EffectOnTrackPosition = { 0.f };
		_float3	EffectPosition = {};

		//Sound
	}CHARCTER_DESC;
protected:
	CCharacter(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & strPrototypeTag);
	CCharacter(const CCharacter& rhs);
	virtual ~CCharacter() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	CBody*			Get_Body();
	CWeapon*		Get_Weapon(const wstring & strWeaponTag);
	CCollider*		Get_Collider();
	CNavigation*	Get_Navigation();

	virtual _bool Picking(_Out_ _float3 * vPickedPos) override;
public:
	virtual HRESULT Ready_Components() PURE;
	virtual HRESULT Ready_PartObjects() PURE;

	CGameObject* Find_PartObject(const wstring & strPartTag);
	HRESULT Add_PartObject(const wstring & strPrototypeTag, const wstring & strPartTag, void* pArg);
	HRESULT Add_Body(const wstring & strPrototypeTag, CBody::BODY_DESC pArg);
	HRESULT Add_Weapon(const wstring & strPrototypeTag, string strBoneName, CWeapon::WEAPON_DESC pArg, const wstring & strWeaponTag = TEXT("Part_Weapon"));

public:
	void	Set_EventNotify(string strPath, string JsonFileName);
	HRESULT	LoadAnimJson(string strPath, string strFileName);
public:
	_int Get_CurrentAnimIndex();
	void	Set_Animation(
		_uint _iNextAnimation
		, CModel::ANIM_STATE _eAnimState = CModel::ANIM_STATE::ANIM_STATE_NORMAL
		, _bool _bIsTransition = true
		, _bool _bUseAnimationPos = true
		, _uint iTargetKeyFrameIndex = 0);

	void	Set_AnimState(CModel::ANIM_STATE _eAnimState);

	_bool	Is_Animation_End();
	_bool	Is_UpperAnimation_End();

	_bool	Is_Inputable_Front(_uint _iIndexFront);
	_float	Get_TrackPosition();
	_bool	Compare_TrackPosition_Is_Over(_float fTrackPosition);

	void	Set_TrackPosition(_int iNewTrackPostion);
	CHARCTER_DESC Get_CharcterDesc() { return CharAnimDesc; }

	void Go_Straight(_float fTimeDelta);
	void Go_Straight_L45(_float fTimeDelta);
	void Go_Straight_R45(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);
	void Go_Backward_L45(_float fTimeDelta);
	void Go_Backward_R45(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);

public:
	_bool Is_Use_Gravity();
	void Set_UseGravity(_bool _bUseGravity);

public:
	virtual void Set_Enable(_bool _Enable) override;

public:
	void Knockback(_vector vDir, _float fForce = 0.3f);
	void Look_At_And_Knockback(_float3 vTargetPos, _float fForce = 0.3f);


public:
	virtual Hit_Type Set_Hitted(_float iDamage, _vector vDir, _float fForce, _float fStiffnessRate, Direction eHitDirection, Power eHitPower, _bool bIsMelee = false, _bool bKnockUp = false);

	virtual void Hitted_Left(Power ePower) {};
	virtual void Hitted_Right(Power ePower) {};
	virtual void Hitted_Front(Power ePower) {};
	virtual void Hitted_Knock(_bool bIsCannonball = false) {};
	virtual void Hitted_Dead(Power ePower) {};
	virtual void Hitted_Stun(Power ePower) { 
		//Set_Invincible(true);
		Hitted_Dead(ePower); 
	};
	virtual void Hitted_Finish() {
		//Set_Invincible(true);
		Hitted_Dead(Power::Heavy);
	};
	virtual void Hitted_Weakness() {};
	virtual void Hitted_Electrocute() {};
	virtual void Hitted_OpenState_Pull() {};
	virtual void Hitted_Opened(Direction eDirection) {};
	virtual void Hitted_KnockUp() {
		Hitted_Front(Power::Heavy);
	};

	
public:
	void Add_Force(_vector In_vDir, _float In_fPower);

public:
	_float Get_Hp() { return m_iHp; };
	void Set_Hp(_float _iHp) { m_iHp = _iHp; };

public:
	CCharacter* Get_Target() { return m_pTarget; };
	void Set_Target(CCharacter* pTarget) { m_pTarget = pTarget; };

	void Get_Damaged(_float iDamage) {m_iHp -= iDamage;}
public:
	void Look_At_OnLand(_fvector vTargetPos);

	void Look_At_Target();
	void Look_At_TargetBoss();
	void Look_At_Target_Lerp(_float fTimeDelta);
	void Search_Target(const wstring& strLayerTag, const _float fSearchDistance = MAX_SEARCH);

	_float Target_Contained_Angle(_float4 vStandard, _float4 vTargetPos); /* 내 Look과 타겟을 향하는 벡터 사이의 끼인각을 구하는함수 */
	_bool Lerp_ToOrigin_Look(_float4 vOriginLook, _float fSpeed, _float fTimeDelta);
	CCharacter* Select_The_Nearest_Enemy(const wstring& strLayerTag, _float fMaxDistance = MAX_SEARCH);

public:
	_float Calc_Distance(_float3 vTargetPos);
	_float Calc_Distance(CGameObject* pTarget);
	_float Calc_Distance();

	_float Calc_Distance_Front(_float3 vTargetPos);
	_float Calc_The_Nearest_Enemy_Distance(const wstring& strLayerTag);

	void Move_In_Proportion_To_Enemy(_float fTimeDelta, _float fSpeedCap = 1.0f);
	void Dragged(_float fTimeDelta, _float3 vTargetPos);

	_float3 Calc_Front_Pos(_float3 vDiff = _float3(0.f, 0.f, 1.f));

public:
	_bool Is_In_Frustum() { return m_bIsInFrustum; }



public:	//!For Animation Split
	void Set_Animation_Upper(_uint _iAnimationIndex, CModel::ANIM_STATE _eAnimState = CModel::ANIM_STATE::ANIM_STATE_END, _uint iTargetKeyFrameIndex = 0);
	_bool Is_Splitted() { return m_pBody->Is_Splitted(); }
	void Set_Splitted(_bool _bIsSplitted) { m_pBody->Set_Splitted(_bIsSplitted); };
	void Reset_UpperAngle();


public:
	void Set_StiffnessRate(_float fStiffnessRate);
	void Set_StiffnessRate_Upper(_float fStiffnessRate);
	_float Get_StiffnessRate() { return m_pBody->Get_StiffnessRate(); }

public:
	void Set_Weapons_Enable_False();
	CWeapon* Set_Weapon_Enable(const wstring& strWeaponTag, _bool bActivate);
	CWeapon* Set_Weapon_Collisions_Enable(const wstring& strWeaponTag, _bool bActivate);


public:
	_float3 Get_WeaknessPos() { return m_vWeaknessPos; };
	virtual void Set_WeaknessPos();

public:
	_uint Get_CurrentKeyFrames(_uint iIndex = 0);

public:
	void Set_UseMouseMove(_bool _bIsUseMouseMove);


public:
#pragma region ===========> HP <=========== 
	//void	Set_CurHP(_float fCurHP) { m_fCurHP = fCurHP; }
	//_float	Get_CurHP() { return m_fCurHP; }
	//void	Set_MaxHP(_float fMaxHP) { m_fMaxHP = fMaxHP; }
	//_float	Get_MaxHP() { return m_fMaxHP; }
	void	Set_CurHP(_float fCurHP) { m_iHp = fCurHP; }
	_float	Get_CurHP() { return m_iHp; }
	void	Set_MaxHP(_float fMaxHP) { m_iMaxHp = fMaxHP; }
	_float	Get_MaxHP() { return m_iMaxHp; }


public:
	_bool Is_Invincible() { return m_bIsInvincible; };
	void Set_Invincible(_bool _bIsInvincible) { m_bIsInvincible = _bIsInvincible; };

public:
	_bool Is_Revealed_Weakness() { return m_bIsRevealedWeakness; };
	void Set_Reveal_Weakness(_bool _bIsRevealedWeakness) { m_bIsRevealedWeakness = _bIsRevealedWeakness; };
	void Set_WeaknessCount(_int _iWeaknessCount) { m_iWeaknessCount = _iWeaknessCount; }


public:
	_bool Is_Stun() { return m_bIsInvincible; };
	void Set_Stun(_bool _bIsStun) { m_bIsStun = _bIsStun; };
	
	_bool Is_ElectrocuteTime() { return m_fElectrocuteTime > 0.f; };
	void Set_ElectrocuteTime(_float _fElectrocuteTime) { m_fElectrocuteTime = max(m_fElectrocuteTime, _fElectrocuteTime); };
	void Update_ElectrocuteTime(_float fTimeDelta) {
		_float fResult = m_fElectrocuteTime - fTimeDelta;
		m_fElectrocuteTime = fResult > 0.f ? fResult : 0.f;
	};

public:
	void Set_RootMoveRate(_float3 vRate) { m_vRootMoveRate = vRate; };
	void Reset_RootMoveRate() { m_vRootMoveRate = _float3(1.f, 1.f, 1.f); };
	void Set_MonsterAttackState(_bool bState) { m_bMonsterAttackState = bState; };

public:
	_bool Is_KnockUp() { return m_bIsKnockUp; };
	void Set_KnockUp(_bool _bIsKnockUp) { m_bIsKnockUp = _bIsKnockUp; };

protected:
	CHARCTER_DESC CharAnimDesc = {};

protected:
	_float m_iHp = { 1.f };
	_float m_iMaxHp = { 1.f };
	
	/* _float 타입의 HP를 사용해주세요. */
	//_float m_fMaxHP = { 40.f };
	//_float m_fCurHP = { 40.f };

protected:
	_bool m_bMonsterAttackState = { false }; /* 몬스터 공격상태이면 */
	_bool m_bIsInvincible = { false };
	_bool m_bIsStun = { false };
	_float m_fElectrocuteTime = { 0.f };



protected:
	//Power m_eStrength = { Power::Light };
	_float m_fStiffnessRate = { 1.f };


protected:
	CNavigation* m_pNavigationCom = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };
	CBody* m_pBody = { nullptr };
	vector<CWeapon*> m_Weapons;

protected:
	CCharacter* m_pTarget = { nullptr };
	_bool m_bIsInFrustum = { false };

protected:
	_float3		m_vWeaknessPos = { 0.f, 0.f, 0.f };
	_float3		m_vWeaknessPos_Local = { 0.f, 1.f, 0.f };
	_bool		m_bIsRevealedWeakness = { false };
	_int		m_iWeaknessCount = { 3 };

protected:
	_bool		m_bIsKnockUp = { false };

protected:
	_float3		m_vRootMoveRate = { 1.f, 1.f, 1.f };


protected:
	CPhysXCharacterController* m_pPhysXControllerCom = { nullptr };
	PxControllerCollisionFlags m_LastCollisionFlags;

protected:
	map<const wstring, class CGameObject*>		m_PartObjects;

public:
	_bool		m_bLookAt = true;

protected:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual CGameObject* Pool() PURE;
	virtual void Free() override;
};

END