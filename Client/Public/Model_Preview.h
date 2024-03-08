#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CModel_Preview final : public CGameObject
{
public:
	typedef struct tagModelPreviewDesc : public CGameObject::GAMEOBJECT_DESC
	{
		wstring		strProtoTag = { TEXT("") };
		wstring		strModelTag = TEXT("");

		_bool		bRender				= { TRUE };

		_uint		iAnimIndex			= { 0 };	// 플레이어 Idle애니메이션은 Player_IdlePose 8번 / 공격 1번은 Player_MeleeCombo_01 193번
		_int		iRenderGroup		= { CRenderer::RENDER_NONBLEND };	//! 밖에서 렌더러의 렌더그룹을 인트로 형변환해서 던져주자
		_uint		iShaderPassIndex	= { 0 };	 // false == m_bDissolve ? 0 : 3;

	}MODEL_PREVIEW_DESC;

private:
	CModel_Preview(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CModel_Preview(const CModel_Preview& rhs);
	virtual ~CModel_Preview() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	void	Set_AnimIndex(_uint iAnimIndex);


public:
	MODEL_PREVIEW_DESC* Get_Desc() { return &m_tDesc; }


private:
	MODEL_PREVIEW_DESC	m_tDesc = {};


private:
	CShader*	m_pShaderCom	= { nullptr };
	CModel*		m_pModelCom		= { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	/* 원형객체를 생성한다. */
	static CModel_Preview* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;
};

END