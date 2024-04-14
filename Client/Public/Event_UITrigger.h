#pragma once

#include "Engine_Defines.h"
#include "Client_Defines.h"
#include "Event_Trigger.h"
#include "Monster_Character.h"

BEGIN(Client)
class CUI_Manager;

class CEvent_UITrigger final : public CEvent_Trigger
{
public:
	typedef struct tagUITriggerDesc : public CEvent_Trigger::TRIGGER_DESC
	{
		_uint	   iSpawnGroupIndex = 0;
		string	   strSpawnMonsterJsonPath = "";
	}UI_TRIGGERDESC;


protected:
	CEvent_UITrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CEvent_UITrigger() = default;

public:
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Activate() override;
	virtual _bool			Activate_Condition() override;
	virtual _bool			End_Condition() override;

	virtual _bool			Write_Json(json& Out_Json) override;
	virtual void			Load_FromJson(const json& In_Json) override;

protected:
	virtual HRESULT			Ready_Component() override;

public:
	void					Set_SpawnGroupIndex(_uint iSpawnGroupIndex) { m_tUITriggerDesc.iSpawnGroupIndex = iSpawnGroupIndex; }
	void					Set_ForceActivate(_bool bForceActivate) { m_bForceActivate = bForceActivate; } //! °­Á¦·Î Activate
	void					Set_ColliderOwnerPosition() { m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix()); }
	virtual void			Set_ColliderSize(_float3 vColliderSize) override;
	virtual void			Set_ColliderCenter(_float3 vColliderCenter) override;


	UI_TRIGGERDESC* Get_UITriggerDesc() { return &m_tUITriggerDesc; }

private:
	//vector<MONSTER

	UI_TRIGGERDESC				 m_tUITriggerDesc = {};
	_bool									 m_bJsonReady = false;
	_bool									 m_bForceActivate = false;
	vector<CMonster_Character::MONSTER_DESC> m_vecCreateMonsterDesc;
	CUI_Manager*							 m_pUIManager = nullptr;

public:
	static CEvent_UITrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void					   Free() override;


};

END