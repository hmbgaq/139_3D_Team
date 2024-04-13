#pragma once

#include "Client_Defines.h"
#include "Body.h"

BEGIN(Engine)
END

BEGIN(Client)

class CBody_Player final : public CBody
{

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
	virtual HRESULT Render_CSM(_uint i)  override;

public:
	virtual void	OnCollisionEnter(CCollider* other)	override;
	virtual void	OnCollisionStay(CCollider* other)	override;
	virtual void	OnCollisionExit(CCollider* other)	override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	virtual void Check_Frustum() override;
	_int iTemp = 0;

	/* 소영 추가 - 림라이트용 */
private:
	_bool m_bRim = { false };


public:
	static CBody_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;
	virtual void Free() override;

};

END