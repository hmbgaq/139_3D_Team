#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "UI_Weakness.h"

BEGIN(Engine)
class CShader;
class CCollider;
class CModel;
END


BEGIN(Client)

/* 셰이더 테스트용 모델입니다! */
class CShaderParsed_Object final : public CGameObject
{
public:
	typedef struct tagDummyDesc
	{
		string strModelProtoTag = {};
		string strShaderProtoTag = {};
	}CREATE_DESC;
private:
	CShaderParsed_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CShaderParsed_Object(const CShaderParsed_Object& rhs);
	virtual ~CShaderParsed_Object() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;
	virtual HRESULT Render_OutLine() override;

private:
	HRESULT			Ready_Components(void* pArg);
	HRESULT			Bind_ShaderResources();

private:
	_int			m_iRenderPass = {};
	_float			m_fTimeDelta = {};
	_float			m_fDissolveWeight = {};

	_float4			m_vCamPos = {};
	_float			m_fRimPower = {};
	_float4			m_vRimColor = { 1.0f, 1.f, 1.f, 0.3f };
	_float3			m_vBloomPower = _float3(0.1f, 0.1f, 0.1f);

private:
	CModel*		m_pModelCom = { nullptr };
	CShader*	m_pShaderCom = { nullptr };
	CCollider*	m_pColliderCom = { nullptr };

public:
	static CShaderParsed_Object*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual CGameObject*			Pool();
	virtual void					Free() override;

};

END