#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Engine)
END

BEGIN(Client)

class CWeapon_Bandit_Sniper final : public CWeapon
{
protected:
	CWeapon_Bandit_Sniper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CWeapon_Bandit_Sniper(const CWeapon_Bandit_Sniper& rhs);
	virtual ~CWeapon_Bandit_Sniper() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual	HRESULT Render_Shadow() override;
	virtual HRESULT Render_CSM(_uint i) override;
	virtual HRESULT Render_OutLine() override;

public:
	void			Set_RenderOutLine(_bool bActive) { m_bRender_OutLine = bActive; }
	void			Sniping(_float4 vDir, _float3 fPos);

private:
	HRESULT			Load_Json();
	HRESULT			Option_Setting();

protected:
	virtual HRESULT Ready_Components();
	HRESULT			Bind_ShaderResources();
	
private: /* For. Shader */
	_float			m_fCamFar = {};
	_float4			m_vCamPos = {};

	/* RimBloom */
	_float4			m_vRimColor = {};
	_float3			m_vBloomPower = {};
	_float			m_fRimPower = {};

	/* OutLine */
	_bool		m_bRender_OutLine = { false };
	_float4		m_vLineColor = {};
	_float 		m_fLineThick = {};
	_float		m_fLineThick_Ratio = {};
	_float		m_fLineTimeAcc = {};
	_bool		m_bIncrease = { true };

public:
	static CWeapon_Bandit_Sniper* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool();
	virtual void Free() override;
};

END