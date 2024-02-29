#pragma once

#include "VIBuffer_Instancing.h"
#include "Easing_Utillity.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Particle final : public CVIBuffer_Instancing
{
public:
	typedef struct tagLerpDesc 
	{
		/* Time */
		_float		fStartTime	= { 0.f };
		_float		fEndTime	= { 0.f };
		_float		fTimeAcc	= { 0.f };

		/* Value */
		_float3		vStartValue		= { 0.f, 0.f, 0.f };
		_float3		vTargetValue	= { 0.f, 0.f, 0.f };
		_float3		vCurValue		= { 0.f, 0.f, 0.f };

		/* State */
		_bool		bActive			= { FALSE };
		EASING_TYPE	eEasing_Type	= { LINEAR };

		void Set_Start(const _float3 _fStartValue, const _float3& _fTargetValue, const _float& _fEndTime, const EASING_TYPE& _eType = LINEAR)
		{
			if (_fEndTime < 0)
				return;

			bActive = TRUE;

			fTimeAcc = 0.f;
			fEndTime = _fEndTime;

			vStartValue = vCurValue = _fStartValue;
			vTargetValue = _fTargetValue;

			eEasing_Type = _eType;
		}

		void Clear()
		{
			bActive = FALSE;

			fStartTime = fTimeAcc = fEndTime = { 0.f };

			vStartValue = vCurValue = vTargetValue = { 0.f, 0.f, 0.f, 0.f };

			eEasing_Type = { LINEAR };
		}

		_float4 Update_Lerp(const _float& fTimeDelta)
		{
			if (!bActive)
				return vCurValue;

			fTimeAcc += fTimeDelta;

			if (fTimeAcc >= fEndTime)
			{
				vCurValue = vTargetValue;
				bActive = FALSE;
				return vCurValue;
			}
			const _float fTime = Easing::LerpToType(fStartTime, fEndTime, fTimeAcc, fEndTime, eEasing_Type);

			XMStoreFloat3(&vCurValue, XMVectorLerp(vStartValue, vTargetValue, fTime));

			return vCurValue;
		}
	}LERP_DESC;

	typedef struct tagParticleBufferDesc
	{
		// 반복 여부
		_bool* pParticleLoop = nullptr;

		// 파티클 개수
		_uint* pNumEffectMaxCount = nullptr;

		// 분포 범위
		_float3* pRange         = nullptr;
		_float2* pRangeDistance = nullptr;

#pragma region 크기
		_bool*   pScaleSameRate = nullptr;
		_float2* pScaleStart    = nullptr;

		_bool* pScaleChange             = nullptr;
		_float2* pScaleChangeStartDelay = nullptr;

		_bool* pScaleChangeRandom = nullptr;
		_float2* pScaleChangeTime = nullptr;

		_bool* pScaleAdd    = nullptr;
		_bool* pScaleLoop   = nullptr;
		_bool* pScaleLoopStart = nullptr;

		_float2* pScaleMin   = nullptr;
		_float2* pScaleMax   = nullptr;
		_float2* pScaleSpeed = nullptr;
#pragma endregion

#pragma region 이동
		_float2* pVelocitySpeed = nullptr;

		_float3* pVelocityMinStart = nullptr;
		_float3* pVelocityMaxStart = nullptr;

		_bool* pVelocityChange             = nullptr;
		_float2* pVelocityChangeStartDelay = nullptr;

		_bool* pVelocityChangeRandom = nullptr;
		_float2* pVelocityChangeTime = nullptr;

		_bool* pVelocityLoop     = nullptr;
		_uint* pVelocityCountCur = nullptr;
		_uint* pVelocityCountMax = nullptr;

		_uint* pVelocityUse = nullptr;
		_float3* pVelocityMin  = nullptr;
		_float3* pVelocityMax  = nullptr;
		_float2* pVelocityTime = nullptr;
#pragma endregion

#pragma region 회전
		_bool* pBillboard = nullptr;

		_bool* pRandomAxis = nullptr;
		_vector* pAxis = nullptr;

		_bool* pRandomAngle = nullptr;
		_float* pAngle = nullptr;

		_bool* pRotationChange = nullptr;
		_float2* pRotationChangeStartDelay = nullptr;

		_bool* pRotationChangeRandom = nullptr;
		_float2* pRotationChangeTime = nullptr;

		_bool* pRotationAdd = nullptr;

		_float2* pRotationSpeed = nullptr;
#pragma endregion

		// 지속 시간
		_float2* pLifeTime = nullptr;

		// 박스 범위
		_bool* pUseBox = nullptr;
		_float3* pBoxMin = nullptr;
		_float3* pBoxMax = nullptr;

		// 텍스처
		_bool* pRandomStartIndex = nullptr;
		_float2* pUVIndex    = nullptr;
		_float2* pUVMaxCount = nullptr;

		// 애니메이션
		_bool* pAnimation        = nullptr;
		_bool* pAnimationLoop    = nullptr;
		_float2* pAnimationSpeed = nullptr;

#pragma region 알파
		_float2* pStartAlpha = nullptr;

		_bool* pFadeCreate  = nullptr;
		_bool* pFadeDelete  = nullptr;
		_float2* pFadeSpeed = nullptr;

		_bool* pFadeChange = nullptr;
		_bool* pFadeIn     = nullptr;
		_float2* pFadeChangeStartDelay = nullptr;
#pragma endregion

#pragma region 색상
		_bool* pColorRandom = nullptr;
		_float4* pColorS = nullptr;

		_bool* pColorChange = nullptr;

		_bool* pColorChangeRandom = nullptr;
		_float2* pColorChangeRandomTime = nullptr;

		_bool* pColorLoop   = nullptr;
		_float2* pColorChangeStartDelay = nullptr;

		_float2* pColorChangeStartM = nullptr;
		_float4* pColorM = nullptr;

		_float2* pColorChangeStartF = nullptr;
		_float4* pColorF = nullptr;

		_float2* pColorDuration = nullptr;
#pragma endregion

#pragma region 블러
		_bool*   pBloomPowerRandom = nullptr;
		_float4* pBloomPower = nullptr;
		_bool*   pBlurPowerRandom = nullptr;
		_float*  pBlurPower = nullptr;
#pragma endregion

#pragma region 리지드바디
		_bool* pRigidbody   = nullptr;
		_bool* pGravity     = nullptr;
		_bool* pStopZero    = nullptr;
		_bool* pStopStartY  = nullptr;
		_bool* pGroundSlide = nullptr;

		_float4* pMaxVelocity = nullptr;
		_float* pMass      = nullptr;
		_float* pFricCoeff = nullptr;
#pragma endregion

	} PARTICLE_BUFFER_DESC;

public:
	typedef struct tagParticleDesc
	{
		_bool bIsDie = false;

		// 크기
		_float fScaleChangeStartTime;
		_float fScaleChangeStartDelay;
		_float fScaleTimeAccs;
		_float fScaleChange;
		_bool  bScaleAdd;
		_float fScaleSpeed;

		// 위치
		_float  fVelocityChangeStartTime;
		_float  fVelocityChangeStartDelay;
		_float  fVelocityRanTimeAccs;
		_float  fVelocityRanChange;
		_float  fVelocitySpeeds;
		_float4 vVelocity;
		_uint   iVelocityCountCur;
		_float  fVeloityTimeAccs;
		_float3* pVelocity       = nullptr;
		_float*  pVelocityChange = nullptr;

		// 회전
		_float  fRotationChangeStartTime;
		_float  fRotationChangeStartDelay;
		_float  fRotationTime;
		_float  fRotationChangeTime;
		_float  fRotationSpeed;

		// 지속 시간
		_float fTimeAccs;       
		_float fLifeTimes;      

		// 애니메이션
		_bool  bAmimationFinish = false;
		_float fAccIndex;
		_float fAnimationSpeed; 

		// 알파
		_bool bFadeCreateSucc;
		_float  fAlphaChangeStartTime;
		_float  fAlphaChangeStartDelay;
		_float  fAlphaSpeed;

		// 색상
		_float  fColorChangeStartTime;
		_float  fColorChangeStartDelay;

		LERP_DESC LerpInfo;
		_uint   iColorIndex;
		_float  fColorChangeStartM;
		_float  fColorChangeStartF;
		_float  fColorChangeEndTime;

		_float  fColorAccs;
		_float  fColorChangeTime;
		_float3 fNextColor;

		_float fViewZ;

	} PARTICLE_INFO_DESC;

	typedef struct tagParticleShaderDesc
	{
		_float2 fUVIndex;  //8
		_float2 fMaxCount; //8

		_float3 fColor; //12
		_float  fAlpha; //4

		_float3 fAxis;  //12
		_float  fAngle; //4

		_float3 fBloomPower; //12
		_float  fBlurPower;  //4

	} PARTICLE_SHADER_DESC;

	typedef struct tagParticleRigidbodyDesc
	{
		_float4 vForce    = {}; // 크기, 방향
		_float4 vAccel    = {}; // 가속도
		_float4 vVelocity = {}; // 속도(크기:속력,방향)

		_float4 vForceA    = {}; // 크기, 방향
		_float4 vAccelA    = {}; // 추가 가속도
		_float4 vVelocityA = {}; // 속도(크기:속력,방향)

	} PARTICLE_RIGIDBODY_DESC;

private:
	CVIBuffer_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Particle(const CVIBuffer_Particle& rhs);
	virtual ~CVIBuffer_Particle() = default;

public:
	vector<PARTICLE_SHADER_DESC>& Get_ParticleShaderInfo() { return m_vecParticleShaderDesc; }
	_bool Get_Finished() { return m_bFinished; }

public:
	void Restart_ParticleBufferDesc(_uint iCount);
	void Sort_Z(_uint iCount);
	void Add_Velocity(_uint iCount, _float4 _vMinVelocity, _float4 _vMaxVelocity);
	void Set_Color(_float3 fColor);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	void Tick(_float fTimeDelta);
	virtual HRESULT Bind_VIBuffers(_uint iCount);
	virtual HRESULT Render();

private:
	ID3D11Buffer* m_pVBInstance = { nullptr };
	VTXINSTANCE* m_pVertices = { nullptr };

	class CGameInstance* m_pGameInstance = { nullptr };

private:
	_bool m_bFinished = { FALSE };

	_uint m_iMaxCount = { 1000 }; // 파티클 최대 개수

	PARTICLE_BUFFER_DESC			m_tParticleDesc;
	vector<PARTICLE_INFO_DESC>		m_vecParticleInfoDesc;
	vector<PARTICLE_SHADER_DESC>	m_vecParticleShaderDesc;
	vector<PARTICLE_RIGIDBODY_DESC> m_vecParticleRigidbodyDesc;

private:
	_float4 Get_NewPosition_Particle();
	_float4 Get_NewVelocity_Particle();
	void Tick_Rigidbody(_float fTimeDelta, _uint iParticleID);

public:
	static CVIBuffer_Particle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free();
};

END