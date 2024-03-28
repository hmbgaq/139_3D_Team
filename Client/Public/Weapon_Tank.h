#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Engine)

class CModel;

END

BEGIN(Client)

class CWeapon_Tank final : public CWeapon
{
protected:
	CWeapon_Tank(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CWeapon_Tank(const CWeapon_Tank& rhs);
	virtual ~CWeapon_Tank() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual	HRESULT Render_Shadow() override { return S_OK; };

public:
	void	Set_Animation(
		_uint _iNextAnimation
		, CModel::ANIM_STATE _eAnimState = CModel::ANIM_STATE::ANIM_STATE_NORMAL
		, _uint iTargetKeyFrameIndex = 0);
	_bool	Is_Animation_End();
	CModel::ANIM_STATE Get_AnimState();

public:
	virtual void	Set_Enable(_bool _Enable) override;

private:
	HRESULT			Load_Json();
	HRESULT			Option_Setting();

protected:
	virtual HRESULT Ready_Components();
	HRESULT			Bind_ShaderResources();

public:
	static CWeapon_Tank* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool();
	virtual void Free() override;
};

END