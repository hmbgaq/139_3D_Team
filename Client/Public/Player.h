#pragma once

#include "Client_Defines.h"
#include "Character.h"

BEGIN(Engine)
class CCollider;
class CNavigation;
END

BEGIN(Client)

class CPlayer final : public CCharacter
{
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual CComponent* Find_Component(const wstring & strComTag, const wstring & strPartTag = TEXT("")) override;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	CGameObject* Find_PartObject(const wstring& strPartTag);


private:

	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Add_PartObject(const wstring& strPrototypeTag, const wstring& strPartTag, void* pArg);



public:
	/* 원형객체를 생성한다. */
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;
};

END