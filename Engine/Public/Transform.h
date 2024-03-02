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

	_float3 Get_Position_Float4()
	{
		_float4 vPos;
		XMStoreFloat4(&vPos, Get_State(CTransform::STATE::STATE_POSITION));
		return vPos;
	}

public:
	virtual HRESULT Initialize_Prototype(_float fSpeedPerSec, _float fRotationPerSec);	

public:
	void	Move_On_Navigation(_vector vMove, class CNavigation* pNavigation = nullptr);

	void	Go_Straight(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void	Go_Straight_L45(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void	Go_Straight_R45(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void	Go_Backward(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void	Go_Backward_L45(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void	Go_Backward_R45(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void	Go_Left(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void	Go_Right(_float fTimeDelta, class CNavigation* pNavigation = nullptr);


	void	Turn(_fvector vAxis, _float fTimeDelta);
	void	Rotation(_fvector vAxis, _float fRadian);
	_bool	Rotation_Lerp(_float fRadian, _float fTimeDelta);

	void	Go_Target(_fvector vTargetPos, _float fTimeDelta, _float fSpare = 0.1f);
	void	Look_At(_fvector vTargetPos);
	void	Look_At_OnLand(_fvector vTargetPos);
	void	Look_At_Direction(_fvector _vLook);
	void	Look_At_Lerp(_fvector vTargetPos, _float fTimeDelta);

	_vector Calc_Look_Dir(_fvector vTargetPos);
	_float3 Calc_Look_Dir(_float3 vTargetPos);



public:
	void		Add_RootBone_Position(const _float3& vPos, class CNavigation* pNavigation = nullptr);

public:
	_float3		CalculateSlidingVector(const _fvector& velocity, const _fvector& normal);


public:
	HRESULT		Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);
	

private:
	_float				m_fSpeedPerSec = { 0.0f };
	_float				m_fRotationPerSec = { 0.0f };

	_float4x4			m_WorldMatrix = {};

	_float				m_fRadian = { 0.f };

	_float4				m_fPosition = {};

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _float fSpeedPerSec, _float fRotationPerSec);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

