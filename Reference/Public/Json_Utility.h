#pragma once
#include "Engine_Defines.h"
#include "Json/json.hpp"
using namespace nlohmann;

BEGIN(Engine)

class ENGINE_DLL CJson_Utility
{
public:
	static HRESULT		Load_Json(const char* _strPath, json& pOut);
	static HRESULT		Save_Json(const char* _strPath, json _json);



public:
	static void			Write_Float2(json& Out_Json, const _float2& In_Float2);
	static void			Write_Float3(json& Out_Json, const _float3& In_Float3);
	static void			Write_Float4(json& Out_Json, const _float4& In_Float4);
	static void			Load_Float2(const json& In_Json, _float2& Out_Float2);
	static void			Load_Float3(const json& In_Json, _float3& Out_Float3);
	static void			Load_Float4(const json& In_Json, _float4& Out_Float4);
	static void			Load_JsonFloat4x4(const json& _Json, _float4x4& Out_Float4x4);
	
	static wstring		String_To_WString(const json& _Json);

	static XMFLOAT4		Get_VectorFromJson(json& _json);
	static XMFLOAT4X4	Get_MatrixFromJson(json& _json);

public:
	//static wstring Complete_Path(const char* _strFileName);


};

END