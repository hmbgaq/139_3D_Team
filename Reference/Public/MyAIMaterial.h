#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CMyAIMaterial
{
public:
	CMyAIMaterial() {};
	CMyAIMaterial(aiMaterial* _pMaterial) { 
		//m_pMaterial = _pMaterial; 
		//m_bIsBianry = false; 

		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
		{
			aiString AIStr;

			_pMaterial->GetTexture(aiTextureType(i), 0, &AIStr);

			mTextures[i] = AIStr.C_Str();
		}
	};

	CMyAIMaterial(ifstream& is) {
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
		{
			size_t istringSize;
			read_typed_data(is, istringSize);
			mTextures[i].resize(istringSize);
			is.read(&mTextures[i][0], istringSize);
		}
	};

private:
	//_bool			m_bIsBianry = { false };
	//aiMaterial*		m_pMaterial = { nullptr };

	string          mTextures[AI_TEXTURE_TYPE_MAX];
	//HRESULT			hGetTextureResult;

public:
	void Bake_Binary(ofstream& os)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
		{
			write_typed_data(os, mTextures[i].size());
			os.write(&mTextures[i][0], mTextures[i].size());
		}
	}

	string			Get_Textures(_uint _iIndex) 
	{
		return mTextures[_iIndex];
	}
	//HRESULT GetTexture(aiTextureType type, _uint  index, aiString* path) {
	//	return (HRESULT)m_pMaterial->GetTexture(type, index, path); 
	//	//return (HRESULT)(mMaterials[type] != "");
	//}



	
};

END