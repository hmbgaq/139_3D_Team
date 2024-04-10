#pragma once

#include "Infected.h"

BEGIN(Client)

class CInfected_D final : public CInfected
{
protected:
	CInfected_D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CInfected_D(const CInfected_D& rhs);
	virtual ~CInfected_D() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;
	virtual HRESULT Render_OutLine() override;
	virtual HRESULT Update_RenderState(_int CurrentHP) override;

public:
	virtual void Explosion() override;

protected:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Ready_Option();

	virtual void Check_Frustum() override;


private:
	CActor<CInfected>* m_pActor = { nullptr };

public:
	static CInfected_D* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;
	virtual void Free() override;

};

END

/* 패턴 정리
* Infected_IdleAct_01 이걸로 걍 쓰자. 알에서 태어나서 비틀대다가 달려오는컨셉인거임 
0. 스폰 : 알에서 서있는 상태 그대로 스폰 / 다른 스폰애니메이션을 못봤음 ( 몬스터 자체는 30마리 이상봄 근데 모두다 스폰애니메이션X)
Infected_Sprint_F_01
1. 공격 : 무조건 거리상관없이 플레이어한테 달려들음 -> 일직선
Infected_Taunt_01 정없으면 이걸로라도 ? /electrocute loop / 
2. 자폭 : 자폭사거리에 들어오면 제자리에서 멈추고 온몸을 털면서 자폭 시전
3. 자폭이후 파편튀고 끝
*/




