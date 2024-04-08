#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)

class CCharacter;

END

BEGIN(Client)

class CPlayer;
class CData_Manager;

class CSpringCamera final : public CCamera
{
public:
	typedef struct tagSpringCameraDesc : public tagCameraDesc
	{
		_float		fMouseSensor = 0.0f;
	}SPRING_CAMERA_DESC;

private:
	CSpringCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CSpringCamera(const CSpringCamera& rhs);
	virtual ~CSpringCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	virtual HRESULT Ready_Components();

public:
	virtual _bool Write_Json(json& Out_Json) override;

public:
	void CameraRotation(_float fTimeDelta);
	void StartCameraRotation(_fmatrix StartRotationMatrix);
	void Lock_On(_float fTimeDelta);

public:
	void RotatePlayer();
	void Mouse_Fix();
	void Reset_Angle() { 
		//m_fAngle = 0.f; 
		m_fPitch = 0.f;
	};
	_float Get_Pitch() { return m_fPitch; };

public:
	void Set_CameraOffset(_float3 _CameraOffset); /*{ m_CameraOffset = _CameraOffset; }*/
	_float3 Get_CameraOffset() { return m_CameraOffset; }

	void Set_pPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer;} 
	void Set_pTarget(CTransform* _pTarget) { m_ptarget = _pTarget; }
	CTransform* Get_pTarget() { return m_ptarget; }

	void Set_pTargetCharacter(CCharacter* _pCharacter);
	CCharacter* Get_pTargetCharacter() { return m_pCharacter; }

	//카메라 타겟 포지션 ! 
	void Set_TargetPosition(_float3 _TargetPosition) { m_TargetPosition = _TargetPosition; }
	_float3 Get_TargetPosition() { return m_TargetPosition; }

	void Lerp_CameraPosition(_float fTimeDelta);
	//스프링카메라 탄력 조정 
	void Set_SpringConstant(_float _SpringConstant) { SpringConstant = _SpringConstant; }
public:
	void Shake_Camera(_float fTimeDelta);
	void Set_ShakeCamera(_bool _bShake) { m_bShake = _bShake; }
	_bool Get_ShakeCamera() { return m_bShake; }

	void Set_ShakeCameraTime(_float _Shaketime) { m_fShakeTime = _Shaketime; }
	void Set_ShakeCameraMinMax(_float2 _fShakeMinMax) { m_fShakeMinMax = _fShakeMinMax; }

private:
	// 	수평 , 수직 수행거리
	_float hDist = 0.f;
	_float vDist = 0.f;
	_float SpringConstant = 1000.f; //용수철 상수(spring constant)가 커지면 스프링의 탄력이 줄어든다.	//시작값은 카메라를 움직이고 싶은 범위에 따라 달라진다.
	_float DampConstant = 0.f;//용수철 상수를 바탕으로 한 감쇠(dampening)상수
	_float3 Velocity = {};
	_float3 ActualPosition = {};//속도벡터와 카메라의 실제 위치를 나타내는 벡터
	_float3 PreActualPosition = {};//카메라 보간을 위해 이전 프레임 포지션가져옴
	_float3 NewTargetPosition = {};
	_float3 CameraTickPos = {};// tick 에서 값이 자꾸 이상하게 초기화되서 이걸로 다시 값을 맞춰줘야함 
	CTransform* m_ptarget = { nullptr };//카메라가 따라다닐 타깃 오브젝트 //타깃 오브젝트는 위치, 방향벡터 , 타깃의 위쪽을 가리키는 벡터를 지닌다.
	CPlayer* m_pPlayer = { nullptr };
	CCharacter* m_pCharacter = { nullptr };
	_float	m_fAngle = 0.f;
	_float  m_fPitch = 0.f;
	_matrix cameraMatrix;//최종 카메라 행렬
	//_bool m_bCheck = false;//마우스 가운데 모으기 
	//_bool m_bFix = true;//마우스 가운데 모으기
	_bool m_bPlayerCheck = false; // 카메라 타겟이 플레이어 인지 아닌지  확인 하기 위 한 용도  
	_bool m_bChangeOffset = false;

	//shake camera
	_bool m_bShake = false;// 카메라를 흔들 것인지 아닌것인지 
	_float m_fShakeTime = 0.2f;
	_float2 m_fShakeMinMax = { 0.3f ,0.3f};
	//_float m_fTimeDelta = 0.f;

	// !SH Add
	CData_Manager* m_pDataManager = nullptr;
private:
	//카메라 X,Y,Z
	_float3 m_CameraOffset = {};
	_float3 m_TargetPosition = {};
	_float CameraMoveSpeed = 1.f;//카메라 보간용 이동속도
	_float CameraMoveSpeedtest = 0.1f;//카메라 보간용 이동속도
private:
	_float				m_fMouseSensor = { 0.0f };
public:
	static CSpringCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool();
	virtual void Free() override;

};

END