#pragma once

#include "Imgui_Window.h"

BEGIN(Engine)
class CGameObject;
class CAnimation;
END

BEGIN(Client)

class CWindow_AnimTool final : public CImgui_Window
{
public:
	struct WINDOW_MAPTOOL_DESC : public ImGuiDESC
	{
	};

protected:
	CWindow_AnimTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CWindow_AnimTool() = default;


public:
	virtual	HRESULT Initialize() override;
	virtual	void	Tick(_float fTimeDelta) override;
	virtual void	Render() override;

public:
	_float			Get_Speed() const { return m_fSpeed; }
	_bool			Is_Stop() const { return m_bStop; }

public:
	void			Call_UpdatePreViewModel();
	void			Call_NextAnimationKey(const _uint& In_Key);

private:
	void			Add_EffectKeyEvent();
	void			Add_EnableWeaponEvent(const _bool In_bEnable);
	void			Add_SoundKeyEvent();
	void			Add_RandomSoundKeyEvent();

	void			Save_KeyEvent();
	HRESULT			Load_KeyEvent();
	void			Clear_KeyEvent();
	void			ClearBack_KeyEvent();
	void			ClearAll_KeyEvent();
	void			Clear_WeaponEvent();

	void			Draw_Player();
	void			Draw_Monster();
	void			Draw_KeyEventEditer();
	void			Draw_AnimationList();

private:
	CAnimation*		m_pCurrentAnimation = nullptr;

	_float			m_fCurrentTime = 0.f;
	_float			m_fMaxAnimationTime = 0.f;
	_float			m_fSpeed = 1.f;

	_bool			m_bStop = false;
	_bool			m_bHold = false;

	vector<string>	m_AllAnimationKeys;
	_int			m_CurrentAnimationIndex = 0;

	string			m_strKeyEventFileName = "";
	string			m_strSoundFileName = "";

public:
	_bool			m_bKeyEventEditer = false;
public:
	static CWindow_AnimTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END

