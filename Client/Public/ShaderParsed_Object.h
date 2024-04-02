#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "UI_Weakness.h"

BEGIN(Engine)
class CShader;
class CCollider;
class CModel;
END


BEGIN(Client)

/* 셰이더 테스트용 모델입니다! */
class CShaderParsed_Object final : public CGameObject
{
public:
	enum class RENDER_TYPE {NORMAL, MESH_CHECK, RENDER_TEST, RENDER_END };

	typedef struct tagDummyDesc
	{
		CModel::TYPE eType = CModel::TYPE::TYPE_END;

		string	strModelProtoTag = {};
		string	strShaderProtoTag = {};

		_int	iMeshNumber; 
		_int	iTotalMeshNumber; 
		_int	iRenderPass;

		_float	m_fRimPower = {};
		_float4	m_vRimColor = { 1.0f, 1.f, 1.f, 0.3f };
		_float3	m_vBloomPower = _float3(0.1f, 0.1f, 0.1f); 
	}CREATE_DESC;

private:
	CShaderParsed_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CShaderParsed_Object(const CShaderParsed_Object& rhs);
	virtual ~CShaderParsed_Object() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;
	virtual HRESULT Render_OutLine() override;

private:
	HRESULT			Ready_Components(void* pArg);
	HRESULT			Bind_ShaderResources();

public:
	void Set_RenderType(RENDER_TYPE eRenderType) { m_eRenderType = eRenderType; }
	void Set_RenderPass(_int iRenderPass) { m_iRenderPass = iRenderPass; }
	void Set_DiscardMeshNumber(_int iMeshNumber) { m_iDiscardMeshNumber = iMeshNumber; }

private:
	/* Tool 에서 조정할 값들 */
	RENDER_TYPE		m_eRenderType = RENDER_TYPE::NORMAL;
	_int			m_iRenderPass = {};
	_int			m_iDiscardMeshNumber;


private:
	_int			m_iTotalMeshNumber;

	_float4			m_vCamPos = {};
	_float			m_fRimPower = {};
	_float4			m_vRimColor = { 1.0f, 1.f, 1.f, 0.3f };
	_float3			m_vBloomPower = _float3(0.1f, 0.1f, 0.1f);

	CModel::TYPE	m_eType = CModel::TYPE::TYPE_END;

public:
	/* Tool에서 셋팅하기위한 모든 Get, Set 함수 모음 */


private:
	CModel*		m_pModelCom = { nullptr };
	CShader*	m_pShaderCom = { nullptr };
	CCollider*	m_pColliderCom = { nullptr };

public:
	static CShaderParsed_Object*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual CGameObject*			Pool();
	virtual void					Free() override;

};

END