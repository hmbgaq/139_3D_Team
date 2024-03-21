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

public:
	_float Ratation_Target_Test();

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
	CActor<CMother>* Get_Actor() { return m_pActor; }
	void Set_Actor(CActor<CMother>* _pActor) { m_pActor = _pActor; }
private:
	CActor<CMother>* m_pActor = { nullptr };

private:
	CEffect* m_pMapEffect = { nullptr }; // TEST
public://뼈 13개 
	vector<string> m_vRandomBones = { "LeftHandIK","RightHandIK" ,"Hips","Spine1","Head","Neck","RightShoulder","LeftElbowRoll","LeftShoulderRoll_01","Right_BigWing_03","Left_BigWing_03","LeftLeg","RightLeg" };
public:
	vector<class CUI_Weakness*> m_pWeakneesUIs;
	_bool m_bTurn = true;

public:
	/* 원형객체를 생성한다. */
	static CMother* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;

	enum Mother_State
	{

	};
};

END
