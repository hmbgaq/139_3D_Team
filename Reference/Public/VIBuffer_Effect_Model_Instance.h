#pragma once
#include "VIBuffer_Model_Instance.h"


BEGIN(Engine)
class CMesh;

class ENGINE_DLL CVIBuffer_Effect_Model_Instance : public CVIBuffer_Model_Instance
{
public:
	typedef struct tagVIBuffer_EffectModelInstanceDesc
	{
		// 저장해야 하는 고정 정보들
		_int			iCurNumInstance = { 1000 };		// 초기화 값이 최대 개수가 됨	
		class CModel*	pModel	= { nullptr };

		/* RigidBody */
		_bool		bUseRigidBody = { TRUE };
		_bool		bKinetic	= { TRUE };		// 키네틱, 즉 TRUE면 속도 계산 함)
		_bool		bUseGravity = { TRUE };
		FORCE_MODE	eForce_Mode = { FORCE_MODE::IMPULSE };

		_float		fGravity		= { -9.8f };	// 중력 가속도
		_float		fFriction		= { 0.1f };		// 마찰 계수
		_float		fSleepThreshold = { 0.05f };	// 슬립 한계점
		_byte		byFreezeAxis	= { 0 };		// 축 고정 확인용 바이트

		_float2		vMinMaxPower = { 0.1f, 250.f };


		/* For.Position */
		_float4		vCenterPosition = { 0.f, 0.f, 0.f, 1.f };
		_float2		vMinMaxRange	= { 0.1f, 3.f };

		/* For.Rotation */
		_float2		vMinMaxRotationOffsetX = { 0.0f, 360.f };
		_float2		vMinMaxRotationOffsetY = { 0.0f, 360.f };
		_float2		vMinMaxRotationOffsetZ = { 0.0f, 360.f };


		void Reset_Desc()
		{
			ZeroMemory(this, sizeof(EFFECT_MODEL_INSTANCE_DESC));
		}

	}EFFECT_MODEL_INSTANCE_DESC;


	typedef struct tagParticleRigidbodyDesc
	{
		// 업데이트 돌면서 변하는 정보들(저장X)
		_bool			bSleep = { FALSE };

		_float3			vAccel = { 0.f, 0.f, 0.f };		// 가속도
		_float3			vVelocity = { 0.f, 0.f, 0.f };	// 속도

		_float			fMass = { 10.f };				// 질량

	} PARTICLE_RIGIDBODY_DESC;

private:
    CVIBuffer_Effect_Model_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CVIBuffer_Effect_Model_Instance(const CVIBuffer_Effect_Model_Instance& rhs);
    virtual ~CVIBuffer_Effect_Model_Instance() = default;

public:
	virtual _bool Write_Json(json& Out_Json)		override;
	virtual void Load_FromJson(const json& In_Json)	override;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Bind_VIBuffers(_uint iMeshContainerIndex);
	virtual void		Update(_float fTimeDelta) override;

	void				Init_Instance(_int iNumInstance) override;
	virtual				HRESULT	Render(_int iMeshIndex) override;

public:
	void ReSet();
	void ReSet_Info(_uint iNum);

	/* For.RigidBody */
public:
	_float3 Update_Kinetic(_uint iNum, _float fTimeDelta);
	void	Update_Kinematic(_uint iNum);


	void	Add_Force(_uint iNum, _fvector vForce, FORCE_MODE eMode);
	void	Clear_Force(_uint iNum, const FORCE_MODE& eMode);
	void	Clear_Power(_uint iNum);


	const _bool	Check_Sleep(_uint iNum);
	void		Sleep(_uint iNum) { Clear_Power(iNum); m_vecParticleRigidbodyDesc[iNum].bSleep = TRUE; }
	void		Wake(_uint iNum) { m_vecParticleRigidbodyDesc[iNum].bSleep = FALSE; }

	void		Set_FreezeAxis(_uint iNum, AXIS eAxis) { m_tBufferDesc.byFreezeAxis ^= 1 << (_int)eAxis; }
	_bool		Is_FrozeAxis(_uint iNum, AXIS eAxis) { return m_tBufferDesc.byFreezeAxis & 1 << (_int)eAxis; }


public:
	EFFECT_MODEL_INSTANCE_DESC* Get_Desc() { return &m_tBufferDesc; }

private:
	EFFECT_MODEL_INSTANCE_DESC			m_tBufferDesc;
	vector<PARTICLE_RIGIDBODY_DESC>		m_vecParticleRigidbodyDesc;

public:
	static CVIBuffer_Effect_Model_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END