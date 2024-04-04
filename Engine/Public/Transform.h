#pragma once

#include "Component.h"

/* 객체들의 월드 상태를 표현하기위한 데이터를 가진다. (월드행렬) */
/* 해당 월드 상에서의 변환을 위한 기능을 가진다. */

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	virtual _bool Write_Json(json& Out_Json)			override;
	virtual void  Load_FromJson(const json& In_Json)	override;

public:
	/* 행렬의 행의 정보를 교체한다. */
	void Set_State(STATE eState, const _float4& vState) {
		memcpy(&m_WorldMatrix.m[eState], &vState, sizeof(_float3));
	}
	void Set_State(STATE eState, _fvector vState) {
		m_WorldMatrix.m[eState][0] = XMVectorGetX(vState);
		m_WorldMatrix.m[eState][1] = XMVectorGetY(vState);
		m_WorldMatrix.m[eState][2] = XMVectorGetZ(vState);
		m_WorldMatrix.m[eState][3] = XMVectorGetW(vState);
	}

	_vector Get_State(STATE eState) {
		return XMVectorSet(m_WorldMatrix.m[eState][0], 
			m_WorldMatrix.m[eState][1], 
			m_WorldMatrix.m[eState][2], 
			m_WorldMatrix.m[eState][3]);
	}

	_float3 Get_Scaled() { 
		return _float3(XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_WorldMatrix).r[STATE_RIGHT])),
			XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_WorldMatrix).r[STATE_UP])),
			XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_WorldMatrix).r[STATE_LOOK])));
	}

	_float3 Get_Rotated() //! 승용 추가 수정할거면 물어봐주셈
	{
		_float3 vRotation;

		_float4x4 WorldMatrix = Get_WorldFloat4x4();
		_matrix rotationMatrix = XMMatrixIdentity();

		vRotation.x = asin(WorldMatrix._32); // 피치
		vRotation.y = atan2(-WorldMatrix._31, WorldMatrix._33); // 요
		vRotation.z = atan2(-WorldMatrix._12, WorldMatrix._22); // 롤

		return vRotation;
	}

	_matrix Get_WorldMatrix() {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}
	_float4x4 Get_WorldFloat4x4() {
		return m_WorldMatrix;
	}
	_matrix Get_WorldMatrixInverse() {
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}
	_float4x4 Get_WorldFloat4x4Inverse() {
		_float4x4	InverseMatrix;
		XMStoreFloat4x4(&InverseMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));
		return InverseMatrix;
	}
	void Set_WorldMatrix(_float4x4 matrix) { m_WorldMatrix = matrix; }
	void Set_WorldMatrix(_fmatrix WorldMatrix) { XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix); }
	void Set_Scaling(_float fScaleX, _float fScaleY, _float fScaleZ);

	void Set_Speed(_float fSpeed) { m_fSpeedPerSec = fSpeed; }
	_float Get_Speed() { return m_fSpeedPerSec; }

	void Set_RotationSpeed(_float fRotationSpeed) { m_fRotationPerSec = fRotationSpeed;}
	_float Get_RotationSpeed() { return m_fRotationPerSec;}


	void Set_Position(const _float3& vState)
	{
		_vector vPosVec = XMLoadFloat3(&vState);

		XMStoreFloat4(&m_fPosition, vPosVec);
		Set_State(STATE::STATE_POSITION, m_fPosition);
	}

	_float3 Get_Position() 
	{
		_float3 vPos;
		XMStoreFloat3(&vPos, Get_State(CTransform::STATE::STATE_POSITION));
		return vPos;
	}

	_float4 Get_Position_Float4()
	{
		_float4 vPos;
		XMStoreFloat4(&vPos, Get_State(CTransform::STATE::STATE_POSITION));
		return vPos;
	}

	_float3 Calc_Front_Pos(_float3 vDiff = _float3(0.f, 0.f, 1.f));

public:
	virtual HRESULT Initialize_Prototype(_float fSpeedPerSec, _float fRotationPerSec);	

public:
	void	Move_On_Navigation(_vector vMove, class CNavigation* pNavigation = nullptr);
	void	Move_On_Navigation_ForSliding(_vector vMove, const _float fTimeDelta, class CNavigation* pNavigation = nullptr);

	void	Go_Straight(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void	Go_Straight_L45(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void	Go_Straight_R45(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void	Go_Backward(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void	Go_Backward_L45(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void	Go_Backward_R45(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void	Go_Left(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void	Go_Right(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void	Go_Up(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void	Go_Down(_float fTimeDelta, class CNavigation* pNavigation = nullptr);

	void	Turn(_fvector vAxis, _float fTimeDelta);
	void	Rotation(_fvector vAxis, _float fRadian);
	void RotationToProjectile(_fvector vAxis, _float fRadian);
	_bool	Rotation_Lerp2(_float fRadian, _float fTimeDelta);
	void	Rotation_Quaternion(_float3 vRotation);
	_bool	Rotation_Lerp(_float fRadian, _float fTimeDelta, _float fMinRadian = 1.0f);

	_bool Rotation_LerpTest(_float fRadian, _float fTimeDelta);

	void	Go_Target(_fvector vTargetPos, _float fTimeDelta, _float fSpare = 0.1f);
	_bool	Go_TargetArrivalCheck(_fvector vTargetPos, _double fTimeDelta, _float fSpare = 0.1f);
	void	Look_At(_fvector vTargetPos);
	void	Look_At_OnLand(_fvector vTargetPos);
	void	Look_At_OnLandBoss(_fvector vTargetPos);
	void	Look_At_Direction(_fvector _vLook);
	void	Look_At_Lerp(_fvector vTargetPos, _float fTimeDelta, _float fMinRadian = 0.5f);

	_vector Calc_Look_Dir(_fvector vTargetPos);
	_vector Calc_Look_Dir_XZ(_fvector vTargetPos);


	/* ---------------- 소영 추가사항 ---------------- */
	void Move_Position(_float4 vDir, _float fSpeed, _float fTimeDelta); /* Dir쪽으로 speed만큼 계속해서 이동하는 함수 */

	_vector Get_Right() { return Get_State(CTransform::STATE::STATE_RIGHT); }
	_vector Get_Up() { return Get_State(CTransform::STATE::STATE_UP); }
	_vector Get_Pos() { return Get_State(CTransform::STATE::STATE_POSITION); }
	_vector Get_Look() { return Get_State(CTransform::STATE::STATE_LOOK); }
	
	void Set_Right(const _fvector vState) { Set_State(STATE_RIGHT, vState); }
	void Set_Right(_float _x, _float _y, _float _z) { _float4 Temp; Temp.x = _x; Temp.y = _y; Temp.z = _z; Temp.w = 0.f; Set_State(STATE_RIGHT, Temp); }
	void Set_Up(const _fvector vState) { Set_State(STATE_UP, vState); }
	void Set_Up(_float _x, _float _y, _float _z) { _float4 Temp; Temp.x = _x; Temp.y = _y; Temp.z = _z; Temp.w = 0.f; Set_State(STATE_UP, Temp); }
	void Set_Look(const _fvector vState) { Set_State(STATE_LOOK, vState); }
	void Set_Look(_float _x, _float _y, _float _z) { _float4 Temp; Temp.x = _x; Temp.y = _y; Temp.z = _z; Temp.w = 0.f; Set_State(STATE_LOOK, Temp); }
	void Set_Pos(const _fvector vState) { Set_State(STATE_POSITION, vState); }
	void Set_Pos(const _float3 vState) { _float4 Temp; Temp.x = vState.x; Temp.y = vState.y; Temp.z = vState.z;	Temp.w = 1.f; Set_State(STATE_POSITION, Temp); }
	void Set_Pos(_float _x, _float _y, _float _z) { _float4 Temp; Temp.x = _x; Temp.y = _y; Temp.z = _z; Temp.w = 1.f; Set_State(STATE_POSITION, Temp); }

	void RotationXaxis(_float fTimeDelta) { Turn(XMVectorSet(1.f, 0.f, 0.f, 0.f), fTimeDelta); }
	void RotationYaxis(_float fTimeDelta) { Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta); }
	void RotationZaxis(_float fTimeDelta) { Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), fTimeDelta); }

public:
	void		Add_RootBone_Position(const _float3& vPos, class CNavigation* pNavigation = nullptr);
	void		Add_RootBone_ForTarget(const _float3& vPos, class CNavigation* pNavigation = nullptr, CTransform* pTargetTransform = nullptr);
	void		Add_RootBone_Position(const _float3& vPos, const _float fTimeDelta, class CNavigation* pNavigation = nullptr);

public:
	_bool		Calc_FrontCheck(const _float3& vTargetPos); //! true일시 타겟이 앞에있다 ,  false일시 타겟이 뒤에있다
	_bool		Calc_LeftCheck_ForCamLook(const _float3& vTargetPos); //!  true 일시 카메라 기준오른쪽 , false일시 카메라 기준 왼쪽
	

public:
	_float3		Calculate_SlidingVector(const _fvector& velocity, const _fvector& normal);

public:
	HRESULT		Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);
	
public:
	_bool Is_Ground() { return m_bIsGround; };
	_float Get_GravityAcc() { return m_fGravityAcc; }
	void Update_GravityAcc(_float fTimeDelta) { 
		_float fAcc = (m_fGravityAcc + fTimeDelta * 1.5f);
		m_fGravityAcc = ((true == m_bIsGround) ? 0 : fAcc);
	}
	void Reset_GravityAcc() { m_fGravityAcc = 0.f; };
public:
	_float				m_fRadian = { 0.f };

private:
	_float				m_fSpeedPerSec = { 0.0f };
	_float				m_fRotationPerSec = { 0.0f };

	_float4x4			m_WorldMatrix = {};


	_float4				m_fPosition = {};
	_bool				m_bIsGround = { false };
	_float				m_fGravityAcc = { 0.f };

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _float fSpeedPerSec, _float fRotationPerSec);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

