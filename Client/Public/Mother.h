#pragma once

#include "Client_Defines.h"
#include "Monster_Character.h"
#include "Actor.h"

BEGIN(Client)


class CMother final : public CMonster_Character
{
private:
	CMother(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CMother(const CMother& rhs);
	virtual ~CMother() = default;

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
	virtual void Hitted_Front(Power ePower) override;
	virtual void Hitted_Dead(Power ePower)	override;
	virtual void Hitted_Stun(Power ePower) override;
	virtual void Hitted_Finish() override;
	virtual void Hitted_Weakness() override;


public:
	CActor<CMother>* Get_Actor() { return m_pActor; }
	void Set_Actor(CActor<CMother>* _pActor) { m_pActor = _pActor; }
private:
	CActor<CMother>* m_pActor = { nullptr };

private:
	CEffect* m_pMapEffect = { nullptr }; // TEST

public:
	CGameObject* m_pMonster1 = { nullptr };
	CGameObject* m_pMonster2 = { nullptr };
	vector<CUI_Weakness*> m_pWeakneesUIs;
	_bool m_bTurn = true;
	_bool m_bPhase = true;
	_bool m_bfirstCheck = true;
	_bool m_bSonRespawn = false;
	_bool m_bStun = true;
	_bool m_bStunCheck1 = true;
	_bool m_bStunCheck2 = true;
	//motherweakness
	_bool m_bWeakness1 = false;
	_bool m_bWeakness2 = false;

	_uint m_iSonDead = 0;
	_uint m_iLoopCount = 0;
	_uint m_iShootingCount = 0;
	_float m_fTimeDelta = 0.f;
public:
	/* 원형객체를 생성한다. */
	static CMother* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;

	enum Mother_State
	{
		GrandParasiter_Intro,
		GrandParasiter_Turn,
		Parasiter_Idle_01,
		GrandParasiter_Turn_L45,
		Parasiter_Turn_L45,
		GrandParasiter_Turn_R45,
		Parasiter_Turn_R45,
		Parasiter_Death,
		Parasiter_Hit_02,
		Parasiter_Hit_03,
		Parasiter_HitLight_L_01,
		Parasiter_HitLight_R_01,
		Parasiter_Idle_closed_01,
		Parasiter_ShakeTree_Loop,
		Parasiter_ShakeTree_Start,
		Parasiter_ShakeTree_Stop,
		Parasiter_SpittingMines_End_400,
		Parasiter_SpittingMines_Loop_400,
		Parasiter_SpittingMines_Start_400,
		Parasiter_Stun_Loop,
		Parasiter_Stun_Start,
		Parasiter_Stun_Stop,
		Parasiter_Vomit_End_02,
		Parasiter_Vomit_Loop_02,
		Parasiter_Vomit_Start_02
	};
};

END
