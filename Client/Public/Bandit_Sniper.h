#pragma once

#include "Client_Defines.h"
#include "Monster_Character.h"
#include "Actor.h"

BEGIN(Client)

class CBandit_Sniper final : public CMonster_Character
{

private:
	CBandit_Sniper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CBandit_Sniper(const CBandit_Sniper& rhs);
	virtual ~CBandit_Sniper() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	
public:
	_bool Get_ProtectExist() { return m_bProtectExist; }
	_float3 Get_BonePos() { return m_pWeapon->Get_Position(); }

	/* For. Bullet */
	_int Get_BulletCnt() { return m_iBulletCnt; }
	void Set_BulletCnt(_int iCnt) { m_iBulletCnt = iCnt; }
	void Add_BulletCnt() { m_iBulletCnt += 1; }


private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Ready_Option();

private:
	_bool					m_bProtectExist		= {};
	_bool					m_bTaunt = false;
	_int					m_iBulletCnt = 0;

	CActor<CBandit_Sniper>* m_pActor			= { nullptr };
	CWeapon*				m_pWeapon			= { nullptr };

public:
	virtual void Hitted_Left(Power ePower)	override;
	virtual void Hitted_Right(Power ePower) override;
	virtual void Hitted_Front(Power ePower) override;
	virtual void Hitted_Knock(_bool bIsCannonball) override;
	virtual void Hitted_Dead(Power ePower)	override;

public:
	static CBandit_Sniper* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;
	virtual void Free() override;

public:
	enum Bandit_Sniper_State
	{
		Sniper_Ironsights_Reload					 , //  (0) Reload - 하나씩 총 5발 재장전
		Sniper_Melee_01								 , //  (1) (Melee)CloseAttack - 총 왼손에 들고 오른손으로 제자리 휘두르기 
		Sniper_Melee_02								 , //  (2) (Melee)CloseAttack - 총 왼손에 들고 오른손으로 제자리 휘두르기(강)
		Sniper_Melee_03								 , //  (3) (Melee)CloseAttack - 총들고 발로차기 
		Sniper_MeleeDynamic_01						 , //  (4) (Melee)CloseAttack - 한발나가서 총 왼손에 들고 오른손으로 휘두르기 
		Sniper_MeleeDynamic_02						 , //  (5) (Melee)CloseAttack - 한발나가서 총을 양손으로 쥐고 총신으로 때리기 
		Sniper_MeleeDynamic_03						 , //  (6) (Melee)CloseAttack - 한발 나가서 총을 양손으로 쥐로 발로 차기 
		Sniper_CoverHigh_Idle						 , //  (7) (Cover)Sniping - 총을 양손으로 쥔채 제자리에서 숨고르기 
		Sniper_CoverHigh_Reload						 , //  (8) (Cover)Sniping - 리볼버처럼 한번에 여러발 재장전 
		Sniper_CoverHigh_Side_L_AimPose				 , //  (9) (Cover)Sniping - 몬스터 기준 서서 왼쪽으로 총쏘는 연결동작(2)   - 겨누는중 (정지)
		Sniper_CoverHigh_Side_L_Start				 , // (10) (Cover)Sniping - 몬스터 기준 서서 왼쪽으로 총쏘는 연결동작(1)   - 살짝 이동
		Sniper_CoverHigh_Side_L_Stop				 , // (11) (Cover)Sniping - 몬스터 기준 서서 왼쪽으로 총쏘는 연결동작(3)   - 제자리로
		Sniper_CoverHigh_Side_R_AimPose				 , // (12) (Cover)Sniping - 몬스터 기준 서서 오른쪽으로 총쏘는 연결동작(2) - 겨누는중 (정지)
		Sniper_CoverHigh_Side_R_Start				 , // (13) (Cover)Sniping - 몬스터 기준 서서 오른쪽으로 총쏘는 연결동작(1) - 살짝 이동
		Sniper_CoverHigh_Side_R_Stop				 , // (14) (Cover)Sniping - 몬스터 기준 서서 오른쪽으로 총쏘는 연결동작(3) - 제자리로
		Sniper_CoverLow_Idle						 , // (15) (Cover)Sniping - 수그리고 숨고르기
		Sniper_CoverLow_Over_AimPose				 , // (16) (Cover)Sniping - 몬스터 기준 서서 정면으로 총쏘는 연결동작(2) - 겨누는중 (정지)
		Sniper_CoverLow_Over_Start					 , // (17) (Cover)Sniping - 몬스터 기준 서서 정면으로 총쏘는 연결동작(1) - 살짝 이동
		Sniper_CoverLow_Over_Stop					 , // (18) (Cover)Sniping - 몬스터 기준 서서 정면으로 총쏘는 연결동작(3) - 제자리로
		Sniper_CoverLow_Reload						 , // (19) (Cover)Sniping - 앉은상태에서 리볼버처럼 다섯발 재장전
		Sniper_CoverLow_Side_L_AimPose				 , // (20) (Cover)Sniping - 몬스터 기준 앉아있다가 왼쪽으로 서서 총쏘는 연결동작(2)   - 겨누는중 (정지)
		Sniper_CoverLow_Side_L_Start				 , // (21) (Cover)Sniping - 몬스터 기준 앉아있다가 왼쪽으로 서서 총쏘는 연결동작(1)   - 살짝 이동
		Sniper_CoverLow_Side_L_Stop					 , // (22) (Cover)Sniping - 몬스터 기준 앉아있다가 왼쪽으로 서서 총쏘는 연결동작(3)   - 제자리로
		Sniper_CoverLow_Side_R_AimPose				 , // (23) (Cover)Sniping - 몬스터 기준 앉아있다가 오른쪽으로 서서 총쏘는 연결동작(2)   - 겨누는중 (정지)
		Sniper_CoverLow_Side_R_Start				 , // (24) (Cover)Sniping - 몬스터 기준 앉아있다가 오른쪽으로 서서 총쏘는 연결동작(1)   - 살짝 이동
		Sniper_CoverLow_Side_R_Stop					 , // (25) (Cover)Sniping - 몬스터 기준 앉아있다가 오른쪽으로 서서 총쏘는 연결동작(3)   - 제자리로
		Sniper_Crouch_Idle							 , // (26) (Cover)Sniping - 몬스터 기준 서있다가 정면으로 앉아서 총쏘는 연결동작(2)   - 겨누는중 (정지)
		Sniper_Crouch_Start							 , // (27) (Cover)Sniping - 몬스터 기준 서있다가 정면으로 앉아서 총쏘는 연결동작(1)   - 살짝 이동
		Sniper_Crouch_Stop							 , // (28) (Cover)Sniping - 몬스터 기준 서있다가 정면으로 앉아서 총쏘는 연결동작(3)   - 제자리로
		Sniper_Crouch_Turn_90_L						 , // (29) (Cover)Sniping - 앉아서 겨누고 있다가 왼쪽으로 90도 회전 
		Sniper_Crouch_Turn_90_R						 , // (30) (Cover)Sniping - 앉아서 겨누고 있다가 오른쪽으로 90도 회전 
		Sniper_Idle									 , // (31) Idle - 서서 숨고르는중
		Sniper_IdlePose								 , // (32) Idle - 2프레임짜리 중간연결용 
		Sniper_Run_B								 , // (33) Movement - 뒤로 달리기 
		Sniper_Run_BL								 , // (34) Movement - 왼쪽을 뒷걸음질로 달리기 
		Sniper_Run_BR								 , // (35) Movement - 오른쪽을 뒷걸음질로 달리기
		Sniper_Run_F								 , // (36) Movement - 정면 달리기 
		Sniper_Run_FL								 , // (37) Movement - 왼쪽 달리기 
		Sniper_Run_FR								 , // (38) Movement - 오른쪽 달리기 
		Sniper_Sprint_F								 , // (39) Movement - 정면 질주 
		Sniper_Taunt_01								 , // (40) (Cover)Sniping - 총쏜뒤에 하는 도발(1)
		Sniper_Taunt_02								 , // (41) (Cover)Sniping - 총쏜뒤에 하는 도발(2)
		Sniper_Taunt_03								 , // (42) (Cover)Sniping - 총쏜뒤에 하는 도발(3) - 잘안보임
		Sniper_Turn_L180							 , // (43) Movement - 왼쪽으로 180도 뒤돌기 
		Sniper_Turn_L90								 , // (44) Movement - 왼쪽으로 90도 회전 
		Sniper_Turn_R180							 , // (45) Movement - 오른쪽으로 180도 뒤돌기 
		Sniper_Turn_R90								 , // (46) Movement - 오른쪽으로 90도 회전
		Sniper_Walk_B								 , // (47) Movement - 뒤로 걷기 
		Sniper_Walk_BL								 , // (48) Movement - 뒷걸음질로 왼쪽으로 걷기 
		Sniper_Walk_BR								 , // (49) Movement - 뒷걸음질로 오른쪽으로 걷기 
		Sniper_Walk_F								 , // (50) Movement - 정면으로 걷기 
		Sniper_Walk_FL								 , // (51) Movement - 왼쪽으로 걷기 
		Sniper_Walk_FR								 , // (52) Movement - 오른쪽으로 걷기 
		Sniper_WalkAim_B							 , // (53) Movement - 에임 고정한채 뒤로 걷기 
		Sniper_WalkAim_BL							 , // (54) Movement - 에임 고정한채 뒷걸음질로 왼쪽으로 걷기 
		Sniper_WalkAim_BR							 , // (55) Movement - 에임 고정한채 뒷걸음질로 오른쪽으로 걷기 
		Sniper_WalkAim_F							 , // (56) Movement - 에임 고정한채 앞으로 걷기 
		Sniper_WalkAim_FL							 , // (57) Movement - 에임 고정한채 왼쪽으로 걷기 
		Sniper_WalkAim_FR							 , // (58) Movement - 에임 고정한채 오른쪽으로 걷기 
		Sniper_DeathLight_B_01						 , // (59) Death - 정면으로 죽음
		Sniper_DeathLight_F_01						 , // (60) Death - 뒤로 엎어지면서 죽음
		Sniper_DeathNormal_B_01						 , // (61) Death - 정면으로 좀 많이 나가면서 죽음
		Sniper_DeathNormal_F_01						 , // (62) Death - 뒤로 좀많이 나가면서 죽음
		Sniper_HitHeavy_B_01						 , // (63) Hit - 앞으로 엎어질듯 말듯 이동하면서 많이 이동한뒤 뒤돌아봄 - 정면으로 맞음
		Sniper_HitHeavy_BL_01						 , // (64) Hit - 앞으로 엎어질듯 말듯 이동하면서 많이 이동한뒤 뒤돌아봄 - 왼쪽으로 맞음
		Sniper_HitHeavy_BR_01						 , // (65) Hit - 앞으로 엎어질듯 말듯 이동하면서 많이 이동한뒤 뒤돌아봄 - 오른쪽으로 맞음
		Sniper_HitHeavy_F_01						 , // (66) Hit - 뒤로 많이 밀려나감 - 정면으로 맞음
		Sniper_HitHeavy_FL_01						 , // (67) Hit - 뒤로 많이 밀려나감 - 왼쪽으로 맞음
		Sniper_HitHeavy_FR_01						 , // (68) Hit - 뒤로 많이 밀려나감 - 오른쪽으로 맞음
		Sniper_KnockFrontLight_B_01					 , // (69) Hit - 앞으로 엎어진뒤 다시 일어나서 뒤돌아봄
		Sniper_KnockFrontLight_F_01					 , // (70) Hit - 왼쪽으로 맞아서 뒤로 넘어진뒤 다시 일어남 - 정면응시 
		Sniper_KnockFrontLight_F_02					 , // (71) Hit - 오른쪽으로 맞아서 뒤로 넘어진뒤 다시 일어남 - 정면응시 
		Sniper_Weakspot_Death_01					 , // (72) Death - 죽을듯 말듯하다가 거의 제자리에서 죽음 
		Sniper_Stun_Loop							 , // (73) Hit - 술취한상태처럼 비틀댐
		Sniper_Burn_Loop_TEMP						 , // (74) Hit - 불에 타는중
		Sniper_CoverHigh_LeanLeft_HitHeavy_01_TEMP	 , // (75) 권총 패스 
		Sniper_CoverHigh_LeanRight_HitHeavy_01_TEMP	 , // (76) 권총 패스 
		Sniper_CoverLow_LeanLeft_HitHeavy_01_TEMP	 , // (77) 권총 패스 
		Sniper_CoverLow_LeanOver_HitHeavy_01_TEMP	 , // (78) 권총 패스 
		Sniper_CoverLow_LeanRight_HitHeavy_01_TEMP	 , // (79) 권총 패스 
		Sniper_Electrocute_Loop_TEMP				   // (80) Hit - 플레이어 전기공격
	};
};

END







