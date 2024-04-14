#pragma once

#include "Client_Defines.h"
#include "Body_Monster.h"

BEGIN(Engine)
END

BEGIN(Client)

class CBody_Infected abstract : public CBody_Monster
{
public:
	enum class RENDER_STATE { ORIGIN, ATTACK, HITTED, NAKED, RENDER_STATE_END };

protected:
	CBody_Infected(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CBody_Infected(const CBody_Infected& rhs);
	virtual ~CBody_Infected() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

public: /* For. RenderState */
	void	Set_RenderState(RENDER_STATE _state) { m_eRender_State = _state; }
	HRESULT Set_StateHit();
	HRESULT Set_StateDead();
	void Set_Dissolve(_bool bDissolve) { m_bDissolve = bDissolve; }

protected:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

protected:
	map<CBody_Infected::RENDER_STATE, vector<_int>> m_vDiscardMesh = {};
	RENDER_STATE		m_eRender_State = RENDER_STATE::RENDER_STATE_END;

	_float				m_gCamFar		= {};
	_float4				m_vCamPos		= {};
	_float				m_fRimPower		= {};
	_float4				m_vRimColor		= {};
	_float3				m_vBloomPower	= {};

	CTexture*			m_pDissolveTexture = { nullptr };
	_bool				m_bDissolve = { false };

	/* Dissolve */
	_float				m_fDissolveWeight = 0.f;
	_float				m_fDissolve_feather = 0.f;
	_float3				m_vDissolve_Color = { 0.f, 0.f, 0.f };
	_float				m_fDissolve_Discard = 0.f;
	_float				m_fTimeAcc = 0.f;

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual CGameObject*	Pool() PURE;
	virtual void			Free() override;

};

END