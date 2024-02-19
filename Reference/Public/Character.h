#pragma once

#include "GameObject.h"
#include "Model.h"

#include "Body.h"
#include "Weapon.h"

#include "Engine_Enum.h"

BEGIN(Engine)

class CNavigation;


class ENGINE_DLL CCharacter abstract : public CGameObject
{
protected:
	CCharacter(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
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
	CBody* Get_Body();
	CWeapon* Get_Weapon(const wstring & strWeaponTag);
	CCollider* Get_Collider();
	CNavigation* Get_Navigation();

	virtual _bool Picking(_Out_ _float3 * vPickedPos) override;
public:
	virtual HRESULT Ready_Components() PURE;
	virtual HRESULT Ready_PartObjects() PURE;

	CGameObject* Find_PartObject(const wstring & strPartTag);
	HRESULT Add_PartObject(const wstring & strPrototypeTag, const wstring & strPartTag, void* pArg);
	HRESULT Add_Body(const wstring & strPrototypeTag, CBody::BODY_DESC pArg);
	HRESULT Add_Weapon(const wstring & strPrototypeTag, string strBoneName, CWeapon::WEAPON_DESC pArg, const wstring & strWeaponTag = TEXT("Part_Weapon"));

public:
	void	Set_Animation(
		_uint _iNextAnimation
		, CModel::ANIM_STATE _eAnimState = CModel::ANIM_STATE::ANIM_STATE_NORMAL
		, _bool _bIsTransition = true
		, _bool _bUseAnimationPos = true
		, _uint iTargetKeyFrameIndex = 0);

	_bool	Is_Animation_End();
	_bool	Is_Inputable_Front(_uint _iIndexFront);

	void Go_Straight(_float fTimeDelta);	
	void Go_Straight_L45(_float fTimeDelta);
	void Go_Straight_R45(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);	
	void Go_Backward_L45(_float fTimeDelta);
	void Go_Backward_R45(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);		
	void Go_Right(_float fTimeDelta);		


	_bool Is_Rotate_In_CameraDir() {
		return m_bRotate_In_CameraDir;
	}
	void Set_Rotate_In_CameraDir(_bool _bRotate_In_CameraDir) {
		m_bRotate_In_CameraDir = _bRotate_In_CameraDir;
	}

public:
	_int Get_Hp() {
		return m_iHp;
	};

	void Set_Hp(_uint _iHp) {
		m_iHp = _iHp;
	};

protected:
	_int m_iHp = { 0 };
	_bool	m_bRotate_In_CameraDir = { false };

protected:
	CNavigation* m_pNavigationCom = { nullptr };
	CBody* m_pBody = { nullptr };
	vector<CWeapon*> m_Weapons;

protected:
	map<const wstring, class CGameObject*>		m_PartObjects;

protected:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END