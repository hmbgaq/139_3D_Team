#pragma once

#include "Client_Defines.h"
#include "Body.h"

BEGIN(Engine)
END

BEGIN(Client)

class CBody_Bandit_Sniper final : public CBody
{
private:
	CBody_Bandit_Sniper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CBody_Bandit_Sniper(const CBody_Bandit_Sniper& rhs);
	virtual ~CBody_Bandit_Sniper() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBody_Bandit_Sniper* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;
	virtual void Free() override;
};

END

/* 
Mesh 인덱스 설명
0 : 옆구리 망토
1 : 모자
2 :
3 : 어깨 케이브
4 :
5 : 바지
6 : 목에있는 피떡
7 : 등에있는 피떡
8 : 등뒤 왼쪽피떡
9 : 등뒤 오른쪽 피떡
10 : 손

*/