#pragma once
#include "VIBuffer_Instancing.h"

BEGIN(Engine)

// 안씀 -> CVIBuffer_Particle로 사용합니다.
class ENGINE_DLL CVIBuffer_Particle_Point final : public CVIBuffer_Instancing
{
public:
	enum TYPE_ACTION { SPHERE, SPARK, FALL, RISE, BLINK, TORNADO, TYPE_ACTION_END };
	enum TYPE_FADE { FADE_NONE, FADE_OUT, FADE_IN, TYPE_FADE_END };

	typedef struct tagParticleBufferDesc
	{
		TYPE_ACTION		eType_Action = { SPHERE };
		TYPE_FADE		eType_Fade = { FADE_NONE };
		EASING_TYPE		eType_ColorLerp = { EASING_TYPE::LINEAR };

		_bool		bLoop = { TRUE };
		_bool		bReverse = { FALSE };

		/* 파티클 입자 개수 */
		_int			  iCurNumInstance = { 200 };

		/* LifeTime */
		_float2		vMinMaxLifeTime = { 0.1f, 3.f };

		/* For.Position */
		_float2		vMinMaxRange = { 0.1f, 3.f };
		_float2		vMinMaxRangeLength = { 0.1f, 5.f };

		_float4		vCenterPosition = { 0.f, 0.f, 0.f, 1.f };

		/* For.Speed */
		_float2		vMinMaxSpeed = { 0.1f, 1.f };

		/* 가속도 */
		_float		fSpeedAcc = { 1.f };
		_float		fAccPosition = { 0.1f };

		/* For.Gravity */
		_bool		bUseGravity = { FALSE };
		_float		fGravityAcc = { -5.f };
		_float		fUseGravityPosition = { 0.1f };

		/* For.Rotation */
		_float2		vMinMaxRotationOffsetX = { 0.0f, 360.f };
		_float2		vMinMaxRotationOffsetY = { 0.0f, 360.f };
		_float2		vMinMaxRotationOffsetZ = { 0.0f, 360.f };
		_float3		vRotationOffset = { 0.f, 0.f, 0.f };

		_float3     vCurrentRotation;
		_float3		vMinMaxRotationForce;

		/* For.Scale */
		_float2     vMinMaxScale;
		_float2     vAddScale;
		_float2		vCurrentScale;

		/* For.Color */
		_float2     vMinMaxRed		= { 1.f, 1.f };
		_float2     vMinMaxGreen	= { 1.f, 1.f };
		_float2     vMinMaxBlue		= { 1.f, 1.f };
		_float2     vMinMaxAlpha	= { 1.f, 1.f };

		_float4     vCurrentColor = { 1.f, 1.f, 1.f, 1.f };


		void Reset_Desc()
		{
			ZeroMemory(this, sizeof(PARTICLE_BUFFER_DESC));

		}

	}PARTICLE_BUFFER_DESC;



private:
	CVIBuffer_Particle_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Particle_Point(const CVIBuffer_Particle_Point& rhs);
	virtual ~CVIBuffer_Particle_Point() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iNumInstance);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(_float fTimeDelta) override;

public:
	void ReSet();

public:
	virtual _bool Write_Json(json& Out_Json)		override;
	virtual void Load_FromJson(const json& In_Json)	override;

public:
	void	Sort_Z(_uint iCount);

	/* For.Desc */
public:
	PARTICLE_BUFFER_DESC* Get_Desc() { return &m_tBufferDesc; }

public:
	// 파티클 입자 개수
	_uint			Get_NumInstance() { return m_iNumInstance; }
	void			Set_NumInstance(_uint iNum) { m_iNumInstance = iNum; }

	_float			Get_TimePosition() { return m_fTimeAcc; }
	void			Set_TimePosition(_float fTimePosition) { m_fTimeAcc = fTimePosition; }

	
private:
	_float* m_pLengths		= { nullptr };

private:
	PARTICLE_BUFFER_DESC		m_tBufferDesc = {};
	//vector<PARTICLE_INFO_DESC>		m_vecParticleInfoDesc;
	//vector<PARTICLE_SHADER_DESC>	m_vecParticleShaderDesc;
	//vector<PARTICLE_RIGIDBODY_DESC> m_vecParticleRigidbodyDesc;

public:
	static CVIBuffer_Particle_Point* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumInstance);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END