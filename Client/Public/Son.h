#pragma once

#include "Client_Defines.h"
#include "Monster_Character.h"
#include "Actor.h"

BEGIN(Client)

class CSon final : public CMonster_Character
{
private:
	CSon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CSon(const CSon& rhs);
	virtual ~CSon() = default;

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

protected:
	virtual void Hitted_Left(Power ePower)	override;
	virtual void Hitted_Right(Power ePower) override;
	virtual void Hitted_Front(Power ePower) override;
	virtual void Hitted_Dead(Power ePower)	override;
	virtual void Hitted_Stun(Power ePower) override;
	virtual void Hitted_Finish() override;
	virtual void Hitted_Weakness() override;


public:
	CActor<CSon>* Get_Actor() { return m_pActor; }
	void Set_Actor(CActor<CSon>* _pActor) { m_pActor = _pActor; }
private:
	CActor<CSon>* m_pActor = { nullptr };

public:
	vector<class CUI_Weakness*> m_pWeakneesUIs;
	_bool m_bTurn = true;

public:
	/* 원형객체를 생성한다. */
	static CSon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;

	enum Son_State
	{

	};

};

END