#pragma once

#include "Engine_Defines.h"
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
	
		string			ENGINE_DLL Wstring_To_String(const wstring _wstring);
		wstring			ENGINE_DLL string_To_WString(const string _string);
		string			ENGINE_DLL capitalizeString(string strWord);


		// Color 
		_float4 AliceBlue = { 0.941176534f, 0.972549081f, 1.000000000f, 1.000000000f };
		_float4 AntiqueWhite = { 0.980392218f, 0.921568692f, 0.843137324f, 1.000000000f };
		_float4 Aqua = { 0.000000000f, 1.000000000f, 1.000000000f, 1.000000000f };
		_float4 Aquamarine = { 0.498039246f, 1.000000000f, 0.831372619f, 1.000000000f };
		_float4 Azure = { 0.941176534f, 1.000000000f, 1.000000000f, 1.000000000f };
		_float4 Beige = { 0.960784376f, 0.960784376f, 0.862745166f, 1.000000000f };
		_float4 Bisque = { 1.000000000f, 0.894117713f, 0.768627524f, 1.000000000f };
		_float4 Black = { 0.000000000f, 0.000000000f, 0.000000000f, 1.000000000f };
		_float4 BlanchedAlmond = { 1.000000000f, 0.921568692f, 0.803921640f, 1.000000000f };
		_float4 Blue = { 0.000000000f, 0.000000000f, 1.000000000f, 1.000000000f };
		_float4 BlueViolet = { 0.541176498f, 0.168627456f, 0.886274576f, 1.000000000f };
		_float4 Brown = { 0.647058845f, 0.164705887f, 0.164705887f, 1.000000000f };
		_float4 BurlyWood = { 0.870588303f, 0.721568644f, 0.529411793f, 1.000000000f };
		_float4 CadetBlue = { 0.372549027f, 0.619607866f, 0.627451003f, 1.000000000f };
		_float4 Chartreuse = { 0.498039246f, 1.000000000f, 0.000000000f, 1.000000000f };
		_float4 Chocolate = { 0.823529482f, 0.411764741f, 0.117647067f, 1.000000000f };
		_float4 Coral = { 1.000000000f, 0.498039246f, 0.313725501f, 1.000000000f };
		_float4 CornflowerBlue = { 0.392156899f, 0.584313750f, 0.929411829f, 1.000000000f };
		_float4 Cornsilk = { 1.000000000f, 0.972549081f, 0.862745166f, 1.000000000f };
		_float4 Crimson = { 0.862745166f, 0.078431375f, 0.235294133f, 1.000000000f };
		_float4 Cyan = { 0.000000000f, 1.000000000f, 1.000000000f, 1.000000000f };
		_float4 DarkBlue = { 0.000000000f, 0.000000000f, 0.545098066f, 1.000000000f };
		_float4 DarkCyan = { 0.000000000f, 0.545098066f, 0.545098066f, 1.000000000f };
		_float4 DarkGoldenrod = { 0.721568644f, 0.525490224f, 0.043137256f, 1.000000000f };
		_float4 DarkGray = { 0.662745118f, 0.662745118f, 0.662745118f, 1.000000000f };
		_float4 DarkGreen = { 0.000000000f, 0.392156899f, 0.000000000f, 1.000000000f };
		_float4 DarkKhaki = { 0.741176486f, 0.717647076f, 0.419607878f, 1.000000000f };
		_float4 DarkMagenta = { 0.545098066f, 0.000000000f, 0.545098066f, 1.000000000f };
		_float4 DarkOliveGreen = { 0.333333343f, 0.419607878f, 0.184313729f, 1.000000000f };
		_float4 DarkOrange = { 1.000000000f, 0.549019635f, 0.000000000f, 1.000000000f };
		_float4 DarkOrchid = { 0.600000024f, 0.196078449f, 0.800000072f, 1.000000000f };
		_float4 DarkRed = { 0.545098066f, 0.000000000f, 0.000000000f, 1.000000000f };
		_float4 DarkSalmon = { 0.913725555f, 0.588235319f, 0.478431404f, 1.000000000f };
		_float4 DarkSeaGreen = { 0.560784340f, 0.737254918f, 0.545098066f, 1.000000000f };
		_float4 DarkSlateBlue = { 0.282352954f, 0.239215702f, 0.545098066f, 1.000000000f };
		_float4 DarkSlateGray = { 0.184313729f, 0.309803933f, 0.309803933f, 1.000000000f };
		_float4 DarkTurquoise = { 0.000000000f, 0.807843208f, 0.819607913f, 1.000000000f };
		_float4 DarkViolet = { 0.580392182f, 0.000000000f, 0.827451050f, 1.000000000f };
		_float4 DeepPink = { 1.000000000f, 0.078431375f, 0.576470613f, 1.000000000f };
		_float4 DeepSkyBlue = { 0.000000000f, 0.749019623f, 1.000000000f, 1.000000000f };
		_float4 DimGray = { 0.411764741f, 0.411764741f, 0.411764741f, 1.000000000f };
		_float4 DodgerBlue = { 0.117647067f, 0.564705908f, 1.000000000f, 1.000000000f };
		_float4 Firebrick = { 0.698039234f, 0.133333340f, 0.133333340f, 1.000000000f };
		_float4 FloralWhite = { 1.000000000f, 0.980392218f, 0.941176534f, 1.000000000f };
		_float4 ForestGreen = { 0.133333340f, 0.545098066f, 0.133333340f, 1.000000000f };
		_float4 Fuchsia = { 1.000000000f, 0.000000000f, 1.000000000f, 1.000000000f };
		_float4 Gainsboro = { 0.862745166f, 0.862745166f, 0.862745166f, 1.000000000f };
		_float4 GhostWhite = { 0.972549081f, 0.972549081f, 1.000000000f, 1.000000000f };
		_float4 Gold = { 1.000000000f, 0.843137324f, 0.000000000f, 1.000000000f };
		_float4 Goldenrod = { 0.854902029f, 0.647058845f, 0.125490203f, 1.000000000f };
		_float4 Gray = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
		_float4 Green = { 0.000000000f, 0.501960814f, 0.000000000f, 1.000000000f };
		_float4 GreenYellow = { 0.678431392f, 1.000000000f, 0.184313729f, 1.000000000f };
		_float4 Honeydew = { 0.941176534f, 1.000000000f, 0.941176534f, 1.000000000f };
		_float4 HotPink = { 1.000000000f, 0.411764741f, 0.705882370f, 1.000000000f };
		_float4 IndianRed = { 0.803921640f, 0.360784322f, 0.360784322f, 1.000000000f };
		_float4 Indigo = { 0.294117659f, 0.000000000f, 0.509803951f, 1.000000000f };
		_float4 Ivory = { 1.000000000f, 1.000000000f, 0.941176534f, 1.000000000f };
		_float4 Khaki = { 0.941176534f, 0.901960850f, 0.549019635f, 1.000000000f };
		_float4 Lavender = { 0.901960850f, 0.901960850f, 0.980392218f, 1.000000000f };
		_float4 LavenderBlush = { 1.000000000f, 0.941176534f, 0.960784376f, 1.000000000f };
		_float4 LawnGreen = { 0.486274540f, 0.988235354f, 0.000000000f, 1.000000000f };
		_float4 LemonChiffon = { 1.000000000f, 0.980392218f, 0.803921640f, 1.000000000f };
		_float4 LightBlue = { 0.678431392f, 0.847058892f, 0.901960850f, 1.000000000f };
		_float4 LightCoral = { 0.941176534f, 0.501960814f, 0.501960814f, 1.000000000f };
		_float4 LightCyan = { 0.878431439f, 1.000000000f, 1.000000000f, 1.000000000f };
		_float4 LightGoldenrodYellow = { 0.980392218f, 0.980392218f, 0.823529482f, 1.000000000f };
		_float4 LightGreen = { 0.564705908f, 0.933333397f, 0.564705908f, 1.000000000f };
		_float4 LightGray = { 0.827451050f, 0.827451050f, 0.827451050f, 1.000000000f };
		_float4 LightPink = { 1.000000000f, 0.713725507f, 0.756862819f, 1.000000000f };
		_float4 LightSalmon = { 1.000000000f, 0.627451003f, 0.478431404f, 1.000000000f };
		_float4 LightSeaGreen = { 0.125490203f, 0.698039234f, 0.666666687f, 1.000000000f };
		_float4 LightSkyBlue = { 0.529411793f, 0.807843208f, 0.980392218f, 1.000000000f };
		_float4 LightSlateGray = { 0.466666698f, 0.533333361f, 0.600000024f, 1.000000000f };
		_float4 LightSteelBlue = { 0.690196097f, 0.768627524f, 0.870588303f, 1.000000000f };
		_float4 LightYellow = { 1.000000000f, 1.000000000f, 0.878431439f, 1.000000000f };
		_float4 Lime = { 0.000000000f, 1.000000000f, 0.000000000f, 1.000000000f };
		_float4 LimeGreen = { 0.196078449f, 0.803921640f, 0.196078449f, 1.000000000f };
		_float4 Linen = { 0.980392218f, 0.941176534f, 0.901960850f, 1.000000000f };
		_float4 Magenta = { 1.000000000f, 0.000000000f, 1.000000000f, 1.000000000f };
		_float4 Maroon = { 0.501960814f, 0.000000000f, 0.000000000f, 1.000000000f };
		_float4 MediumAquamarine = { 0.400000036f, 0.803921640f, 0.666666687f, 1.000000000f };
		_float4 MediumBlue = { 0.000000000f, 0.000000000f, 0.803921640f, 1.000000000f };
		_float4 MediumOrchid = { 0.729411781f, 0.333333343f, 0.827451050f, 1.000000000f };
		_float4 MediumPurple = { 0.576470613f, 0.439215720f, 0.858823597f, 1.000000000f };
		_float4 MediumSeaGreen = { 0.235294133f, 0.701960802f, 0.443137288f, 1.000000000f };
		_float4 MediumSlateBlue = { 0.482352972f, 0.407843173f, 0.933333397f, 1.000000000f };
		_float4 MediumSpringGreen = { 0.000000000f, 0.980392218f, 0.603921592f, 1.000000000f };
		_float4 MediumTurquoise = { 0.282352954f, 0.819607913f, 0.800000072f, 1.000000000f };
		_float4 MediumVioletRed = { 0.780392230f, 0.082352944f, 0.521568656f, 1.000000000f };
		_float4 MidnightBlue = { 0.098039225f, 0.098039225f, 0.439215720f, 1.000000000f };
		_float4 MintCream = { 0.960784376f, 1.000000000f, 0.980392218f, 1.000000000f };
		_float4 MistyRose = { 1.000000000f, 0.894117713f, 0.882353008f, 1.000000000f };
		_float4 Moccasin = { 1.000000000f, 0.894117713f, 0.709803939f, 1.000000000f };
		_float4 NavajoWhite = { 1.000000000f, 0.870588303f, 0.678431392f, 1.000000000f };
		_float4 Navy = { 0.000000000f, 0.000000000f, 0.501960814f, 1.000000000f };
		_float4 OldLace = { 0.992156923f, 0.960784376f, 0.901960850f, 1.000000000f };
		_float4 Olive = { 0.501960814f, 0.501960814f, 0.000000000f, 1.000000000f };
		_float4 OliveDrab = { 0.419607878f, 0.556862772f, 0.137254909f, 1.000000000f };
		_float4 Orange = { 1.000000000f, 0.647058845f, 0.000000000f, 1.000000000f };
		_float4 OrangeRed = { 1.000000000f, 0.270588249f, 0.000000000f, 1.000000000f };
		_float4 Orchid = { 0.854902029f, 0.439215720f, 0.839215755f, 1.000000000f };
		_float4 PaleGoldenrod = { 0.933333397f, 0.909803987f, 0.666666687f, 1.000000000f };
		_float4 PaleGreen = { 0.596078455f, 0.984313786f, 0.596078455f, 1.000000000f };
		_float4 PaleTurquoise = { 0.686274529f, 0.933333397f, 0.933333397f, 1.000000000f };
		_float4 PaleVioletRed = { 0.858823597f, 0.439215720f, 0.576470613f, 1.000000000f };
		_float4 PapayaWhip = { 1.000000000f, 0.937254965f, 0.835294187f, 1.000000000f };
		_float4 PeachPuff = { 1.000000000f, 0.854902029f, 0.725490212f, 1.000000000f };
		_float4 Peru = { 0.803921640f, 0.521568656f, 0.247058839f, 1.000000000f };
		_float4 Pink = { 1.000000000f, 0.752941251f, 0.796078503f, 1.000000000f };
		_float4 Plum = { 0.866666734f, 0.627451003f, 0.866666734f, 1.000000000f };
		_float4 PowderBlue = { 0.690196097f, 0.878431439f, 0.901960850f, 1.000000000f };
		_float4 Purple = { 0.501960814f, 0.000000000f, 0.501960814f, 1.000000000f };
		_float4 Red = { 1.000000000f, 0.000000000f, 0.000000000f, 1.000000000f };
		_float4 RosyBrown = { 0.737254918f, 0.560784340f, 0.560784340f, 1.000000000f };
		_float4 RoyalBlue = { 0.254901975f, 0.411764741f, 0.882353008f, 1.000000000f };
		_float4 SaddleBrown = { 0.545098066f, 0.270588249f, 0.074509807f, 1.000000000f };
		_float4 Salmon = { 0.980392218f, 0.501960814f, 0.447058856f, 1.000000000f };
		_float4 SandyBrown = { 0.956862807f, 0.643137276f, 0.376470625f, 1.000000000f };
		_float4 SeaGreen = { 0.180392161f, 0.545098066f, 0.341176480f, 1.000000000f };
		_float4 SeaShell = { 1.000000000f, 0.960784376f, 0.933333397f, 1.000000000f };
		_float4 Sienna = { 0.627451003f, 0.321568638f, 0.176470593f, 1.000000000f };
		_float4 Silver = { 0.752941251f, 0.752941251f, 0.752941251f, 1.000000000f };
		_float4 SkyBlue = { 0.529411793f, 0.807843208f, 0.921568692f, 1.000000000f };
		_float4 SlateBlue = { 0.415686309f, 0.352941185f, 0.803921640f, 1.000000000f };
		_float4 SlateGray = { 0.439215720f, 0.501960814f, 0.564705908f, 1.000000000f };
		_float4 Snow = { 1.000000000f, 0.980392218f, 0.980392218f, 1.000000000f };
		_float4 SpringGreen = { 0.000000000f, 1.000000000f, 0.498039246f, 1.000000000f };
		_float4 SteelBlue = { 0.274509817f, 0.509803951f, 0.705882370f, 1.000000000f };
		_float4 Tan = { 0.823529482f, 0.705882370f, 0.549019635f, 1.000000000f };
		_float4 Teal = { 0.000000000f, 0.501960814f, 0.501960814f, 1.000000000f };
		_float4 Thistle = { 0.847058892f, 0.749019623f, 0.847058892f, 1.000000000f };
		_float4 Tomato = { 1.000000000f, 0.388235331f, 0.278431386f, 1.000000000f };
		_float4 Transparent = { 0.000000000f, 0.000000000f, 0.000000000f, 0.000000000f };
		_float4 Turquoise = { 0.250980407f, 0.878431439f, 0.815686345f, 1.000000000f };
		_float4 Violet = { 0.933333397f, 0.509803951f, 0.933333397f, 1.000000000f };
		_float4 Wheat = { 0.960784376f, 0.870588303f, 0.701960802f, 1.000000000f };
		_float4 White = { 1.000000000f, 1.000000000f, 1.000000000f, 1.000000000f };
		_float4 WhiteSmoke = { 0.960784376f, 0.960784376f, 0.960784376f, 1.000000000f };
		_float4 Yellow = { 1.000000000f, 1.000000000f, 0.000000000f, 1.000000000f };
		_float4 YellowGreen = { 0.603921592f, 0.803921640f, 0.196078449f, 1.000000000f };
	}
}