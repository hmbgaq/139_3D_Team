#pragma once

#include "Engine_Defines.h"
#include "Client_Defines.h"
#include "Event.h"
#include "Base.h"

BEGIN(Engine)
class CCollider;
class CGameInstance;
class CTransfrom;
END


BEGIN(Client)

class CPlayer;

class CEvent_Trigger abstract : public IEvent, public CBase
{

public:
	enum TRIGGER_TYPE { TRIGGER_MONSTER, TRIGGER_BOSS, TRIGGER_UI, TRIGGER_CUTSCENE, TRIGGERTYPE_END};


	typedef struct tagTriggerDesc
	{
		_float3		  vColliderSize = {};
		_float3		  vColliderCenter = {};
		_bool		  bOnTrigger = false;
		string		  strTriggerNameTag = "";
		TRIGGER_TYPE  eTriggerType = TRIGGERTYPE_END;
	}TRIGGER_DESC;


protected:
			 CEvent_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CEvent_Trigger() = default;

public:
	virtual HRESULT			Initialize(void* pArg);
	virtual void			Activate() override;
	virtual _bool			Activate_Condition() override;
	virtual _bool			End_Condition() override;

	virtual _bool			Write_Json(json& Out_Json) override;
	virtual void			Load_FromJson(const json& In_Json) override;

public:
	class CCollider*		Get_TriggerCollider() { return m_pColliderCom; }
	void					Set_TriggerCollider(class CCollider* pCollider) { m_pColliderCom = pCollider; }

	TRIGGER_TYPE			Get_TriggerType() { return m_tTriggerDesc.eTriggerType; }
	void					Set_TriggerType(TRIGGER_TYPE eTriggerType) { m_tTriggerDesc.eTriggerType = eTriggerType;}

	class CTransform*		Get_Transform() { return m_pTransformCom; }


protected:
	virtual HRESULT			Ready_Component();


public:
	void Set_Actor(CPlayer* pActor) {
		m_pActor = pActor;
	}

	void Set_NameTag(const char* pChar) { m_tTriggerDesc.strTriggerNameTag = pChar;}
	virtual void Set_ColliderSize(_float3 vColliderSize);
	virtual void Set_ColliderCenter(_float3 vColliderCenter);


public:
	virtual void Free() override;

protected:
	TRIGGER_DESC		 m_tTriggerDesc = {};

	CPlayer*			 m_pActor = { nullptr };
	_bool				 m_bSetFindActor = false;

	CCollider*			 m_pColliderCom = { nullptr};
	CTransform*			 m_pTransformCom = { nullptr };

	CGameInstance*		 m_pGameInstance = { nullptr };
	ID3D11Device*		 m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

};

END