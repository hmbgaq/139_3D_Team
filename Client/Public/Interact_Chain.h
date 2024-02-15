#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)

/* Gara클래스로 임시사용 용도임 */

class CInteract_Chain  final : public CGameObject
{
private:
	CInteract_Chain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteract_Chain(const CInteract_Chain& rhs);
	virtual ~CInteract_Chain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_OutLine() override;

private:
	CShader*	m_pShaderCom = { nullptr };
	CModel*		m_pModelCom = { nullptr };
	CTexture*	m_pMaskTextureCom = { nullptr };

	_int		iRenderPass = 0;
	_bool		m_bInteractActive = { false };

	_float4		m_vLineColor = { 1.f, 1.f, 1.f, 1.f };
	_float		m_fLineThick = 0;
	_float		m_fTimeAcc = 0;
	_bool		m_bIncrease = true;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CInteract_Chain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

