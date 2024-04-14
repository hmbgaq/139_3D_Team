#pragma once

#include "Client_Defines.h"
#include "Monster_Character.h"
#include "Actor.h"

BEGIN(Client)

/* 모델 : Character/Gores/Blow_Up 에 있는 모델 : 몇번 맞아서 피부 메시 뜯겨나간 메시 -> 구현안하는게 좋을듯.. 얘가 하면 모든애들이 다 해야함. */
/* Memo */
// Body, Weapon 등의 클래스는 Monster가 따로 뺀거 아니라서 바로 CBody , CWeapon 상속받아서 만들기

class CInfected abstract : public CMonster_Character
{
public:
	enum class	INFECTED_TYPE {
	INFECTED_VESSEL_A, INFECTED_VESSEL_B, INFECTED_VESSEL_C, 
	INFECTED_PROTEUS, INFECTED_WASTER, INFECTED_END };

	typedef struct tagDesc
	{
		INFECTED_TYPE eType = INFECTED_TYPE::INFECTED_END;
			
		_float fAttack_Distance = 0.f; // 0 ~ Attack = 공격사거리 
		_float fWalk_Distance = 0.f;   // Attack ~ Walk = walk Animation으로 접근 
								 // Walk Dist < 보다 값이 클경우 Run -> Attack 도달하면 공격    
		_float WalkSpeed = 0.f;
		_float RunSpeed = 0.f;

		_int RandomNumber = 0; // SMath::Random(1, 10);

		//m_eInfo.fAttack_Distance = 3.5f;
		//m_eInfo.fWalk_Distance = 10.f;
	public:
		tagDesc()
		{
			ZeroMemory(this, sizeof(tagDesc));
		}
		INFECTED_TYPE Get_Type() { return eType; }
	}INFECTED_DESC;


protected:
	CInfected(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CInfected(const CInfected& rhs);
	virtual ~CInfected() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	INFECTED_DESC Get_Info() { return m_eInfo; }
	virtual HRESULT Update_RenderState(_int CurrentHP) PURE;
	void	Set_CntDead_Time(_float CountDown) { m_fCntDead_Time = CountDown; }
	void	Set_CntDead_Active(_bool bActive) { m_bCntDead_Active = bActive; }

public:
	virtual void Explosion() {};


protected:
	HRESULT Ready_Components();
	_bool	m_bCntDead_Active = { false };
	_float	m_fCntDead_Time = {};
	_float fTimeAcc = 0.f;

protected:
	virtual void Hitted_Left(Power ePower)	override;	// (플레이어 기준)왼쪽으로 떄린 평타 
	virtual void Hitted_Right(Power ePower) override;	// (플레이어 기준)오른쪽으로 떄린 평타 
	virtual void Hitted_Front(Power ePower) override;	// (플레이어 기준)정면으로 떄린 평타 
	
	/* 플레이어에 의한 피격 */
	virtual void Hitted_Knock(_bool bIsCannonball) override;	// 플레이어 캐논볼 스킬
	virtual void Hitted_Electrocute()	override;				// 감전스킬맞고 어어 거리는 루프상태 
	virtual void Hitted_OpenState_Pull() override;				// 플레이어 끌어당기는스킬?
	virtual void Hitted_Opened(Direction eDirection) override;  // 끌어당긴뒤 제자리에서 쳐맞는애니메이션 
	virtual void Hitted_KnockUp() override;						// 퍼올리기 스킬 맞았을때 

	virtual void Hitted_Dead(Power ePower)	override;	// HP다 닳아서 죽을때 

protected:
	INFECTED_DESC	m_eInfo = {};
	
private:
	CActor<CInfected>* m_pActor = { nullptr };


public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual CGameObject* Pool() PURE;

	virtual void Free() override;

public:
	enum Infected_State
	{
		Infected_Melee_RD_01					 , // (0) 제자리 공격 - 한손 퍼올리기 
		Infected_Melee_RM_01					 , // (1) 제자리 공격 - 한손 수평공격
		Infected_Melee_RM_02					 , // (2) 제자리 공격 - 양손 공격 - 한쪽에서 쓸어가기 
		Infected_Melee_RU_01					 , // (3) 제자리 공격 - 한손 내려찍기 
		Infected_Melee_RU_02					 , // (4) 제자리 공격 - 양손 공격 - 양손만세 
		Infected_MeleeDynamic_RU_01				 , // (5) 이동공격 / 짧은 거리 - 한손 내려찍기 
		Infected_MeleeDynamic_RU_02				 , // (6) 이동공격 / 짧은 거리 - 한손 수평공격
		Infected_MeleeDynamic_RU_03				 , // (7) 이동공격 / 짧은 거리 - 양손 내려찍기 
		Infected_Sprint_F_Melee_02				 , // (8) 이동공격 / 긴 거리 - 한손 내려찍기 
		Infected_Sprint_F_Melee_03				 , // (9) 이동공격 / 긴 거리 - 한손 수평공격
		Infected_Idle							 , // (10) Idle / 그냥 서있기만함 
		Infected_IdleAct_01						 , // (11) Idle / 반쯤 엎드리기까지 하는 Idle 비틀거림
		Infected_IdleAct_02						 , // (12) Idle / 술취해서 약간 비틀거리는정도 
		Infected_IdleAct_03						 , // (13) Idle / 제자리에서 주변 둘러보기 
		Infected_IdlePose						 , // (14) Idle / 안움직이고 고정상태임 - 안쓰는용도인듯 ?
		Infected_InteractionVault200			 , // (15) Dynamic / 자기 키만한 높이의 담 넘어감
		Infected_JumpDown_200_forward			 , // (16) 높은곳에서 뛰어내려서 슈퍼히어로랜딩하고 일어서기까지 
		Infected_RelaxedIdleAct_01				 , // (17) Idle / 옆으로 돌아서 보기까지 하고 다시 원래대로 돌아옴
		Infected_RelaxedIdleAct_02				 , // (18) Idle / 몸을 반쯤 돌려서 뒤까지 쳐다보고난뒤에 돌아옴
		Infected_RelaxedIdleAct_03				 , // (19) Idle / 자기 겨드랑이 확인함
		Infected_Run_B							 , // (20) Move / 빠르게 뒤로 움직임
		Infected_Run_F							 , // (21) Move / 빠르게 앞으로 움직임
		Infected_Run_FL							 , // (22) Move / 빠르게 왼쪽로 움직임(몬스터기준) + 바라보기 
		Infected_Run_FL45						 , // (23) Move / 빠르게 왼쪽 대각선 움직임(몬스터기준)	
		Infected_Run_FR							 , // (24) Move / 빠르게 오른쪽로 움직임(몬스터기준) + 바라보기 
		Infected_Run_FR45						 , // (25) Move / 빠르게 오른쪽 대각선 움직임(몬스터기준)
		Infected_SpawnClimb_01					 , // (26) Spawn / 일반적으로 올라옴
		Infected_SpawnClimb_02					 , // (27) Spawn / 땅에서 앞구르기하고 올라옴
		Infected_SpawnCrawl_01					 , // (28) Spawn / 앞으로 기어오면서 올라옴
		Infected_SpawnCrawl_02					 , // (29) Spawn / 앞으로 기어오면서 올라옴(2) - 차이모르겠다
		Infected_SpawnFromCoffin0				 , // (30) Spawn / 하늘보고 누워있다가 일어서면서 올라옴
		Infected_SpawnFromCoffin65				 , // (31) Spawn / 뭔가에 걸려서 구르며 등장
 		Infected_SpawnFromCoffin90				 , // (32) Dynamic / 양손 뻗으며 움직이기  
		Infected_SpawnFromEgg_01				 , // (33) Spawn / 구르려다 실패하고 이상하게 구름 
		Infected_SpawnFromEgg_02				 , // (34) Spawn / 제자리 일어서면서 스폰 
		Infected_SpawnJumpDown_01				 , // (35) Spawn / 높은데서 점프하면서 나타남 
		Infected_SpawnJumpOut_01				 , // (36) Spawn / 뭔가 짚으면서 나타남 
		Infected_SpawnJumpOut_03				 , // (37) Spawn / 뭔가 짚으면서 나타남 
		Infected_SpecialIdle					 , // (38) Idle / 바닥에 앉아있음
		Infected_SpecialIdle_to_Idle			 , // (39) Idle / 바닥에 앉아있다가 일어남 
		Infected_Sprint_F_01					 , // (40) Move / 빠르게 두손들고 달려옴 
		Infected_Taunt_01						 , // (41) Move / 두손들고 좀비처럼 다가옴 
		Infected_Taunt_02						 , // (42) Move / 왼쪽 180도 회전 
		Infected_Turn_L180						 , // (43) Move / 왼쪽 180도 회전 
		Infected_Turn_L90						 , // (44) Move / 왼쪽 90도 회전 
		Infected_Turn_R180						 , // (45) Move / 오른쪽 180도 회전
		Infected_Turn_R90						 , // (46) Move / 오른쪽 90도 회전
		Infected_Walk_B							 , // (47) Move / 천천히 뒷걸음질 
		Infected_Walk_F							 , // (48) Move / 천천히 앞으로 이동 
		Infected_Walk_FL						 , // (49) Move / 천천히 (몬스터기준) 왼쪽 바라보면서 이동 
		Infected_Walk_FL45						 , // (50) Move / 천천히 (몬스터기준) 왼쪽 대각선 이동 
		Infected_Walk_FR						 , // (51) Move / 천천히 (몬스터기준) 오른쪽 바라보면서 이동
		Infected_Walk_FR45						 , // (52) Move / 천천히 (몬스터기준) 오른쪽 대각선 이동 
		Infected_DeathHeavy_F_01_NEW			 , // (53) Dead / 뒤로 꽈당하고 누으면서 죽음
		Infected_DeathLight_B_01_NEW			 , // (54) Dead / 뺨맞고 왼쪽으로 고개 돌리면서 앞으로 고꾸라짐
		Infected_DeathLight_F_01_NEW			 , // (55) Dead / 뺨맞고 오른쪽으로 고개 돌리면서 뒤로 고꾸라짐
		Infected_HitHeavy_B_01_NEW				 , // (56) Hit / 앞으로 넘어질듯 하다가 안넘어지고 이동하다가 뒤돌아봄
		Infected_HitHeavy_BL_01_NEW				 , // (57) Hit / 왼뺨맞고 앞으로 이동 - 안넘어지고 뒤돌아봄
		Infected_HitHeavy_BR_01_NEW				 , // (58) Hit / 오른쪽 뺨 맞고 앞으로 이동 - 안넘어지고 뒤돌아봄
		Infected_HitHeavy_F_01_NEW				 , // (59) Hit / 턱맞고 뒷걸음질치면서 뒤로 이동 
		Infected_HitHeavy_F_02_NEW				 , // (60) Hit / 턱맞고 뒷걸음질치면서 뒤로 이동 - 좀더 쎄게 맞은듯?
		Infected_HitHeavy_FL90_01_NEW			 , // (61) Hit / 왼쪽갈비뼈 맞고 오른쪽으로 사이드 무빙 
		Infected_HitHeavy_FL_01_NEW				 , // (62) Hit / 오른쪽 뺨맞고 뒷걸음질 - 풀파워인듯
		Infected_HitHeavy_FL_02_NEW				 , // (63) Hit / 오른쪽 뺨맞고 뒷걸음질 - 더 풀파워인듯
		Infected_HitHeavy_FR90_01_NEW			 , // (64) Hit / 오른쪽 갈비뼈 맞고 왼쪽으로 사이드 무빙
		Infected_HitHeavy_FR_01_NEW				 , // (65) Hit / 왼쪽 뺨 맞고 뒷걸음질 - 풀파워인듯 
		Infected_HitHeavy_FR_02_NEW				 , // (66) Hit / 왼쪽 뺨맞고 뒷걸음질 - 더 풀파워인듯
		Infected_HitLight_B_01_NEW				 , // (67) Hit / 사용x 메시 뭉쳐있음 
		Infected_HitLight_BL_01_NEW				 , // (68) Hit / 사용x 메시 뭉쳐있음 
		Infected_HitLight_BR_01_NEW				 , // (69) Hit / 사용x 메시 뭉쳐있음 
		Infected_HitLight_F_01_NEW				 , // (70) Hit / 사용x 메시 뭉쳐있음 
		Infected_HitLight_FL_01_NEW				 , // (71) Hit / 사용x 메시 뭉쳐있음 
		Infected_HitLight_FR_01_NEW				 , // (72) Hit / 사용x 메시 뭉쳐있음 
		Infected_HitLight_L_01_NEW				 , // (73) Hit / 사용x 메시 뭉쳐있음 
		Infected_HitLight_R_01_NEW				 , // (74) Hit / 사용x 메시 뭉쳐있음 
		Infected_HitLightOpened_F_01_NEW		 , // (75) Hit / 사용x 메시 뭉쳐있음 
		Infected_HitNormal_B_01_NEW				 , // (76) Hit / 사용x 메시 뭉쳐있음 
		Infected_HitNormal_BL_01_NEW			 , // (77) Hit / 사용x 메시 뭉쳐있음 
		Infected_HitNormal_BR_01_NEW			 , // (78) Attack / 아이고 그러시구나 하고 앞으로 좀만 가더니 뒤돌아봄
		Infected_HitNormal_F_01_NEW				 , // (79) Hit / 살짝 왼뺨 맞은것처럼 하고 앞으로 이동해서 뒤돌아봄
		Infected_HitNormal_F_02_NEW				 , // (80) Hit / 턱주가리 두걸음 뒤로 
		Infected_HitNormal_FL90_01_NEW			 , // (81) Hit / 아예 하고 허리굽혀서 오른쪽으로 이동 
		Infected_HitNormal_FL_01_NEW			 , // (82) Hit / 살짝 왼뺨 맞은것처럼 하고 뒤로 이동
		Infected_HitNormal_FR90_01_NEW			 , // (83) Hit / 살짝 오른뺨 맞은것처럼 하고 왼쪽로 이동
		Infected_HitNormal_FR_01_NEW			 , // (84) Hit / 오른쪽 뺨 맞고 뒤로 이동 
		Infected_KnockFrontLight_B_01_NEW		 , // (85) KnockDown / 제자리에서 앞으로 쓰러지고 일어나기까지 
		Infected_KnockFrontLight_F_01_NEW		 , // (86) KnockDown / 왼쪽 뺨맞고 뒤로 자빠지고 일어나기까지 
		Infected_Scared_01						 , // (87) KnockDown / 복부 가격당해서 엎드리면서 뒤로 물러서기
		Infected_Scared_02						 , // (88) KnockDown / 오른뺨맞고 뺨 가린채로 잠깐잠깐 하면서 얼굴 가리면서 뒷걸음질 
		Infected_Scared_03						 , // (89) KnockDown / 왼뺨맞고 뺨 가린채로 잠깐잠깐 하면서 얼굴 가리면서 뒷걸음질 
		Infected_Scared_04						 , // (90) KnockDown / 오른뺨맞고 잠깐 하고 팔뻗은채로 뒷걸음질 
		Infected_Burn_Loop						 , // (91) KnockDown / 비틀비틀 술취한것 + 아니 안취했다고 
		Infected_Electrocute_Loop				 , // (92) Electric / 감전되서 으어어어 하는중 
		Infected_HitHeavyToStun_01_NEW			 , // (93) KnockDown / 턱주가리 맞고 뒤로 이동 
		Infected_HitNormalToStun_01_NEW			 , // (94) KnockDown / 왼뺨 복부 같이맞고 약간 물러섬 
		Infected_Stun_Loop_NEW					 , // (95) Electric / Stun상태 거의 몸이 반으로 접힘  
		Infected_Dodge_F_01_TEMP				 , // (96) Dodge / 앞으로 갑자기 튀어나감 
		Infected_Dodge_L_01_TEMP				 , // (97) Dodge / 왼쪽으로 갑자기 튀어나감 
		Infected_Dodge_R_01_TEMP				 , // (98) Dodge / 오른쪽으로 갑자기 튀어나감 
		Infected_Hit_whip_end_down				 , // (99) KnockDown / 대자로 공중에서 내려오다가 바닥에 닿아서 퍽 하고 일어남 
		Infected_HitLightOpened_L_01			 , // (100) Hit / 왼쪽으로 렌티어 리볼버 첫무기 한타에 맞았을때 
		Infected_HitLightOpened_R_01			 , // (101) Hit / 오른쪽으로 렌티어 리볼버 첫무기 한타에 맞았을때 
		Infected_HitNormal_F_03_NEW				 , // (102) Hit / 턱맞고 뒤로 두걸음
		Infected_KnockFrontCannonball_F_01_TEMP	 , // (103) Hit / 캐논볼 맞고 뒤로 날라감 
		Infected_MeleeDynamic_Dodge_B_4m		 , // (104) Dodge / 뒤로 엄청 멀리 회피함 
		Infected_OpenStatePull_F_01				 , // (105) Move / 물속? 에서 움직이는듯함 앞으로 
		Infected_OpenStateTimeout				 , // (106) Hit / 오른뺨 잠깐 찌릿 
		Infected_SpawnGround					 , // (107) Spawn / 땅속에서 기어나옴 
		Infected_VortexGrenade_Loop				 , // (108) Hit / 소용돌이 폭탄 ??? 루프상태임 
		Infected_VortexGrenade_Stop				 , // (109) Hit / 소용돌이 폭탄이 끝나서 안으로 글려들어가는거인듯? 
		Infected_KnockUp_Low_Fixed				
	};
};

END

