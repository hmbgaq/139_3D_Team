#pragma once


#include "Character_Client.h"
#include "Actor.h"

BEGIN(Client)

class CPlayer;

class CMonster_Character abstract : public CCharacter_Client
{


public:
	typedef struct tagMonsterDesc : public CGameObject::tagGameObjectDesc
	{
		_float4x4	WorldMatrix = XMMatrixIdentity();
		_bool		bPreview = false;
		wstring		strProtoTypeTag = L""; //! 파싱용
		_uint		iMonsterGroupIndex = 0; //!  승용 : 몬스터 그룹별로 저장후, 트리거 충돌 시 특정 그룹만 Create시키는 식으로 하려고함.
	}MONSTER_DESC;

protected:
	CMonster_Character(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CMonster_Character(const CMonster_Character& rhs);
	virtual ~CMonster_Character() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Search_Target(const _float fSearchDistance = 20.f);

	_uint Get_MonsterGroupIndex() { return m_tMonsterDesc.iMonsterGroupIndex; }
	void Set_MonsterGroupIndex(_uint iGroupIndex) { m_tMonsterDesc.iMonsterGroupIndex = iGroupIndex; }

public:
	CPlayer* Set_Player_Finisher_Pos(_float3 vPlayerPos);
	CPlayer* Set_Finish_Pos(_float3 vPos);

	_int iMeshNumber = 0;
	MONSTER_DESC* Get_MonsterDesc() { return &m_tMonsterDesc; }


protected:
	MONSTER_DESC			m_tMonsterDesc = {};

protected:
	HRESULT Ready_Components() PURE;
	HRESULT Ready_PartObjects() PURE;

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual CGameObject* Pool() PURE;
	virtual void Free() override;
};

END




