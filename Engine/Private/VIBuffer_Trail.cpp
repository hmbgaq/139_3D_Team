#include "VIBuffer_Trail.h"

#include "Engine_Defines.h"

//#include "GameInstance.h"
//#include "Transform.h"
//#include "Bone.h"

CVIBuffer_Trail::CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail& rhs)
    : CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Trail::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CVIBuffer_Trail::Initialize(void* pArg)
{
    if (!pArg)
        return E_FAIL;

    m_tBufferDesc = *(TRAIL_BUFFER_DESC*)pArg;
    ZeroMemory(&m_tBufferDesc.iCatMullRomIndex, sizeof(_uint) * 4);
#pragma region VERTEXBUFFER

    /* 버텍스 버퍼 생성 */
    m_iStride = sizeof(VTXPOSTEX);
    m_iNumVertices = 2 * m_tBufferDesc.iMaxCnt + 2;
    m_iNumVertexBuffers = 1;
    m_tBufferDesc.iVtxCnt = 0;

    VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

    for (_uint i = 0; i < m_iNumVertices; i += 2)
    {
        pVertices[i].vPosition = m_tBufferDesc.vPos_0;
        pVertices[i + 1].vPosition = m_tBufferDesc.vPos_1;

        pVertices[i].vTexcoord = _float2(1.f, 0.f);
        pVertices[i + 1].vTexcoord = _float2(1.f, 1.f);
    }

    pVertices[0].vTexcoord = _float2(0.f, 0.f);
    pVertices[1].vTexcoord = _float2(0.f, 1.f);


    /* 인덱스 버퍼 생성 */
    m_iIndexStride = sizeof(FACEINDICES16);
    m_iNumPrimitive = m_tBufferDesc.iMaxCnt * 2;
    m_iNumIndices = 3 * m_iNumPrimitive;
    m_eIndexFormat = DXGI_FORMAT_R16_UINT;
    m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    FACEINDICES16* pIndices = new FACEINDICES16[m_iNumIndices];
    ZeroMemory(pIndices, sizeof(FACEINDICES16) * m_iNumIndices);

    for (_uint i = 0; i < m_iNumPrimitive; i += 2)
    {
        pIndices[i] = { _ushort(i), _ushort(i + 2), _ushort(i + 3) };
        pIndices[i + 1] = { _ushort(i), _ushort(i + 3), _ushort(i + 1) };
    }

    /* ------------------------------------------------------------------ */

    ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
    m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
    m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_BufferDesc.StructureByteStride = m_iStride;
    m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_BufferDesc.MiscFlags = 0;

    ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
    m_SubResourceData.pSysMem = pVertices;

    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return E_FAIL;

    /* ------------------------------------------------------------------ */

    ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
    m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumPrimitive;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_BufferDesc.StructureByteStride = 0;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;

    ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
    m_SubResourceData.pSysMem = pIndices;

    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return E_FAIL;

    /* ------------------------------------------------------------------ */

    Safe_Delete_Array(pVertices);
    Safe_Delete_Array(pIndices);

    return S_OK;
}


void CVIBuffer_Trail::Update(_float _fTimeDelta, _matrix _ParentMatrix)
{
    if (nullptr == m_pVB)
        return;

    _matrix ParentMatrix = _ParentMatrix;

    ParentMatrix.r[0] = XMVector3Normalize(ParentMatrix.r[0]);
    ParentMatrix.r[1] = XMVector3Normalize(ParentMatrix.r[1]);
    ParentMatrix.r[2] = XMVector3Normalize(ParentMatrix.r[2]);

    D3D11_MAPPED_SUBRESOURCE tSubResource;

    m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &tSubResource);

    if (m_tBufferDesc.iVtxCnt >= m_iNumVertices)
    {
        _uint iRemoveCnt(m_tBufferDesc.iLerpPointNum * 2);
        m_tBufferDesc.iVtxCnt -= iRemoveCnt;

        for (_uint i(0); i < m_tBufferDesc.iVtxCnt; i += 2)
        {
            ((VTXPOSTEX*)tSubResource.pData)[i].vPosition = ((VTXPOSTEX*)tSubResource.pData)[iRemoveCnt + i].vPosition;
            ((VTXPOSTEX*)tSubResource.pData)[i + 1].vPosition = ((VTXPOSTEX*)tSubResource.pData)[iRemoveCnt + i + 1].vPosition;
        }
    }

    _vector vPos[2] =
    {
       XMVectorSetW(XMLoadFloat3(&m_tBufferDesc.vPos_0), 1.f),
       XMVectorSetW(XMLoadFloat3(&m_tBufferDesc.vPos_1), 1.f),
    };

    vPos[0] = XMVector3TransformCoord(vPos[0], ParentMatrix);
    vPos[1] = XMVector3TransformCoord(vPos[1], ParentMatrix);

    if (0 == m_tBufferDesc.iVtxCnt)
    {
        for (_uint i(0); i < m_iNumVertices; i += 2)
        {
            XMStoreFloat3(&((VTXPOSTEX*)tSubResource.pData)[i].vPosition, vPos[0]);
            XMStoreFloat3(&((VTXPOSTEX*)tSubResource.pData)[i + 1].vPosition, vPos[1]);
        }
    }
    else
    {
        XMStoreFloat3(&((VTXPOSTEX*)tSubResource.pData)[m_tBufferDesc.iVtxCnt].vPosition, vPos[0]);
        XMStoreFloat3(&((VTXPOSTEX*)tSubResource.pData)[m_tBufferDesc.iVtxCnt + 1].vPosition, vPos[1]);
    }

    // 12개가 포함된 개수라며 6개면, 6 * (12 + 2) = VtxCnt
    m_tBufferDesc.iVtxCnt += 2;

#pragma region CatMullRom
    _uint iEndIndex(m_tBufferDesc.iVtxCnt + m_tBufferDesc.iLerpPointNum * 2);
    if (iEndIndex < m_iNumVertices)
    {
        m_tBufferDesc.iCatMullRomIndex[2] = iEndIndex - 2;  //2개씩 빼는 이유 : 위 아래 2개 쌍 만큼 계산하기 때문에     
        m_tBufferDesc.iCatMullRomIndex[3] = iEndIndex;

        XMStoreFloat3(&((VTXPOSTEX*)tSubResource.pData)[iEndIndex - 2].vPosition, XMLoadFloat3((&((VTXPOSTEX*)tSubResource.pData)[m_tBufferDesc.iVtxCnt - 2].vPosition)));
        XMStoreFloat3(&((VTXPOSTEX*)tSubResource.pData)[iEndIndex - 1].vPosition, XMLoadFloat3((&((VTXPOSTEX*)tSubResource.pData)[m_tBufferDesc.iVtxCnt - 1].vPosition)));

        XMStoreFloat3(&((VTXPOSTEX*)tSubResource.pData)[iEndIndex].vPosition, vPos[0]);
        XMStoreFloat3(&((VTXPOSTEX*)tSubResource.pData)[iEndIndex + 1].vPosition, vPos[1]);

        for (_uint i(0); i < m_tBufferDesc.iLerpPointNum; ++i)
        {
            _uint iIndex(i * 2 + m_tBufferDesc.iVtxCnt - 2);
            _float fWeight(_float(i + 1) / (m_tBufferDesc.iLerpPointNum + 1));

            _vector vPos0 = XMLoadFloat3(&((VTXPOSTEX*)tSubResource.pData)[m_tBufferDesc.iCatMullRomIndex[0]].vPosition);
            _vector vPos1 = XMLoadFloat3(&((VTXPOSTEX*)tSubResource.pData)[m_tBufferDesc.iCatMullRomIndex[1]].vPosition);
            _vector vPos2 = XMLoadFloat3(&((VTXPOSTEX*)tSubResource.pData)[m_tBufferDesc.iCatMullRomIndex[2]].vPosition);
            _vector vPos3 = XMLoadFloat3(&((VTXPOSTEX*)tSubResource.pData)[m_tBufferDesc.iCatMullRomIndex[3]].vPosition);

            _vector LerpPoint = XMVectorCatmullRom(vPos0, vPos1, vPos2, vPos3, fWeight);
            XMStoreFloat3(&((VTXPOSTEX*)tSubResource.pData)[iIndex].vPosition, LerpPoint);

            vPos0 = XMLoadFloat3(&((VTXPOSTEX*)tSubResource.pData)[m_tBufferDesc.iCatMullRomIndex[0] + 1].vPosition);
            vPos1 = XMLoadFloat3(&((VTXPOSTEX*)tSubResource.pData)[m_tBufferDesc.iCatMullRomIndex[1] + 1].vPosition);
            vPos2 = XMLoadFloat3(&((VTXPOSTEX*)tSubResource.pData)[m_tBufferDesc.iCatMullRomIndex[2] + 1].vPosition);
            vPos3 = XMLoadFloat3(&((VTXPOSTEX*)tSubResource.pData)[m_tBufferDesc.iCatMullRomIndex[3] + 1].vPosition);

            LerpPoint = XMVectorCatmullRom(vPos0, vPos1, vPos2, vPos3, fWeight);
            XMStoreFloat3(&((VTXPOSTEX*)tSubResource.pData)[iIndex + 1].vPosition, LerpPoint);

        }

        m_tBufferDesc.iVtxCnt = iEndIndex + 2;

        m_tBufferDesc.iCatMullRomIndex[0] = m_tBufferDesc.iCatMullRomIndex[1];
        m_tBufferDesc.iCatMullRomIndex[1] = m_tBufferDesc.iCatMullRomIndex[2];
    }
#pragma endregion CatMullRom
    for (_uint i(0); i < m_tBufferDesc.iVtxCnt; i += 2)
    {
        ((VTXPOSTEX*)tSubResource.pData)[i].vTexcoord = _float2(_float(i) / _float(m_tBufferDesc.iVtxCnt - 2.f), 1.f);
        ((VTXPOSTEX*)tSubResource.pData)[i + 1].vTexcoord = _float2(_float(i) / _float(m_tBufferDesc.iVtxCnt - 2.f), 0.f);
    }

    m_pContext->Unmap(m_pVB, 0);

}


void CVIBuffer_Trail::Tick(_float fTimeDelta)
{

}

void CVIBuffer_Trail::Reset_Points(_matrix _ParentMatrix)
{
    _matrix ParentMatrix = _ParentMatrix;

    ParentMatrix.r[0] = XMVector3Normalize(ParentMatrix.r[0]);
    ParentMatrix.r[1] = XMVector3Normalize(ParentMatrix.r[1]);
    ParentMatrix.r[2] = XMVector3Normalize(ParentMatrix.r[2]);


    D3D11_MAPPED_SUBRESOURCE      tSubResource;

    m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &tSubResource);


    _vector vPos[2] =
    {
       XMVectorSetW(XMLoadFloat3(&m_tBufferDesc.vPos_0), 1.f),
       XMVectorSetW(XMLoadFloat3(&m_tBufferDesc.vPos_1), 1.f),
    };

    vPos[0] = XMVector3TransformCoord(vPos[0], ParentMatrix);
    vPos[1] = XMVector3TransformCoord(vPos[1], ParentMatrix);

    for (_uint i = 0; i < m_iNumVertices; i += 2)
    {
        XMStoreFloat3(&((VTXPOSTEX*)tSubResource.pData)[i].vPosition, vPos[0]);
        XMStoreFloat3(&((VTXPOSTEX*)tSubResource.pData)[i + 1].vPosition, vPos[1]);
    }

    m_pContext->Unmap(m_pVB, 0);

}

_bool CVIBuffer_Trail::Write_Json(json& Out_Json)
{


    return true;
}

void CVIBuffer_Trail::Load_FromJson(const json& In_Json)
{

}


CVIBuffer_Trail* CVIBuffer_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(pDevice, pContext);

    /* 원형객체를 초기화한다.  */
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CVIBuffer_Rect");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CVIBuffer_Trail::Clone(void* pArg)
{
    CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(*this);

    /* 원형객체를 초기화한다.  */
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CVIBuffer_Rect");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CVIBuffer_Trail::Free()
{
    __super::Free();
}