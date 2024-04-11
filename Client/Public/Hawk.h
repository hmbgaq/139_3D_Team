#pragma once

#include "Character_Client.h"
#include "Actor.h"

BEGIN(Engine)
END

BEGIN(Client)


class CHawk final : public CCharacter_Client
{
private:
	CHawk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CHawk(const CHawk& rhs);
	virtual ~CHawk() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();


private:
	CActor<CHawk>* m_pActor = { nullptr };

public:
	static CHawk* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;
	virtual void Free() override;


public:
	enum Hawk_State
	{

	};

};

END

