#pragma once

#include "Client_Defines.h"
#include "Body.h"

BEGIN(Engine)
END

BEGIN(Client)

class CBody_Player final : public CBody
{
public:
	enum class RENDER_PASS { RENDER_ORIGIN, RENDER_HEAL, RENDER_SUPERCHARGE, RENDER_SNOWMOUNTAIN, RENDER_END };

private:
	CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CBody_Player(const CBody_Player& rhs);
	virtual ~CBody_Player() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;
	virtual HRESULT Render_OutLine() override;
	virtual HRESULT Render_CSM(_uint i)  override;

public:
	virtual void	OnCollisionEnter(CCollider* other)	override;
	virtual void	OnCollisionStay(CCollider* other)	override;
	virtual void	OnCollisionExit(CCollider* other)	override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	virtual void Check_Frustum() override;
	HRESULT Ready_ShaderOption();
	_int iTemp = 0;

	/* 소영 추가 - 렌더링용 */
private:
	_float4 m_vLineColor = {0.f, 0.f, 0.f, 0.f};
	_float m_fLineThick = { 0.f };
	_float m_fLineTimeAcc = { 0.f };

public:
	void Set_PlayerRender(RENDER_PASS eRender) { m_ePlayerRenderPass = eRender; }
	
private:
	RENDER_PASS m_ePlayerRenderPass = RENDER_PASS::RENDER_END;


public:
	static CBody_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;
	virtual void Free() override;

};

END