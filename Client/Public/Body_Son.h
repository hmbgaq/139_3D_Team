#pragma once

#include "Client_Defines.h"
#include "Body.h"

BEGIN(Engine)
END

BEGIN(Client)

class CBody_Son final : public CBody
{
public:
	enum class RENDER_STATE { ORIGIN, ATTACK, HITTED, NAKED, RENDER_STATE_END };

private:
	CBody_Son(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CBody_Son(const CBody_Son& rhs);
	virtual ~CBody_Son() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

public:
	virtual void	OnCollisionEnter(CCollider* other)	override;
	virtual void	OnCollisionStay(CCollider* other)	override;
	virtual void	OnCollisionExit(CCollider* other)	override;

public:
	void SetUp_Animation(_uint iAnimIndex);
public: /* For. RenderState */
	void	Set_RenderState(RENDER_STATE _state) { m_eRender_State = _state; }
private:
	map<CBody_Son::RENDER_STATE, vector<_int>> m_vDiscardMesh = {};
	RENDER_STATE		m_eRender_State = RENDER_STATE::RENDER_STATE_END;

	_float				m_gCamFar = {};
	_float4				m_vCamPos = {};
	_float				m_fRimPower = {};
	_float4				m_vRimColor = {};
	_float3				m_vBloomPower = {};

	_int			iDiscardMeshNumber = 0;
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	virtual void Check_Frustum() override;



public:
	/* 원형객체를 생성한다. */
	static CBody_Son* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;

};

END
