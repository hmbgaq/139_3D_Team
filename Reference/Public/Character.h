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
	virtual void Set_Enable(_bool _Enable) override;

public:
	virtual Hit_Type Set_Hitted(_uint iDamage, _vector vDir, _float fForce, _float fStiffnessRate, Direction eHitDirection, Power eHitPower);

	virtual void Hitted_Left(Power ePower) {};
	virtual void Hitted_Right(Power ePower) {};
	virtual void Hitted_Front(Power ePower) {};
	virtual void Hitted_Knock(_bool bIsCannonball = false) {};
	virtual void Hitted_Dead(Power ePower) {};

public:
	void Add_Force(_vector In_vDir, _float In_fPower);

public:
	_int Get_Hp() { return m_iHp; };
	void Set_Hp(_uint _iHp) { m_iHp = _iHp; };

public:
	CCharacter* Get_Target() { return m_pTarget; };
	void Set_Target(CCharacter* pTarget) { m_pTarget = pTarget; };

	void Get_Damaged(_uint iDamage) {m_iHp -= iDamage;}
public:
	void Look_At_Target();
	void Look_At_Target_Lerp(_float fTimeDelta);
	void Search_Target(const wstring& strLayerTag, const _float fSearchDistance = MAX_SEARCH);

	_float Target_Contained_Angle(_float4 vStandard, _float4 vTargetPos); /* 내 Look과 타겟을 향하는 벡터 사이의 끼인각을 구하는함수 */
	_bool Lerp_ToOrigin_Look(_float4 vOriginLook, _float fSpeed, _float fTimeDelta);
	CCharacter* Select_The_Nearest_Enemy(const wstring& strLayerTag, _float fMaxDistance = MAX_SEARCH);

public:
	_float Calc_Distance(_float3 vTargetPos);
	_float Calc_Distance(CGameObject* pTarget);
	_float Calc_Distance();
	_float Calc_The_Nearest_Enemy_Distance(const wstring& strLayerTag);

	void Move_In_Proportion_To_Enemy(_float fTimeDelta, _float fSpeedCap = 1.0f);



public:	//!For Animation Split
	void Set_Animation_Upper(_uint _iAnimationIndex, CModel::ANIM_STATE _eAnimState = CModel::ANIM_STATE::ANIM_STATE_END, _uint iTargetKeyFrameIndex = 0);
	_bool Is_Splitted() { return m_pBody->Is_Splitted(); }
	void Set_Splitted(_bool _bIsSplitted) { m_pBody->Set_Splitted(_bIsSplitted); };

public:
	void Set_StiffnessRate(_float fStiffnessRate);
	void Set_StiffnessRate_Upper(_float fStiffnessRate);

public:
	void Set_Weapons_Enable_False();
	CWeapon* Set_Weapon_Enable(const wstring& strWeaponTag, _bool bActivate);
	CWeapon* Set_Weapon_Collisions_Enable(const wstring& strWeaponTag, _bool bActivate);


public:
	_float3 Get_WeaknessPoint() { return m_vWeaknessPoint; };
	virtual void Set_WeaknessPoint();
public:
#pragma region ===========> HP <=========== 
	//void	Set_CurHP(_float fCurHP) { m_fCurHP = fCurHP; }
	//_float	Get_CurHP() { return m_fCurHP; }
	//void	Set_MaxHP(_float fMaxHP) { m_fMaxHP = fMaxHP; }
	//_float	Get_MaxHP() { return m_fMaxHP; }
	void	Set_CurHP(_float fCurHP) { m_iHp = fCurHP; }
	_int	Get_CurHP() { return m_iHp; }
	void	Set_MaxHP(_float fMaxHP) { m_iMaxHp = fMaxHP; }
	_int	Get_MaxHP() { return m_iMaxHp; }
protected:
	_int m_iHp = { 1 };
	_int m_iMaxHp = { 1 };
	
	/* _float 타입의 HP를 사용해주세요. */
	//_float m_fMaxHP = { 40.f };
	//_float m_fCurHP = { 40.f };


protected:
	//Power m_eStrength = { Power::Light };
	_float m_fStiffnessRate = { 1.f };


public:
	_float m_fCurrentTrackPosition = {0.f};

protected:
	CNavigation* m_pNavigationCom = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };
	CBody* m_pBody = { nullptr };
	vector<CWeapon*> m_Weapons;
	CHARCTER_DESC CharAnimDesc = {};

protected:
	CCharacter* m_pTarget = { nullptr };
	_float3		m_vWeaknessPoint = { 0.f, 0.f, 0.f };
	_float3		m_vWeaknessPoint_Local = { 0.f, 1.f, 0.f };


protected:
	CPhysXCharacterController* m_pPhysXControllerCom = { nullptr };
	PxControllerCollisionFlags m_LastCollisionFlags;

protected:
	map<const wstring, class CGameObject*>		m_PartObjects;

public:
	_bool		m_bLookAt = true;
	_bool		m_bTrigger = false;
protected:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual CGameObject* Pool() PURE;
	virtual void Free() override;
};

END