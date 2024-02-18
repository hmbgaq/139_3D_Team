#ifndef Engine_Typedef_h__
#define Engine_Typedef_h__

namespace Engine
{
	typedef		bool						_bool;

	typedef		signed char					_byte;
	typedef		unsigned char				_ubyte;

	typedef		char						_char;
	typedef		wchar_t						_tchar;
	
	typedef		signed short				_short;
	typedef		unsigned short				_ushort;

	typedef		signed int					_int;
	typedef		unsigned int				_uint;

	typedef		XMINT2						_int2;

	typedef		signed long					_long;
	typedef		unsigned long				_ulong;

	typedef		signed long long			_longlong;
	typedef		unsigned long long			_ulonglong;

	typedef		float						_float;
	typedef		double						_double;	

	///* 저장용 데이터 타입. */
	//typedef		XMFLOAT4X4					_float4x4;

	///* 연산용 데이터 타입. */
	//typedef		XMMATRIX					_matrix;
	//typedef		FXMMATRIX					_fmatrix;
	//typedef		CXMMATRIX					_cmatrix;

	///* 저장용 데이터 타입. */
	//typedef		XMFLOAT4					_float4;
	//typedef		XMFLOAT3					_float3;
	//typedef		XMFLOAT2					_float2;

	///* 연산용 데이터 타입. */
	//typedef		XMVECTOR					_vector;
	//typedef		FXMVECTOR					_fvector;
	//typedef		GXMVECTOR					_gvector;
	//typedef		HXMVECTOR					_hvector;
	//typedef		CXMVECTOR					_cvector;

	typedef void(*OnEvent)();
	typedef _bool(*Condition)();


	typedef struct tagInt32
	{
		tagInt32(unsigned int _ix, unsigned int _iy, unsigned int _iz)
			: ix(_ix)
			, iy(_iy)
			, iz(_iz) {	}

		tagInt32()
			: ix(0)
			, iy(0)
			, iz(0) {	}

		unsigned int ix, iy, iz;
	} _uint3;

	typedef struct tagFaceIndices32
	{
		_ulong _1, _2, _3;
	}FACEINDICES32;

	typedef struct tagFaceIndices16
	{
		unsigned short _1, _2, _3;
	}FACEINDICES16;

	typedef struct ENGINE_DLL float2 : public DirectX::XMFLOAT2
	{
		float2() : XMFLOAT2{} {}
		float2(_float _x, _float _y) : XMFLOAT2(_x, _y) {}

		float2(XMFLOAT2 _rhs) : XMFLOAT2(_rhs) {}
		float2(XMVECTOR _vector) { XMStoreFloat2(this, _vector); }

		_float length() const noexcept;
		float2 normalize() const noexcept;

		// assignment operators
		float2& operator = (const XMVECTOR&) noexcept;

		float2& operator += (const XMVECTOR&) noexcept;
		float2& operator -= (const XMVECTOR&) noexcept;
		float2& operator *= (const XMVECTOR&) noexcept;
		float2& operator /= (const XMVECTOR&) noexcept;

		float2& operator += (const float2&) noexcept;
		float2& operator -= (const float2&) noexcept;
		float2& operator *= (const float2&) noexcept;
		float2& operator /= (const float2&) noexcept;

		float2& operator *= (float) noexcept;
		float2& operator /= (float) noexcept;

		// unary operators
		float2 operator + () const noexcept;
		float2 operator - () const noexcept;

		// binary operators
		float2 operator + (const XMVECTOR&) const noexcept;
		float2 operator - (const XMVECTOR&) const noexcept;
		float2 operator * (const XMVECTOR&) const noexcept;
		float2 operator / (const XMVECTOR&) const noexcept;

		float2 operator + (const float2&) const noexcept;
		float2 operator - (const float2&) const noexcept;
		float2 operator * (const float2&) const noexcept;
		float2 operator / (const float2&) const noexcept;

		float2 operator * (float) const noexcept;
		float2 operator / (float) const noexcept;

		_bool operator == (const XMVECTOR&) const noexcept;
		_bool operator != (const XMVECTOR&) const noexcept;
		_bool operator == (const float2&) const noexcept;
		_bool operator != (const float2&) const noexcept;

		operator XMVECTOR() const noexcept;
	} _float2;

	typedef struct ENGINE_DLL	float3 : public DirectX::XMFLOAT3
	{
		float3() : XMFLOAT3{} {}
		float3(_float _x, _float _y, _float _z) : XMFLOAT3(_x, _y, _z) {}

		float3(XMFLOAT3 _rhs) : XMFLOAT3(_rhs) {}
		float3(XMVECTOR _vector) { XMStoreFloat3(this, _vector); }

		_float length() const noexcept;
		float3 normalize() const noexcept;

		// assignment operators
		float3& operator = (const XMVECTOR&) noexcept;

		float3& operator += (const XMVECTOR&) noexcept;
		float3& operator -= (const XMVECTOR&) noexcept;
		float3& operator *= (const XMVECTOR&) noexcept;
		float3& operator /= (const XMVECTOR&) noexcept;

		float3& operator += (const float3&) noexcept;
		float3& operator -= (const float3&) noexcept;
		float3& operator *= (const float3&) noexcept;
		float3& operator /= (const float3&) noexcept;

		float3& operator *= (float) noexcept;
		float3& operator /= (float) noexcept;

		// unary operators
		float3 operator + () const noexcept;
		float3 operator - () const noexcept;

		// binary operators
		float3 operator + (const XMVECTOR&) const noexcept;
		float3 operator - (const XMVECTOR&) const noexcept;
		float3 operator * (const XMVECTOR&) const noexcept;
		float3 operator / (const XMVECTOR&) const noexcept;

		float3 operator + (const float3&) const noexcept;
		float3 operator - (const float3&) const noexcept;
		float3 operator * (const float3&) const noexcept;
		float3 operator / (const float3&) const noexcept;

		float3 operator * (float) const noexcept;
		float3 operator / (float) const noexcept;

		_bool operator == (const XMVECTOR&) const noexcept;
		_bool operator != (const XMVECTOR&) const noexcept;
		_bool operator == (const float3&) const noexcept;
		_bool operator != (const float3&) const noexcept;

		operator XMVECTOR() const noexcept;
	} _float3;

	typedef struct ENGINE_DLL	float4 : public DirectX::XMFLOAT4
	{
		float4() : XMFLOAT4{} {}
		float4(_float _x, _float _y, _float _z, _float _w) : XMFLOAT4(_x, _y, _z, _w) {}
		float4(XMFLOAT3 _float3, _float _w) : XMFLOAT4(_float3.x, _float3.y, _float3.z, _w) {}

		float4(XMFLOAT4 _rhs) : XMFLOAT4(_rhs) {}
		float4(XMVECTOR _vector) { XMStoreFloat4(this, _vector); }

		_float length() const noexcept;
		float4 normalize() const noexcept;

		// assignment operators
		float4& operator = (const XMVECTOR&) noexcept;

		float4& operator += (const XMVECTOR&) noexcept;
		float4& operator -= (const XMVECTOR&) noexcept;
		float4& operator *= (const XMVECTOR&) noexcept;
		float4& operator /= (const XMVECTOR&) noexcept;

		float4& operator += (const float4&) noexcept;
		float4& operator -= (const float4&) noexcept;
		float4& operator *= (const float4&) noexcept;
		float4& operator /= (const float4&) noexcept;

		float4& operator *= (float) noexcept;
		float4& operator /= (float) noexcept;

		// unary operators
		float4 operator + () const noexcept;
		float4 operator - () const noexcept;

		// binary operators
		float4 operator + (const XMVECTOR&) const noexcept;
		float4 operator - (const XMVECTOR&) const noexcept;
		float4 operator * (const XMVECTOR&) const noexcept;
		float4 operator / (const XMVECTOR&) const noexcept;

		float4 operator + (const float4&) const noexcept;
		float4 operator - (const float4&) const noexcept;
		float4 operator * (const float4&) const noexcept;
		float4 operator / (const float4&) const noexcept;

		float4 operator * (float) const noexcept;
		float4 operator / (float) const noexcept;

		_bool operator == (const XMVECTOR&) const noexcept;
		_bool operator != (const XMVECTOR&) const noexcept;
		_bool operator == (const float4&) const noexcept;
		_bool operator != (const float4&) const noexcept;

		operator XMVECTOR() const noexcept;
	} _float4, _quaternion, _color;

	typedef struct ENGINE_DLL	float4x4 : public DirectX::XMFLOAT4X4
	{
		float4x4() : XMFLOAT4X4{} {}
		float4x4(float4 _a, float4 _b, float4 _c, float4 _d) : XMFLOAT4X4(
			_a.x, _a.y, _a.z, _a.w,
			_b.x, _b.y, _b.z, _b.w,
			_c.x, _c.y, _c.z, _c.w,
			_d.x, _d.y, _d.z, _d.w
		) {}
		float4x4(XMFLOAT4X4 _float4x4) : XMFLOAT4X4(_float4x4) {}
		float4x4(XMMATRIX _matrix) { XMStoreFloat4x4(this, _matrix); }

		float4x4 inverse() const noexcept;
		float4 row(_uint _index) const noexcept;

		// assignment operators
		float4x4& operator = (const XMMATRIX&) noexcept;

		float4x4& operator += (const XMMATRIX&) noexcept;
		float4x4& operator -= (const XMMATRIX&) noexcept;
		float4x4& operator *= (const XMMATRIX&) noexcept;

		float4x4& operator += (const float4x4&) noexcept;
		float4x4& operator -= (const float4x4&) noexcept;
		float4x4& operator *= (const float4x4&) noexcept;

		float4x4& operator *= (float) noexcept;
		float4x4& operator /= (float) noexcept;

		// unary operators
		float4x4 operator + () const noexcept;
		float4x4 operator - () const noexcept;

		// binary operators
		float4x4 operator + (const XMMATRIX&) const noexcept;
		float4x4 operator - (const XMMATRIX&) const noexcept;
		float4x4 operator * (const XMMATRIX&) const noexcept;

		float4x4 operator + (const float4x4&) const noexcept;
		float4x4 operator - (const float4x4&) const noexcept;
		float4x4 operator * (const float4x4&) const noexcept;

		float4x4 operator * (float) const noexcept;
		float4x4 operator / (float) const noexcept;

		_bool operator == (const XMMATRIX&) const noexcept;
		_bool operator != (const XMMATRIX&) const noexcept;
		_bool operator == (const float4x4&) const noexcept;
		_bool operator != (const float4x4&) const noexcept;

		operator XMMATRIX() const noexcept;
	} _float4x4;

	typedef DirectX::XMVECTOR	_vector;
	typedef DirectX::FXMVECTOR	_fvector;
	typedef DirectX::GXMVECTOR	_gvector;
	typedef DirectX::HXMVECTOR	_hvector;
	typedef DirectX::CXMVECTOR	_cvector;

	typedef DirectX::XMMATRIX	_matrix;
	typedef DirectX::FXMMATRIX	_fmatrix;
	typedef DirectX::CXMMATRIX	_cmatrix;

	typedef array<float3, 3>	POLYGON;
}

namespace Engine
{
#pragma region _float2

	__forceinline _float
		float2::length() const noexcept
	{
		return XMVectorGetX(XMVector2Length(*this));
	}

	__forceinline float2
		float2::normalize() const noexcept
	{
		return XMVector2Normalize(*this);
	}

#pragma region Assignment Operators

	__forceinline float2&
		float2::operator = (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat2(this, _v);

		return *this;
	}

	__forceinline float2&
		float2::operator += (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat2(this, XMVectorAdd(XMLoadFloat2(this), _v));

		return *this;
	}

	__forceinline float2&
		float2::operator -= (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat2(this, XMVectorSubtract(XMLoadFloat2(this), _v));

		return *this;
	}

	__forceinline float2&
		float2::operator *= (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat2(this, XMVectorMultiply(XMLoadFloat2(this), _v));

		return *this;
	}

	__forceinline float2&
		float2::operator /= (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat2(this, XMVectorDivide(XMLoadFloat2(this), _v));

		return *this;
	}

	__forceinline float2&
		float2::operator += (const float2& _v) noexcept
	{
		XMStoreFloat2(this, XMVectorAdd(XMLoadFloat2(this), XMLoadFloat2(&_v)));

		return *this;
	}

	__forceinline float2&
		float2::operator -= (const float2& _v) noexcept
	{
		XMStoreFloat2(this, XMVectorSubtract(XMLoadFloat2(this), XMLoadFloat2(&_v)));

		return *this;
	}

	__forceinline float2&
		float2::operator *= (const float2& _v) noexcept
	{
		XMStoreFloat2(this, XMVectorMultiply(XMLoadFloat2(this), XMLoadFloat2(&_v)));

		return *this;
	}

	__forceinline float2&
		float2::operator /= (const float2& _v) noexcept
	{
		XMStoreFloat2(this, XMVectorDivide(XMLoadFloat2(this), XMLoadFloat2(&_v)));

		return *this;
	}

	__forceinline float2&
		float2::operator *= (float _f) noexcept
	{
		XMStoreFloat2(this, XMVectorScale(XMLoadFloat2(this), _f));

		return *this;
	}

	__forceinline float2&
		float2::operator /= (float _f) noexcept
	{
		XMStoreFloat2(this, XMVectorDivide(XMLoadFloat2(this), XMVectorReplicate(_f)));

		return *this;
	}

#pragma endregion
#pragma region Unary Operators

	__forceinline float2
		float2::operator + () const noexcept
	{
		return *this;
	}

	__forceinline float2
		float2::operator - () const noexcept
	{
		float2 out;

		XMStoreFloat2(&out, XMVectorScale(XMLoadFloat2(this), -1.f));

		return out;
	}

#pragma endregion
#pragma region Binary Operators

	__forceinline float2
		float2::operator + (const XMVECTOR& _v) const noexcept
	{
		float2 out;

		XMStoreFloat2(&out, XMVectorAdd(XMLoadFloat2(this), _v));

		return out;
	}

	__forceinline float2
		float2::operator - (const XMVECTOR& _v) const noexcept
	{
		float2 out;

		XMStoreFloat2(&out, XMVectorSubtract(XMLoadFloat2(this), _v));

		return out;
	}

	__forceinline float2
		float2::operator * (const XMVECTOR& _v) const noexcept
	{
		float2 out;

		XMStoreFloat2(&out, XMVectorMultiply(XMLoadFloat2(this), _v));

		return out;
	}

	__forceinline float2
		float2::operator / (const XMVECTOR& _v) const noexcept
	{
		float2 out;

		XMStoreFloat2(&out, XMVectorDivide(XMLoadFloat2(this), _v));

		return out;
	}

	__forceinline float2
		float2::operator + (const float2& _v) const noexcept
	{
		float2 out;

		XMStoreFloat2(&out, XMVectorAdd(XMLoadFloat2(this), XMLoadFloat2(&_v)));

		return out;
	}

	__forceinline float2
		float2::operator - (const float2& _v) const noexcept
	{
		float2 out;

		XMStoreFloat2(&out, XMVectorSubtract(XMLoadFloat2(this), XMLoadFloat2(&_v)));

		return out;
	}

	__forceinline float2
		float2::operator * (const float2& _v) const noexcept
	{
		float2 out;

		XMStoreFloat2(&out, XMVectorMultiply(XMLoadFloat2(this), XMLoadFloat2(&_v)));

		return out;
	}

	__forceinline float2
		float2::operator / (const float2& _v) const noexcept
	{
		float2 out;

		XMStoreFloat2(&out, XMVectorDivide(XMLoadFloat2(this), XMLoadFloat2(&_v)));

		return out;
	}

	__forceinline float2
		float2::operator * (float _f) const noexcept
	{
		float2 out;

		XMStoreFloat2(&out, XMVectorScale(XMLoadFloat2(this), _f));

		return out;
	}

	__forceinline float2
		float2::operator / (float _f) const noexcept
	{
		float2 out;

		XMStoreFloat2(&out, XMVectorDivide(XMLoadFloat2(this), XMVectorReplicate(_f)));

		return out;
	}

	__forceinline _bool
		float2::operator == (const XMVECTOR& _v) const noexcept
	{
		return XMVector2Equal(XMLoadFloat2(this), _v);
	}

	__forceinline _bool
		float2::operator != (const XMVECTOR& _v) const noexcept
	{
		return !XMVector2Equal(XMLoadFloat2(this), _v);
	}

	__forceinline _bool
		float2::operator == (const float2& _v) const noexcept
	{
		return XMVector2Equal(XMLoadFloat2(this), XMLoadFloat2(&_v));
	}

	__forceinline _bool
		float2::operator != (const float2& _v) const noexcept
	{
		return !XMVector2Equal(XMLoadFloat2(this), XMLoadFloat2(&_v));
	}

#pragma endregion

	__forceinline
		float2::operator XMVECTOR() const noexcept
	{
		return XMLoadFloat2(this);
	}

#pragma endregion

#pragma region _float3

	__forceinline _float
		float3::length() const noexcept
	{
		return XMVectorGetX(XMVector3Length(*this));
	}

	__forceinline float3
		float3::normalize() const noexcept
	{
		return XMVector3Normalize(*this);
	}

#pragma region Assignment Operators

	__forceinline float3&
		float3::operator = (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat3(this, _v);

		return *this;
	}

	__forceinline float3&
		float3::operator += (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat3(this, XMVectorAdd(XMLoadFloat3(this), _v));

		return *this;
	}

	__forceinline float3&
		float3::operator -= (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat3(this, XMVectorSubtract(XMLoadFloat3(this), _v));

		return *this;
	}

	__forceinline float3&
		float3::operator *= (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat3(this, XMVectorMultiply(XMLoadFloat3(this), _v));

		return *this;
	}

	__forceinline float3&
		float3::operator /= (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat3(this, XMVectorDivide(XMLoadFloat3(this), _v));

		return *this;
	}

	__forceinline float3&
		float3::operator += (const float3& _v) noexcept
	{
		XMStoreFloat3(this, XMVectorAdd(XMLoadFloat3(this), XMLoadFloat3(&_v)));

		return *this;
	}

	__forceinline float3&
		float3::operator -= (const float3& _v) noexcept
	{
		XMStoreFloat3(this, XMVectorSubtract(XMLoadFloat3(this), XMLoadFloat3(&_v)));

		return *this;
	}

	__forceinline float3&
		float3::operator *= (const float3& _v) noexcept
	{
		XMStoreFloat3(this, XMVectorMultiply(XMLoadFloat3(this), XMLoadFloat3(&_v)));

		return *this;
	}

	__forceinline float3&
		float3::operator /= (const float3& _v) noexcept
	{
		XMStoreFloat3(this, XMVectorDivide(XMLoadFloat3(this), XMLoadFloat3(&_v)));

		return *this;
	}

	__forceinline float3&
		float3::operator *= (float _f) noexcept
	{
		XMStoreFloat3(this, XMVectorScale(XMLoadFloat3(this), _f));

		return *this;
	}

	__forceinline float3&
		float3::operator /= (float _f) noexcept
	{
		XMStoreFloat3(this, XMVectorDivide(XMLoadFloat3(this), XMVectorReplicate(_f)));

		return *this;
	}

#pragma endregion
#pragma region Unary Operators

	__forceinline float3
		float3::operator + () const noexcept
	{
		return *this;
	}

	__forceinline float3
		float3::operator - () const noexcept
	{
		float3 out;

		XMStoreFloat3(&out, XMVectorScale(XMLoadFloat3(this), -1.f));

		return out;
	}

#pragma endregion
#pragma region Binary Operators

	__forceinline float3
		float3::operator + (const XMVECTOR& _v) const noexcept
	{
		float3 out;

		XMStoreFloat3(&out, XMVectorAdd(XMLoadFloat3(this), _v));

		return out;
	}

	__forceinline float3
		float3::operator - (const XMVECTOR& _v) const noexcept
	{
		float3 out;

		XMStoreFloat3(&out, XMVectorSubtract(XMLoadFloat3(this), _v));

		return out;
	}

	__forceinline float3
		float3::operator * (const XMVECTOR& _v) const noexcept
	{
		float3 out;

		XMStoreFloat3(&out, XMVectorMultiply(XMLoadFloat3(this), _v));

		return out;
	}

	__forceinline float3
		float3::operator / (const XMVECTOR& _v) const noexcept
	{
		float3 out;

		XMStoreFloat3(&out, XMVectorDivide(XMLoadFloat3(this), _v));

		return out;
	}

	__forceinline float3
		float3::operator + (const float3& _v) const noexcept
	{
		float3 out;

		XMStoreFloat3(&out, XMVectorAdd(XMLoadFloat3(this), XMLoadFloat3(&_v)));

		return out;
	}

	__forceinline float3
		float3::operator - (const float3& _v) const noexcept
	{
		float3 out;

		XMStoreFloat3(&out, XMVectorSubtract(XMLoadFloat3(this), XMLoadFloat3(&_v)));

		return out;
	}

	__forceinline float3
		float3::operator * (const float3& _v) const noexcept
	{
		float3 out;

		XMStoreFloat3(&out, XMVectorMultiply(XMLoadFloat3(this), XMLoadFloat3(&_v)));

		return out;
	}

	__forceinline float3
		float3::operator / (const float3& _v) const noexcept
	{
		float3 out;

		XMStoreFloat3(&out, XMVectorDivide(XMLoadFloat3(this), XMLoadFloat3(&_v)));

		return out;
	}

	__forceinline float3
		float3::operator * (float _f) const noexcept
	{
		float3 out;

		XMStoreFloat3(&out, XMVectorScale(XMLoadFloat3(this), _f));

		return out;
	}

	__forceinline float3
		float3::operator / (float _f) const noexcept
	{
		float3 out;

		XMStoreFloat3(&out, XMVectorDivide(XMLoadFloat3(this), XMVectorReplicate(_f)));

		return out;
	}

	__forceinline _bool
		float3::operator == (const XMVECTOR& _v) const noexcept
	{
		return XMVector3Equal(XMLoadFloat3(this), _v);
	}

	__forceinline _bool
		float3::operator != (const XMVECTOR& _v) const noexcept
	{
		return !XMVector3Equal(XMLoadFloat3(this), _v);
	}

	__forceinline _bool
		float3::operator == (const float3& _v) const noexcept
	{
		return XMVector3Equal(XMLoadFloat3(this), XMLoadFloat3(&_v));
	}

	__forceinline _bool
		float3::operator != (const float3& _v) const noexcept
	{
		return !XMVector3Equal(XMLoadFloat3(this), XMLoadFloat3(&_v));
	}

#pragma endregion

	__forceinline
		float3::operator XMVECTOR() const noexcept
	{
		return XMLoadFloat3(this);
	}

#pragma endregion

#pragma region _float4

	__forceinline _float
		float4::length() const noexcept
	{
		return XMVectorGetX(XMVector4Length(*this));
	}

	__forceinline float4
		float4::normalize() const noexcept
	{
		return XMVector4Normalize(*this);
	}

#pragma region Assignment Operators

	__forceinline float4&
		float4::operator = (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat4(this, _v);

		return *this;
	}

	__forceinline float4&
		float4::operator += (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat4(this, XMVectorAdd(XMLoadFloat4(this), _v));

		return *this;
	}

	__forceinline float4&
		float4::operator -= (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat4(this, XMVectorSubtract(XMLoadFloat4(this), _v));

		return *this;
	}

	__forceinline float4&
		float4::operator *= (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat4(this, XMVectorMultiply(XMLoadFloat4(this), _v));

		return *this;
	}

	__forceinline float4&
		float4::operator /= (const XMVECTOR& _v) noexcept
	{
		XMStoreFloat4(this, XMVectorDivide(XMLoadFloat4(this), _v));

		return *this;
	}

	__forceinline float4&
		float4::operator += (const float4& _v) noexcept
	{
		XMStoreFloat4(this, XMVectorAdd(XMLoadFloat4(this), XMLoadFloat4(&_v)));

		return *this;
	}

	__forceinline float4&
		float4::operator -= (const float4& _v) noexcept
	{
		XMStoreFloat4(this, XMVectorSubtract(XMLoadFloat4(this), XMLoadFloat4(&_v)));

		return *this;
	}

	__forceinline float4&
		float4::operator *= (const float4& _v) noexcept
	{
		XMStoreFloat4(this, XMVectorMultiply(XMLoadFloat4(this), XMLoadFloat4(&_v)));

		return *this;
	}

	__forceinline float4&
		float4::operator /= (const float4& _v) noexcept
	{
		XMStoreFloat4(this, XMVectorDivide(XMLoadFloat4(this), XMLoadFloat4(&_v)));

		return *this;
	}

	__forceinline float4&
		float4::operator *= (float _f) noexcept
	{
		XMStoreFloat4(this, XMVectorScale(XMLoadFloat4(this), _f));

		return *this;
	}

	__forceinline float4&
		float4::operator /= (float _f) noexcept
	{
		XMStoreFloat4(this, XMVectorDivide(XMLoadFloat4(this), XMVectorReplicate(_f)));

		return *this;
	}

#pragma endregion
#pragma region Unary Operators

	__forceinline float4
		float4::operator + () const noexcept
	{
		return *this;
	}

	__forceinline float4
		float4::operator - () const noexcept
	{
		float4 out;

		XMStoreFloat4(&out, XMVectorScale(XMLoadFloat4(this), -1.f));

		return out;
	}

#pragma endregion
#pragma region Binary Operators

	__forceinline float4
		float4::operator + (const XMVECTOR& _v) const noexcept
	{
		float4 out;

		XMStoreFloat4(&out, XMVectorAdd(XMLoadFloat4(this), _v));

		return out;
	}

	__forceinline float4
		float4::operator - (const XMVECTOR& _v) const noexcept
	{
		float4 out;

		XMStoreFloat4(&out, XMVectorSubtract(XMLoadFloat4(this), _v));

		return out;
	}

	__forceinline float4
		float4::operator * (const XMVECTOR& _v) const noexcept
	{
		float4 out;

		XMStoreFloat4(&out, XMVectorMultiply(XMLoadFloat4(this), _v));

		return out;
	}

	__forceinline float4
		float4::operator / (const XMVECTOR& _v) const noexcept
	{
		float4 out;

		XMStoreFloat4(&out, XMVectorDivide(XMLoadFloat4(this), _v));

		return out;
	}

	__forceinline float4
		float4::operator + (const float4& _v) const noexcept
	{
		float4 out;

		XMStoreFloat4(&out, XMVectorAdd(XMLoadFloat4(this), XMLoadFloat4(&_v)));

		return out;
	}

	__forceinline float4
		float4::operator - (const float4& _v) const noexcept
	{
		float4 out;

		XMStoreFloat4(&out, XMVectorSubtract(XMLoadFloat4(this), XMLoadFloat4(&_v)));

		return out;
	}

	__forceinline float4
		float4::operator * (const float4& _v) const noexcept
	{
		float4 out;

		XMStoreFloat4(&out, XMVectorMultiply(XMLoadFloat4(this), XMLoadFloat4(&_v)));

		return out;
	}

	__forceinline float4
		float4::operator / (const float4& _v) const noexcept
	{
		float4 out;

		XMStoreFloat4(&out, XMVectorDivide(XMLoadFloat4(this), XMLoadFloat4(&_v)));

		return out;
	}

	__forceinline float4
		float4::operator * (float _f) const noexcept
	{
		float4 out;

		XMStoreFloat4(&out, XMVectorScale(XMLoadFloat4(this), _f));

		return out;
	}

	__forceinline float4
		float4::operator / (float _f) const noexcept
	{
		float4 out;

		XMStoreFloat4(&out, XMVectorDivide(XMLoadFloat4(this), XMVectorReplicate(_f)));

		return out;
	}

	__forceinline _bool
		float4::operator == (const XMVECTOR& _v) const noexcept
	{
		return XMVector4Equal(XMLoadFloat4(this), _v);
	}

	__forceinline _bool
		float4::operator != (const XMVECTOR& _v) const noexcept
	{
		return !XMVector4Equal(XMLoadFloat4(this), _v);
	}

	__forceinline _bool
		float4::operator == (const float4& _v) const noexcept
	{
		return XMVector4Equal(XMLoadFloat4(this), XMLoadFloat4(&_v));
	}

	__forceinline _bool
		float4::operator != (const float4& _v) const noexcept
	{
		return !XMVector4Equal(XMLoadFloat4(this), XMLoadFloat4(&_v));
	}

#pragma endregion

	__forceinline
		float4::operator XMVECTOR() const noexcept
	{
		return XMLoadFloat4(this);
	}

#pragma endregion

#pragma region _float4x4

	__forceinline float4x4
		float4x4::inverse() const noexcept
	{
		return XMMatrixInverse(nullptr, *this);
	}

	__forceinline float4
		float4x4::row(_uint _index) const noexcept
	{
		float4 out;
		XMStoreFloat4(&out, XMLoadFloat4x4(this).r[_index]);

		return out;
	}

#pragma region Assignment Operators

	__forceinline float4x4&
		float4x4::operator = (const XMMATRIX& _m) noexcept
	{
		XMStoreFloat4x4(this, _m);

		return *this;
	}

	__forceinline float4x4&
		float4x4::operator += (const XMMATRIX& _m) noexcept
	{
		XMStoreFloat4x4(this, XMLoadFloat4x4(this) + _m);

		return *this;
	}

	__forceinline float4x4&
		float4x4::operator -= (const XMMATRIX& _m) noexcept
	{
		XMStoreFloat4x4(this, XMLoadFloat4x4(this) - _m);

		return *this;
	}

	__forceinline float4x4&
		float4x4::operator *= (const XMMATRIX& _m) noexcept
	{
		XMStoreFloat4x4(this, XMLoadFloat4x4(this) * _m);

		return *this;
	}

	__forceinline float4x4&
		float4x4::operator += (const float4x4& _m) noexcept
	{
		XMStoreFloat4x4(this, XMLoadFloat4x4(this) + XMLoadFloat4x4(&_m));

		return *this;
	}

	__forceinline float4x4&
		float4x4::operator -= (const float4x4& _m) noexcept
	{
		XMStoreFloat4x4(this, XMLoadFloat4x4(this) - XMLoadFloat4x4(&_m));

		return *this;
	}

	__forceinline float4x4&
		float4x4::operator *= (const float4x4& _m) noexcept
	{
		XMStoreFloat4x4(this, XMLoadFloat4x4(this) * XMLoadFloat4x4(&_m));

		return *this;
	}

	__forceinline float4x4&
		float4x4::operator *= (float _f) noexcept
	{
		XMStoreFloat4x4(this, XMLoadFloat4x4(this) * _f);

		return *this;
	}

	__forceinline float4x4&
		float4x4::operator /= (float _f) noexcept
	{
		assert(_f != 0.f);

		XMStoreFloat4x4(this, XMLoadFloat4x4(this) / _f);

		return *this;
	}

#pragma endregion
#pragma region Unary Operators

	__forceinline float4x4
		float4x4::operator + () const noexcept
	{
		return *this;
	}

	__forceinline float4x4
		float4x4::operator - () const noexcept
	{
		float4x4 out;

		XMStoreFloat4x4(&out, XMLoadFloat4x4(this) * -1.f);

		return out;
	}

#pragma endregion
#pragma region Binary Operators

	__forceinline float4x4
		float4x4::operator + (const XMMATRIX& _m) const noexcept
	{
		float4x4 out;

		XMStoreFloat4x4(&out, XMLoadFloat4x4(this) + _m);

		return out;
	}

	__forceinline float4x4
		float4x4::operator - (const XMMATRIX& _m) const noexcept
	{
		float4x4 out;

		XMStoreFloat4x4(&out, XMLoadFloat4x4(this) - _m);

		return out;
	}

	__forceinline float4x4
		float4x4::operator * (const XMMATRIX& _m) const noexcept
	{
		float4x4 out;

		XMStoreFloat4x4(&out, XMLoadFloat4x4(this) * _m);

		return out;
	}

	__forceinline float4x4
		float4x4::operator + (const float4x4& _m) const noexcept
	{
		float4x4 out;

		XMStoreFloat4x4(&out, XMLoadFloat4x4(this) + XMLoadFloat4x4(&_m));

		return out;
	}

	__forceinline float4x4
		float4x4::operator - (const float4x4& _m) const noexcept
	{
		float4x4 out;

		XMStoreFloat4x4(&out, XMLoadFloat4x4(this) - XMLoadFloat4x4(&_m));

		return out;
	}

	__forceinline float4x4
		float4x4::operator * (const float4x4& _m) const noexcept
	{
		float4x4 out;

		XMStoreFloat4x4(&out, XMLoadFloat4x4(this) * XMLoadFloat4x4(&_m));

		return out;
	}

	__forceinline float4x4
		float4x4::operator * (float _f) const noexcept
	{
		float4x4 out;

		XMStoreFloat4x4(&out, XMLoadFloat4x4(this) * _f);

		return out;
	}

	__forceinline float4x4
		float4x4::operator / (float _f) const noexcept
	{
		assert(_f != 0.f);

		float4x4 out;

		XMStoreFloat4x4(&out, XMLoadFloat4x4(this) / _f);

		return out;
	}

	__forceinline _bool
		float4x4::operator == (const XMMATRIX& _m) const noexcept
	{
		_float4x4 m;

		XMStoreFloat4x4(&m, _m);

		return !memcmp(this, &m, sizeof(float4x4));
	}

	__forceinline _bool
		float4x4::operator != (const XMMATRIX& _m) const noexcept
	{
		_float4x4 m;

		XMStoreFloat4x4(&m, _m);

		return memcmp(this, &m, sizeof(float4x4));
	}

	__forceinline _bool
		float4x4::operator == (const float4x4& _m) const noexcept
	{
		return !memcmp(this, &_m, sizeof(float4x4));
	}

	__forceinline _bool
		float4x4::operator != (const float4x4& _m) const noexcept
	{
		return memcmp(this, &_m, sizeof(float4x4));
	}

#pragma endregion

	__forceinline
		float4x4::operator XMMATRIX() const noexcept
	{
		return XMLoadFloat4x4(this);
	}

#pragma endregion


}

#endif // Engine_Typedef_h__
