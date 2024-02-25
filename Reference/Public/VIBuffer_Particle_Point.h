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
		EASING_TYPE		eType_Easing = { LINEAR };

		// States
		_bool		bBillBoard = { TRUE };

		_bool		bActive_Tool = { TRUE };
		//_bool		bPlay		 = { TRUE };
		_bool		bLoop		 = { TRUE };
		_bool		bReverse	 = { FALSE };


		/* 파티클 개수 */
		_uint		iCurNumInstance;

		/* LifeTime */
		_float2		vMinMaxLifeTime;

		/* For.Position */
		_float2		vMinMaxRange = { 0.1f, 3.f };

		_float4		vCenterPosition;
		_float4		vOffsetPosition;
		_float4		vCurrentPosition;

		_float2		vMinMaxLengthPosition;

		/* For.Speed */
		_float2		vMinMaxSpeed;
		/* 가속도 */
		_float		fSpeedAcc		= { 1.f };
		_float		fAccPosition	= { 0.1f };

		/* For.Gravity */
		_bool		bUseGravity = { FALSE };
		_float		fGravityAcc = { -5.f };
		_float3		vCurrentGravity;
		_float		fUseGravityPosition = { 0.1f };

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
		_float2     vMinMaxRed		= { 0.f, 1.f };
		_float2     vMinMaxGreen	= { 0.f, 1.f };
		_float2     vMinMaxBlue		= { 0.f, 1.f };
		_float2     vMinMaxAlpha	= { 0.f, 1.f };

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
			//bPlay	 = { TRUE };
			bReverse = { FALSE };
			bLoop	 = { TRUE };
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
	void			Set_NumInstance(_uint iNum) { m_iNumInstance = iNum; m_tBufferDesc.iCurNumInstance = iNum; }

	_float			Get_TimePosition() { return m_fTimeAcc; }
	void			Set_TimePosition(_float fTimePosition) { m_fTimeAcc = fTimePosition; }

	/* For.Desc */
public:
		PARTICLE_POINT_DESC* Get_Desc() { return &m_tBufferDesc; }

	
private:
	_float* m_pLengths	= { nullptr };

	PARTICLE_POINT_DESC			m_tBufferDesc;

	public:
		static CVIBuffer_Particle_Point* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iNumInstance);
		virtual CComponent* Clone(void* pArg) override;
		virtual void Free() override;
};

END