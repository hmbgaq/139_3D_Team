#pragma once

#include "Client_Defines.h"
#include "Monster_Character.h"
#include "Actor.h"

BEGIN(Client)

class CMotherMouth final : public CMonster_Character
{
private:
	CMotherMouth(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CMotherMouth(const CMotherMouth& rhs);
	virtual ~CMotherMouth() = default;

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


public:
	CActor<CMotherMouth>* Get_Actor() { return m_pActor; }
	void Set_Actor(CActor<CMotherMouth>* _pActor) { m_pActor = _pActor; }
private:
	CActor<CMotherMouth>* m_pActor = { nullptr };

public:
	/* 원형객체를 생성한다. */
	static CMotherMouth* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;

	enum MotherMouth_State
	{

	};
};

END

