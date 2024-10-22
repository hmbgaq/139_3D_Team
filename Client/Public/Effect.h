#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CEffect_Trail;

class CEffect final : public CGameObject
{
public:
	enum TYPE_DEAD { DEAD_AUTO, DEAD_OWNER, DEAD_NONE, TYPE_DEAD_END };

	typedef struct tagEffectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		/* 저장해야 하는 고정 정보들 */
		TYPE_DEAD eType_Dead		= { DEAD_AUTO };

		_bool	bPlay				= { TRUE };
		_bool	bLoop				= { TRUE };
		_bool	bFinished			= { FALSE };
		_bool	bReverse			= { FALSE };
		_bool	bRender				= { FALSE };

		_int	iPartSize			= { 0 };

		// Times
		_float	fLifeTime			= { 5.f };
		_float	fWaitingTime		= { 0.f };	/* 이 값이 넘어가야 m_fTimeAcc가 누적되기 시작한다. */
		_float	fRemainTime			= { 0.f };	/* 라이프타임이 지나고, 이 시간이 넘어가야 이펙트 종료. */
		_float	fSequenceTime		= { 0.f };	/* 총 시퀀스 시간(fWaitingTime + fLifeTime + fRemainTime) */


		// 주인
		_bool		 bParentPivot	= { FALSE };

		/* 업데이트 되면서 바뀌는 정보들(저장x) */
		_bool	bActive_Tool = { TRUE };

		// 시간 누적
		_float	fTimeAcc = { 0.f };			/* 시간 누적 */
		_float	fWaitingAcc = { 0.f };		/* 시작 딜레이 시간 누적 */
		_float  fRemainAcc = { 0.f };
		_float	fSequenceAcc = { 0.f };		/* 시퀀스 시간 누적 */
		_float  fLerpTimeAcc = { 0.f };		/* 러프 중인 시간누적 */

		_float  fLifeTimeRatio = { 0.f };	/* 라이프타임을 0~1로 보간한 값 */

		// 주인
		// 주인 객체는 게임오브젝트에 있는 것 사용
		_float4x4	 matPivot		= {};	/* XMStoreFloat4x4(&m_tEffectDesc.matPivot, XMMatrixIdentity()); */
		_float4x4	 matCombined	= {};

		_bool		bUseSocket		= { FALSE };
		string		strBoneTag		= { "" };

		_bool		bAttachStatic = { FALSE };


		_bool		bAttachTool = { false };
		_float4x4	matPivot_Tool = {};

		string		strFileName		= { "" };	// Json에 저장X (Json파일 이름 .json포함)


		void Reset_Pivot()
		{
			XMStoreFloat4x4(&matPivot, XMMatrixIdentity());
			XMStoreFloat4x4(&matCombined, XMMatrixIdentity());

			bUseSocket = false;
			strBoneTag = { "" };

			bAttachStatic = false;

			bAttachTool = false;
			XMStoreFloat4x4(&matPivot_Tool, XMMatrixIdentity());
		}


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
	virtual _bool Write_Json(json& Out_Json)		override;
	virtual void Load_FromJson(const json& In_Json)	override;

public:
	void	Update_PivotMat();

public:
	void	ReSet_Effect();
	void	Init_ReSet_Effect();
	void	End_Effect();
	void	End_Effect_ForPool();

	_bool	Get_Finished() { return m_tEffectDesc.bFinished; }


public:
	HRESULT			Ready_Trail(_uint iLevelIndex, string strFileName);
	void			Set_Nullptr_Trail() { m_pTrail = nullptr; }
	void			Set_Trail(CEffect_Trail* pTrail) { m_pTrail = pTrail; };	// 툴 용
	CEffect_Trail*	Get_Trail() { return m_pTrail; }							// 툴 용

public:
	map<const wstring, class CGameObject*>* Get_PartObjects() { return &m_PartObjects; }
	CGameObject* Get_FirstPartObject() { return m_PartObjects.begin()->second; }	// 툴 용


	CGameObject*	Find_PartObject(const wstring& strPartTag);
	HRESULT			Add_PartObject(const wstring& strPrototypeTag, const wstring& strPartTag, void* pArg);
	void			Delete_PartObject(const wstring& strPartTag);


public:
	EFFECT_DESC* Get_Desc() { return &m_tEffectDesc; }

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects(const wstring& strPrototypeTag, const wstring& strPartTag, void* pArg);

private:
	EFFECT_DESC		m_tEffectDesc = {};

	map<const wstring, class CGameObject*>		m_PartObjects;


private:
	CEffect_Trail* m_pTrail = { nullptr };


public:
	/* 원형객체를 생성한다. */
	static CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg)	override;
	virtual CGameObject* Pool()				override;

	virtual void Free() override;
};

END