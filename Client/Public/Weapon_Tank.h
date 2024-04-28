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
	virtual void	OnCollisionEnter(CCollider* other)	override;
	virtual void	OnCollisionStay(CCollider* other)	override;
	virtual void	OnCollisionExit(CCollider* other)	override;


public:
	//void	Set_Animation(
	//	_uint _iNextAnimation
	//	, CModel::ANIM_STATE _eAnimState = CModel::ANIM_STATE::ANIM_STATE_NORMAL
	//	, _uint iTargetKeyFrameIndex = 0);
	//_bool	Is_Animation_End();
	//CModel::ANIM_STATE Get_AnimState();

public:
	virtual void	Set_Enable(_bool _Enable) override;

private:
	HRESULT			Load_Json();
	HRESULT			Option_Setting();

protected:
	virtual HRESULT Ready_Components();
	HRESULT			Bind_ShaderResources();

	/* 소영 추가 */
private:
	CTexture* m_pDissolveTexture = { nullptr };

private:
	_bool				m_bDissolve = { false };
	_bool				m_bRimLight = { false };

	/* Dissolve */
	_float				m_fDissolveWeight = 0.f;
	_float				m_fDissolve_feather = 0.f;
	_float3				m_vDissolve_Color = { 0.f, 0.f, 0.f };
	_float				m_fDissolve_Discard = 0.f;
	_float				m_fTimeAcc = 0.f;

	/* RimLight */
	_float4				m_vRimColor = { 0.f, 0.f, 0.f, 0.f };   /* RimLight */
	_float				m_fRimPower = 5.f;                      /* RimLight */
	_float3				m_vBloomPower = {};

public:
	static CWeapon_Tank* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool();
	virtual void Free() override;
};

END