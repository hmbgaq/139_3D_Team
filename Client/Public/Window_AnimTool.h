#pragma once

#include "Imgui_Window.h"

BEGIN(Engine)
class CGameObject;
class CAnimation;
END

BEGIN(Client)
class CPreviewAnimationModel;

class CWindow_AnimTool final : public CImgui_Window
{
	enum class ROOTNODE_FLAG
	{
		X = (1 << 0),
		Y = (1 << 1),
		Z = (1 << 2),
		FLAG_END
	};
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

	void			Create_Object(const wstring& strLayerTag, const wstring& strPrototypeTag);
	void			Draw_Player();
	void			Draw_Monster();
	void			Draw_KeyEventEditer();
	void			Draw_AnimationList(_float fTimeDelta);

public://문자열 변환 
	char*			ConverWStringtoC(const wstring& wstr);
	char*			ConvertWCtoC(const wchar_t* str);
	wchar_t*		ConvertCtoWC(const char* str);
private:
	CPreviewAnimationModel*	m_pPreViewModel = { nullptr };
	CAnimation*				m_pCurrentAnimation = { nullptr };
	CGameObject*			m_PickingObject = { nullptr };

	
	_float					m_fSpeed = 1.f;
	_float					m_fCurrentTrackPosition = 0.f;
	_float					m_fDuration = 0.f;

	_int					m_CurrentAnimationIndex = 0;
	_int					m_iCreateObjectSize = 0;
	_uint					m_iAnimationNum = 0;

	string					m_strKeyEventFileName = "";
	string					m_strSoundFileName = "";

	vector<string>			m_AllAnimationKeys;
	vector<string>			m_vObjectTag;
	vector<CGameObject*>	m_CreateList;
	vector<CAnimation*>		m_pAnimation;
public:
	_bool					m_bStop = false;
	_bool					m_bHold = false;
	_bool					m_bKeyEventEditer = false;
	_bool					m_bCreateCheck = false;
	_bool					m_bDeleteCheck = false;
	_bool					m_bListCheck = false;
	_bool					m_bCloneCount = false;
public:
	static CWindow_AnimTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END

