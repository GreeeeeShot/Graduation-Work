#include "stdafx.h"
#include "Scene.h"
#include "Player.h"
#include "TextureResource.h"
#include "ShaderResource.h"
#include "EffectManager.h"
#include "IOCP_Client.h"

CScene::CScene(char* cSceneTextFileName)
{
	/*m_ppObjects = NULL;*/
	m_nObjects = 0;

	m_pPlayersMgrInform = NULL;
	m_cSceneTextFileName = cSceneTextFileName;

	m_pResultUI = NULL;
	m_pSkydom = NULL;

	m_pLights = NULL;
	m_pd3dcbLights = NULL;

	m_pVoxelTerrain = NULL;
	m_pWaveEffect = NULL;
	m_fGravitationalAcceleration = GRAVITATIONAL_ACCELERATION;

	m_fAccumulatedTime = 0.0f;
}

CScene::~CScene()
{
}

void CScene::BuildObjects(ID3D11Device *pd3dDevice)
{
	CEffectManager::InitEffectManager();

	m_pResultUI = new CSprite();
	m_pResultUI->SetSpriteTexture(CTextureResource::pUI_Result);
	m_pResultUI->SetShader(CShaderResource::pUITexturedShader);
	m_pResultUI->BuildSpritePieces(pd3dDevice, 600.0f, 280.0f, 3);
	m_pResultUI->SetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	m_pSkydom = new CGameObject();
	m_pSkydom->SetMaterial(CMaterialResource::pStandardMaterial);
	m_pSkydom->SetTexture(CTextureResource::pSkydomTexture);
	m_pSkydom->SetMesh(CMeshResource::pSkyMesh);
	m_pSkydom->SetShader(CShaderResource::pTexturedNotLightingShader);
	m_pSkydom->SetPosition(0.f, 0.f, 0.f);

	m_pWaveEffect = new CWaveObject();
	m_pWaveEffect->SetMaterial(CMaterialResource::pWaveMaterial);
	m_pWaveEffect->SetTexture(CTextureResource::pWaveTexture);
	m_pWaveEffect->SetNormalMap(CTextureResource::pWaveNormalMap);
	m_pWaveEffect->SetMesh(CMeshResource::pWaveMesh);
	m_pWaveEffect->SetShader(CShaderResource::pTexturedNormalMappingShader);

	for (int i = 0; i < 2; i++)
	{
		m_pShips[i] = new CGameObject();
		m_pShips[i]->SetMaterial(CMaterialResource::pStandardMaterial);
		m_pShips[i]->SetTexture(CTextureResource::pShipTexture);
		m_pShips[i]->SetMesh(CMeshResource::pShipMesh);
		m_pShips[i]->SetShader(CShaderResource::pTexturedLightingShader);
	}
	m_pShips[0]->SetPosition(-42.0f, 0.5f,0.0f);
	m_pShips[1]->SetPosition(42.0f, 0.5f, 0.0f);

	m_cTimeManager.SetTimeLimitSec(GAME_TIME_LIMMIT);

	m_pTreasureChest = new CTreasureChest();

	m_pVoxelTerrain = new CVoxelTerrain();
	CBlueprint* pTestMapBP = new CBlueprint();
	m_pVoxelTerrain->CreateVoxelTerrain(pd3dDevice, pTestMapBP->LoadBlueprint(m_cSceneTextFileName));
	delete pTestMapBP;

	CreateShaderVariables(pd3dDevice);							// 조명을 입력받는 상수 버퍼를 생성함과 동시에 조명 정보를 집어 넣는다.
	InitScene(pd3dDevice);
}

void CScene::DestroyScene(void)
{
	CEffectManager::InitEffectManager();

	if (m_pSkydom) delete m_pSkydom;
	if (m_pWaveEffect) delete m_pWaveEffect;

	for (int i = 0; i < 2; i++)
	{
		if (m_pShips[i]) delete m_pShips[i];
	}
	if (m_pTreasureChest) delete m_pTreasureChest;
	if (m_pVoxelTerrain) delete m_pVoxelTerrain;
}

void CScene::InitScene(ID3D11Device *pd3dDevice)
{
	CEffectManager::InitEffectManager();
	m_RespawnManager.InitRespawnManager();
	m_cTimeManager.SetTimeLimitSec(GAME_TIME_LIMMIT);
	m_pTreasureChest->SetPosition(0.0f, 10.f, 0.0f);
	for (int i = 0; i < m_pPlayersMgrInform->m_iPlayersNum; i++)
	{
		if(m_pPlayersMgrInform->m_ppPlayers[i]) m_RespawnManager.RegisterRespawnManager(m_pPlayersMgrInform->m_ppPlayers[i], false);
	}
	//m_pPlayersMgrInform->m_ppPlayers[m_pPlayersMgrInform->m_iMyPlayerID]->InitPlayer(D3DXVECTOR3(-7.0f, 40.0f, 7.0f));

	if (m_pVoxelTerrain)
	{
		delete m_pVoxelTerrain;
		m_pVoxelTerrain = NULL;
	}
	
	m_pVoxelTerrain = new CVoxelTerrain();
	CBlueprint* pTestMapBP = new CBlueprint();
	m_pVoxelTerrain->CreateVoxelTerrain(pd3dDevice, pTestMapBP->LoadBlueprint(m_cSceneTextFileName));
	delete pTestMapBP;
	m_fAccumulatedTime = 0.0f;
}

void CScene::ReleaseObjects()
{
	ReleaseShaderVariables();
	DestroyScene();
}

void CScene::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));
	m_pLights->m_d3dxcGlobalAmbient = D3DXCOLOR(0.05f, 0.01f, 0.01f, 1.0f);

	//3개의 조명(점 광원, 스팟 광원, 방향성 광원)을 설정한다.
	m_pLights->m_pLights[0].m_bEnable = 1.0f;
	m_pLights->m_pLights[0].m_nType = POINT_LIGHT;				
	m_pLights->m_pLights[0].m_fRange = 600.0f;
	m_pLights->m_pLights[0].m_d3dxcAmbient = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights->m_pLights[0].m_d3dxcDiffuse = D3DXCOLOR(0.8f, 0.3f, 0.2f, 1.0f);
	m_pLights->m_pLights[0].m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
	m_pLights->m_pLights[0].m_d3dxvPosition = D3DXVECTOR3(0.0f, 100.0f, -160.0f);
	m_pLights->m_pLights[0].m_d3dxvDirection = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[0].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.001f, 0.0001f);
	m_pLights->m_pLights[1].m_bEnable = 1.0f;
	m_pLights->m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[1].m_fRange = 450.0f;
	m_pLights->m_pLights[1].m_d3dxcAmbient = D3DXCOLOR(0.5f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[1].m_d3dxcDiffuse = D3DXCOLOR(0.8f, 0.6f, 0.3f, 1.0f);
	m_pLights->m_pLights[1].m_d3dxcSpecular = D3DXCOLOR(0.95f, 0.8f, 0.6f, 16.0f);
	m_pLights->m_pLights[1].m_d3dxvPosition = D3DXVECTOR3(-0.0f, 45.0f, -250.0f);
	m_pLights->m_pLights[1].m_d3dxvDirection = D3DXVECTOR3(0.0f, -1.0f, 4.6f);
	m_pLights->m_pLights[1].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.001f, 0.0000f);
	m_pLights->m_pLights[1].m_fFalloff = 18.0f;
	m_pLights->m_pLights[1].m_fPhi = (float)cos(D3DXToRadian(50.0f));
	m_pLights->m_pLights[1].m_fTheta = (float)cos(D3DXToRadian(20.0f));
	m_pLights->m_pLights[2].m_bEnable = 1.0f;
	m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[2].m_d3dxcAmbient = D3DXCOLOR(0.16f, 0.16f, 0.13f, 1.0f);
	m_pLights->m_pLights[2].m_d3dxcDiffuse = D3DXCOLOR(0.6f, 0.3f, 0.1f, 1.0f);
	m_pLights->m_pLights[2].m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 0.6f, 0.0f);
	m_pLights->m_pLights[2].m_d3dxvDirection = D3DXVECTOR3(-0.0f, -1.0f, 0.8f);
	m_pLights->m_pLights[3].m_bEnable = 1.0f;
	m_pLights->m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[3].m_fRange = 60.0f;
	m_pLights->m_pLights[3].m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[3].m_d3dxcDiffuse = D3DXCOLOR(0.5f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[3].m_d3dxcSpecular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[3].m_d3dxvPosition = D3DXVECTOR3(500.0f, 300.0f, 500.0f);
	m_pLights->m_pLights[3].m_d3dxvDirection = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	m_pLights->m_pLights[3].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[3].m_fFalloff = 20.0f;
	m_pLights->m_pLights[3].m_fPhi = (float)cos(D3DXToRadian(40.0f));
	m_pLights->m_pLights[3].m_fTheta = (float)cos(D3DXToRadian(15.0f));

	m_pLights->m_pLights[4].m_bEnable = 1.0f;
	m_pLights->m_pLights[4].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[4].m_d3dxcAmbient = D3DXCOLOR(0.16f, 0.16f, 0.13f, 1.0f);
	m_pLights->m_pLights[4].m_d3dxcDiffuse = D3DXCOLOR(0.6f, 0.3f, 0.1f, 1.0f);
	m_pLights->m_pLights[4].m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 0.6f, 0.0f);
	m_pLights->m_pLights[4].m_d3dxvDirection = D3DXVECTOR3(0.0f, -1.0f, -0.8f);

	m_pLights->m_pLights[5].m_bEnable = 1.0f;
	m_pLights->m_pLights[5].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[5].m_fRange = 0.8f;
	m_pLights->m_pLights[5].m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.7f, 0.7f, 1.0f);
	m_pLights->m_pLights[5].m_d3dxcDiffuse = D3DXCOLOR(0.0f, 0.5f, 0.6f, 1.0f);
	m_pLights->m_pLights[5].m_d3dxcSpecular = D3DXCOLOR(1.0f, 0.1f, 1.0f, 0.0f);
	m_pLights->m_pLights[5].m_d3dxvPosition = D3DXVECTOR3(-43.0f, 3.0f, 0.0f);
	m_pLights->m_pLights[5].m_d3dxvDirection = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[5].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

	m_pLights->m_pLights[6].m_bEnable = 1.0f;
	m_pLights->m_pLights[6].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[6].m_d3dxcAmbient = D3DXCOLOR(0.16f, 0.16f, 0.13f, 1.0f);
	m_pLights->m_pLights[6].m_d3dxcDiffuse = D3DXCOLOR(0.6f, 0.3f, 0.1f, 1.0f);
	m_pLights->m_pLights[6].m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 0.6f, 0.0f);
	m_pLights->m_pLights[6].m_d3dxvDirection = D3DXVECTOR3(0.8f, -1.0f, -0.0f);

	/*m_pLights->m_pLights[6].m_bEnable = 1.0f;
	m_pLights->m_pLights[6].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[6].m_fRange = 200.0f;
	m_pLights->m_pLights[6].m_d3dxcAmbient = D3DXCOLOR(0.01f, 0.01f, 0.1f, 1.0f);
	m_pLights->m_pLights[6].m_d3dxcDiffuse = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[6].m_d3dxcSpecular = D3DXCOLOR(0.7f, 0.7f, 0.7f, 50.0f);
	m_pLights->m_pLights[6].m_d3dxvPosition = D3DXVECTOR3(70.f, 100.0f,0.);
	m_pLights->m_pLights[6].m_d3dxvDirection = D3DXVECTOR3(-0.f, -1.0f, 0.);
	m_pLights->m_pLights[6].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.01f, 0.0f);
	m_pLights->m_pLights[6].m_fFalloff = 20.0f;
	m_pLights->m_pLights[6].m_fPhi = (float)cos(D3DXToRadian(80.0f));
	m_pLights->m_pLights[6].m_fTheta = (float)cos(D3DXToRadian(40.0f));

	m_pLights->m_pLights[6].m_bEnable = 1.0f;
	m_pLights->m_pLights[6].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[6].m_d3dxcAmbient = D3DXCOLOR(0.01f, 0.01f, 0.01f, 1.0f);
	m_pLights->m_pLights[6].m_d3dxcDiffuse = D3DXCOLOR(0.3f, 0.6f, 0.60f, 1.0f);
	m_pLights->m_pLights[6].m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 8.0f);
	m_pLights->m_pLights[6].m_d3dxvDirection = D3DXVECTOR3(-0.0f, -1.0f, 1.2f);
*/
	m_pLights->m_pLights[7].m_bEnable = 1.0f;
	m_pLights->m_pLights[7].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[7].m_d3dxcAmbient = D3DXCOLOR(0.3f, 0.3f, 0.23f, 1.0f);
	m_pLights->m_pLights[7].m_d3dxcDiffuse = D3DXCOLOR(0.4f, 0.3f, 0.1f, 1.0f);
	m_pLights->m_pLights[7].m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f,0.6f, 0.0f);
	m_pLights->m_pLights[7].m_d3dxvDirection = D3DXVECTOR3(0.2f, -5.0f, -0.2f);

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(d3dBufferDesc));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(LIGHTS);
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pLights;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dcbLights);

	// 안개 설정을 한다.
	m_pFog = new FOG;
	::ZeroMemory(m_pFog, sizeof(FOG));
	m_pFog->m_fFogEnable = 0.0f;
	m_pFog->m_fFogStart = 30.0f;
	m_pFog->m_fFogRange = 120.0f;
	m_pFog->m_d3dxcFogColor = D3DXCOLOR(1.35f, 1.0f, 0.8f, 1.0f);

	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(FOG);
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pFog;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dcbFog);	
}

void CScene::ReleaseShaderVariables()
{
	if (m_pLights) delete m_pLights;
	if (m_pd3dcbLights) m_pd3dcbLights->Release();
	if (m_pFog) delete m_pFog;
	if (m_pd3dcbFog) m_pd3dcbFog->Release();
}

void CScene::UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, LIGHTS *pLights)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbLights, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	LIGHTS *pcbLight = (LIGHTS *)d3dMappedResource.pData;
	memcpy(pcbLight, pLights, sizeof(LIGHTS));
	pd3dDeviceContext->Unmap(m_pd3dcbLights, 0);

	pd3dDeviceContext->Map(m_pd3dcbFog, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	FOG *pcbFog = (FOG *)d3dMappedResource.pData;
	ZeroMemory(pcbFog, sizeof(FOG));
	pcbFog->m_fFogEnable = m_pFog->m_fFogEnable;
	pcbFog->m_fFogStart = m_pFog->m_fFogStart;
	pcbFog->m_fFogRange = m_pFog->m_fFogRange;
	pcbFog->m_d3dxcFogColor = m_pFog->m_d3dxcFogColor;
	pd3dDeviceContext->Unmap(m_pd3dcbFog, 0);

	pd3dDeviceContext->PSSetConstantBuffers(PS_SLOT_LIGHT, 1, &m_pd3dcbLights);
	pd3dDeviceContext->PSSetConstantBuffers(PS_SLOT_FOG, 1, &m_pd3dcbFog);
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	static bool liftbox = false;
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case 'a': case 'A':
			
			m_pPlayersMgrInform->GetMyPlayer()->m_bIsPushed = false;
			SetPacket(OUTBOX, 0, 0, 0);
			liftbox = false;
			if(!m_pPlayersMgrInform->GetMyPlayer()->m_bIsPushed) m_pTreasureChest->BeRelievedFromLiftingPlayer();
			break;
		case 'M': case 'm':
			printf("박스위치가? %f %f %f\n", m_pTreasureChest->GetPosition().x, m_pTreasureChest->GetPosition().y, m_pTreasureChest->GetPosition().z);
		default:
			break;
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'a': case 'A':
			if (!m_pPlayersMgrInform->GetMyPlayer()->m_bIsPushed)
			{
				CSoundManager::FMODSoundPlayAndRepeat(SOUND_TYPE_EFFECT_SWING);
				//m_pPlayersMgrInform->GetMyPlayer()->PushPlayer(m_pTreasureChest);
				m_pPlayersMgrInform->GetMyPlayer()->LiftPlayer(m_pTreasureChest, m_pVoxelTerrain);
				m_pPlayersMgrInform->GetMyPlayer()->PushPlayers(m_pPlayersMgrInform->m_ppPlayers, m_pPlayersMgrInform->m_iPlayersNum);
				m_pPlayersMgrInform->GetMyPlayer()->m_bIsPushed = true;
				
				if (!liftbox)
				{
					SetPacket(LIFTBOX, 0, 0, 0);
					liftbox = true;
				}
				switch (m_pPlayersMgrInform->GetMyPlayer()->m_PlayerType)
				{
					case PLAYER_TYPE_PIRATE: 
						m_pPlayersMgrInform->GetMyPlayer()->SetFBXAnimForType(PIRATE_ANIM_TYPE_ATTACK);
						break;
					case PLAYER_TYPE_COW_GIRL:
						m_pPlayersMgrInform->GetMyPlayer()->SetFBXAnimForType(PIRATE_ANIM_TYPE_ATTACK);
						break;
					default:
						break;
				}
			}
			break;
		case '1': case '2': case '3': case '4': case '5':
			if (!m_pPlayersMgrInform->GetMyPlayer()->m_bIsDigOrInstall)
			{
				CSoundManager::FMODSoundPlayAndRepeat(SOUND_TYPE_EFFECT_SELECT_VOXEL);
				m_pPlayersMgrInform->GetMyPlayer()->ChangeVoxelPocketSlotIdx(wParam - '0' - 1);
			}
			break;
		case '8':
			m_pFog->m_fFogEnable = m_pFog->m_fFogEnable ? 0.0f : 1.0f;
			break;
		}
	default:
		break;
	}
	return(false);
}

bool CScene::ProcessInput()
{
	return(false);
}

void CScene::MoveObjectUnderPhysicalEnvironment(CPlayer *pPlayer, float fTimeElapsed)
{
	static D3DXVECTOR3 d3dxvSearchDir[18] =
	{
		//D3DXVECTOR3(0.0f, 0.0f, 1.0f),               // 상
		//D3DXVECTOR3(0.0f, 0.0f, -1.0f),               // 하
		//D3DXVECTOR3(1.0f, 0.0f, 0.0f),               // 우
		//D3DXVECTOR3(-1.0f, 0.0f, 0.0f),               // 좌
		D3DXVECTOR3(0.0f, -1.0f, 1.0f),               // 상
		D3DXVECTOR3(1.0f, -1.0f, 0.0f),               // 우
		D3DXVECTOR3(0.0f, -1.0f, -1.0f),               // 하
		D3DXVECTOR3(-1.0f, -1.0f, 0.0f),               // 좌

		D3DXVECTOR3(0.0f, 0.0f, 1.0f),               // 상
		D3DXVECTOR3(1.0f, 0.0f, 0.0f),               // 우
		D3DXVECTOR3(0.0f, 0.0f, -1.0f),               // 하
		D3DXVECTOR3(-1.0f, 0.0f, 0.0f),               // 좌

		D3DXVECTOR3(0.0f, -1.0f, 0.0f),
		D3DXVECTOR3(1.0f, -1.0f, -1.0f),   D3DXVECTOR3(0.0f, -1.0f, -1.0f),   D3DXVECTOR3(-1.0f, -1.0f, -1.0f),
		D3DXVECTOR3(1.0f, -1.0f, 0.0f),                                 D3DXVECTOR3(-1.0f, -1.0f, 0.0f),
		D3DXVECTOR3(1.0f, -1.0f, 1.0f),      D3DXVECTOR3(0.0f, -1.0f, 1.0f),      D3DXVECTOR3(-1.0f, -1.0f, 1.0f),

		D3DXVECTOR3(0.0f, 1.0f, 0.0f),
	/*	D3DXVECTOR3(1.0f, 1.0f, -1.0f),      D3DXVECTOR3(0.0f, 1.0f, -1.0f),      D3DXVECTOR3(-1.0f, 1.0f, -1.0f),
		D3DXVECTOR3(1.0f, 1.0f, 0.0f),                                 D3DXVECTOR3(-1.0f, 1.0f, 0.0f),
		D3DXVECTOR3(1.0f, 1.0f, 1.0f),      D3DXVECTOR3(0.0f, 1.0f, 1.0f),      D3DXVECTOR3(-1.0f, 1.0f, 1.0f)*/
	};

	// 세계의 중력을 받는다.
	D3DXVECTOR3 d3dxvPostV = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR2 d3dxvVelocityXZ = D3DXVECTOR2(0.0f, 0.0f);
	D3DXVECTOR3 d3dxvG = D3DXVECTOR3(0.0f, GRAVITATIONAL_ACCELERATION, 0.0f);
	D3DXVECTOR3 d3dxvMovingDir = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//printf("Post Velocity : (%f, %f, %f) \n", d3dxvPostV.x, d3dxvPostV.y, d3dxvPostV.z);

	// 복셀 지형맵과의 충돌 체크를 한다.
	D3DXMATRIX d3dxmtxFromPosToIdx;
	D3DXVECTOR4 d3dxvColLayerRow;

	D3DXMatrixIdentity(&d3dxmtxFromPosToIdx);
	D3DXMatrixInverse(&d3dxmtxFromPosToIdx, NULL, &m_pVoxelTerrain->m_d3dxmtxFromIdxToPos);
	
	D3DXVECTOR3 d3dxvBottomCollisionPivot;
	D3DXVECTOR3 d3dxvMiddleCollisionPivot;
	const float fPivotOffset = 0.00001f;

	CVoxel *pCollidedVoxel = NULL;
	CVoxel *pFloorVoxel = NULL;
	D3DXVECTOR3 d3dxvInspectionIdx;
	bool bIsSlopeVoxel = false;
	D3DXVECTOR3 d3dxvSlopeRearNormal;
	bool bIsSwamp = false;
	bool bIsFloor = false;
	bool bIsJumped = false;
	bool bIsCollidedWithFloor = true;

	CVoxel *pLastColliedVoxel = NULL;

	/*printf("Player Pos (%f, %f, %f) \n",
		m_pPlayersMgrInform->GetMyPlayer()->GetPosition().x,
		m_pPlayersMgrInform->GetMyPlayer()->GetPosition().y,
		m_pPlayersMgrInform->GetMyPlayer()->GetPosition().z);*/
	while (1)
	{
		if (pPlayer->m_d3dxvVelocity.y > 0.0f)
		{
			break;
		}
		bIsCollidedWithFloor = true;
		/*printf("pPlayer->GetPosition() bottom1 (%f, %f, %f) \n",
		pPlayer->GetPosition().x, pPlayer->GetPosition().y + pPlayer->m_pMesh->m_AABB.m_d3dxvMin.y, pPlayer->GetPosition().z);*/
		D3DXVECTOR3 d3dxvRevision(0.0f, 0.0f, 0.0f);
		d3dxvBottomCollisionPivot = D3DXVECTOR3(pPlayer->GetPosition().x, pPlayer->GetPosition().y + pPlayer->m_pMesh->m_AABB.m_d3dxvMin.y + fPivotOffset, pPlayer->GetPosition().z);

		D3DXVec3Transform(&d3dxvColLayerRow, &d3dxvBottomCollisionPivot, &d3dxmtxFromPosToIdx);

		d3dxvColLayerRow.x = round(d3dxvColLayerRow.x);
		d3dxvColLayerRow.y = round(d3dxvColLayerRow.y);
		d3dxvColLayerRow.z = round(d3dxvColLayerRow.z);

		if ((0 <= d3dxvColLayerRow.x && d3dxvColLayerRow.x < m_pVoxelTerrain->m_iMaxCol)
			&& (0 <= d3dxvColLayerRow.y && d3dxvColLayerRow.y < m_pVoxelTerrain->m_iMaxLayer)
			&& (0 <= d3dxvColLayerRow.z && d3dxvColLayerRow.z < m_pVoxelTerrain->m_iMaxRow))
		{
			//pLastColliedVoxel = pCollidedVoxel;
			if (pCollidedVoxel = m_pVoxelTerrain->m_ppVoxelObjectsForCollision[
				(int)((d3dxvColLayerRow.y) * m_pVoxelTerrain->m_iLayerStride
					+ (d3dxvColLayerRow.z) * m_pVoxelTerrain->m_iRowStride
					+ (d3dxvColLayerRow.x))])
			{
			//	pCollidedVoxel->m_pMesh->m_AABB.m_pRenderObject->SetPosition(pCollidedVoxel->GetPosition());
			//	pCollidedVoxel->m_pMesh->m_AABB.m_pRenderObject->Render(CGameManager::GetInstance()->m_pGameFramework->m_pd3dDeviceContext);
				float fDistance = 0.0f;

				if (!pCollidedVoxel->m_bIsSlope)
				{
					d3dxvRevision.y = ((pCollidedVoxel->GetPosition().y + m_pVoxelTerrain->m_fCubeHeight / 2.0f)
						- (pPlayer->GetPosition().y + pPlayer->m_pMesh->m_AABB.m_d3dxvMin.y));
					pPlayer->Moving(d3dxvRevision);
					pPlayer->m_CameraOperator.MoveCameraOperator(d3dxvRevision);
					pLastColliedVoxel = pCollidedVoxel;
					bIsFloor = true;
				}
				else //pCollidedVoxel->m_bIsSlope)
				{
					fDistance = pCollidedVoxel->GetHeightByPlayerPos(pPlayer->GetPosition()) - (pPlayer->GetPosition().y + pPlayer->m_pMesh->m_AABB.m_d3dxvMin.y);
					if (fDistance >= 0.0f)
					{
						d3dxvRevision.y = fDistance;
						pPlayer->m_d3dxvVelocity.y = 0.0f;
						pPlayer->Moving(d3dxvRevision);
						pPlayer->m_CameraOperator.MoveCameraOperator(d3dxvRevision);
						bIsFloor = true;
					}
					pLastColliedVoxel = pCollidedVoxel;
					bIsCollidedWithFloor = false;
				}
			}
			else bIsCollidedWithFloor = false;
		}
		else bIsCollidedWithFloor = false;


		if (!bIsCollidedWithFloor)
		{
			if (pLastColliedVoxel)
			{
				if (!pLastColliedVoxel->m_bIsSlope)
				{
					if (-0.2f < pPlayer->m_d3dxvVelocity.y)
					{
						pPlayer->m_d3dxvVelocity.y = 0.0f;
					}
					else
					{
						pPlayer->m_d3dxvVelocity = pPlayer->m_d3dxvVelocity - 2.0f * (D3DXVec3Dot(&pPlayer->m_d3dxvVelocity, &D3DXVECTOR3(0.0f, 1.0f, 0.0f)) * D3DXVECTOR3(0.0f, 1.0f, 0.0f));
						pPlayer->m_d3dxvVelocity.y *= pLastColliedVoxel->m_fReflection;
					}
					if (pLastColliedVoxel->m_eVoxelType == VOXEL_TYPE_SWAMP_0)
					{
						bIsSwamp = true;
						d3dxvRevision.y = -0.1f;
						pPlayer->Moving(d3dxvRevision);
						pPlayer->m_CameraOperator.MoveCameraOperator(d3dxvRevision);
					}
					bIsFloor = true;
				}
				else //pCollidedVoxel->m_bIsSlope)
				{
					if (bIsFloor)
					{
						d3dxvSlopeRearNormal = pLastColliedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxRearNormal;
						bIsSlopeVoxel = true;
					}
				}
				if (bIsFloor)
				{
					d3dxvVelocityXZ = -D3DXVECTOR2(pPlayer->m_d3dxvVelocity.x, pPlayer->m_d3dxvVelocity.z);
					float fLengthVelocityXZ = D3DXVec2Length(&d3dxvVelocityXZ);
					D3DXVec2Normalize(&d3dxvVelocityXZ, &d3dxvVelocityXZ);
					float fDeceleration = pLastColliedVoxel->m_fFriction * fTimeElapsed;
					fDeceleration += PLAYER_FRICTION * fTimeElapsed;
					if (fDeceleration > fLengthVelocityXZ) fDeceleration = fLengthVelocityXZ;
					d3dxvVelocityXZ = d3dxvVelocityXZ * fDeceleration;
					pPlayer->m_d3dxvVelocity.x += d3dxvVelocityXZ.x;
					pPlayer->m_d3dxvVelocity.z += d3dxvVelocityXZ.y;
					if (pPlayer->m_d3dxvMoveDir.y != 0.0f && pLastColliedVoxel->m_eVoxelType != VOXEL_TYPE_SWAMP_0) bIsJumped = true;//m_d3dxvVelocity.y = 4.95f;
					pFloorVoxel = pLastColliedVoxel;
				}
				break;
			}
			else break;
		}
	}

	if (!bIsFloor)         // 복셀과 충돌한 것이 아니라면
	{
		for (int i = 0; i < 2; i++)
		{
			if (CPhysicalCollision::IsCollided(
				&CPhysicalCollision::MoveAABB(pPlayer->m_pMesh->m_AABB, pPlayer->GetPosition()),
				&CPhysicalCollision::MoveAABB(m_pShips[i]->m_pMesh->m_AABB, m_pShips[i]->GetPosition())))
			{
				//printf("\ndddddddddddddddddsdfsdd\n");
				/*if (pPlayer->GetPosition().y > m_pShips[i]->GetPosition().y + m_pShips[i]->m_pMesh->m_AABB.m_d3dxvMax.y)
				{
*/
				D3DXVECTOR3 d3dxvRevision = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				d3dxvRevision.y = m_pShips[i]->GetPosition().y + m_pShips[i]->m_pMesh->m_AABB.m_d3dxvMax.y - (pPlayer->GetPosition().y + pPlayer->m_pMesh->m_AABB.m_d3dxvMin.y);
				pPlayer->Moving(d3dxvRevision);
				pPlayer->m_CameraOperator.MoveCameraOperator(d3dxvRevision);

				pPlayer->m_d3dxvVelocity.y = 0.0;
				d3dxvVelocityXZ = -D3DXVECTOR2(pPlayer->m_d3dxvVelocity.x, pPlayer->m_d3dxvVelocity.z);
				float fLengthVelocityXZ = D3DXVec2Length(&d3dxvVelocityXZ);
				D3DXVec2Normalize(&d3dxvVelocityXZ, &d3dxvVelocityXZ);
				float fDeceleration = SHIP_FRICTION * fTimeElapsed;
				fDeceleration += PLAYER_FRICTION * fTimeElapsed;
				if (fDeceleration > fLengthVelocityXZ) fDeceleration = fLengthVelocityXZ;
				d3dxvVelocityXZ = d3dxvVelocityXZ * fDeceleration;
				pPlayer->m_d3dxvVelocity.x += d3dxvVelocityXZ.x;
				pPlayer->m_d3dxvVelocity.z += d3dxvVelocityXZ.y;
				if (pPlayer->m_d3dxvMoveDir.y != 0.0f) bIsJumped = true;//m_d3dxvVelocity.y = 4.95f;
				bIsFloor = true;
				break;
				/*}*/

			}
		}
	}
	else
	{
		pPlayer->BeRelievedFromLiftingPlayer();
	}

	d3dxvMiddleCollisionPivot = D3DXVECTOR3(pPlayer->GetPosition().x, pPlayer->GetPosition().y + fPivotOffset, pPlayer->GetPosition().z);

	D3DXVec3Transform(&d3dxvColLayerRow, &d3dxvMiddleCollisionPivot, &d3dxmtxFromPosToIdx);

	d3dxvColLayerRow.x = round(d3dxvColLayerRow.x);
	d3dxvColLayerRow.y = round(d3dxvColLayerRow.y);
	d3dxvColLayerRow.z = round(d3dxvColLayerRow.z);

	for (int i = 0; i < 18; i++)
	{
		d3dxvInspectionIdx.x = d3dxvColLayerRow.x + d3dxvSearchDir[i].x;
		d3dxvInspectionIdx.y = d3dxvColLayerRow.y + d3dxvSearchDir[i].y;
		d3dxvInspectionIdx.z = d3dxvColLayerRow.z + d3dxvSearchDir[i].z;

		if ((0 <= d3dxvInspectionIdx.x && d3dxvInspectionIdx.x < m_pVoxelTerrain->m_iMaxCol)
			&& (0 <= d3dxvInspectionIdx.y && d3dxvInspectionIdx.y < m_pVoxelTerrain->m_iMaxLayer)
			&& (0 <= d3dxvInspectionIdx.z && d3dxvInspectionIdx.z < m_pVoxelTerrain->m_iMaxRow))
		{

			if ((pCollidedVoxel = m_pVoxelTerrain->m_ppVoxelObjectsForCollision[
				(int)((d3dxvInspectionIdx.y) * m_pVoxelTerrain->m_iLayerStride
					+ (d3dxvInspectionIdx.z) * m_pVoxelTerrain->m_iRowStride
					+ (d3dxvInspectionIdx.x))]) != NULL)
			{
				//pCollidedVoxel->m_pMesh->m_AABB.m_pRenderObject->SetPosition(pCollidedVoxel->GetPosition());
				//pCollidedVoxel->m_pMesh->m_AABB.m_pRenderObject->Render(CGameManager::GetInstance()->m_pGameFramework->m_pd3dDeviceContext);

				if (CPhysicalCollision::IsCollided(
					&CPhysicalCollision::MoveAABB(pPlayer->m_pMesh->m_AABB, pPlayer->GetPosition()),
					&CPhysicalCollision::MoveAABB(pCollidedVoxel->m_pMesh->m_AABB, pCollidedVoxel->GetPosition())))
				{
					D3DXVECTOR3 d3dxvRevision(0.0f, 0.0f, 0.0f);

					if (i < 8)
					{
						if (bIsSwamp && i < 4)
						{
							i = 3;
							continue;
						}
						//printf("pPlayer->m_d3dxvVelocity.y : %f \n", pPlayer->m_d3dxvVelocity.y);
						if (!pCollidedVoxel->m_bIsSlope)
						{
							if (i % 2 == 0)
							{
								if (i < 4 && bIsSlopeVoxel && 0.98 < D3DXVec3Dot(&D3DXVECTOR3(0.0f, 0.0f, d3dxvSearchDir[i].z), &d3dxvSlopeRearNormal)) continue;
								if (i >= 4 || 0.0f < D3DXVec3Dot(&D3DXVECTOR3(0.0f, 0.0f, d3dxvSearchDir[i].z), &pPlayer->m_d3dxvVelocity) && pPlayer->m_d3dxvVelocity.y <= -0.3f)
								{
									pPlayer->BeRelievedFromLiftingPlayer();
									pPlayer->m_d3dxvVelocity = pPlayer->m_d3dxvVelocity - 2.0f * (D3DXVec3Dot(&pPlayer->m_d3dxvVelocity, &D3DXVECTOR3(0.0f, 0.0f, -d3dxvSearchDir[i].z)) * D3DXVECTOR3(0.0f, 0.0f, -d3dxvSearchDir[i].z));
									pPlayer->m_d3dxvVelocity.z *= pCollidedVoxel->m_fReflection;
									//d3dxvPostV.y = m_d3dxvVelocity.y;
									//d3dxvPostV.y *= pCollidedVoxel->m_fReflection;

									d3dxvRevision.z = ((pCollidedVoxel->GetPosition().z - d3dxvSearchDir[i].z * m_pVoxelTerrain->m_fCubeDepth / 2.0f)
										- (pPlayer->GetPosition().z + d3dxvSearchDir[i].z * pPlayer->m_pMesh->m_AABB.m_d3dxvMax.z));
									//d3dxvPostV.z = 0.0f;
								}
							}
							else
							{
								if (i < 4 && bIsSlopeVoxel && 0.98 < D3DXVec3Dot(&D3DXVECTOR3(-d3dxvSearchDir[i].x, 0.0f, 0.0f), &d3dxvSlopeRearNormal)) continue;
								if (i >= 4 || 0.0f < D3DXVec3Dot(&D3DXVECTOR3(-d3dxvSearchDir[i].x, 0.0f, 0.0f), &pPlayer->m_d3dxvVelocity) && pPlayer->m_d3dxvVelocity.y <= -0.3f)
								{
									pPlayer->BeRelievedFromLiftingPlayer();
									pPlayer->m_d3dxvVelocity = pPlayer->m_d3dxvVelocity - 2.0f * (D3DXVec3Dot(&pPlayer->m_d3dxvVelocity, &D3DXVECTOR3(-d3dxvSearchDir[i].x, 0.0f, 0.0f)) * D3DXVECTOR3(-d3dxvSearchDir[i].x, 0.0f, 0.0f));
									pPlayer->m_d3dxvVelocity.x *= pCollidedVoxel->m_fReflection;
									//d3dxvPostV.y = m_d3dxvVelocity.y;
									//d3dxvPostV.y *= pCollidedVoxel->m_fReflection;

									d3dxvRevision.x = ((pCollidedVoxel->GetPosition().x + d3dxvSearchDir[i].x * m_pVoxelTerrain->m_fCubeWidth / 2.0f)
										- (pPlayer->GetPosition().x - d3dxvSearchDir[i].x * pPlayer->m_pMesh->m_AABB.m_d3dxvMax.x));
								}
								//d3dxvPostV.x = 0.0f;
							}
							/*Moving(d3dxvRevision);
							m_CameraOperator.MoveCameraOperator(d3dxvRevision);*/
						}
						else
						{
							if (i % 2 == 0)         // 하
							{
								/*printf("pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxRearNormal (%f, %f, %f) \n",
								pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxRearNormal.x,
								pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxRearNormal.y,
								pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxRearNormal.z);*/
								if (i < 4 && bIsSlopeVoxel && 0.98 < D3DXVec3Dot(&D3DXVECTOR3(0.0f, 0.0f, d3dxvSearchDir[i].z), &d3dxvSlopeRearNormal)) continue;
								if (0.98f < D3DXVec3Dot(&D3DXVECTOR3(0.0f, 0.0f, -d3dxvSearchDir[i].z), &pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxRearNormal))
								{
									if (i >= 4 || 0.0f < D3DXVec3Dot(&D3DXVECTOR3(0.0f, 0.0f, d3dxvSearchDir[i].z), &pPlayer->m_d3dxvVelocity) && pPlayer->m_d3dxvVelocity.y <= -0.3f)
									{
										pPlayer->BeRelievedFromLiftingPlayer();
										pPlayer->m_d3dxvVelocity = pPlayer->m_d3dxvVelocity - 2.0f * (D3DXVec3Dot(&pPlayer->m_d3dxvVelocity, &D3DXVECTOR3(0.0f, 0.0f, -d3dxvSearchDir[i].z)) * D3DXVECTOR3(0.0f, 0.0f, -d3dxvSearchDir[i].z));
										pPlayer->m_d3dxvVelocity.z *= pCollidedVoxel->m_fReflection;
										//d3dxvPostV.y = m_d3dxvVelocity.y;
										//d3dxvPostV.y *= pCollidedVoxel->m_fReflection;

										d3dxvRevision.z = ((pCollidedVoxel->GetPosition().z - d3dxvSearchDir[i].z * m_pVoxelTerrain->m_fCubeDepth / 2.0f)
											- (pPlayer->GetPosition().z + d3dxvSearchDir[i].z * pPlayer->m_pMesh->m_AABB.m_d3dxvMax.z));
										//d3dxvPostV.z = 0.0f;
									}
								}
							}
							else                // 좌
							{
								if (i < 4 && bIsSlopeVoxel && 0.98 < D3DXVec3Dot(&D3DXVECTOR3(-d3dxvSearchDir[i].x, 0.0f, 0.0f), &d3dxvSlopeRearNormal)) continue;
								if (0.98f < D3DXVec3Dot(&D3DXVECTOR3(d3dxvSearchDir[i].x, 0.0f, 0.0f), &pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxRearNormal))
								{
									if (i >= 4 || 0.0f < D3DXVec3Dot(&D3DXVECTOR3(-d3dxvSearchDir[i].x, 0.0f, 0.0f), &pPlayer->m_d3dxvVelocity) && pPlayer->m_d3dxvVelocity.y <= -0.3f)
									{
										pPlayer->BeRelievedFromLiftingPlayer();
										pPlayer->m_d3dxvVelocity = pPlayer->m_d3dxvVelocity - 2.0f * (D3DXVec3Dot(&pPlayer->m_d3dxvVelocity, &D3DXVECTOR3(-d3dxvSearchDir[i].x, 0.0f, 0.0f)) * D3DXVECTOR3(-d3dxvSearchDir[i].x, 0.0f, 0.0f));
										pPlayer->m_d3dxvVelocity.x *= pCollidedVoxel->m_fReflection;
										//d3dxvPostV.y = m_d3dxvVelocity.y;
										//d3dxvPostV.y *= pCollidedVoxel->m_fReflection;

										d3dxvRevision.x = ((pCollidedVoxel->GetPosition().x + d3dxvSearchDir[i].x * m_pVoxelTerrain->m_fCubeWidth / 2.0f)
											- (pPlayer->GetPosition().x - d3dxvSearchDir[i].x * pPlayer->m_pMesh->m_AABB.m_d3dxvMax.x));
									}
								}
							}
						}
						pPlayer->Moving(d3dxvRevision);
						pPlayer->m_CameraOperator.MoveCameraOperator(d3dxvRevision);
					}
					else if (!bIsFloor && i < 17)      // 평범한 복셀에 한해서
					{
						if (pPlayer->m_d3dxvVelocity.y > 0.0f)
						{
							i = 16;
							continue;
						}
						float fDistance = 0.0f;
						if (!pCollidedVoxel->m_bIsSlope)
						{
							if (-0.2f < pPlayer->m_d3dxvVelocity.y)
							{
								pPlayer->m_d3dxvVelocity.y = 0.0f;
							}
							else
							{
								pPlayer->m_d3dxvVelocity = pPlayer->m_d3dxvVelocity - 2.0f * (D3DXVec3Dot(&pPlayer->m_d3dxvVelocity, &D3DXVECTOR3(0.0f, 1.0f, 0.0f)) * D3DXVECTOR3(0.0f, 1.0f, 0.0f));
								pPlayer->m_d3dxvVelocity.y *= pCollidedVoxel->m_fReflection;
							}
							//printf("m_d3dxvVelocity.y : %f \n", m_d3dxvVelocity.y);
							d3dxvRevision.y = ((pCollidedVoxel->GetPosition().y + m_pVoxelTerrain->m_fCubeHeight / 2.0f)
								- (pPlayer->GetPosition().y + pPlayer->m_pMesh->m_AABB.m_d3dxvMin.y));
							if (pCollidedVoxel->m_eVoxelType == VOXEL_TYPE_SWAMP_0)
							{
								bIsSwamp = true;
								d3dxvRevision.y += -0.1f;
							}
						}
						else //pCollidedVoxel->m_bIsSlope)
						{
							fDistance = pCollidedVoxel->GetHeightByPlayerPos(pPlayer->GetPosition()) - (pPlayer->GetPosition().y + (pPlayer->m_pMesh->m_AABB.m_d3dxvMin.y));
							if (fDistance < -0.0f) break;// i = 13;
							pPlayer->m_d3dxvVelocity.y = 0.0f;
							d3dxvRevision.y = fDistance;
							/*if (i == 0 || i == 1)
							{
							d3dxvSlopeRearNormal = pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxRearNormal;
							bIsSlopeVoxel = true;
							}*/

						}//printf("ddd\n");

						 //printf("Velocity : %f, %f, %f\n", m_d3dxvVelocity.x, m_d3dxvVelocity.y, m_d3dxvVelocity.z);
						 // 마찰력을 적용한다.
						d3dxvVelocityXZ = -D3DXVECTOR2(pPlayer->m_d3dxvVelocity.x, pPlayer->m_d3dxvVelocity.z);
						float fLengthVelocityXZ = D3DXVec2Length(&d3dxvVelocityXZ);
						D3DXVec2Normalize(&d3dxvVelocityXZ, &d3dxvVelocityXZ);
						float fDeceleration = pCollidedVoxel->m_fFriction * fTimeElapsed;
						fDeceleration += PLAYER_FRICTION * fTimeElapsed;
						if (fDeceleration > fLengthVelocityXZ) fDeceleration = fLengthVelocityXZ;
						d3dxvVelocityXZ = d3dxvVelocityXZ * fDeceleration;
						pPlayer->m_d3dxvVelocity.x += d3dxvVelocityXZ.x;
						pPlayer->m_d3dxvVelocity.z += d3dxvVelocityXZ.y;

						//printf("%f \n", fDis*tance);
						if (pPlayer->m_d3dxvMoveDir.y != 0.0f && pCollidedVoxel->m_eVoxelType != VOXEL_TYPE_SWAMP_0) bIsJumped = true;//m_d3dxvVelocity.y = 4.95f;
						pPlayer->Moving(d3dxvRevision);
						pPlayer->m_CameraOperator.MoveCameraOperator(d3dxvRevision);
						/*d3dxvCollisionPivot = D3DXVECTOR3(pPlayer->GetPosition().x, pPlayer->GetPosition().y + pPlayer->m_pMesh->m_AABB.m_d3dxvMin.y + 0.02f, pPlayer->GetPosition().z);
						D3DXVec3Transform(&d3dxvColLayerRow, &d3dxvCollisionPivot, &d3dxmtxFromPosToIdx);

						d3dxvColLayerRow.x = round(d3dxvColLayerRow.x);
						d3dxvColLayerRow.y = round(d3dxvColLayerRow.y);
						d3dxvColLayerRow.z = round(d3dxvColLayerRow.z);*/
						/*printf("<<<<<<<<<<<<<<<<<COLLIDE!!!!>>>>>>>>>>>>>>>>>>>>>\n");*/
						bIsFloor = true;
						pFloorVoxel = pCollidedVoxel;
						pPlayer->BeRelievedFromLiftingPlayer();
						i = 16;
						//break;
					}
					else if (i < 18)
					{
						if (pPlayer->m_d3dxvVelocity.y < 0.0f || pPlayer->GetPosition().y + pPlayer->m_pMesh->m_AABB.m_d3dxvMax.y > pCollidedVoxel->GetPosition().y + pCollidedVoxel->m_pMesh->m_AABB.m_d3dxvMax.y)
						{
							break;
						}
						pPlayer->m_d3dxvVelocity.y = 0.0f;
						//d3dxvRevision.x = d3dxvRevision.z = 0.0f;
						d3dxvRevision.y = ((pCollidedVoxel->GetPosition().y - m_pVoxelTerrain->m_fCubeHeight / 2.0f)
							- (pPlayer->GetPosition().y + pPlayer->m_pMesh->m_AABB.m_d3dxvMax.y));
						pPlayer->Moving(d3dxvRevision);
						pPlayer->m_CameraOperator.MoveCameraOperator(d3dxvRevision);

						if ((0 <= d3dxvColLayerRow.x && d3dxvColLayerRow.x < m_pVoxelTerrain->m_iMaxCol)
							&& (0 <= d3dxvColLayerRow.y && d3dxvColLayerRow.y < m_pVoxelTerrain->m_iMaxLayer)
							&& (0 <= d3dxvColLayerRow.z && d3dxvColLayerRow.z < m_pVoxelTerrain->m_iMaxRow))
						{
							if ((pCollidedVoxel = m_pVoxelTerrain->m_ppVoxelObjectsForCollision[
								(int)((d3dxvColLayerRow.y) * m_pVoxelTerrain->m_iLayerStride
									+ d3dxvColLayerRow.z * m_pVoxelTerrain->m_iRowStride
									+ d3dxvColLayerRow.x)]) != NULL)
							{
								if (CPhysicalCollision::IsCollided(
									&CPhysicalCollision::MoveAABB(pPlayer->m_pMesh->m_AABB, pPlayer->GetPosition()),
									&CPhysicalCollision::MoveAABB(pCollidedVoxel->m_pMesh->m_AABB, pCollidedVoxel->GetPosition())))
								{
									if (pCollidedVoxel->m_bIsSlope)
									{
										D3DXVECTOR2 d3dxvFrontNormalXZ =
											D3DXVECTOR2(pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxFrontNormal.x,
												pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxFrontNormal.z);
										D3DXVec2Normalize(&d3dxvFrontNormalXZ, &d3dxvFrontNormalXZ);
										//d3dxvFrontNormalXZ * 0.1f;

										d3dxvRevision.x = round(d3dxvFrontNormalXZ.x) == 0.0f ? 0.0f : ((d3dxvFrontNormalXZ.x * 0.5f + pCollidedVoxel->GetPosition().x) - pPlayer->GetPosition().x) * 0.9;
										d3dxvRevision.y = 0.0f;
										d3dxvRevision.z = round(d3dxvFrontNormalXZ.y) == 0.0f ? 0.0f : ((d3dxvFrontNormalXZ.y * 0.5f + pCollidedVoxel->GetPosition().z) - pPlayer->GetPosition().z) * 0.9;

										round(d3dxvFrontNormalXZ.x) != 0.0f ? pPlayer->m_d3dxvVelocity.x = 0.0f : pPlayer->m_d3dxvVelocity.z = 0.0f;

										pPlayer->Moving(d3dxvRevision);
										pPlayer->m_CameraOperator.MoveCameraOperator(d3dxvRevision);
										//if (m_d3dxvMoveDir.y != 0.0f) m_d3dxvVelocity.y = 4.95f;

										bIsSlopeVoxel = true;
									}
								}
							}
								bIsJumped = false;
								pPlayer->BeRelievedFromLiftingPlayer();
								//break;
								break;
						}
						else break;
					}

				}
			}
		}
	}
	
	//printf("Player Bottom : %f \n", GetPosition().y + m_pMesh->m_AABB.m_d3dxvMin.y);

	if (!pPlayer->m_pLiftingPlayer)
	{
		d3dxvPostV = pPlayer->m_d3dxvVelocity;

		// 외부에서 가해진 이동 속도를 갱신한다
		D3DXVECTOR3 d3dxvMoveDir = D3DXVECTOR3(pPlayer->m_d3dxvMoveDir.x, 0.0f, pPlayer->m_d3dxvMoveDir.z);
		D3DXVec3Normalize(&d3dxvMoveDir, &d3dxvMoveDir);
		d3dxvMovingDir = PLAYER_ACCELERATION_MAGNITUDE * d3dxvMoveDir * fTimeElapsed;// +d3dxvPostV;

																					 // 최대 속도를 제한한다.
		d3dxvVelocityXZ = D3DXVECTOR2(d3dxvMovingDir.x, d3dxvMovingDir.z);

		/*if (D3DXVec2Length(&d3dxvVelocityXZ) > PLAYER_MAX_VELOCITY_MAGNITUDE)
		{
		D3DXVec2Normalize(&d3dxvVelocityXZ, &d3dxvVelocityXZ);
		d3dxvVelocityXZ *= PLAYER_MAX_VELOCITY_MAGNITUDE;
		}*/
		d3dxvPostV.x += d3dxvVelocityXZ.x;
		d3dxvPostV.z += d3dxvVelocityXZ.y;

		// 속도에 중력가속도를 적용한다.
		d3dxvPostV += d3dxvG * fTimeElapsed;

		// 최대 속도 제한
		if (d3dxvPostV.y > PLYAER_MAX_JUMP_VELOCITY_MAGNITUDE)
		{
			d3dxvPostV.y = PLYAER_MAX_JUMP_VELOCITY_MAGNITUDE;
		}

		//printf("[bIsFloor : %d] \n", bIsFloor);
		if (!bIsFloor)         // 복셀과 충돌한 것이 아니라면
		{
			// 바람 저항을 계산한다.
			//d3dxvVelocityXZ = D3DXVECTOR2(d3dxvPostV.x, d3dxvPostV.z);
			//d3dxvVelocityXZ *= (AIR_RESISTANCE_COEFFICIENT);
			d3dxvVelocityXZ = -D3DXVECTOR2(d3dxvPostV.x, d3dxvPostV.z);
			float fLengthVelocityXZ = D3DXVec2Length(&d3dxvVelocityXZ);
			D3DXVec2Normalize(&d3dxvVelocityXZ, &d3dxvVelocityXZ);
			float fDeceleration = AIR_RESISTANCE_COEFFICIENT * fTimeElapsed;
			//d3dxvVelocityXZ *= (pCollidedVoxel->m_fDamping * PLAYER_FRICTION);
			if (fDeceleration > fLengthVelocityXZ) fDeceleration = fLengthVelocityXZ;
			d3dxvVelocityXZ = d3dxvVelocityXZ * fDeceleration;
			d3dxvPostV.x += d3dxvVelocityXZ.x;
			d3dxvPostV.z += d3dxvVelocityXZ.y;
			//d3dxvPostV.x = d3dxvVelocityXZ.x;
			//d3dxvPostV.z = d3dxvVelocityXZ.y;
		}
		else if (pFloorVoxel)
		{
			if (pFloorVoxel->m_eVoxelType == VOXEL_TYPE_JUMP_0) CSoundManager::FMODSoundPlayAndRepeat(GetSoundTypeForType(pFloorVoxel->m_eVoxelType), 0.5f, false);
			else if(pPlayer->IsMoving()) CSoundManager::FMODSoundPlayAndRepeat(GetSoundTypeForType(pFloorVoxel->m_eVoxelType), 0.5f, false);
		}
		

		d3dxvVelocityXZ = D3DXVECTOR2(d3dxvPostV.x, d3dxvPostV.z);

		if (bIsSlopeVoxel)
		{
			if (D3DXVec2Length(&d3dxvVelocityXZ) > D3DXVec3Length(&D3DXVECTOR3(0.0f, d3dxvPostV.y, 0.0f)))
			{
				d3dxvPostV.y = -D3DXVec2Length(&d3dxvVelocityXZ);
			}
		}
		if (bIsJumped)
		{
			d3dxvPostV.y = pPlayer->m_d3dxvVelocity.y = pPlayer->m_fJumpdVelYM;//4.95f;// +d3dxvPostV.y;
			CSoundManager::FMODSoundPlayAndRepeat(SOUND_TYPE_EFFECT_JUMP);
			CSoundManager::FMODSoundPlayAndRepeat(SOUND_TYPE_EFFECT_SHOUT, 0.8f);
		}

		// 최대 속도 제한
		if (D3DXVec2Length(&d3dxvVelocityXZ) > pPlayer->m_fMaxRunVelM)
		{
			D3DXVec2Normalize(&d3dxvVelocityXZ, &d3dxvVelocityXZ);
			d3dxvVelocityXZ *= pPlayer->m_fMaxRunVelM;
			
		}

		if (bIsSwamp && D3DXVec2Length(&d3dxvVelocityXZ) > PLAYER_MAX_VELOCITY_MAGNITUDE_IN_SWAMP)
		{
			D3DXVec2Normalize(&d3dxvVelocityXZ, &d3dxvVelocityXZ);
			d3dxvVelocityXZ *= PLAYER_MAX_VELOCITY_MAGNITUDE_IN_SWAMP;
		}
		
		// 최종 속도 생성
		d3dxvPostV.x = d3dxvVelocityXZ.x;
		d3dxvPostV.z = d3dxvVelocityXZ.y;
		// 이동량 갱신
		d3dxvMovingDir = ((d3dxvPostV + pPlayer->m_d3dxvVelocity) * fTimeElapsed) / 2.0f;

		// 실제 이동
		pPlayer->Moving(d3dxvMovingDir);
		pPlayer->m_CameraOperator.MoveCameraOperator(d3dxvMovingDir);

		// 현재 속도 갱신
		pPlayer->m_d3dxvVelocity = d3dxvPostV;
		//printf("Player Bottome : %.6f \n", GetPosition().y - 0.5f);
		/*printf("pPlayer->GetPosition() bottom2 (%f, %f, %f) \n",
		pPlayer->GetPosition().x, pPlayer->GetPosition().y + pPlayer->m_pMesh->m_AABB.m_d3dxvMin.y, pPlayer->GetPosition().z);*/
		// 이동 방향 초기화
		
		//d3dxvVelocityXZ = D3DXVECTOR2(m_d3dxvVelocity.x, m_d3dxvVelocity.z);
		//printf("Player Velocity : %f\n", D3DXVec2Length(&d3dxvVelocityXZ));
		//printf("Velocity : (%.5f, %.5f, %.5f) \n", m_d3dxvVelocity.x, m_d3dxvVelocity.y, m_d3dxvVelocity.z);
		//printf("-----------------------------\n");
	}

	// 스태미나 갱신
	if (pPlayer->m_bIsActive)
	{
		D3DXVECTOR3 d3dxvVel = pPlayer->m_d3dxvVelocity;

		if (D3DXVec2Length(&D3DXVECTOR2(d3dxvVel.x, d3dxvVel.z)) < 0.3f)
		{
			if (!pPlayer->m_pLiftedPlayer) pPlayer->IncreaseStamina(fTimeElapsed);      // 움직이지 않고 있다면,
			else pPlayer->IncreaseStamina(fTimeElapsed / 4.0f);
		}
		else
		{
			if (pPlayer->m_pLiftedPlayer)
			{
				if (pPlayer->IsMoving()) pPlayer->DecreaseStamina(fTimeElapsed);
			}
			else pPlayer->IncreaseStamina(fTimeElapsed);
		}
	}
	pPlayer->m_d3dxvMoveDir = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

void CScene::AnimateObjects(ID3D11Device *pd3dDevice, ID3D11DeviceContext*pd3dDeviceContext, float fTimeElapsed)
{
	m_fAccumulatedTime += fTimeElapsed;
	m_cTimeManager.DecreaseTimeSecRemaining(fTimeElapsed);
	if(m_pWaveEffect) m_pWaveEffect->m_pMesh->Animate(pd3dDeviceContext, fTimeElapsed);						// 클라 영역
	
	CEffectManager::UpdateEffectManager(fTimeElapsed);
	
	static float time = 0.0f;
	if (m_pLights->m_pLights[5].m_bEnable == 1.0f)
	{
		time += fTimeElapsed;
		if (time > 0.18f)
		{
			m_pLights->m_pLights[5].m_bEnable = 0.0f;
			time = 0.0f;
		}
	}

	m_pLights->m_d3dxvCameraPosition.x = m_pPlayersMgrInform->GetMyPlayer()->m_CameraOperator.m_Camera.GetPosition().x;
	m_pLights->m_d3dxvCameraPosition.y = m_pPlayersMgrInform->GetMyPlayer()->m_CameraOperator.m_Camera.GetPosition().y;
	m_pLights->m_d3dxvCameraPosition.z = m_pPlayersMgrInform->GetMyPlayer()->m_CameraOperator.m_Camera.GetPosition().z;
	
	for (int i = 0; i < m_pPlayersMgrInform->m_iPlayersNum; i++)
	{
		if (m_pPlayersMgrInform->m_ppPlayers[i])
		{
			m_pPlayersMgrInform->m_ppPlayers[i]->m_bIsActive = m_pPlayersMgrInform->m_ppPlayers[i]->m_serverActive;

			if (m_pPlayersMgrInform->m_ppPlayers[i]->m_bIsActive)
			{
				float mx = m_pPlayersMgrInform->m_ppPlayers[i]->m_SyncPosition.x - m_pPlayersMgrInform->m_ppPlayers[i]->GetPosition().x;
				float my = m_pPlayersMgrInform->m_ppPlayers[i]->m_SyncPosition.y - m_pPlayersMgrInform->m_ppPlayers[i]->GetPosition().y;
				float mz = m_pPlayersMgrInform->m_ppPlayers[i]->m_SyncPosition.z - m_pPlayersMgrInform->m_ppPlayers[i]->GetPosition().z;

				m_pPlayersMgrInform->m_ppPlayers[i]->SetPosition(
					D3DXVECTOR3(m_pPlayersMgrInform->m_ppPlayers[i]->GetPosition().x + (mx*fTimeElapsed),
						m_pPlayersMgrInform->m_ppPlayers[i]->GetPosition().y + (my*fTimeElapsed),
						m_pPlayersMgrInform->m_ppPlayers[i]->GetPosition().z + (mz*fTimeElapsed)));
				
				if (i != m_pPlayersMgrInform->m_iMyPlayerID)
				{
					m_pPlayersMgrInform->m_ppPlayers[i]->m_d3dxvMoveDir.x = (float)m_pPlayersMgrInform->m_ppPlayers[i]->m_MoveX;
					m_pPlayersMgrInform->m_ppPlayers[i]->m_d3dxvMoveDir.z = (float)m_pPlayersMgrInform->m_ppPlayers[i]->m_MoveZ;
					//printf("카메라돌아가냐? %d \n", m_pPlayersMgrInform->m_ppPlayers[0]->m_CameraY);
					m_pPlayersMgrInform->m_ppPlayers[i]->m_CameraOperator.RotateLocalY(CAMERA_ROTATION_DEGREE_PER_SEC*m_pPlayersMgrInform->m_ppPlayers[i]->m_CameraY, fTimeElapsed);
				}

				m_pPlayersMgrInform->m_ppPlayers[i]->RotateMoveDir(fTimeElapsed);
				m_pPlayersMgrInform->m_ppPlayers[i]->Animate(pd3dDeviceContext, m_fAccumulatedTime * 33);

				if (CPhysicalCollision::IsCollided(
					&CPhysicalCollision::MoveAABB(m_pPlayersMgrInform->m_ppPlayers[i]->m_pMesh->m_AABB, m_pPlayersMgrInform->m_ppPlayers[i]->GetPosition()),
					&CPhysicalCollision::MoveAABB(m_pTreasureChest->m_pMesh->m_AABB, m_pTreasureChest->GetPosition())))
				{
					if (CPhysicalCollision::ImpurseBasedCollisionResolution(m_pPlayersMgrInform->m_ppPlayers[i], m_pTreasureChest))
					{
						CSoundManager::FMODSoundPlayAndRepeat(SOUND_TYPE_EFFECT_COLLISION);
					}
					m_pTreasureChest->BeRelievedFromLiftingPlayer();
				}

				// i 번째 플레이어를 기준으로 한 플레이어 간 충격량 기반 충돌 체크
				for (int j = i + 1; j < m_pPlayersMgrInform->m_iPlayersNum; j++)
				{
					if (m_pPlayersMgrInform->m_ppPlayers[j])
					{
						if (m_pPlayersMgrInform->m_ppPlayers[j]->m_bIsActive)
						{
							float fDistance = D3DXVec3Length(&(m_pPlayersMgrInform->m_ppPlayers[i]->GetPosition() - m_pPlayersMgrInform->m_ppPlayers[j]->GetPosition()));
							//printf("pPlayerA Pos : %f %f %f \n", pPlayerA->GetPosition().x, pPlayerA->GetPosition().y, pPlayerA->GetPosition().z);
							float fRadiusA = 0.3f;//pPlayerA->m_pMesh->m_AABB.m_d3dxvMax.x;
							float fRadiusB = 0.3f;//pPlayerB->m_pMesh->m_AABB.m_d3dxvMax.x;
							if (fDistance < fRadiusA + fRadiusB)
							{
								m_pLights->m_pLights[5].m_bEnable = 1.0f;
								m_pLights->m_pLights[5].m_d3dxvPosition = m_pPlayersMgrInform->m_ppPlayers[j]->GetPosition() + (m_pPlayersMgrInform->m_ppPlayers[i]->GetPosition() - m_pPlayersMgrInform->m_ppPlayers[j]->GetPosition()) / 2.0f;
								m_pLights->m_pLights[5].m_d3dxvPosition.y += 0.4f;
							}
							if (CPhysicalCollision::ImpurseBasedCollisionResolution(m_pPlayersMgrInform->m_ppPlayers[i], m_pPlayersMgrInform->m_ppPlayers[j]))
							{
								CSoundManager::FMODSoundPlayAndRepeat(SOUND_TYPE_EFFECT_COLLISION);
							}
							//printf("m_fRange : %f \n", m_pLights->m_pLights[5].m_fRange);
						}
					}
				}

				// 복셀 터레인 및 씬의 환경 변수에 기반한 충돌 체크 및 물리 움직임
				MoveObjectUnderPhysicalEnvironment(m_pPlayersMgrInform->m_ppPlayers[i], fTimeElapsed);
				/*
				if (m_pPlayersMgrInform->m_ppPlayers[i]->GetPosition().y < -1.0f)
				{
					printf("플레이어가 물에 빠져버렸습니다!");
					m_pPlayersMgrInform->m_ppPlayers[i]->MissPlayer();
					m_RespawnManager.RegisterRespawnManager(m_pPlayersMgrInform->m_ppPlayers[i], true);
				}*/
			}
			else continue;
		}
	}

	//printf("Pos (%.2f, %.2f, %.2f)\n", m_pPlayersMgrInform->GetMyPlayer()->GetPosition().x, m_pPlayersMgrInform->GetMyPlayer()->GetPosition().y, m_pPlayersMgrInform->GetMyPlayer()->GetPosition().z);
	D3DXVECTOR3 boxPos = m_pTreasureChest->GetPosition();
	D3DXVECTOR3 boxmove = m_pTreasureChest->m_SyncPosition;
	float mx = m_pTreasureChest->m_SyncPosition.x - m_pTreasureChest->GetPosition().x;
	float my = m_pTreasureChest->m_SyncPosition.y - m_pTreasureChest->GetPosition().y;
	float mz = m_pTreasureChest->m_SyncPosition.z - m_pTreasureChest->GetPosition().z;
	m_pTreasureChest->SetPosition(D3DXVECTOR3(boxPos.x+ (mx*fTimeElapsed), boxPos.y + (my*fTimeElapsed), boxPos.z + (mz*fTimeElapsed)));
	//printf("박스 위치 : %f %f %f\n", boxPos.x, boxPos.y, boxPos.z);
	MoveObjectUnderPhysicalEnvironment(m_pTreasureChest, fTimeElapsed);
	m_pTreasureChest->BeDraggedAwayByLiftingPlayer(fTimeElapsed);

	
	D3DXVECTOR3 d3dxvTreasurecChestPos = m_pTreasureChest->GetPosition();
	CPhysicalCollision::FloatOnWater(m_pTreasureChest, fTimeElapsed, m_pWaveEffect->m_pMesh->GetHeight(d3dxvTreasurecChestPos.x, d3dxvTreasurecChestPos.z), 1025.0f, 1.0f);

	m_RespawnManager.UpdateRespawnManager(fTimeElapsed);
	for (int i = 0; i < m_pPlayersMgrInform->m_iPlayersNum; i++)
	{
		if (i == m_pPlayersMgrInform->m_iMyPlayerID)
			continue;
		if (m_pPlayersMgrInform->m_ppPlayers[i])
		{
			if (m_pPlayersMgrInform->m_ppPlayers[i]->m_IsLift&&!m_pPlayersMgrInform->m_ppPlayers[i]->m_bIsPushed)
			{
				m_pPlayersMgrInform->m_ppPlayers[i]->LiftPlayer(m_pTreasureChest, m_pVoxelTerrain);
				m_pPlayersMgrInform->m_ppPlayers[i]->PushPlayers(m_pPlayersMgrInform->m_ppPlayers, m_pPlayersMgrInform->m_iPlayersNum);
				m_pPlayersMgrInform->m_ppPlayers[i]->m_bIsPushed = true;
			}
			else if(!m_pPlayersMgrInform->m_ppPlayers[i]->m_IsLift&&m_pTreasureChest->m_pLiftingPlayer && m_pPlayersMgrInform->m_ppPlayers[i]->m_bIsPushed)
			{
				m_pPlayersMgrInform->m_ppPlayers[i]->m_bIsPushed = false;
				m_pTreasureChest->BeRelievedFromLiftingPlayer();
			}
		}
	}
	/*
	if (!m_cResultManager.IsGameOver())
	{
		for (int i = 0; i < 2; i++)
		{
			if (CPhysicalCollision::IsCollided(
				&CPhysicalCollision::MoveAABB(m_pShips[i]->m_pMesh->m_AABB, m_pShips[i]->GetPosition()),
				&CPhysicalCollision::MoveAABB(m_pTreasureChest->m_pMesh->m_AABB, m_pTreasureChest->GetPosition())))
			{
				if (m_pTreasureChest->GetPosition().y > m_pShips[i]->GetPosition().y + m_pShips[i]->m_pMesh->m_AABB.m_d3dxvMax.y)
				{
					if (i == 0) m_cResultManager.RegisterWinnerType(BELONG_TYPE_BLUE);
					else m_cResultManager.RegisterWinnerType(BELONG_TYPE_RED);
				}
			}
		}
		if (m_cTimeManager.IsTimeOver())
		{
			m_cResultManager.ResultIsDraw();
		}
	}*/
	if (m_cResultManager.IsGameOver())
	{
		static float fDelayChangeTime = 3.0f;		// 3초후 대기방으로 간다.
		fDelayChangeTime -= fTimeElapsed;

		if (fDelayChangeTime <= 0.0f)
		{
			fDelayChangeTime = 3.0f;
			CGameManager::GetInstance()->ChangeGameState(new CWaitingRoomState());
		}
	}
}

SOUND_TYPE CScene::GetSoundTypeForType(VOXEL_TYPE &eVoxelType)
{
	switch (eVoxelType)
	{
	case VOXEL_TYPE_GRASS_0:
		return SOUND_TYPE_EFFECT_VOXEL_GRASS;
		break;
	case VOXEL_TYPE_WOOD_0:
		return SOUND_TYPE_EFFECT_VOXEL_TREE;
		break;
	case VOXEL_TYPE_ZUNGLE_UPHILL_0:
	case VOXEL_TYPE_ZUNGLE_UPHILL_1:
	case VOXEL_TYPE_ZUNGLE_UPHILL_2:
	case VOXEL_TYPE_ZUNGLE_UPHILL_3:
		return  SOUND_TYPE_EFFECT_VOXEL_GRASS;
		break;
	case VOXEL_TYPE_SWAMP_0:
		return SOUND_TYPE_EFFECT_VOXEL_SWAMP;
		break;
	case VOXEL_TYPE_JUMP_0:
		return SOUND_TYPE_EFFECT_JUMP;
		break;
	case VOXEL_TYPE_SNOW_0:
		return SOUND_TYPE_EFFECT_VOXEL_SNOW;
		break;
	case VOXEL_TYPE_ICE_0:
		return SOUND_TYPE_EFFECT_VOXEL_ICE;
		break;
	case VOXEL_TYPE_SNOW_UPHILL_0:
	case VOXEL_TYPE_SNOW_UPHILL_1:
	case VOXEL_TYPE_SNOW_UPHILL_2:
	case VOXEL_TYPE_SNOW_UPHILL_3:
		return SOUND_TYPE_EFFECT_VOXEL_SNOW;
		break;
	}
}

void CScene::Render(ID3D11DeviceContext*pd3dDeviceContext, CCamera *pCamera)
{
	if (m_pLights && m_pd3dcbLights) UpdateShaderVariable(pd3dDeviceContext, m_pLights);
	

	for (int i = 0; i < 2; i++)
	{
		m_pShips[i]->Render(pd3dDeviceContext);
	}
	m_pTreasureChest->Render(pd3dDeviceContext);
	m_pVoxelTerrain->Render(pd3dDeviceContext, pCamera, true);
	m_pSkydom->Render(pd3dDeviceContext);
	
	if (m_pWaveEffect) m_pWaveEffect->Render(pd3dDeviceContext);

	if (m_cResultManager.IsGameOver())
	{
		if (m_cResultManager.GetResultType() == RESULT_TYPE_DRAW)
		{
			// 무승부 렌더
			m_pResultUI->GetSpritePieceByIndex(2)->Render(pd3dDeviceContext);
		}
		else
		{
			if (m_cResultManager.IsWinner(m_pPlayersMgrInform->GetMyPlayer()->m_BelongType))
			{
				// 승자 렌더
				m_pResultUI->GetSpritePieceByIndex(0)->Render(pd3dDeviceContext);
			}
			else
			{
				// 패자 렌더
				m_pResultUI->GetSpritePieceByIndex(1)->Render(pd3dDeviceContext);
			}
		}
	}

	CEffectManager::RenderEffects(pd3dDeviceContext);
}

CJungleScene::CJungleScene() : CScene("new mapfile test/jungle0_big.txt")
{
	CSoundManager::FMODSoundPlayAndRepeat(SOUND_TYPE_BACK_WAVE);
	CSoundManager::FMODSoundPlayAndRepeat(SOUND_TYPE_BACK_FOREST);
}

CJungleScene::~CJungleScene()
{
	CSoundManager::FMODSoundStop(SOUND_TYPE_BACK_WAVE);
	CSoundManager::FMODSoundStop(SOUND_TYPE_BACK_FOREST);
}

CSnowScene::CSnowScene() : CScene("new mapfile test/snow0_big.txt")
{
	CSoundManager::FMODSoundPlayAndRepeat(SOUND_TYPE_BACK_WAVE);
}

CSnowScene::~CSnowScene()
{
	CSoundManager::FMODSoundStop(SOUND_TYPE_BACK_WAVE);
}