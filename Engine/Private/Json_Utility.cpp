#include "Json_Utility.h"

//#define	JSONFOLDER_PATH	L"../Json/"

HRESULT CJson_Utility::Load_Json(const char* _strPath, json& pOut)
{
	json JsonTemp;

	ifstream fin;

	fin.open(_strPath);

	if (fin.is_open())
	{
		fin >> (JsonTemp);
	}
	else
	{
		return E_FAIL;
	}

	fin.close();
	pOut = JsonTemp;

	return S_OK;
}

HRESULT CJson_Utility::Save_Json(const char* _strPath, json _json)
{
	ofstream fout;

	fout.open(_strPath);

	if (fout.is_open())
	{
		fout << _json << endl;
	}
	else
		return E_FAIL;

	fout.close();

	return S_OK;
}

void CJson_Utility::Write_Float2(json& Out_Json, const _float2& In_Float2)
{
	if (Out_Json.empty())
	{
		Out_Json.emplace();
	}

	_vector vInValue = XMLoadFloat2(&In_Float2);

	Out_Json = vInValue.m128_f32;
}

void CJson_Utility::Write_Float3(json& Out_Json, const _float3& In_Float3)
{
	if (Out_Json.empty())
	{
		Out_Json.emplace();
	}

	_vector vInValue = XMLoadFloat3(&In_Float3);

	Out_Json = vInValue.m128_f32;

}

void CJson_Utility::Write_Float4(json& Out_Json, const _float4& In_Float4)
{
	if (Out_Json.empty())
	{
		Out_Json.emplace();
	}

	_vector vInValue = XMLoadFloat4(&In_Float4);

	Out_Json = vInValue.m128_f32;
}

void CJson_Utility::Load_Float2(const json& In_Json, _float2& Out_Float2)
{
	if (In_Json.empty())
		return;

	Out_Float2.x = In_Json[0];
	Out_Float2.y = In_Json[1];

}

void CJson_Utility::Load_Float3(const json& In_Json, _float3& Out_Float3)
{
	if (In_Json.empty())
		return;

	Out_Float3.x = In_Json[0];
	Out_Float3.y = In_Json[1];
	Out_Float3.z = In_Json[2];
}

void CJson_Utility::Load_Float4(const json& In_Json, _float4& Out_Float4)
{
	if (In_Json.empty())
		return;

	Out_Float4.x = In_Json[0];
	Out_Float4.y = In_Json[1];
	Out_Float4.z = In_Json[2];
	Out_Float4.w = In_Json[3];
}

void CJson_Utility::Load_JsonFloat4x4(const json& _Json, _float4x4& Out_Float4x4)
{
	ZeroMemory(&Out_Float4x4, sizeof(_float4x4));

	for (_uint i = 0; i < 4; ++i)
	{
		for (_uint s = 0; s < 4; ++s)
		{
			Out_Float4x4.m[i][s] = _Json[i][s];
		}
	}
}

wstring CJson_Utility::String_To_WString(const json& _Json)
{
	wstring Result = {};

	if (_Json.is_string()) {
		string strConvert = _Json.get<string>();
		Result = wstring(strConvert.begin(), strConvert.end());
	}

	return Result;
}



XMFLOAT4 CJson_Utility::Get_VectorFromJson(json& _json)
{
	if (_json.empty())
		return XMFLOAT4(0.f, 0.f, 0.f, 1.f);

	return XMFLOAT4(_json[0], _json[1], _json[2], _json[3]);
}

XMFLOAT4X4 CJson_Utility::Get_MatrixFromJson(json& _json)
{
	XMFLOAT4X4 matResult;

	if (_json.empty())
	{
		XMMATRIX	matTemp = XMMatrixIdentity();
		XMStoreFloat4x4(&matResult, matTemp);
	}
	else
	{
		for (int i = 0; i < 4; ++i)
			(*((XMFLOAT4*)&matResult.m[i])) = Get_VectorFromJson(_json[i]);
	}

	return matResult;
}/*

string CJson_Utility::Complete_Path(const char* _strFileName)
{
	string strFullPath = JSONFOLDER_PATH;
	strFullPath += _strFileName;
	strFullPath += L".json";

	return strFullPath;
}*/
