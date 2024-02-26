#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Particle_Point final : public CVIBuffer_Instancing
{
public:
	enum TYPE_ACTION { SPHERE, SPARK, FALL, RISE, BLINK, TORNADO, TYPE_ACTION_END };
	enum TYPE_FADE { FADE_NONE, FADE_OUT, FADE_IN, TYPE_FADE_END };

	typedef struct tagParticleBufferDesc
	{
		TYPE_ACTION*		eType_Action = { nullptr };
		TYPE_FADE*			eType_Fade = { nullptr };
		EASING_TYPE*		eType_ColorLerp = { nullptr };


		_bool*		bLoop		 = { nullptr };
		_bool*		bReverse	 = { nullptr };
		_bool*		bSpriteAnim = { nullptr };


		/* 파티클 개수 */
		_uint*		iCurNumInstance = { nullptr };

		/* LifeTime */
		_float2*		vMinMaxLifeTime = { nullptr };

		/* For.Position */
		_float2*		vMinMaxRange = { nullptr };
		_float2*		vMinMaxRangeLength = { nullptr };

		_float4*		vCenterPosition = { nullptr };

		/* For.Speed */
		_float2*		vMinMaxSpeed = { nullptr };

		/* 가속도 */
		_float*		fSpeedAcc = { nullptr };
		_float*		fAccPosition = { nullptr };

		/* For.Gravity */
		_bool*		bUseGravity = { nullptr };
		_float*		fGravityAcc = { nullptr };
		_float*		fUseGravityPosition = { nullptr };

		/* For.Rotation */
		_float2*		vMinMaxRotationOffsetX = { nullptr };
		_float2*		vMinMaxRotationOffsetY = { nullptr };
		_float2*		vMinMaxRotationOffsetZ = { nullptr };
		_float3*		vRotationOffset = { nullptr };

		_float3*		vCurrentRotation = { nullptr };
		_float3*		vMinMaxRotationForce = { nullptr };

		/* For.Scale */
		_float2*     vMinMaxScale = { nullptr };
		_float2*     vAddScale = { nullptr };
		_float2*	 vCurrentScale = { nullptr };

		/* For.Color */
		_float2*     vMinMaxRed = { nullptr };
		_float2*     vMinMaxGreen = { nullptr };
		_float2*     vMinMaxBlue = { nullptr };
		_float2*     vMinMaxAlpha = { nullptr };

		_float4*     vCurrentColor = { nullptr };

		/* For.Sprite */
		_float*	fSequenceTerm = { nullptr };

		_float2* vTextureSize = { nullptr };		// fSpriteSizeX, fSpriteSizeY
		_float2* vTileSize = { nullptr };			// fAnimationSizeX, fAnimationSizeY

		_int2*	vUV_CurTileIndex = { nullptr };		// iCurrentHor, iCurrentVer
		_int2*	vUV_MinTileCount = { nullptr };		// iMinHor, iMinVer
		_int2*	vUV_MaxTileCount = { nullptr };		// iMaxHor, iMaxVer

		void Reset_Desc()
		{
			ZeroMemory(this, sizeof(PARTICLE_BUFFER_DESC));

		}

	}PARTICLE_BUFFER_DESC;

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
	void			Set_NumInstance(_uint iNum) { m_iNumInstance = iNum; *m_tBufferDesc.iCurNumInstance = iNum; }

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