#pragma once
#include "Client_Defines.h"
#include "Infected.h"
#include "Actor.h"

BEGIN(Client)

class CInfected_B final : public CInfected
{
protected:
	CInfected_B(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CInfected_B(const CInfected_B& rhs);
	virtual ~CInfected_B() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Ready_Option();

public:
	void Attack();

private:
	CActor<CInfected>* m_pActor = { nullptr };

public:
	static CInfected_B* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;
	virtual void Free() override;


};

END




