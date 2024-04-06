#pragma once


#include "Character_Client.h"
#include "Actor.h"

class CUI_EnemyHUD_Shard;

BEGIN(Client)

class CPlayer;

class CMonster_Character abstract : public CCharacter_Client
{
public:
	enum class Monster_State 
	{
		ATTACK,
		ELECTROCUTE,
		Monster_State_End,
	};

public:
	typedef struct tagMonsterDesc : public CGameObject::tagGameObjectDesc
	{
		_float4x4	WorldMatrix = XMMatrixIdentity();
		_bool		bPreview = false;
		wstring		strProtoTypeTag = L""; //! 파싱용
		_uint		iMonsterGroupIndex = 0; //!  승용 : 몬스터 그룹별로 저장후, 트리거 충돌 시 특정 그룹만 Create시키는 식으로 하려고함.
		_int		iStartNaviIndex = -1;
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

	_uint Get_StartNaviIndex() { return m_tMonsterDesc.iStartNaviIndex; }
	void Set_StartNaviIndex(_int iStartNaviIndex) { m_tMonsterDesc.iStartNaviIndex = iStartNaviIndex; }

public:
	CPlayer* Set_Player_Finisher_Pos(_float3 vPlayerPos);
	CPlayer* Set_Finish_Pos(_float3 vPos);

	_int iMeshNumber = 0;
	MONSTER_DESC* Get_MonsterDesc() { return &m_tMonsterDesc; }

	_float Get_AttackDelay() { return m_fAttackDelay; };
	void Set_AttackDelay(_bool _fAttackDelay) { m_fAttackDelay = max(m_fAttackDelay, _fAttackDelay); };

	_uint Get_AttackCount() { return m_iAttackCount; };
	void Add_AttackCount() { m_iAttackCount += 1; };
	void Reset_AttackCount() { m_iAttackCount = 0; };

public:
	virtual void Check_Frustum() override;

public:
	void Set_Monster_State(Monster_State _eMonster_State) { m_eMonster_State = _eMonster_State; };
	_bool Is_Attack_State() { return Monster_State::ATTACK == m_eMonster_State; }

//protected: // !성희 추가
public: // !성희 추가
	// 몬스터 HUD를 준비합니다. (생성 : Level, Owner)
	void Ready_EnemyHUD_Shard(_uint iLevelIndex, CGameObject* pOwner);

	// 몬스터 HUD의 위치를 잡아줍니다. (매 틱마다 불러주세요. | 몬스터의 World매트릭스를 넣어주세요. | Offset만큼 위치를 움직일 수 있습니다. [Defualt (오프셋 안줬을 경우) : 0, 2, 0]
	void Check_EnemyHUD_World(_matrix matWorld, _float3 vOffsetPos = { 0.f, 2.f, 0.f });
	
	// 몬스터 HUD를 삭제(비활성화)합니다. (몬스터가 죽을때 불러주세요)
	void Set_EnemyHUD_Dead();

	CUI_EnemyHUD_Shard* m_pEnemyHUD = { m_pEnemyHUD };

protected:
	MONSTER_DESC			m_tMonsterDesc = {};
	_float					m_fAttackDelay = { 0.f };
	_uint					m_iAttackCount = { 0 };

	Monster_State			m_eMonster_State = { Monster_State::Monster_State_End };
	

protected:
	HRESULT Ready_Components() PURE;
	HRESULT Ready_PartObjects() PURE;

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual CGameObject* Pool() PURE;
	virtual void Free() override;
};

END




