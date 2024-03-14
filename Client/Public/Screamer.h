#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "UI_Weakness.h"

BEGIN(Engine)
class CShader;
class CCollider;
class CModel;
END

/* 셰이더 테스트용 모델입니다! */

BEGIN(Client)

class CScreamer final : public CGameObject
{
private:
	CScreamer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CScreamer(const CScreamer& rhs);
	virtual ~CScreamer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;
	virtual HRESULT Render_OutLine() override;
	virtual HRESULT Render_Cascade_Shadow(_uint i);

private:
	HRESULT			Ready_Components();
	HRESULT			Bind_ShaderResources();

private:
	_int				m_iRenderPass			= {};
	_float				m_fTimeDelta			= {};
	_float				m_fDissolveWeight		= {};

	_float4				m_vCamPos				= {};
	_float				m_fRimPower				= {};
	_float4				m_vRimColor				= { 1.0f, 1.f, 1.f, 0.3f };
	_float3				m_vBloomPower			= _float3(0.1f, 0.1f, 0.1f);

	/* test */
	_bool				m_bRim					= { false };

private:
	CModel*				m_pModelCom				= { nullptr };
	CShader*			m_pShaderCom			= { nullptr };
	CCollider*			m_pColliderCom			= { nullptr };

	/* Test Texture */
	CTexture*			m_pBreakTextureCom		= { nullptr };
	CTexture*			m_pDissolveTexCom		= { nullptr };

	/* Test UI */
	CUI_Weakness*		m_pWeakneesUI = { nullptr };
	_bool				m_bTestUI = false;
	_float				m_fOffsetX = 0.f, m_fOffsetY = 0.f;

public:
	static CScreamer*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual CGameObject*	Pool();
	virtual void			Free() override;

};

END