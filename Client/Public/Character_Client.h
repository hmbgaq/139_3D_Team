#pragma once

#include "Client_Defines.h"
#include "Character.h"

BEGIN(Engine)
END


BEGIN(Client)

class CEffect;
class CEffect_Manager;
class CData_Manager;


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

public:
	CEffect* Create_Effect(CGameObject* pOwner = nullptr);
	CEffect* Create_Effect(_float3 vPos, CGameObject* pOwner = nullptr);
	CEffect* Create_Effect(const wstring& strPartTag);

public:
	void Create_Hitting_Effect(_float3 vPos, Power ePower = Power::Light, string strEffectName = "", CGameObject* pOwner = nullptr);

public:
	_bool Check_EffectOnTrackPosition();

public:
	virtual void Free() override;

protected:
	CEffect_Manager* m_pEffectManager = { nullptr };
	CData_Manager* m_pDataManager = { nullptr };


};

END




