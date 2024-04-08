#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Engine)
END

BEGIN(Client)

class CVampireCommander_Weapon : public CWeapon
{
protected:
	CVampireCommander_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CVampireCommander_Weapon(const CVampireCommander_Weapon& rhs);
	virtual ~CVampireCommander_Weapon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

public:
	void	Play_Trail(_bool bPlay);

protected:
	virtual HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


protected:
	class CEffect_Trail* m_pTrail = { nullptr };	//! 유정 : 트레일 추가


public:
	/* 원형객체를 생성한다. */
	//static CVampireCommander_Weapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual CGameObject* Pool();

	virtual void Free() override;
};

END