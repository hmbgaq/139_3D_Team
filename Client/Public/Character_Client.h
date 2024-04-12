#pragma once

#include "Client_Defines.h"
#include "Character.h"

BEGIN(Engine)
END


BEGIN(Client)

class CEffect;
class CEffect_Manager;
class CData_Manager;
class CUI_Manager;


class CCharacter_Client abstract : public CCharacter
{
protected:
	CCharacter_Client(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CCharacter_Client(const CCharacter_Client& rhs);
	virtual ~CCharacter_Client() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

//public:
//	CEffect* Create_Effect(CGameObject* pOwner = nullptr);
//	CEffect* Create_Effect(_float3 vPos, CGameObject* pOwner = nullptr);
//	CEffect* Create_Effect(const wstring& strPartTag);

public:
	void Reset_UpperAngle();

public:
	void Apply_Shake_And_Blur(Power ePower = Power::Light);
	void Apply_Shake_And_Chroma(Power ePower = Power::Light);
	void Create_Hitting_Effect(_float3 vPos, Power ePower = Power::Light, string strEffectName = "", CGameObject* pOwner = nullptr);
	
public:
	void Hit_Direct(CCharacter* pTarget, _float fDamage, _float fForce, Direction eDir, Power ePower, _bool bIsMelee = false, _bool bIsKnockUp = false)
	{
		_vector vPlayerPos = Get_Position_Vector();
		_vector vDir = pTarget->Calc_Look_Dir_XZ(vPlayerPos);

		pTarget->Set_Hitted(fDamage, vDir, fForce, 1.f, eDir, ePower, bIsMelee);
		Create_Hitting_Effect(pTarget->Get_Position(), ePower);

	}

public:
	_bool Check_EffectOnTrackPosition();

public:
	virtual void Free() override;

protected:
	CData_Manager* m_pDataManager = { nullptr };
	CUI_Manager* m_pUIManager = { nullptr };
};

END




