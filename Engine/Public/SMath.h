#pragma once

#include "Engine_Defines.h"

#include "MyAIMesh.h"
#include "MyAIScene.h"


#define M_PI		3.14159265358979323846
#define M_PI_FLOAT	3.141592f

namespace Engine
{
	struct MESH_DATA;

	namespace SMath
	{
		XMMATRIX		ENGINE_DLL Get_RotationMatrix(FXMMATRIX Mat);
		XMMATRIX		ENGINE_DLL Get_ScaleMatrix(FXMMATRIX Mat);
		XMMATRIX		ENGINE_DLL Get_PositionMatrix(FXMMATRIX Mat);
		XMMATRIX		ENGINE_DLL Get_MatrixNormalize(FXMMATRIX Mat);
		XMMATRIX		ENGINE_DLL Bake_MatrixNormalizeUseLookVector(FXMVECTOR In_vLook);
		XMMATRIX		ENGINE_DLL Get_RotationQuaternion(FXMMATRIX Mat, FXMVECTOR In_vQauternion);

		XMMATRIX		ENGINE_DLL Set_ScaleMatrix(FXMMATRIX Mat, FXMVECTOR Scale);
		XMMATRIX		ENGINE_DLL Go_Right(FXMMATRIX Mat, float fScaler);
		XMMATRIX		ENGINE_DLL Go_Straight(FXMMATRIX Mat, float fScaler);
		XMMATRIX		ENGINE_DLL Go_Up(FXMMATRIX Mat, float fScaler);
		XMMATRIX		ENGINE_DLL Add_PositionWithRotation(FXMMATRIX Mat, FXMVECTOR vPosition);
		XMMATRIX		ENGINE_DLL LookAt(FXMMATRIX Mat, FXMVECTOR In_vPosition);

		XMMATRIX		ENGINE_DLL Bake_WorldMatrix(const XMFLOAT3& In_vScale, const XMFLOAT3& In_vRot, const XMFLOAT3& In_vPos);
		XMMATRIX		ENGINE_DLL Bake_WorldMatrix(const XMFLOAT2& In_vScale, const XMFLOAT3& In_vRot, const XMFLOAT3& In_vPos);

		XMVECTOR		ENGINE_DLL Get_Scale(FXMMATRIX Mat);
		XMVECTOR		ENGINE_DLL Rotate_PositionPitchYawRoll(FXMVECTOR vPosition, FXMVECTOR vPitchYawRoll);

		XMFLOAT3		ENGINE_DLL Extract_PitchYawRollFromRotationMatrix(FXMMATRIX Mat);

		XMFLOAT2		ENGINE_DLL Add_Float2(const XMFLOAT2& Left, const XMFLOAT2& Right);
		XMFLOAT2		ENGINE_DLL Mul_Float2(const XMFLOAT2& Left, const float& Right);
		void			ENGINE_DLL Add_Float2(XMFLOAT2* InOut_Left, const XMFLOAT2& Right);
		void			ENGINE_DLL Mul_Float2(XMFLOAT2* InOut_Left, const float& Right);

		XMFLOAT3		ENGINE_DLL Add_Float3(const XMFLOAT3& Left, const XMFLOAT3& Right);
		XMFLOAT3		ENGINE_DLL Mul_Float3(const XMFLOAT3& Left, const float& Right);
		void			ENGINE_DLL Add_Float3(XMFLOAT3* InOut_Left, const XMFLOAT3& Right);
		void			ENGINE_DLL Mul_Float3(XMFLOAT3* InOut_Left, const float& Right);
		_bool			ENGINE_DLL Equal_Float3(const XMFLOAT3& Left, const XMFLOAT3& Right);

		XMFLOAT4		ENGINE_DLL Add_Float4(const XMFLOAT4& Left, const XMFLOAT4& Right);
		XMFLOAT4		ENGINE_DLL Mul_Float4(const XMFLOAT4& Left, const float& Right);
		void			ENGINE_DLL Add_Float4(XMFLOAT4* InOut_Left, const XMFLOAT4& Right);
		void			ENGINE_DLL Mul_Float4(XMFLOAT4* InOut_Left, const float& Right);

		float			ENGINE_DLL Lerp(const float& fLeft, const float& fRight, float fRatio);
		//XMVECTOR		ENGINE_DLL Lerp_BezierCurve(FXMVECTOR In_Left, FXMVECTOR In_Mid, FXMVECTOR In_Right, const _float In_fRatio);

		int				ENGINE_DLL Random(const int& _iMin, const int& _iMax);
		float			ENGINE_DLL fRandom(const float& _fMin, const float& _fMax);
		XMFLOAT3		ENGINE_DLL vRandom(const XMFLOAT3& _vMin, const XMFLOAT3& _vMax);
		XMFLOAT4		ENGINE_DLL vRandom(const XMFLOAT4& _vMin, const XMFLOAT4& _vMax);
		XMVECTOR		ENGINE_DLL vRandom(const XMVECTOR& _vMin, const XMVECTOR& _vMax);

		//void			ENGINE_DLL Get_MouseRayInWorldSpace(RAY& Out_Ray, const _uint& In_ViewPortWidth, const _uint& In_ViewPortHeight);
		//RAY			ENGINE_DLL Get_MouseRayInWorldSpace(const _uint& In_ViewPortWidth, const _uint& In_ViewPortHeight);

		bool			ENGINE_DLL Is_SphereToRayCollision(const XMFLOAT3& Center, const float& Radius, FXMVECTOR Origin, FXMVECTOR Direction, float& Dist);

		void			ENGINE_DLL Set_ClockwiseTriangle(XMFLOAT3* InOut_TrianglePosition);

		void			ENGINE_DLL Convert_PxVec3FromMeshData(PxVec3* In_PxVec3, CMyAIMesh* pMeshData);
		void			ENGINE_DLL Convert_PxVec3FromMeshDataWithTransformMatrix(PxVec3* In_PxVec3, CMyAIMesh* pMeshData, FXMMATRIX In_TransformMatrix);
		PxExtendedVec3	ENGINE_DLL Convert_PxExtendedVec3(FXMVECTOR In_Vector);
		PxExtendedVec3	ENGINE_DLL Convert_PxExtendedVec3(PxVec3 In_Vector);
		PxVec3			ENGINE_DLL Convert_PxVec3(FXMVECTOR In_Vector);
		PxVec3			ENGINE_DLL Convert_PxVec3(const XMFLOAT3& In_Float3);
		PxVec3			ENGINE_DLL Convert_PxVec3(PxExtendedVec3 In_Vector);
		//XMVECTOR		ENGINE_DLL Convert_Vector(const PxVec3& In_PxVec3);
		//XMVECTOR		ENGINE_DLL Convert_Vector(const PxVec4& In_PxVec4);
		//XMVECTOR		ENGINE_DLL Convert_PxExtendedVec3ToVector(const PxExtendedVec3& In_PxVec3);
		//PxQuat			ENGINE_DLL Convert_PxQuat(FXMVECTOR In_Vector);
		//FMOD_VECTOR		ENGINE_DLL Convert_FMOD_VECTOR(FXMVECTOR In_Vector);


		//_bool			ENGINE_DLL Is_Picked_AbstractTerrain(const RAY& _Ray, _float4* _pOutPos);
		//_bool			ENGINE_DLL Is_Picked_AbstractCube(const RAY& _Ray, MESH_VTX_INFO _VtxInfo, _matrix _WorldMatrix, _float* Out_fDist = nullptr);

		// For. Particle
		const _bool		ENGINE_DLL Is_Equal(const XMFLOAT2 Left, const XMFLOAT2 Right);
		const _bool		ENGINE_DLL Is_Equal(const XMFLOAT3 Left, const XMFLOAT3 Right);
		const _bool		ENGINE_DLL Is_Equal(const XMFLOAT4 Left, const XMFLOAT4 Right);
		const _bool		ENGINE_DLL Is_InRange(const XMFLOAT3 Left, const XMFLOAT3 Right, const _float fRange = DBL_EPSILON);
		const _bool		ENGINE_DLL Is_InRange(const XMFLOAT4 Left, const XMFLOAT4 Right, const _float fRange = DBL_EPSILON);
	
		float			ENGINE_DLL Calculate_AngleBetweenVectors_Radian(const XMFLOAT3& _v1, const XMFLOAT3& _v2);	// 두 벡터 사이의 각도 계산 (라디안)
		float			ENGINE_DLL Calculate_AngleBetweenVectors_Degree(const XMFLOAT3& _v1, const XMFLOAT3& _v2);	// 두 벡터 사이의 각도 계산 (도 Degree)
	
	}
}