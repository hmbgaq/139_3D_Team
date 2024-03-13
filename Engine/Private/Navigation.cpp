#include "..\Public\Navigation.h"
#include "Cell.h"
#include "GameObject.h"
#include "Transform.h"
#include "GameInstance.h"

_float4x4 CNavigation::m_WorldMatrix = { };

CNavigation::CNavigation(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CNavigation::CNavigation(const CNavigation & rhs)
	: CComponent(rhs)
	, m_Cells(rhs.m_Cells)
#ifdef _DEBUG
	, m_pShader(rhs.m_pShader)
#endif
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);


#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif

}

HRESULT CNavigation::Initialize_Prototype(const wstring & strNavigationFilePath)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	HANDLE		hFile = CreateFile(strNavigationFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_ulong		dwByte = { 0 };

	while (true)
	{
		_float3		vPoints[3];

		ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		if (0 == dwByte)
			break;

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, _uint(m_Cells.size()));
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

	CloseHandle(hFile);

	AllSearchDelete_IsNan();

	if (FAILED(Make_Neighbors()))
		return E_FAIL;

#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Navigation.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CNavigation::Initialize(void * pArg)
{

	if(nullptr != pArg)
		m_iCurrentIndex = ((NAVI_DESC*)pArg)->iCurrentIndex;

	m_WorldMatrix = XMMatrixIdentity();

	return S_OK;
}

#ifdef _DEBUG

HRESULT CNavigation::Render()
{
	/* 셀들의 위치가 월드상에 존재한다. */
	if(true == m_Cells.empty())
		return E_FAIL;

	_float4		vColor = { 0.0f, 1.f, 0.f, 1.f };

	if (m_iCurrentIndex == -1)
	{
		vColor = _float4(0.f, 1.f, 0.f, 1.f);
	}
	else
	{
		//! 높이값 올려줄거야.
		vColor = _float4(1.f, 0.f, 0.f, 1.f);
		m_WorldMatrix.m[3][1] = m_WorldMatrix.m[3][1] + 0.1f;
	}


	//m_WorldMatrix.m[3][1] = m_iCurrentIndex == -1 ? m_WorldMatrix.m[3][1] : m_WorldMatrix.m[3][1] + 0.1f;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	

	m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4));

	m_pShader->Begin(0);

	_uint iCurrentLevel = m_pGameInstance->Get_NextLevel();

	if (iCurrentLevel == 6)
	{
		
		for (auto& pCell : m_Cells)
		{
			if (nullptr != pCell)
				pCell->Render(m_pShader);
		}
	}
	else
	{
		if (m_iCurrentIndex == -1)
		{
			for (auto& pCell : m_Cells)
			{
				if (nullptr != pCell)
					pCell->Render(m_pShader);
			}
		}
		else
			m_Cells[m_iCurrentIndex]->Render(m_pShader);
	}
	




	return S_OK;
}

#endif

void CNavigation::Update(_fmatrix WorldMatrix)
{
	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
}

_bool CNavigation::isMove(_fvector vPosition)
{
	if(true == m_Cells.empty())
		return false;

	_int		iNeighborIndex = { -1 };

	if (true == m_Cells[m_iCurrentIndex]->isIn(vPosition, XMLoadFloat4x4(&m_WorldMatrix), &iNeighborIndex))
		return true;

	else
	{
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
					return false;
				if (true == m_Cells[iNeighborIndex]->isIn(vPosition, XMLoadFloat4x4(&m_WorldMatrix), &iNeighborIndex))
				{
					m_iCurrentIndex = iNeighborIndex;
					return true;
				}				
			}
		}
		else
			return false;
	}
}

void CNavigation::SaveData(wstring strSavePath)
{
	HANDLE	hFile = CreateFile(strSavePath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (0 == hFile)
		return;

	_ulong dwByte = 0;

	_int iCellSize = (_int)m_Cells.size();

	for (_int i = 0; i < iCellSize; ++i)
	{
		_float3 vPoints[3];

		vPoints[0] = *m_Cells[i]->Get_Point(CCell::POINT_A);
		vPoints[1] = *m_Cells[i]->Get_Point(CCell::POINT_B);
		vPoints[2] = *m_Cells[i]->Get_Point(CCell::POINT_C);

		WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);


	}

	WriteFile(hFile, &m_iCurrentIndex, sizeof(_int), &dwByte, nullptr);

	CloseHandle(hFile);
}

void CNavigation::LoadData(wstring strLoadPath)
{
	_uint iCellSize = m_Cells.size();
	m_iCurrentIndex = -1;

	for (_uint i = 0; i < iCellSize; ++i)
	{	
		if(iCellSize <= i)
			break;

		Safe_Release(m_Cells[i]);

	}

	m_Cells.clear();

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	HANDLE		hFile = CreateFile(strLoadPath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (0 == hFile)
	{
		MSG_BOX("네비게이션 로드실패");
		return;
	}

	_ulong		dwByte = { 0 };

	while (true)
	{
		_float3		vPoints[3];

		ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		if (0 == dwByte)
			break;

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, _uint(m_Cells.size()));
		if (nullptr == pCell)
		{
			MSG_BOX("네비게이션 로드 Cell Create 실패");
			return;
		}

		m_Cells.push_back(pCell);
	}

	CloseHandle(hFile);

	AllSearchDelete_IsNan();
	if (FAILED(Make_Neighbors()))
	{
		MSG_BOX("네비게이션 로드 이웃 생성 실패");
		return;
	}
}


void CNavigation::AddCell(CCell* pCell)
{
	if (nullptr == pCell)
		return;

	m_Cells.push_back(pCell);

	Make_Neighbors();
}

void CNavigation::Delete_Cell(const _uint iIndex)
{
	if (iIndex >= m_Cells.size())
		return;

	for (auto& cell : m_Cells)
	{
		for (size_t line = 0; line < CCell::LINE_END; ++line)
		{
			_int neighborIndex = cell->Get_NeighborIndex(static_cast<CCell::LINE>(line));

			if (neighborIndex == iIndex)
			{
				cell->SetUp_Neighbor(static_cast<CCell::LINE>(line), nullptr);
			}
			else if (neighborIndex > iIndex && neighborIndex - 1 < m_Cells.size())
			{
				cell->SetUp_Neighbor(static_cast<CCell::LINE>(line), m_Cells[neighborIndex - 1]);
			}
		}
	}

	Safe_Release(m_Cells[iIndex]);
	m_Cells.erase(m_Cells.begin() + iIndex);

	for (_uint i = 0; i < m_Cells.size(); ++i)
	{
		m_Cells[i]->Set_Index(i);
	}
}

void CNavigation::AllSearchDelete_IsNan()
{
	_int iCellSize = m_Cells.size();
	vector<CCell*> vecNanCells;
	vector<_int> vecNanCellIndex;

	for (_int i = 0; i < iCellSize; ++i)
	{
		_float3 vPointA = *m_Cells[i]->Get_Point(CCell::POINT_A);
		_float3 vPointB = *m_Cells[i]->Get_Point(CCell::POINT_B);
		_float3 vPointC = *m_Cells[i]->Get_Point(CCell::POINT_C);

		if (isnan<float>(vPointA.x) || isnan<float>(vPointB.x) || isnan<float>(vPointC.x) ||
			isnan<float>(vPointA.y) || isnan<float>(vPointB.y) || isnan<float>(vPointC.y) ||
			isnan<float>(vPointA.z) || isnan<float>(vPointB.z) || isnan<float>(vPointC.z))
		{
			vecNanCellIndex.push_back(i);
			
		}
	}

	_uint iIsNanCellSize = vecNanCellIndex.size();
	for (_int i = 0; i < iIsNanCellSize; ++i)
	{
		Safe_Release(m_Cells[vecNanCellIndex[i]]);
	}

	m_Cells.erase(std::remove(m_Cells.begin(), m_Cells.end(), nullptr), m_Cells.end());
}

void CNavigation::InRangeCellChange(CCell* pCell, _int ePoint, _float3 vSearchPos)
{
	_int iCellSize = (_int)m_Cells.size();


	_float3 vPosition = *pCell->Get_Point(CCell::POINT(ePoint));

	vector<CCell::POINT> vecPoints;
	vector<_int> vecIndexs;

	for (_int i = 0; i < iCellSize; ++i)
	{
		_float3 vPointA = *m_Cells[i]->Get_Point(CCell::POINT_A);
		_float3 vPointB = *m_Cells[i]->Get_Point(CCell::POINT_B);
		_float3 vPointC = *m_Cells[i]->Get_Point(CCell::POINT_C);

		CCell* pTargetCell = m_Cells[i];

		_float3 vPointBool = pTargetCell->Get_Compare_Point(&vPosition);

		if (vPointBool.x == 1.f)
		{
			vecPoints.push_back(CCell::POINT_A);
			vecIndexs.push_back(i);

		}
		else if (vPointBool.y == 1.f)
		{
			vecPoints.push_back(CCell::POINT_B);
			vecIndexs.push_back(i);
		}
		else if (vPointBool.z == 1.f)
		{
			vecPoints.push_back(CCell::POINT_C);
			vecIndexs.push_back(i);
		}
	}

	_int iVectorSize = (_int)vecPoints.size();


	for (_int i = 0; i < iVectorSize; ++i)
	{
		m_Cells[vecIndexs[i]]->Set_Point(vecPoints[i], vSearchPos);
	}


	Make_Neighbors();
}

_int CNavigation::Get_SelectRangeCellIndex(CGameObject* pTargetObject)
{
	_int iCellSize = (_int)m_Cells.size();

	CTransform* pTransform = pTargetObject->Get_Transform();

	_vector vPos = pTransform->Get_State(CTransform::STATE_POSITION);

	for (_int i = 0; i < iCellSize; ++i)
	{

		if (true == m_Cells[i]->isInRange(vPos, XMLoadFloat4x4(&m_WorldMatrix)))
				return m_Cells[i]->Get_Index();
	}

	return -1;
}

_float CNavigation::Compute_Height(_float3 vPosition, _bool* pGround)
{
	_vector vPlane = {};

	if (m_iCurrentIndex == -1)
		return _float();

	CCell* pCell = m_Cells[m_iCurrentIndex];

	_vector vA = XMVectorSetW(XMLoadFloat3(pCell->Get_Point(CCell::POINT_A)), 1.f);
	_vector vB = XMVectorSetW(XMLoadFloat3(pCell->Get_Point(CCell::POINT_B)), 1.f);
	_vector vC = XMVectorSetW(XMLoadFloat3(pCell->Get_Point(CCell::POINT_C)), 1.f);

	vPlane = XMPlaneFromPoints(vA, vB, vC);

	_float fA = XMVectorGetX(vPlane);
	_float fB = XMVectorGetY(vPlane);
	_float fC = XMVectorGetZ(vPlane);
	_float fD = XMVectorGetW(vPlane);

	_float fX = vPosition.x;
	_float fY = vPosition.y;
	_float fZ = vPosition.z;

	_float height = (-fA * fX) - (fC * fZ) - fD;

	if (pGround != nullptr)
	{
		// 플레이어의 Y값을 기준으로 땅에 있다고 판단
		*pGround = (fY <= height);
	}

	return height;
}

HRESULT CNavigation::Make_Neighbors()
{
// 	_bool bAB = false, bBC = false, bCA = false;
// 
// 
// 	for (auto& pSourCell : m_Cells)
// 	{
// 		for (auto& pDestCell : m_Cells)
// 		{
// 			if (pSourCell == pDestCell)
// 				continue;
// 
// 			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
// 			{
// 				pSourCell->SetUp_Neighbor(CCell::LINE_AB, pDestCell);
// 				bAB = true;
// 			}
// 			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
// 			{
// 				pSourCell->SetUp_Neighbor(CCell::LINE_BC, pDestCell);
// 				bBC = true;
// 			}
// 			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
// 			{
// 				pSourCell->SetUp_Neighbor(CCell::LINE_CA, pDestCell);
// 				bCA = true;
// 			}
// 
// 			if (false == bAB && false == bBC && false == bCA)
// 				pSourCell->Reset_Line();
// 
// 		}
// 	}
	for (auto& pSrcCell : m_Cells)
	{
		if (pSrcCell == nullptr)
			continue;
		for (auto& pDstCell : m_Cells)
		{
			if (pDstCell == nullptr || pSrcCell == pDstCell)
				continue;

			if (true == pDstCell->Compare_Points(pSrcCell->Get_Point(CCell::POINT_A), pSrcCell->Get_Point(CCell::POINT_B)))
			{
				pSrcCell->SetUp_Neighbor(CCell::LINE_AB, pDstCell);
			}

			if (true == pDstCell->Compare_Points(pSrcCell->Get_Point(CCell::POINT_B), pSrcCell->Get_Point(CCell::POINT_C)))
			{
				pSrcCell->SetUp_Neighbor(CCell::LINE_BC, pDstCell);
			}

			if (true == pDstCell->Compare_Points(pSrcCell->Get_Point(CCell::POINT_C), pSrcCell->Get_Point(CCell::POINT_A)))
			{
				pSrcCell->SetUp_Neighbor(CCell::LINE_CA, pDstCell);
			}
		}
	}

	return S_OK;
}

CNavigation * CNavigation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & strNavigationFilePath)
{
	CNavigation*		pInstance = new CNavigation(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(strNavigationFilePath)))
	{
		MSG_BOX("Failed to Created : CNavigation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CNavigation::Clone(void * pArg)
{
	CNavigation*		pInstance = new CNavigation(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNavigation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif
}
