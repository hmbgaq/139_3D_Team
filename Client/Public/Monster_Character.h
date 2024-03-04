#pragma once

#include "Client_Defines.h"
#include "Character.h"
#include "Actor.h"

BEGIN(Client)

class CMonster_Character abstract : public CCharacter
{
protected:
	CMonster_Character(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CMonster_Character(const CMonster_Character& rhs);
	virtual ~CMonster_Character() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


protected:
	HRESULT Ready_Components() PURE;
	HRESULT Ready_PartObjects() PURE;


public:
	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual CGameObject* Pool() PURE;

	virtual void Free() override;


};

END




