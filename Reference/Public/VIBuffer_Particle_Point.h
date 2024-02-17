#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Particle_Point final : public CVIBuffer_Instancing
{
public:
	enum TYPE_ACTION { SPHERE, SPARK, FALL, RISE, BLINK, TORNADO, TYPE_ACTION_END };
	enum TYPE_FADE { FADE_NONE, FADE_OUT, FADE_IN, TYPE_FADE_END };

	typedef struct tagParticlePoint
	{
		TYPE_ACTION		eType_Action = { SPHERE };
		TYPE_FADE		eType_Fade = { FADE_NONE };

		/* 상태 */
		_bool		bActive		= { TRUE };
		_bool		bBillBoard	= { TRUE };

		_bool		bIsPlay		= { TRUE };
		_bool		bReverse	= { FALSE };
		_bool		bLoop		= { TRUE };

		/* LifeTime */
		_float2		vMinMaxLifeTime;

		_float2		vMinMaxSpawnTime;

		/* 파티클 개수 */
		_uint		iCurNumInstance;

		/* For.Position */
		_float2		vMinMaxRange = { 0.1f, 3.f };

		_float4		vCenterPosition;
		_float4		vOffsetPosition;
		_float4		vCurrentPosition;

		_float		fMaxLengthPosition = { 5.f };
		_float		fCurLengthPosition;

		/* For.Speed */
		_float2		vMinMaxSpeed;
		/* 가속도 */
		_float		fAcceleration = { 1.f };
		_float		fAccPosition = { 0.1f };

		/* For.Gravity */
		_float		fGravityAcc;
		_float3		vCurrentGravity;

		/* For.Rotation */
		_float2		vMinMaxRotationOffsetX = { 0.0f, 360.f };
		_float2		vMinMaxRotationOffsetY = { 0.0f, 360.f };
		_float2		vMinMaxRotationOffsetZ = { 0.0f, 360.f };
		_float3		vRotationOffset;

		_float3     vCurrentRotation;
		_float3		vMinMaxRotationForce;

		/* For.Scale */
		_float2     vMinMaxScale;
		_float2     vAddScale;
		_float2		vCurrentScale;

		_float2		vMinMaxScaleForce;

		/* For.Color */
		_float4     vCurrentColor;

		_float4		vColorSpeed;
		_float4		vColorForce;

		/* For.Alpha */
		_float2		fMinMaxAlpha;
		_float		fAlphaForce;

		/* For.Sprite */
		_float2		vSpriteUV;
		_float2     vSpriteUVForce;

		_int		iSpriteFrameIndex = { 1 };

		void Reset_Desc()
		{
			ZeroMemory(this, sizeof(PARTICLE_POINT_DESC));
			bIsPlay = { TRUE };
			bReverse = { FALSE };
			bLoop = { TRUE };
		}

	}PARTICLE_POINT_DESC;

public:
	virtual _bool Write_Json(json& Out_Json)		override;
	virtual void Load_FromJson(const json& In_Json)	override;

private:
	CVIBuffer_Particle_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Particle_Point(const CVIBuffer_Particle_Point& rhs);
	virtual ~CVIBuffer_Particle_Point() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iNumInstance);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(_float fTimeDelta) override;

	//void		Update_Particle(_float fTimeDelta, const _bool _bUseParentMatrix);

public:
	void ReSet();


public:
	_uint			Get_NumInstance() { return m_iNumInstance; }
	void			Set_NumInstance(_uint iNum) { m_iNumInstance = iNum; m_ParticleDesc.iCurNumInstance = iNum; }

	_float			Get_TimePosition() { return m_fTimeAcc; }
	void			Set_TimePosition(_float fTimePosition) { m_fTimeAcc = fTimePosition; }

	/* For.Desc */
public:
		PARTICLE_POINT_DESC* Get_Desc() { return &m_ParticleDesc; }

		void			Set_Type_Action(TYPE_ACTION eType) { m_ParticleDesc.eType_Action = eType; }
		void			Set_Type_Fade(TYPE_FADE eType) { m_ParticleDesc.eType_Fade = eType; }

		void			Set_Loop(_bool bLoop) { m_ParticleDesc.bLoop = bLoop; }

		void			Set_Play(_bool bPlay) { m_ParticleDesc.bIsPlay = bPlay; }

		void			Set_ReversePlay(_bool bReverse) { m_ParticleDesc.bReverse = bReverse; }

		void			Set_LifeTime(_float fMin, _float fMax) { m_ParticleDesc.vMinMaxLifeTime = _float2(fMin, fMax); }

		void			Set_Range(_float fMinRange, _float fMaxFange) { m_ParticleDesc.vMinMaxRange = _float2(fMinRange, fMaxFange); }

		void			Set_MaxLengthPosition(_float fLength) { m_ParticleDesc.fMaxLengthPosition = fLength; };

		void			Set_AddScale(_float fX, _float fY) { m_ParticleDesc.vAddScale = _float2(fX, fY); }

		void			Set_RotationOffset(MINMAX eMinMax, AXIS eAxis, _float fRotationOffset);

		void			Set_Acceleration(_float fAcceleration) { m_ParticleDesc.fAcceleration = fAcceleration; }

		void			Set_AccPosition(_float fAccPosition) { m_ParticleDesc.fAccPosition = fAccPosition; }

		void			Set_Color(_float fRed, _float fGreen, _float fBlue);


private:
	PARTICLE_POINT_DESC			m_ParticleDesc;


	public:
		static CVIBuffer_Particle_Point* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iNumInstance);
		virtual CComponent* Clone(void* pArg) override;
		virtual void Free() override;
};

END