#pragma once
#include "Client_Defines.h"
#include "UI_Base.h"

/* 체력 프레임 */
class CUI_MonsterHpFrame final : public CUI_Base
{
public:
	enum MONSTERTYPE { SMALL, MID, LARGE, BOSS, SIDE, NONE };

	typedef struct tagMonsterHPDesc : public CUI_Base::UI_DESC
	{
		CTransform* pOwnerTransform;
		//STATUS_DESC* pOwnerStatus;
		MONSTERTYPE		eMonsterType = LARGE;
		_float			fCrntHPUV = 1.0f;
		_float			fPrevHPUV = 1.0f;
		_float			fScreenPosXOffset = 1.f;
	}MONSTER_FRAME;

private:
	CUI_MonsterHpFrame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CUI_MonsterHpFrame(const CUI_MonsterHpFrame& rhs);
	virtual ~CUI_MonsterHpFrame() = default;

public:
	virtual HRESULT			Initialize_Prototype() override; //! 원형객체의 초기화를 위한 함수.
	virtual HRESULT			Initialize(void* pArg) override; //! 사본객체의 초기화를 위한 함수. ( void*를 매개인자로 받아 특수한 사본객체 처리가 가능하다. )
	virtual void			Priority_Tick(_float fTimeDelta) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	virtual HRESULT			Ready_Components() override;
	virtual HRESULT			Bind_ShaderResources() override;

private:
	void					Compute_OwnerCamDistance();
	_bool					In_Frustum();

private:
	LEVEL					m_eCurrentLevel = LEVEL_END;

private:
	MONSTER_FRAME			m_tInfo = {};
	_float					m_fCurrentHp;
	_float					m_fPrevHp;
	/*_float				m_fHPBarTimeDuration = 100.f;*/

	const _float			m_fDefaultScale = 0.5f;
	_float					m_fScaleOffsetX = 1.f;


	_float					m_fScaleOffsetY = 1.f;

	_float					m_fPosXOffset = 0.f;
	_float					m_fPosYOffset = 0.f;
	_float					m_fOwnerCamDistance;

public:
	static CUI_MonsterHpFrame* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag); //! 원형객체 생성
	virtual CGameObject*	Clone(void* pArg) override; //! 사본객체 생성
	virtual CGameObject*	Pool() override;
	virtual void			Free() override;

};

