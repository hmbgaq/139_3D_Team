#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CEffect final : public CGameObject
{
public:
	typedef struct tagEffectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_bool	bActive_Tool		= { TRUE };
		_bool	bPlay				= { TRUE };
		_bool	bLoop				= { TRUE };
		_bool	bFinished			= { FALSE };
		_bool	bReverse			= { FALSE };
		_bool	bRender				= { FALSE };

		_int	iPartSize			= { 0 };

		// Times
		EASING_TYPE		eType_Easing = { LINEAR };

		_float	fTimeAcc			= { 0.f };	/* 시간 누적 */
		_float	fWaitingAcc			= { 0.f };	/* 시작 딜레이 시간 누적 */
		_float  fRemainAcc			= { 0.f };
		_float	fSequenceAcc		= { 0.f };	/* 시퀀스 시간 누적 */

		_float	fLifeTime			= { 5.f };
		_float	fWaitingTime		= { 0.f };	/* 이 값이 넘어가야 m_fTimeAcc가 누적되기 시작한다. */
		_float	fRemainTime			= { 0.f };	/* 라이프타임이 지나고, 이 시간이 넘어가야 이펙트 종료. */
		_float	fSequenceTime		= { 0.f };	/* 총 시퀀스 시간(fWaitingTime + fLifeTime + fRemainTime) */

		_float  fLifeTimeRatio		= { 0.f };	/* 라이프타임을 0~1로 보간한 값 */

		// 주인
		CGameObject* pOwner			= { nullptr };
		wstring		 strOwnerTag	= { TEXT("") };
		_bool		 bParentPivot	= { FALSE };
		_float4x4	 matPivot		= {};		/* XMStoreFloat4x4(&m_matPivot, XMMatrixIdentity()) */
		_float4x4	 matOffset		= {};

	}EFFECT_DESC;


private:
	CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CEffect(const CEffect& rhs);
	virtual ~CEffect() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Priority_Tick(_float fTimeDelta)	override;
	virtual void	Tick(_float fTimeDelta)				override;
	virtual void	Late_Tick(_float fTimeDelta)		override;
	virtual HRESULT Render()							override;


public:
	virtual _bool	Write_Json(json& Out_Json) override;
	virtual void	Load_FromJson(const json& In_Json) override;


public:
	virtual _bool Write_Json(json& Out_Json)		override;
	virtual void Load_FromJson(const json& In_Json)	override;

public:
	void	ReSet_Effect();
	void	End_Effect();

public:
	map<const wstring, class CGameObject*>* Get_PartObjects() { return &m_PartObjects; }
	CGameObject* Get_FirstPartObject() { return m_PartObjects.begin()->second; }

	CGameObject*	Find_PartObject(const wstring& strPartTag);
	HRESULT			Add_PartObject(const wstring& strPrototypeTag, const wstring& strPartTag, void* pArg);


	void			Set_Owner(CGameObject* pOwner);
	CGameObject*	Get_Owner() {return m_tEffectDesc.pOwner; }

public:
	EFFECT_DESC* Get_Desc() { return &m_tEffectDesc; }
	_float	Get_EasingTimeAcc() { return m_fEasingTimeAcc; }

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects(const wstring& strPrototypeTag, const wstring& strPartTag, void* pArg);

private:
	EFFECT_DESC		m_tEffectDesc = {};
	_float4x4		m_matCombined = {};
	map<const wstring, class CGameObject*>		m_PartObjects;

	_float m_fEasingTimeAcc = { 0.f };

public:
	/* 원형객체를 생성한다. */
	static CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg)	override;
	virtual CGameObject* Pool()				override;

	virtual void Free() override;
};

END