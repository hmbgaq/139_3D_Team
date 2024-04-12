#pragma once

#include "Character_Client.h"
#include "Actor.h"

BEGIN(Engine)
END

BEGIN(Client)


class CHawk final : public CCharacter_Client
{
private:
	CHawk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CHawk(const CHawk& rhs);
	virtual ~CHawk() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Check_Frustum() override;

public:
	void FlyAway();

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();


private:
	CActor<CHawk>* m_pActor = { nullptr };

public:
	static CHawk* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;
	virtual void Free() override;


public:
	enum Hawk_State
	{
		Hawk_Calling_Ground_01,
		Hawk_Calling_Ground_02,
		Hawk_Drinking,
		Hawk_Eating_01,
		Hawk_Eating_02,
		Hawk_Eating_03,
		Hawk_Eating_04,
		Hawk_Fly_Glide,
		Hawk_Fly_Heavy,
		Hawk_Fly_Light,
		Hawk_Loco_Run,
		Hawk_Loco_Walk,
		Hawk_Stand_Ground_00,
		Hawk_Stand_Ground_01,
		Hawk_Stand_Ground_02,
		Hawk_Trans_FlyHeavy_to_StandGround,
		Hawk_Trans_StandGround_to_FlyHeavy,
	};

};

END

