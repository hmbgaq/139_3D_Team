#pragma once

#include "Client_Defines.h"
#include "Monster_Character.h"
#include "Actor.h"

BEGIN(Client)
class CMother;

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
	//vector<class CUI_Weakness*> m_pWeakneesUIs;
	_bool m_bTurn = true;
	CMother* m_pMother = { nullptr };

public:
	_float m_fTimeDelta = 0.f;
public:
	/* 원형객체를 생성한다. */
	static CSon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;

	enum Son_State //스턴은 L버전은 위치값이 바뀌기 때문에 R로 사용 하면 될 거 같음 
	{ 
		Parasiter_Tentacle_Death                          ,
		Parasiter_Worm_Attack_01						  ,
		Parasiter_Worm_Attack_Hit_L						  ,//고장
		Parasiter_Worm_Attack_Rest_01					  ,
		Parasiter_Worm_Attack_Hit_R						  ,//고장
		Parasiter_Worm_Attack_Stop_01					  ,
		Parasiter_Worm_BiteAttack_Common_Stop_2000		  ,
		Parasiter_Worm_BiteAttack_Common_Stop_400		  ,
		Parasiter_Worm_burn								  ,//고장
		Parasiter_Worm_Idle								  ,
		Parasiter_Worm_Hide								  ,
		Parasiter_Worm_Hide_2							  ,
		Parasiter_Worm_Hit								  ,
		Parasiter_Worm_OneBiteAttack_2000				  ,
		Parasiter_Worm_OneBiteAttack_400				  ,
		Parasiter_Worm_OneBiteAttack_Rest_2000			  ,
		Parasiter_Worm_OneBiteAttack_Rest_400			  ,
		Parasiter_Worm_RestFromSpit_Start_L				  ,
		Parasiter_Worm_RestFromSpit_Start_R				  ,
		Parasiter_Worm_RestStraight_Start_L				  ,
		Parasiter_Worm_RestStraight_Start_R				  ,
		Parasiter_Worm_Spawn							  ,
		Parasiter_Worm_Spit_Far							  ,
		Parasiter_Worm_Spit_Near						  ,
		Parasiter_Worm_Stun_End_L						  ,
		Parasiter_Worm_Stun_End_R						  ,
		Parasiter_Worm_Stun_Hit_L						  ,
		Parasiter_Worm_Stun_Loop_R						  ,
		Parasiter_Worm_Stun_Hit_R						  ,
		Parasiter_Worm_Stun_Loop_L						  ,
		Parasiter_Worm_Stun_Start						  ,
		Parasiter_Worm_Stun_Start_L						  ,
		ShakeAndBite_Loop_2000							  ,
		ShakeAndBite_Loop_400							  ,
		ShakeAndBite_Start_2000							  ,
		ShakeAndBite_Start_400

	};

};

END