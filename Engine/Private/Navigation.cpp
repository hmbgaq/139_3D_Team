#include "..\Public\Navigation.h"
#include "Cell.h"
#include "GameObject.h"
#include "Transform.h"
#include "GameInstance.h"

_float4x4 CNavigation::m_WorldMatrix = { };

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{

}

CNavigation::CNavigation(const CNavigation& rhs)
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

HRESULT CNavigation::Initialize_Prototype(const wstring& strNavigationFilePath)
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

HRESULT CNavigation::Initialize(void* pArg)
{

	if (nullptr != pArg)
		m_iCurrentIndex = ((NAVI_DESC*)pArg)->iCurrentIndex;

	m_WorldMatrix = XMMatrixIdentity();

	_int iCommitChange = 0; //! 커밋 용 변경

	return S_OK;
}

#ifdef _DEBUG

HRESULT CNavigation::Render()
{
	/* 셀들의 위치가 월드상에 존재한다. */
	if (true == m_Cells.empty())
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
	if (true == m_Cells.empty())
		return false;

	_int		iNeighborIndex = { -1 };

	if (m_iCurrentIndex == -1)
		return false;

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

_bool CNavigation::isMove_ForSliding(_fvector vPosition, _fvector vLook, float4* vOutSlidingDir)
{
	/* 일단 이웃이 없다는 의미로 디폴트 -1로 셋팅해둔다. */
	_int      iNeighborIndex = { -1 };

	/* 이동한 지점의 결과가 현재 셀 내부에 있을경우 true반환
	 * 이동한 지점의 결과가 외부에 있을경우 iNeighborIndex에 값으로 받아온다. */

	if (m_iCurrentIndex == -1)
		return false;

	if (true == m_Cells[m_iCurrentIndex]->Is_Out(vPosition, vLook, XMLoadFloat4x4(&m_WorldMatrix), &iNeighborIndex, vOutSlidingDir))
	{
		/* 현재 셀 기준 외부로 나가는중 + 받아온 이웃셀 인덱스가 -1이 아님 = 이웃 인덱스로 이동확정 */
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				/* 이동을 하는지점이 Navigation 내부에는 있지만 이웃한 지점이 아님 -> 이동을 하면안됨 */
				if (-1 == iNeighborIndex)
					return false;

				if (false == m_Cells[iNeighborIndex]->Is_Out(vPosition, vLook, XMLoadFloat4x4(&m_WorldMatrix), &iNeighborIndex, vOutSlidingDir))
				{
					//if (true == m_Cells[iNeighborIndex]->Get_Active())
					m_iCurrentIndex = iNeighborIndex;
					//else
					//	return false;

					break;
				}
			}
			return true;
		}
		else
		{
			/* 나간방향의 이웃셀이 없을경우 = 이동불가 */
			return false;
		}
	}
	else
	{
		/* 이동한 지점의 결과가 현재 셀 내부 -> Move True */
		return true;
	}

	return false;

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
	_uint iCellSize = (_uint)m_Cells.size();
	m_iCurrentIndex = -1;

	for (_uint i = 0; i < iCellSize; ++i)
	{
		if (iCellSize <= i)
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


void CNavigation::SpawnCell_Setting(_float3 vPos)
{
	cout << "셀찾기 시작 "<< endl;

	for (auto& Cell : m_Cells)
	{
		_vector points[3] = {};

		for (int i = 0; i < 3; ++i)
		{
			points[i] = XMLoadFloat3(Cell->Get_Point(static_cast<CCell::POINT>(i)));
			points[i].m128_f32[3] = 0;
		}

		if (0.f < Compute_CCW(points[0], points[1], points[2])) /* 외적의 결과가 양수 = 시계 */
			swap(points[1], points[2]);	/* 반시계로 돌린다. -> 내외부 판별은 반시계로 정렬되어있어야함. */

		// 두 벡터를 만듭니다.
		_vector v0 = points[1] - points[0];
		_vector v1 = points[2] - points[0];

		// 외적을 계산하여 평면의 법선 벡터를 구합니다.
		_vector normal = XMVector3Cross(v0, v1);

		// 삼각형의 평면 방정식의 상수 D를 구합니다.
		_float D = -XMVector3Dot(points[0], normal).m128_f32[0];
		
		// 점 vPos를 평면 방정식에 대입하여 부호를 확인합니다.
		_float Result = XMVectorGetX(XMVector3Dot(vPos, normal)) + D;

		cout << Result << endl;
		if (Result < 0)
		{
			cout << "★☆★☆★☆★☆★☆★☆★☆★☆★☆찾음★☆★☆★☆★☆★☆★☆★☆" << endl;
			Set_CurrentIndex(Cell->Get_CurrentIndex());
			return;
		}
		else
			continue;
	}

	return;
}

void CNavigation::AddCell(CCell* pCell)
{
	if (nullptr == pCell)
		return;

	m_Cells.push_back(pCell);

	Make_Neighbors();
}

HRESULT CNavigation::Delete_Cell(const _uint iIndex)
{
	for (vector<CCell*>::iterator iter = m_Cells.begin(); iter != m_Cells.end();)
	{
		if ((*iter)->Get_Index() == iIndex)
		{
			Safe_Release(*iter);
			iter = m_Cells.erase(iter);

			Make_Neighbors();

			return S_OK;
		}
		else
			++iter;
	}

	return E_FAIL;
}

void CNavigation::AllSearchDelete_IsNan()
{
	_int iCellSize = (_int)m_Cells.size();
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

	_int iIsNanCellSize = (_int)vecNanCellIndex.size();
	for (_int i = 0; i < iIsNanCellSize; ++i)
	{
		Safe_Release(m_Cells[vecNanCellIndex[i]]);
	}

	m_Cells.erase(std::remove(m_Cells.begin(), m_Cells.end(), nullptr), m_Cells.end());
}

_float CNavigation::Compute_CCW(_float3 vPointA, _float3 vPointB, _float3 vPointC)
{	
	/* 2D평면에서 회전을 계산하므로 Y성분이 양수인지 음수인지에 따라 시계&반시계가 나뉜다. */
	_vector vLineAB = XMVector3Normalize(XMVectorSetY(XMLoadFloat3(&vPointB) - XMLoadFloat3(&vPointA), 0.f));
	_vector vLineAC = XMVector3Normalize(XMVectorSetY(XMLoadFloat3(&vPointC) - XMLoadFloat3(&vPointA), 0.f));

	_float vNormalY = XMVectorGetY(XMVector3Cross(vLineAB, vLineAC));

	return vNormalY;
}

void CNavigation::InRangeCellChange(CCell* pCell, _int ePoint, _float3 vSearchPos)
{
	_float3 originPoint = *pCell->Get_Point(CCell::POINT(ePoint));

	vector<pair<CCell*, CCell::POINT>> pointsToUpdate;

	for (auto& pcell : m_Cells)
	{
		for (int i = 0; i < CCell::POINT_END; ++i)
		{
			_float3* point = pcell->Get_Point(static_cast<CCell::POINT>(i));

			if (XMVector3Equal(XMLoadFloat3(&originPoint), XMLoadFloat3(point)))
				pointsToUpdate.push_back(make_pair(pcell, static_cast<CCell::POINT>(i)));
		}
	}

	m_iSelcetPointsSize = (_int)pointsToUpdate.size();

	for (auto& iter : pointsToUpdate)
	{
		iter.first->Set_Point(iter.second, vSearchPos);
	}
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
	_float fResult = {};

	CCell* pCell = m_Cells[m_iCurrentIndex]; /* 현재 어디 셀에 위치한지 리턴 */

	/* 평면을 나타내는 벡터 */
	_vector vPlane = XMPlaneFromPoints(XMVectorSetW(XMLoadFloat3(pCell->Get_Point(CCell::POINT_A)), 1.f),
		XMVectorSetW(XMLoadFloat3(pCell->Get_Point(CCell::POINT_B)), 1.f),
		XMVectorSetW(XMLoadFloat3(pCell->Get_Point(CCell::POINT_C)), 1.f));

	/* 현재 위치 - 평면위의 한점이될 위치 : 아직 y값을 모르니 x, z 만 대입*/
	_float fx = XMVectorGetX(vPosition);
	_float fz = XMVectorGetZ(vPosition);

	/* ax + by + cz + D = 0 의 형태에서 y 구하기 */
	/* D : 원점부터 평면까지 거리를 의미하는 정사영의 크기
	 * D>0 : 평면 내부에 위치 //  D = 0 평면위에 위치 //  D<0 : 평면밖에 위치 */
	_float fa = XMVectorGetX(vPlane);
	_float fb = XMVectorGetY(vPlane);
	_float fc = XMVectorGetZ(vPlane);
	_float fd = XMVectorGetW(vPlane);

	fResult = (-fa * fx - fc * fz - fd) / fb;

	if (pGround != nullptr)
	{
		*pGround = (vPosition.y < abs(fResult));
	}

	return fResult;

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

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strNavigationFilePath)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(strNavigationFilePath)))
	{
		MSG_BOX("Failed to Created : CNavigation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CNavigation::Clone(void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

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