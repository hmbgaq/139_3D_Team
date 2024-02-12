#include "..\Public\QuadTree.h"
#include "GameInstance.h"

CQuadTree::CQuadTree()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CQuadTree::Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	m_iCorners[CORNER_LT] = iLT;
	m_iCorners[CORNER_RT] = iRT;
	m_iCorners[CORNER_RB] = iRB;
	m_iCorners[CORNER_LB] = iLB;

	if (1 == iRT - iLT)
		return S_OK;

	m_iCenter = (iLT + iRB) >> 1;

	_uint		iLC, iTC, iRC, iBC;

	iLC = (iLT + iLB) >> 1;
	iTC = (iLT + iRT) >> 1;
	iRC = (iRT + iRB) >> 1;
	iBC = (iLB + iRB) >> 1;

	m_Children[CORNER_LT] = CQuadTree::Create(iLT, iTC, m_iCenter, iLC);
	m_Children[CORNER_RT] = CQuadTree::Create(iTC, iRT, iRC, m_iCenter);
	m_Children[CORNER_RB] = CQuadTree::Create(m_iCenter, iRC, iRB, iBC);
	m_Children[CORNER_LB] = CQuadTree::Create(iLC, m_iCenter, iBC, iLB);

	return S_OK;
}

void CQuadTree::Culling(const _float3 * pVerticesPos, _uint * pIndices, _uint * pNumIndices)
{
	if (nullptr == m_Children[CORNER_LT] || 
		true == isDraw(pVerticesPos))
	{
		_uint		iIndices[4] = {
			m_iCorners[CORNER_LT],
			m_iCorners[CORNER_RT],
			m_iCorners[CORNER_RB],
			m_iCorners[CORNER_LB],
		};

		_bool		isDraw[4] = { 
			nullptr == m_Neighbors[NEIGHBOR_LEFT] ? true : m_Neighbors[NEIGHBOR_LEFT]->isDraw(pVerticesPos),
			nullptr == m_Neighbors[NEIGHBOR_TOP] ? true : m_Neighbors[NEIGHBOR_TOP]->isDraw(pVerticesPos),
			nullptr == m_Neighbors[NEIGHBOR_RIGHT] ? true : m_Neighbors[NEIGHBOR_RIGHT]->isDraw(pVerticesPos),
			nullptr == m_Neighbors[NEIGHBOR_BOTTOM] ? true : m_Neighbors[NEIGHBOR_BOTTOM]->isDraw(pVerticesPos),
		};

		_bool		isIn[4] = {
			m_pGameInstance->isIn_LocalPlanes(XMLoadFloat3(&pVerticesPos[iIndices[0]]), 0.f),
			m_pGameInstance->isIn_LocalPlanes(XMLoadFloat3(&pVerticesPos[iIndices[1]]), 0.f),
			m_pGameInstance->isIn_LocalPlanes(XMLoadFloat3(&pVerticesPos[iIndices[2]]), 0.f),
			m_pGameInstance->isIn_LocalPlanes(XMLoadFloat3(&pVerticesPos[iIndices[3]]), 0.f),
		};

		if (true == isDraw[NEIGHBOR_LEFT] &&
			true == isDraw[NEIGHBOR_TOP] &&
			true == isDraw[NEIGHBOR_RIGHT] &&
			true == isDraw[NEIGHBOR_BOTTOM])
		{
			if (true == isIn[0] ||
				true == isIn[1] ||
				true == isIn[2])
			{
				pIndices[(*pNumIndices)++] = iIndices[0];
				pIndices[(*pNumIndices)++] = iIndices[1];
				pIndices[(*pNumIndices)++] = iIndices[2];
			}

			if (true == isIn[0] ||
				true == isIn[2] ||
				true == isIn[3])
			{
				pIndices[(*pNumIndices)++] = iIndices[0];
				pIndices[(*pNumIndices)++] = iIndices[2];
				pIndices[(*pNumIndices)++] = iIndices[3];
			}

			return;
		}

		_uint		iLC, iTC, iRC, iBC;

		iLC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_LB]) >> 1;
		iTC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_RT]) >> 1;
		iRC = (m_iCorners[CORNER_RT] + m_iCorners[CORNER_RB]) >> 1;
		iBC = (m_iCorners[CORNER_LB] + m_iCorners[CORNER_RB]) >> 1;

		if (true == isIn[0] ||
			true == isIn[2] ||
			true == isIn[3])
		{
			if (false == isDraw[NEIGHBOR_LEFT])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = iLC;

				pIndices[(*pNumIndices)++] = iLC;
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
			}
			else
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
			}

			if (false == isDraw[NEIGHBOR_BOTTOM])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = iBC;

				pIndices[(*pNumIndices)++] = iBC;
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
			}
			else
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
			}
		}

		if (true == isIn[0] ||
			true == isIn[1] ||
			true == isIn[2])
		{
			if (false == isDraw[NEIGHBOR_TOP])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = iTC;
				pIndices[(*pNumIndices)++] = m_iCenter;

				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = iTC;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
			}
			else
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
				pIndices[(*pNumIndices)++] = m_iCenter;
			}

			if (false == isDraw[NEIGHBOR_RIGHT])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
				pIndices[(*pNumIndices)++] = iRC;
				pIndices[(*pNumIndices)++] = m_iCenter;

				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = iRC;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
			}
			else
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
				pIndices[(*pNumIndices)++] = m_iCenter;
			}
		}
		return;
	}


	_float		fRadius = XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LT]]) - XMLoadFloat3(&pVerticesPos[m_iCenter])).m128_f32[0];

	if (true == m_pGameInstance->isIn_LocalPlanes(XMLoadFloat3(&pVerticesPos[m_iCenter]), fRadius))
	{
		for (size_t i = 0; i < CORNER_END; i++)
		{
			m_Children[i]->Culling(pVerticesPos, pIndices, pNumIndices);
		}
	}	
}

HRESULT CQuadTree::Make_Neighbors()
{
	if (nullptr == m_Children[CORNER_LT]->m_Children[CORNER_LT])
		return E_FAIL;

	m_Children[CORNER_LT]->m_Neighbors[NEIGHBOR_RIGHT] = m_Children[CORNER_RT];
	m_Children[CORNER_LT]->m_Neighbors[NEIGHBOR_BOTTOM] = m_Children[CORNER_LB];

	m_Children[CORNER_RT]->m_Neighbors[NEIGHBOR_LEFT] = m_Children[CORNER_LT];
	m_Children[CORNER_RT]->m_Neighbors[NEIGHBOR_BOTTOM] = m_Children[CORNER_RB];

	m_Children[CORNER_RB]->m_Neighbors[NEIGHBOR_LEFT] = m_Children[CORNER_LB];
	m_Children[CORNER_RB]->m_Neighbors[NEIGHBOR_TOP] = m_Children[CORNER_RT];

	m_Children[CORNER_LB]->m_Neighbors[NEIGHBOR_RIGHT] = m_Children[CORNER_RB];
	m_Children[CORNER_LB]->m_Neighbors[NEIGHBOR_TOP] = m_Children[CORNER_LT];

	if (nullptr != m_Neighbors[NEIGHBOR_RIGHT])
	{
		m_Children[CORNER_RT]->m_Neighbors[NEIGHBOR_RIGHT] = m_Neighbors[NEIGHBOR_RIGHT]->m_Children[CORNER_LT];
		m_Children[CORNER_RB]->m_Neighbors[NEIGHBOR_RIGHT] = m_Neighbors[NEIGHBOR_RIGHT]->m_Children[CORNER_LB];
	}

	if (nullptr != m_Neighbors[NEIGHBOR_BOTTOM])
	{
		m_Children[CORNER_LB]->m_Neighbors[NEIGHBOR_BOTTOM] = m_Neighbors[NEIGHBOR_BOTTOM]->m_Children[CORNER_LT];
		m_Children[CORNER_RB]->m_Neighbors[NEIGHBOR_BOTTOM] = m_Neighbors[NEIGHBOR_BOTTOM]->m_Children[CORNER_RT];
	}

	if (nullptr != m_Neighbors[NEIGHBOR_LEFT])
	{
		m_Children[CORNER_LT]->m_Neighbors[NEIGHBOR_LEFT] = m_Neighbors[NEIGHBOR_LEFT]->m_Children[CORNER_RT];
		m_Children[CORNER_LB]->m_Neighbors[NEIGHBOR_LEFT] = m_Neighbors[NEIGHBOR_LEFT]->m_Children[CORNER_RB];
	}

	if (nullptr != m_Neighbors[NEIGHBOR_TOP])
	{
		m_Children[CORNER_LT]->m_Neighbors[NEIGHBOR_TOP] = m_Neighbors[NEIGHBOR_TOP]->m_Children[CORNER_LB];
		m_Children[CORNER_RT]->m_Neighbors[NEIGHBOR_TOP] = m_Neighbors[NEIGHBOR_TOP]->m_Children[CORNER_RB];
	}


	for (auto& pChild : m_Children)
	{
		if(nullptr != pChild)
			pChild->Make_Neighbors();	
	}

	return S_OK;
}

_bool CQuadTree::isDraw(const _float3 * pVerticesPos)
{
	_float		fWidth = m_iCorners[CORNER_RT] - m_iCorners[CORNER_LT];

	_float		fCamDistance = XMVector3Length(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()) - 
		XMLoadFloat3(&pVerticesPos[m_iCenter])).m128_f32[0];

	if (fCamDistance * 0.1f > fWidth)
		return true;

	return false;
}

shared_ptr<CQuadTree> CQuadTree::Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	shared_ptr<CQuadTree>		pInstance = make_shared<CQuadTree>();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(iLT, iRT, iRB, iLB)))
	{
		MSG_BOX("Failed to Created : CQuadTree");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CQuadTree::Free()
{

	for (auto& pChild : m_Children)
		Safe_Release(pChild);	

	Safe_Release(m_pGameInstance);
}
