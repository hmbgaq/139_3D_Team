#pragma once

#include "Character_Client.h"
#include "Actor.h"

BEGIN(Engine)
END

BEGIN(Client)
class CSpringCamera;


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

	

public:
	void Start_CutSceneHawk();
	void Stop_CutSceneHawk();
	void Set_HawkMovePoints(vector<_float4>& vecHawkMovePoints) { m_vecMovePoints = vecHawkMovePoints; }
	void Set_HawkCamAtPoints(vector<_float4>& vecCamAtPoints);
	void Spline_Move_LogicFunction(const _float fTimeDelta);
	void Spline_Move_Function(const _float fTimeDelta);

//! For MapTool
	void Reset_Matrix() { m_pTransformCom->Set_WorldMatrix(m_vPreCutSceneMatrix);}
	void Set_SplineSpeed(_float fSplineSpeed) { m_fSplineMoveSpeed = fSplineSpeed;}
	

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();


private:
	CActor<CHawk>* m_pActor = { nullptr };


private: //! 승용
	_bool			m_bFindPlayer = false;
	_bool			m_bCutSceneHawk = false;

	vector<_float4> m_vecMovePoints;
	_float			m_fSplineTimeAcc = 0.f;
	_float			m_fSplineMoveSpeed = 12.f;
	_bool			m_bFixMove = false;
	_bool			m_bOnceMove = false;
	_int			m_iCurrentMovePoint = 0;

	_float4x4		m_vPreCutSceneMatrix = {}; //! 컷신 호크가 시작되기 직전의 월드행렬을 기억해둬서 툴에서 리셋하게해주자.

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

