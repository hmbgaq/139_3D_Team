#pragma once

#include "Client_Defines.h"
#include "Bandit_Heavy.h"
#include "Actor.h"

BEGIN(Client)

class CHeavy_Vampiric_Zombie final : public CBandit_Heavy
{
private:
	CHeavy_Vampiric_Zombie(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CHeavy_Vampiric_Zombie(const CHeavy_Vampiric_Zombie& rhs);
	virtual ~CHeavy_Vampiric_Zombie() = default;

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
	virtual void Check_Frustum() override;

public:
	static CHeavy_Vampiric_Zombie* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;

};

END




