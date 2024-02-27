#pragma once

#include "Client_Defines.h"
#include "Character.h"
#include "Actor.h"

BEGIN(Client)

class CInfected : public CCharacter
{
private:
	CInfected(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CInfected(const CInfected& rhs);
	virtual ~CInfected() = default;

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
	CActor<CInfected>* m_pActor = { nullptr };

public:
	/* 원형객체를 생성한다. */
	static CInfected* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;

public:
	enum Infected_State
	{

	};
};

END




