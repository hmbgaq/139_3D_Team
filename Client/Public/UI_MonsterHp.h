#pragma once
#include "Client_Defines.h"
#include "UI_Base.h"

BEGIN(Client)

/* 체력 */
class CUI_MonsterHp final : public CUI_Base
{
public:
	enum MONSTERTYPE { COMMON, BOSS, NONE };

	typedef struct tagMonsterHPDesc : public CUI_Base::UI_DESC
	{
		CTransform* pOwnerTransform;
		//STATUS_DESC* pOwnerStatus;
		MONSTERTYPE		eMonsterType;
		_float			fCrntHPUV = 1.0f;
		_float			fPrevHPUV = 1.0f;
		_float			fScreenPosXOffset = 1.f;
	}MONSTER_HP;

private:
	CUI_MonsterHp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_MonsterHp(const CUI_MonsterHp& rhs);
	virtual ~CUI_MonsterHp() = default;

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
	MONSTER_HP				m_tInfo = {};
	_float					m_fCurrentHp;
	_float					m_fPrevHp;
	/*_float					m_fHPBarTimeDuration = 100.f;*/

	const _float			m_fDefaultScale = 0.5f;
	_float					m_fScaleOffsetX = 1.f;


	_float					m_fScaleOffsetY = 1.f;

	_float					m_fPosXOffset = 0.f;
	_float					m_fPosYOffset = 0.f;
	_float					m_fOwnerCamDistance;

public:
	static CUI_MonsterHp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext); //! 원형객체 생성
	virtual CGameObject* Clone(void* pArg) override; //! 사본객체 생성
	virtual void			Free() override;
};

END


