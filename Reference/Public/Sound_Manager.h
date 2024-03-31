#pragma once

#include "Base.h"
#include "Engine_Defines.h"

#include <mutex>
#include <future>
#include <thread>
#include <io.h>

BEGIN(Engine)

class ENGINE_DLL FSoundData : public CBase
{
private:
	FSoundData() {}
	~FSoundData() {}

public:
	static FSoundData* Create()
	{
		FSoundData* pInstance = new FSoundData();

		return pInstance;
	}
	void Free()
	{
		mapSound.clear();

		delete this;
	}

public:
	using map_sound = unordered_map<wstring, FMOD_SOUND*>;
	map_sound& Get_MapSound() { return mapSound; }

private:
	map_sound mapSound;
};

class ENGINE_DLL CSound_Manager : public CBase
{
private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Ready_Sound();

public:
	// 사운드 재생
	void Play_Sound(const wstring& strGroupKey, const wstring& strSoundKey, CHANNELID eID, _float fVolume = 1.f);
	// 브금 재생
	void Play_BGM(const wstring& strGroupKey, const wstring& strSoundKey, _float fVolume = 1.f);
	// 사운드 정지
	void Stop_Sound(CHANNELID eID);
	// 모든 사운드 정지
	void Stop_All();
	// 채널의 볼륨 설정
	void Set_ChannelVolume(CHANNELID eID, float fVolume);

private:
	// 내부적으로 로드할 사운드 파일 폴더를 지정해 로드하는 함수
	void Load_SoundFile(const wstring& pGroupKey, const string& pPath);
	// 사운드 파일을 그룹 단위로 비동기 로드할 때 쓰이는 함수
	void Load_SoundFile_GroupAsync(const wstring& pGroupKey, const string& pPath);
	// 비동기 처리를 기다리는 함수
	void Wait_GroupAsync();
	// 사운드 파일 단일로 비동기 로드할 때 쓰이는 함수ㅈ
	FMOD_RESULT LoadSoundFile_Async(const string& pPath, const string& pFileName, FMOD_RESULT& hResult, FMOD_SOUND** pSound);

	// 사운드 리소스 정보를 갖는 객체 
	unordered_map<wstring, FSoundData*>	m_mapSound;
	mutex							m_mtxSound;
	vector<future<void>>			m_vecAsyncSoundGroup;

	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM* m_pSystem = nullptr;

	FMOD_CHANNELGROUP* m_pChannelGroup[MAX_CHANNEL_GROUP];
	FMOD_CHANNELGROUP* m_pMasterChanelGroup;

public:
	static CSound_Manager* Create();
	virtual void Free() override;
};

END

/*  Engine::Play_Sound(L"Enemy", L"_PipeHeavySwing.mp3", SOUND_ENEMY_NORMAL_EFFECT, m_tSound.m_fSoundVolume);
전에 이렇게 쓰고있었음 */