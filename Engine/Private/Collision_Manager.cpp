#include "..\Public\Collision_Manager.h"
#include "Collider.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Bounding.h"


CCollision_Manager::CCollision_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCollision_Manager::Initialize(const _uint& In_iNumLayer)
{
	m_iNumLayer = In_iNumLayer;
	m_ColliderList.reserve(In_iNumLayer);
	m_ReservedColliderList.reserve(In_iNumLayer);

	for (_uint i(0); i < In_iNumLayer; ++i)
	{
		m_ColliderList.push_back({});
		m_ReservedColliderList.push_back({});
		m_arrCheck.push_back(0);
	}

	return S_OK;
}

void CCollision_Manager::Tick(const _float& fTimeDelta)
{
	Update_ReservedColliderList();
	Remove_DeadCollision();

	for (size_t iRow(0); iRow < m_arrCheck.size(); ++iRow)
	{
		for (size_t iCol(iRow); iCol < m_arrCheck.size(); ++iCol) // 체크 박스 반만 해야되기 때문에 컬럼은 row부터
		{
			if (m_arrCheck[iRow] & (1 << iCol))
			{
				Update_CollisionGroup((_uint)iRow, (_uint)iCol);
			}
		}
	}

	End_CollisionCheck();
}

void CCollision_Manager::Add_Collision(const _uint& In_iLayer, CCollider* _pCollider)
{
	if (nullptr == _pCollider || In_iLayer >= m_iNumLayer)
		return;

	_bool is_Same = false;

	for (CCollider* pCollier : m_ColliderList[In_iLayer]) 
	{
		if (pCollier == _pCollider) 
		{
			is_Same = true;
			break;
		}
	}

	if (false == is_Same) 
	{
		m_ReservedColliderList[In_iLayer].push_back(_pCollider);
		Safe_AddRef(_pCollider);
	}
}

void CCollision_Manager::Check_Group(const _uint& In_iLeftLayer, const _uint& In_iRightLayer)
{
	_uint iRow((_uint)In_iLeftLayer); // 행
	_uint iCol((_uint)In_iRightLayer); // 열

	if (iRow > iCol)
	{
		iRow = In_iRightLayer;
		iCol = In_iLeftLayer;
	}

	if (m_arrCheck[iRow] & (1 << iCol)) // 이미 그 자리가 1이면
		m_arrCheck[iRow] &= ~(1 << iCol); // 빼주고
	else
		m_arrCheck[iRow] |= (1 << iCol);
}

void CCollision_Manager::Update_ReservedColliderList()
{
	for (size_t i = 0; i < m_ReservedColliderList.size(); ++i)
	{
		for (auto& elem : m_ReservedColliderList[i])
		{
			m_ColliderList[i].push_back(elem);
		}
		m_ReservedColliderList[i].clear();
	}
}

void CCollision_Manager::Remove_DeadCollision()
{
	for (_uint i(0); i < m_ColliderList.size(); i++)
	{
		for (auto iter = m_ColliderList[i].begin(); iter != m_ColliderList[i].end();)
		{
			if (nullptr == (*iter) || false == (*iter)->Get_Enable() || 
				nullptr == (*iter)->Get_Owner() || false == (*iter)->Get_Owner()->Get_Enable() || true == (*iter)->Get_Owner()->Is_Dead())
			{
				Safe_Release(*iter);
				iter = m_ColliderList[i].erase(iter);
			}
			else
			{
				iter++;
			}
		}
	}
}

void CCollision_Manager::Update_CollisionGroup(const _uint& In_iLeftLayer, const _uint& In_iRightLayer)
{
	for (CCollider* elem_Left : m_ColliderList[In_iLeftLayer])
	{
		for (CCollider* elem_Right : m_ColliderList[In_iRightLayer])
		{
			if (nullptr == elem_Left || nullptr == elem_Right || elem_Left == elem_Right || elem_Left->Get_Owner() == elem_Right->Get_Owner() || false == elem_Left->Get_Enable() || false == elem_Right->Get_Enable())
			{
				continue;
			}

			if (elem_Left->Is_Collision(elem_Right))
			{
				elem_Left->Collision(elem_Right);
				elem_Right->Collision(elem_Left);
			}
		}
	}
}

void CCollision_Manager::End_CollisionCheck()
{
	for (_uint i(0); i < m_ColliderList.size(); i++)
	{
		for (CCollider* elem : m_ColliderList[i])
		{
			if (nullptr != elem)
				elem->End_CollisionCheck();
		}
	}
}

CCollision_Manager* CCollision_Manager::Create(const _uint& In_iNumLayer)
{
	CCollision_Manager* pInstance = new CCollision_Manager;

	if (FAILED(pInstance->Initialize(In_iNumLayer)))
	{
		MSG_BOX("Failed to Created : CCollision_Manager");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCollision_Manager::Free()
{

	Safe_Release(m_pGameInstance);
}
