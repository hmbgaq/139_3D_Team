#pragma once

#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	typedef struct tagGraphicDesc
	{
		enum WINMODE { WINMODE_FULL, WINMODE_WIN, WINMODE_END };

		WINMODE			eWinMode;
		HWND			hWnd;
		unsigned int	iBackBufferSizeX;
		unsigned int	iBackBufferSizeY;

	}GRAPHIC_DESC;

	typedef struct tagMtrl_Texture
	{
		class CTexture* pMtrlTextures[AI_TEXTURE_TYPE_MAX];

	}MATERIAL_DESC;

	typedef struct tagLightDesc
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_SPOTLIGHT, TYPE_END };

		bool	bEnable = true;					/* 불 킬건지 끌건지 여부 */
		TYPE	eType = TYPE_END;

		/* Directional Light  */
		XMFLOAT4 vDirection = { 0.f, 0.f, 0.f, 0.f };

		/* Point Light  */
		XMFLOAT4 vPosition = { 0.f, 0.f, 0.f, 0.f };
		float	 fRange = 0.f;

		/* Spot Light  */
		float fCutOff = 0.f;
		float fOuterCutOff = 0.f;

		/* Common */
		XMFLOAT4 vDiffuse = { 0.f, 0.f, 0.f, 0.f }; /* 반사될때 출력되는 주된 색 */
		XMFLOAT4 vAmbient = { 0.f, 0.f, 0.f, 0.f }; /* 광원의 확산위치와 무관하게 똑같은양으로 모든점에서 반사되는 색 */
		XMFLOAT4 vSpecular = { 0.f, 0.f, 0.f, 0.f }; /* 특정방향으로만 반사되는 색 */

	public:
		unsigned int iLightIndex = unsigned int(-1);
		const unsigned int Get_LightIndex() const { return iLightIndex; }

	}LIGHT_DESC;

	typedef struct tagAnimation_Channel_Info
	{
		XMFLOAT3		vScale, vPosition;
		XMFLOAT4		vRotation;
		float			fTrackPosition;
	}KEYFRAME;



#pragma region Shader

	typedef struct ENGINE_DLL tagVertex_Position
	{
		XMFLOAT3		vPosition;

		tagVertex_Position()
		{
			ZeroMemory(this, sizeof(tagVertex_Position));
		}

		static const unsigned int					iNumElements = 1;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXPOS;


	typedef struct ENGINE_DLL tagVertex_Position_Size
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vPSize;

		tagVertex_Position_Size()
		{
			ZeroMemory(this, sizeof(tagVertex_Position_Size));
		}
		static const unsigned int					iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXPOINT;

	typedef struct ENGINE_DLL tagVertex_Position_Texcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		tagVertex_Position_Texcoord()
		{
			ZeroMemory(this, sizeof(tagVertex_Position_Texcoord));
		}

		static const unsigned int					iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXPOSTEX;

	typedef struct ENGINE_DLL tagVertex_Cube
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexcoord;

		tagVertex_Cube()
		{
			ZeroMemory(this, sizeof(tagVertex_Cube));
		}

		static const unsigned int					iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXCUBE;

	typedef struct ENGINE_DLL tagVertex_Position_Normal_Texcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		tagVertex_Position_Normal_Texcoord()
		{
			ZeroMemory(this, sizeof(tagVertex_Position_Normal_Texcoord));
		}

		static const unsigned int					iNumElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVertex_Position_Normal_Texcoord_Tangent
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		tagVertex_Position_Normal_Texcoord_Tangent()
		{
			ZeroMemory(this, sizeof(tagVertex_Position_Normal_Texcoord_Tangent));
		}

		static const unsigned int					iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXMESH;

	typedef struct ENGINE_DLL tagVertex_Position_Normal_Texcoord_Tangent_Blendindices_Blendweight
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;
		XMUINT4			vBlendIndices;
		XMFLOAT4		vBlendWeights;

		tagVertex_Position_Normal_Texcoord_Tangent_Blendindices_Blendweight()
		{
			ZeroMemory(this, sizeof(tagVertex_Position_Normal_Texcoord_Tangent_Blendindices_Blendweight));
		}

		static const unsigned int					iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXANIMMESH;

	/* 파티클 + 인스턴싱 */
	typedef struct ENGINE_DLL tagParticle_Rect /* 셰이더 그리는용도 */
	{
		static const unsigned int					iNumElements = 7;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTX_PARTICLE_RECT;

	typedef struct ENGINE_DLL tagParticle_Point
	{
		static const unsigned int					iNumElements = 7;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTX_PARTICLE_POINT;

	typedef struct 
	{
		XMFLOAT4		vRight, vUp, vLook, vPosition;
		XMFLOAT4		vColor;
	}VTXINSTANCE;


	typedef struct ENGINE_DLL tagVertex_Model_Instance
	{
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vTranslation;
	}VTXMODELINSTANCE;

	typedef struct ENGINE_DLL tagVertex_Model_Instance_Declaration
	{
		static const unsigned int				iNumElements = 8;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXMODEL_INSTANCE_DECLARATION;


	typedef struct ENGINE_DLL tagVertex_Dynamic_Texture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		static const unsigned int					iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXGROUND;

	template <typename T>
	constexpr const T& clamp(const T& value, const T& start, const T& end) 
	{
		return (value < start) ? start : (value > end) ? end : value;
	}

#pragma endregion
	

#pragma region 구조체

	typedef struct ENGINE_DLL tag_InstanceDesc
	{
		_float3         vRotation;
		_float3         vScale;
		_float3			vTranslation;
		_float			fMaxRange;
		_float3			vCenter;

		_matrix Get_Matrix() const
		{
			_matrix TransformationMatrix;
			_matrix RotationMatrix, ScaleMatrix;

			_vector vPitchYawRoll;
			_vector vPosition;

			vPitchYawRoll = XMLoadFloat3(&vRotation);
			vPosition = XMVectorSetW(XMLoadFloat3(&vTranslation), 1.f);

			RotationMatrix = XMMatrixRotationRollPitchYawFromVector(vPitchYawRoll);
			ScaleMatrix = XMMatrixScaling(vScale.x, vScale.y, vScale.z);
			TransformationMatrix = ScaleMatrix * RotationMatrix;
			TransformationMatrix.r[3] = vPosition;

			return TransformationMatrix;
		}

		void	Bake_CenterWithMatrix()
		{
			_vector vCenterFromVector = XMLoadFloat3(&vCenter);
			XMStoreFloat3(&vCenter, XMVector3TransformCoord(vCenterFromVector, Get_Matrix()));
		}
	}INSTANCE_INFO_DESC;

	typedef struct ENGINE_DLL tagEnvironment_Desc
	{
		wstring strModelTag = {};
		_uint	iNumInstance = { 0 };
		_uint	iShaderPassIndex = { 1 };
		vector<INSTANCE_INFO_DESC> vecInstanceInfoDesc;
	}MAPTOOL_INSTANCE_DESC;

	typedef struct ENGINE_DLL tagRayDesc
	{
		XMFLOAT4 vOrigin;
		XMFLOAT3 vDirection;
		float	fLength;
	}RAY;


#pragma endregion 구조체

}

#endif // Engine_Struct_h__
