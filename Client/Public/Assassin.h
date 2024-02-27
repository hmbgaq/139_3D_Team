#pragma once

#include "Client_Defines.h"
#include "Character.h"
#include "Actor.h"

BEGIN(Client)

class CAssassin final : public CCharacter
{
private:
	CAssassin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CAssassin(const CAssassin& rhs);
	virtual ~CAssassin() = default;

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
	CActor<CAssassin>* m_pActor = { nullptr };

public:
	/* 원형객체를 생성한다. */
	static CAssassin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;

public:
	enum Assassin_State
	{											 
		Assassin_Run_F							 ,
		Assassin_Run_FL							 ,
		Assassin_Run_FL45						 ,
		Assassin_Run_FR							 ,
		Assassin_Run_FR45						 ,
		Assassin_Sprint_F						 ,
		Assassin_Walk_B							 ,
		Assassin_Walk_BL						 ,
		Assassin_Walk_BL135						 ,
		Assassin_Walk_BR						 ,
		Assassin_Walk_BR135						 ,
		Assassin_Walk_F							 ,
		Assassin_Walk_FL						 ,
		Assassin_Walk_FL45						 ,
		Assassin_Walk_FR						 ,
		Assassin_Walk_FR45						 ,
		Assassin_IdleAct_01				 ,
		Assassin_IdleAct_02				 ,
		Assassin_IdleAct_03				 ,
		Assassin_AssassinTransformation_01		 ,
		Assassin_AttackScream					 ,
		Assassin_BuffingHowl_Loop				 ,
		Assassin_BuffingHowl_Start				 ,
		Assassin_BuffingHowl_Stop				 ,
		Assassin_DodgeRun_L_01					 ,
		Assassin_DodgeRun_R_01					 ,
		Assassin_DynamicStrong_08				 ,
		Assassin_Riposte						 ,
		Assassin_Riposte_02						 ,
		Assassin_Scared_01						 ,
		Assassin_Scared_02						 ,
		Assassin_SpawnGround_01			 ,
	};
};

END




