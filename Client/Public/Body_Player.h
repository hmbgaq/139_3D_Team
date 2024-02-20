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

public:
	void SetUp_Animation(_uint iAnimIndex);


private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	/* 원형객체를 생성한다. */
	static CBody_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;

	// CBody을(를) 통해 상속됨
	
};

END