#include "stdafx.h"
#include "Field.h"
#include "GameInstance.h"



CField::CField(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CGameObject(pDevice, pContext, strPrototypeTag)
{
	m_bIsPoolObject = false;
}

CField::CField(const CField& rhs)
	: CGameObject(rhs)
{
}

HRESULT CField::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CField::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	DINFO* pInfo = (DINFO*)pArg;

	// 캐스팅된 구조체의 값 담기
	DINFO Info = { pInfo->fX, pInfo->fY, pInfo->fZ };

	Info.vecVertexInfo = pInfo->vecVertexInfo;
	
	// 주소가 아닌, 값이 담긴 구조체 넘겨주기
	ReceiveInfo(Info);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CField::Priority_Tick(_float fTimeDelta)
{
}

void CField::Tick(_float fTimeDelta)
{
	m_pShaderCom->Bind_RawValue("g_vBrushPos", &m_fPickingPos, sizeof(_float4));
}

void CField::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CField::Render()
{
	//TODO 셰이더에게 행렬을 던져주는 행위는 반드시 셰이더의 비긴함수를 호출하기 이전에 해야한다.
	//! 그 이유는, 셰이더의 비긴함수 내에서 pPass->Apply(0, m_pContext); 코드를 수행한다.
	//! Apply 호출 후에 행렬을 던져줘도 에러는 나지 않지만, 안정성이 떨어진다.
	//! Apply 호출 후에 행렬을 던져주면, 어떤 때에는 정상적으로 수행되고, 어떤 때에는 값이 제대로 안 넘어가는 경우가 있다.
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	//! 이 셰이더에 0번째 패스로 그릴거야.
	m_pShaderCom->Begin(1);

	if (m_pVIBufferCom != nullptr) // 지형 생성할때 삭제하고 만들기전에 랜더를 타면 터지기때문에, 예외처리 해두자.
	{
		//! 내가 그리려고 하는 정점, 인덱스 버퍼를 장치에 바인딩해
		m_pVIBufferCom->Bind_VIBuffers();

		//! 바인딩된 정점, 인덱스를 그려
		m_pVIBufferCom->Render();
	}

	return S_OK;
}

// void CField::Write_Json(json& Out_Json)
// {
// 	__super::Write_Json(Out_Json);
// 
// 	
// 	Out_Json.emplace("SizeX", m_tDynamicInfo.fX);
// 	Out_Json.emplace("SizeY", m_tDynamicInfo.fY);
// 	Out_Json.emplace("SizeZ", m_tDynamicInfo.fZ);
// }
// 
// void CField::Load_FromJson(const json& In_Json)
// {
// 	__super::Load_FromJson(In_Json);
// 
// 	
// 
// }



void CField::Picking_Terrain(RAY WorldRay, EDIT_MODE eMode)
{

	if (m_pVIBufferCom->Compute_MousePos(WorldRay,m_pTransformCom->Get_WorldMatrix(), &m_fPickingPos))
	{
		m_pVIBufferCom->Update(XMLoadFloat3(&m_fPickingPos), m_fDrawRadious, m_fPower, (_uint)eMode);
	}
	
}

_float3 CField::GetMousePos(RAY WorldRay)
{

	if (m_pVIBufferCom->Compute_MousePos(WorldRay, m_pTransformCom->Get_WorldMatrix(), &m_fPickingPos))
	{
		m_bOnMouse = true;
		return m_fPickingPos;
	}
	else
		m_bOnMouse = false;

	return _float3();
}

_bool CField::MouseOnTerrain()
{
	return m_bOnMouse;	
}

void CField::Update(_fvector vMousePos, _bool bMode)
{
	return m_pVIBufferCom->Update(vMousePos, bMode);
}

void CField::Move_Field(_float3 vAddPos)
{
	return m_pVIBufferCom->Move_Field(vAddPos);
}


HRESULT CField::Ready_Components()
{

	LEVEL eCurrentLevel = (LEVEL)m_pGameInstance->Get_NextLevel();

	//! For.Com_Shader
	if (FAILED(__super::Add_Component(eCurrentLevel, TEXT("Prototype_Component_Shader_VtxNorTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//! For.Com_VIBuffer_Dynamic
	if (FAILED(__super::Add_Component(eCurrentLevel, TEXT("Prototype_Component_VIBuffer_Field"),	// 생성까지 됐음
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &m_tDynamicInfo)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(eCurrentLevel, TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TYPE_DIFFUSE]))))
		return E_FAIL;

	/* For.Com_Mask */ // 지형을 섞기위한 Mask // 에러 : 툴 레벨로 ID를 주지않아서 터레인 생성에 실패했었고, 제대로 동작이 안되니, Imgui shutdown에 들어가 터졌음
	if (FAILED(__super::Add_Component(eCurrentLevel, TEXT("Prototype_Component_Texture_Terrain_Mask"),
		TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom[TYPE_MASK]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(eCurrentLevel, TEXT("Prototype_Component_Texture_Terrain_Brush"),
		TEXT("Com_Brush"), reinterpret_cast<CComponent**>(&m_pTextureCom[TYPE_BRUSH]))))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CField::Bind_ShaderResources()
{

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	_float4 vColor = { 0.f, 1.f, 0.f, 1.f };

	// 마스크
	if (FAILED(m_pTextureCom[TYPE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
		return E_FAIL;

	//// 브러쉬
	if (FAILED(m_pTextureCom[TYPE_BRUSH]->Bind_ShaderResource(m_pShaderCom, "g_BrushTexture", 0))) // error : 
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vBrushPos", &m_fPickingPos, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrushRange", &m_fDrawRadious, sizeof(float))))
		return E_FAIL;
	//if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))	// 수정
	//	return E_FAIL;

	return S_OK;
}

void CField::ReceiveInfo(DINFO pInfo)
{
	m_tDynamicInfo = { pInfo.fX, pInfo.fY, pInfo.fZ };
	m_tDynamicInfo.vecVertexInfo = pInfo.vecVertexInfo;
}

// void CField::Delete_Component(const wstring& strComTag)
// {
// 	__super::Delete_Component(strComTag);
// 
// 	// ! 삭제가 안될때 !
// 	// Delete_Component에서는 복사된 녀석을 Safe_Release로 래퍼런스 카운트를 줄이기만 한것이라,
// 	// 제대로 타는지 확인하고 여기서 멤버변수도 삭제 시켜줘야한다.
// 
// 	if(m_pVIBufferCom)
// 	Safe_Release(m_pVIBufferCom);
// 	
// 
// }

CField* CField::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CField* pInstance = new CField(pDevice, pContext, strPrototypeTag);

	/* 원형 객체를 초기화한다. */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CField");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CField::Clone(void* pArg) // 여기서 사본을 만들때 지형을 설정할 값을 받아준다.
{
	CField* pInstance = new CField(*this);

	/* 사본 객체를 초기화한다. */
	if (FAILED(pInstance->Initialize(pArg))) // 넘겨줌
	{
		MSG_BOX("Failed to Cloned : CField");
		Safe_Release(pInstance);
	}
	return pInstance;
}

Engine::CGameObject* CField::Pool()
{
	return new CField(*this);
}

void CField::Free()
{
	__super::Free();

	
	Safe_Release(m_pVIBufferCom);

	Safe_Release(m_pShaderCom);

	//Safe_Release(m_pTextureCom);
	// 배열 형식, ->Release 에러 : 텍스처 컴포넌트를 배열 형식으로 바꿨기 때문에, 하나하나 삭제 시켜주도록 바꿔줘야한다.
	for (size_t i = 0; i < TYPE_END; i++)
	{
		Safe_Release(m_pTextureCom[i]);
	}
}


