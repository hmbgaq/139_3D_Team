#pragma once
#include "Base.h"
#include "MyAIScene.h"
//#include "./assimp/importerdesc.h"
BEGIN(Engine)


class ENGINE_DLL CMyAssimp
{
private:
	_bool					m_bIsBinary = { false };
	Assimp::Importer		m_Importer;

public:
	CMyAIScene ReadFile(const string& strModelFilePath, _uint iFlag, _bool _bIsBinary)
	{
		//m_Importer.SetPropertyBool(AI_CONFIG_IMPORT_REMOVE_EMPTY_BONES, false);
		//m_Importer.SetPropertyBool(AI_CONFIG_PP_SBBC_MAX_BONES, true);
		//m_Importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, UINT16_MAX);
		//
		//aiImporterDesc Desc = *m_Importer.GetImporterInfo(0);

		string filePath = strModelFilePath;
		string fileType;

		if (false == _bIsBinary)
		{
			fileType = ".fbx";
			filePath += fileType;

			const aiScene* pAIScene = m_Importer.ReadFile(filePath, iFlag);
			CMyAIScene result = CMyAIScene(pAIScene);
			result.Bake_Binary(strModelFilePath);

			

			return result;
		}
		else
		{
			fileType = ".bin";
			filePath += fileType;

			return CMyAIScene(filePath);
		}
	};

	CMyAIScene ReadFile(const string& strModelFilePath, _uint iFlag) 
	{
		string filePath = strModelFilePath + ".bin";

		ifstream is(filePath, std::ios::binary);

		_bool IsBinFileOpen = is.is_open();
		is.close();

		return ReadFile(strModelFilePath, iFlag, IsBinFileOpen);

	};
	void FreeScene()
	{
		m_Importer.FreeScene();
	};



};

END