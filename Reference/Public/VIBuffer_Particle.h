#pragma once
#include "VIBuffer.h"


BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Particle final : public CVIBuffer
{
public:
	enum TYPE_ACTION		{ SPARK, BLINK, FALL, RISE, TORNADO, TYPE_ACTION_END };
	enum TYPE_FADE			{ FADE_NONE, FADE_OUT, FADE_IN, TYPE_FADE_END };
	enum TYPE_FADE_TAKES	{ LIFE, DIST, HEIGHT, SCALE, MAGIC, TYPE_FADE_TAKES_END };
	enum TYPE_DIRAXIS		{ DIR_RIGHT, DIR_UP, DIR_LOOK, TYPE_DIRAXIS_END };

	typedef struct tagParticleBufferDESC
	{
		// 저장해야 하는 고정 정보들
		_int		iCurNumInstance = { 1000 };	// 초기화 값이 Max인스턴스 개수가 됨


		/* Types */
		_bool		bRecycle = { TRUE };			// 입자를 재사용 할건지
		_bool		bReverse = { FALSE };			// 진행방향 리버스
		TYPE_ACTION eType_Action = { SPARK };		// 움직이는 모양


		/* Times */
		_float2		vMinMaxLifeTime = { 0.1f, 3.f };

		/* Emitter */
		_float		fEmissionTime = { 0.f };		// 저장 O
		_uint		iAddEmitCount = { 0 };			// 저장 O	한번 방출 할 때 몇개씩 추가로 방출할건지
		_float		fEmissionTimeAcc = { 0.f };		// 저장 X
		_uint		iEmitCount = { 0 };				// 저장 X
		_bool		bEmitFinished = { FALSE };		// 저장 X

		/* RigidBody */
		_bool		bUseRigidBody	= { TRUE };
		_bool		bKinetic		= { TRUE };	// 키네틱, 즉 TRUE면 속도 계산 함)
		_bool		bUseGravity		= { TRUE };
		FORCE_MODE	eForce_Mode		= { FORCE_MODE::IMPULSE };

		_float		fGravity = { -9.8f };			// 중력 가속도
		
		EASING_TYPE	eType_FrictionLerp = { EASING_TYPE::LINEAR };
		_float2		vFrictionLerp_Pos = { 0.f, 0.f };		// 어디서부터 러프를 시작하고, 끝낼건지
		_float2		vStartEnd_Friction = { 0.1f, 0.1f };	// 시작과 끝 마찰 계수
		
		_float		fSleepThreshold = { 0.05f };	// 슬립 한계점
		_byte		byFreezeAxis = { 0 };			// 축 고정 확인용 바이트

		_float2		vMinMaxPower = { 0.1f, 250.f };
		_float2		vMinMaxMass = { 10.f, 10.f };

		EASING_TYPE	eType_SpeedLerp = { EASING_TYPE::LINEAR };
		_float2		vMinMaxSpeed = { 1.f, 1.f };
		_float2		vMinMaxTornadoSpeed = { 1.f, 1.f };

		/* For.Position */
		_float3		vMinCenterOffsetPos = { 0.f, 0.f, 0.f };
		_float3		vMaxCenterOffsetPos = { 0.f, 0.f, 0.f };

		_float2		vMinMaxRange = { 0.1f, 3.f };
		_float2		vMinMaxAddRange = { 0.f, 0.f };
		
		_float2		vMinMaxPosY = { 0.1f, 3.f };	// 파티클이 올라갈 최고 높이
		_float2		vMinMaxTheta = { 0.f, 360.f };	// 3.14f * 2.f


		/* For.Rotation */
		TYPE_DIRAXIS eType_Dir = { DIR_UP };

		_bool		bRotAcc = { FALSE };
		//_float3		vRadian = { 0.f, 0.f, 0.f };
		_float3		vMinRadian = { 0.f, 0.f, 0.f };
		_float3		vMaxRadian = { 0.f, 0.f, 0.f };

		_float2		vMinMaxRadianSpeed_X = { 0.f, 0.f };
		_float2		vMinMaxRadianSpeed_Y = { 0.f, 0.f };
		_float2		vMinMaxRadianSpeed_Z = { 0.f, 0.f };

		_float2		vMinMaxRotationOffsetX = { 0.0f, 360.f };
		_float2		vMinMaxRotationOffsetY = { 0.0f, 360.f };
		_float2		vMinMaxRotationOffsetZ = { 0.0f, 360.f };


		/* For.Scale */
		_bool		bScaleRatio = { TRUE };						// 크기 정비율
		_bool		bUseScaleLerp	= { TRUE };
		EASING_TYPE	eType_ScaleLerp = { EASING_TYPE::LINEAR };
		_float2		vScaleLerp_Up_Pos	= { 0.f, 0.3f };		// 0~1로 보간한 라이프 타임에서 어디서부터 러프를 시작하고, 끝낼건지(커지는 용)
		_float2		vScaleLerp_Down_Pos = { 1.f, 1.f };			// 0~1로 보간한 라이프 타임에서 어디서부터 러프를 시작하고, 끝낼건지(작아지는 용)
		_float2		vMinMaxWidth	= { 1.f, 1.f };
		_float2		vMinMaxHeight	= { 1.f, 1.f };


		/* For.Color */
		EASING_TYPE	eType_ColorLerp = { EASING_TYPE::LINEAR };
		_bool		bDynamic_Color	= { TRUE };
		_float2     vMinMaxRed		= { 1.f, 1.f };
		_float2     vMinMaxGreen	= { 1.f, 1.f };
		_float2     vMinMaxBlue		= { 1.f, 1.f };
		_float2     vMinMaxAlpha	= { 1.f, 1.f };
		_float4     vCurrentColor = { 1.f, 1.f, 1.f, 1.f };	// 색


		TYPE_FADE		eType_Fade = { FADE_NONE };
		TYPE_FADE_TAKES	eType_Fade_Takes = { MAGIC };

		// 업데이트 돌면서 변하는 정보들(저장X)
		_float		fTimeAcc = { 0.f };			// 시간 누적
		_float		fLifeTimeRatio = { 0.f };	/* 라이프타임을 0~1로 보간한 값 */
	

		void Reset_Times()
		{
			fTimeAcc = { 0.f };
			fLifeTimeRatio = { 0.f };

			fEmissionTimeAcc = { 0.f };

			//fUpScaleTimeAcc = { 0.f };
			//fDownScaleTimeAcc = { 0.f };
		}

		void Reset_Desc()
		{
			ZeroMemory(this, sizeof(PARTICLE_BUFFER_DESC));
		}


	}PARTICLE_BUFFER_DESC;

	typedef struct tagParticleDesc
	{
		// 업데이트 돌면서 변하는 정보들(저장X)
		_bool bEmit = { FALSE };
		_bool bDie = { FALSE };

		// 시간
		_float	fTimeAccs = { 0.f };
		_float	fLifeTime = { 1.f };
		_float  fLifeTimeRatios = { 0.f };	/* 라이프타임을 0~1로 보간한 값 */


		// 위치
		_float4	vCenterPositions = { 0.f, 0.f, 0.f, 1.f };
		_float	fMaxRange = { 3.f };
		_float	fAddRange = { 0.f };

		_float	fMaxPosY = { 3.f };


		//  방향 
		_float3	vDir = { 1.f, 0.f, 0.f };

		_float3  vOffsetTheta = { 0.f, 0.f, 0.f };


		// 스피드
		_float	fCurSpeed = { 1.f };
		_float  fCurTheta = { 0.f };
		_float	fCurTornadoSpeed = { 1.f };


		// 회전
		_float3	vRadian = { 0.f, 0.f, 0.f };
		_float3	vCurRadian = { 0.f, 0.f, 0.f };
		_float3	vAddRadianSpeed = { 0.f, 0.f, 0.f };


		// 크기
		_float	fUpScaleTimeAccs	= { 0.f };
		_float	fDownScaleTimeAccs	= { 0.f };

		_float2	vCurScales			= { 1.f, 1.f };
		_float2	vMaxScales			= { 1.f, 1.f };
		

		// 색 (알파)
		_float  fAddAlpha			= { 1.f };


		void Reset_ParticleTimes()
		{
			fTimeAccs = { 0.f };
			fLifeTimeRatios = { 0.f };

			fUpScaleTimeAccs = { 0.f };
			fDownScaleTimeAccs = { 0.f };
		}


	} PARTICLE_INFO_DESC;

	typedef struct tagParticleShaderDesc
	{
		// 16 배수여야함 (64)
		
		// 업데이트 돌면서 변하는 정보들(저장X)
		_float4 vCurrentColors = { 1.f, 1.f, 1.f, 1.f }; // 16

		_float3 vRight	= { 1.f, 0.f, 0.f };		// 12
		_float  fCurAddAlpha = { 1.f };				// 4	

		_float3 vUp			= { 0.f, 1.f, 0.f };	// 12
		_float  fPadding2	= { 0.f };				// 4	

		_float3 vLook		= { 0.f, 0.f, 1.f };	//12
		_float  fPadding3	= { 0.f };				// 4	

	} PARTICLE_SHADER_INFO_DESC;

	typedef struct tagParticleRigidbodyDesc
	{
		// 업데이트 돌면서 변하는 정보들(저장X)
		_bool			bForced = { FALSE };
		_bool			bSleep	= { FALSE };

		_float3			vAccel = {0.f, 0.f, 0.f};		// 가속도
		_float3			vVelocity = { 0.f, 0.f, 0.f };	// 속도

		//_float3			vDir = { 1.f, 0.f, 0.f };

		_float			fMass = { 10.f };				// 질량

		_float			fFriction = { 0.1f };			// 마찰 계수
		_float			fFrictionTimeAccs = { 0.f };

	} PARTICLE_RIGIDBODY_DESC;

private:
	CVIBuffer_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Particle(const CVIBuffer_Particle& rhs);
	virtual ~CVIBuffer_Particle() = default;

public:
	virtual _bool Write_Json(json & Out_Json)		override;
	virtual void Load_FromJson(const json & In_Json)	override;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Bind_VIBuffers();
	void				Update(_float fTimeDelta);

	HRESULT				Init_Instance(_int iNumInstance);
	HRESULT				Render();

public:
	void ReSet();
	void ReSet_Info(_uint iNum);


public:
	_float4 Make_Dir(_uint iNum);
	void	Rotation_Instance(_uint iNum);

	void	Update_DirToAxis(_uint iNum);
	void	Make_DirToRight(_uint iNum);
	void	Make_DirToUp(_uint iNum);
	void	Make_DirToLook(_uint iNum);


	/* For.RigidBody */
public:
	_float3 Update_Kinetic(_uint iNum, _float fTimeDelta);
	void	Update_Kinematic(_uint iNum);


	void	Add_Force(_uint iNum, _fvector vForce, FORCE_MODE eMode);
	void	Clear_Force(_uint iNum, const FORCE_MODE & eMode);
	void	Clear_Power(_uint iNum);


	const _bool	Check_Sleep(_uint iNum);
	void		Sleep(_uint iNum) { Clear_Power(iNum); m_vecParticleRigidbodyDesc[iNum].bSleep = TRUE; }
	void		Wake(_uint iNum) { m_vecParticleRigidbodyDesc[iNum].bSleep = FALSE; }

	void		Set_FreezeAxis(AXIS eAxis) { m_tBufferDesc.byFreezeAxis ^= 1 << (_int)eAxis; }
	_bool		Is_FrozeAxis(AXIS eAxis) { return m_tBufferDesc.byFreezeAxis & 1 << (_int)eAxis; }


public:
	PARTICLE_BUFFER_DESC* Get_Desc() { return &m_tBufferDesc; }
	vector<PARTICLE_SHADER_INFO_DESC>& Get_ParticleShaderInfoDescs() { return m_vecParticleShaderInfoDesc; }

private:
	PARTICLE_BUFFER_DESC				m_tBufferDesc;
	vector<PARTICLE_INFO_DESC>			m_vecParticleInfoDesc;
	vector<PARTICLE_SHADER_INFO_DESC>	m_vecParticleShaderInfoDesc;
	vector<PARTICLE_RIGIDBODY_DESC>		m_vecParticleRigidbodyDesc;



	/* 인스턴스 */
private:
	ID3D11Buffer* m_pVBInstance = { nullptr };
	_uint				m_iInstanceStride = { 0 };
	_uint				m_iNumInstance = { 0 };
	_uint				m_iIndexCountPerInstance = { 0 };

public:
	static CVIBuffer_Particle* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END