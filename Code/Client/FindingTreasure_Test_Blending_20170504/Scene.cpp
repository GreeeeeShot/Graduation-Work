#include "stdafx.h"
#include "Scene.h"
#include "Player.h"
#include "TextureResource.h"
#include "ShaderResource.h"

CPlayersMgrInform::CPlayersMgrInform()
{
	m_ppPlayers = NULL;
	m_iPlayersNum = 0;
	m_iMyPlayerID = -1;
}
CPlayersMgrInform::~CPlayersMgrInform()
{

}

CScene::CScene(char* cSceneTextFileName)
{
	/*m_ppObjects = NULL;*/
	m_nObjects = 0;

	m_pPlayersMgrInform = NULL;
	m_cSceneTextFileName = cSceneTextFileName;

	m_pLights = NULL;
	m_pd3dcbLights = NULL;

	m_pVoxelTerrain = NULL;
	m_pWaveEffect = NULL;
	m_fGravitationalAcceleration = GRAVITATIONAL_ACCELERATION;
}

CScene::~CScene()
{
}

void CScene::BuildObjects(ID3D11Device *pd3dDevice)
{
	m_pWaveEffect = new CWaveObject();
	m_pWaveEffect->SetMaterial(CMaterialResource::pWaveMaterial);
	m_pWaveEffect->SetTexture(CTextureResource::pWaveTexture);
	m_pWaveEffect->SetMesh(CMeshResource::pWaveMesh);
	m_pWaveEffect->SetShader(CShaderResource::pTexturedLightingShader);

	for (int i = 0; i < 2; i++)
	{
		m_pShips[i] = new CGameObject();
		m_pShips[i]->SetMaterial(CMaterialResource::pStandardMaterial);
		m_pShips[i]->SetTexture(CTextureResource::pWoodTexture);
		m_pShips[i]->SetMesh(CMeshResource::pShipMesh);
		m_pShips[i]->SetShader(CShaderResource::pTexturedLightingShader);
	}
	m_pShips[0]->SetPosition(-42.0f, 0.5f,0.0f);
	m_pShips[1]->SetPosition(42.0f, 0.5f, 0.0f);

	m_pTreasureChest = new CPlayer();
	m_pTreasureChest->SetMaterial(CMaterialResource::pStandardMaterial);
	m_pTreasureChest->SetTexture(CTextureResource::pWoodTexture);
	m_pTreasureChest->SetMesh(CMeshResource::pTreasureChestMesh);
	m_pTreasureChest->SetShader(CShaderResource::pTexturedLightingShader);
	m_pTreasureChest->SetPosition(0.0f, 10.f, 0.0f);

	m_pVoxelTerrain = new CVoxelTerrain();
	//CShader *pTexturedLightingShader = new CTexturedLightingShader();
	//pTexturedLightingShader->CreateShader(pd3dDevice);
	CBlueprint* pTestMapBP = new CBlueprint();
	//m_pVoxelTerrain->CreateVoxelTerrain(pTestMapBP->LoadBlueprint("sample_house.txt"), pTexturedLightingShader);
	m_pVoxelTerrain->CreateVoxelTerrain(pTestMapBP->LoadBlueprint(m_cSceneTextFileName));
	//m_pVoxelTerrain->CreateVoxelTerrain(pTestMapBP->LoadBlueprint("performance_test_map.txt"));
	delete pTestMapBP;

	CreateShaderVariables(pd3dDevice);							// 조명을 입력받는 상수 버퍼를 생성함과 동시에 조명 정보를 집어 넣는다.
	InitScene();
}

void CScene::InitScene(void)
{
	m_RespawnManager.InitRespawnManager();
	m_pTreasureChest->SetPosition(0.0f, 10.f, 0.0f);
	for (int i = 0; i < m_pPlayersMgrInform->m_iPlayersNum; i++)
	{
		m_RespawnManager.RegisterRespawnManager(m_pPlayersMgrInform->m_ppPlayers[i], false);
	}
	//m_pPlayersMgrInform->m_ppPlayers[m_pPlayersMgrInform->m_iMyPlayerID]->InitPlayer(D3DXVECTOR3(-7.0f, 40.0f, 7.0f));

	if (m_pVoxelTerrain)
	{
		delete m_pVoxelTerrain;
		m_pVoxelTerrain = NULL;
	}
	
	m_pVoxelTerrain = new CVoxelTerrain();
	CBlueprint* pTestMapBP = new CBlueprint();
	m_pVoxelTerrain->CreateVoxelTerrain(pTestMapBP->LoadBlueprint(m_cSceneTextFileName));
	delete pTestMapBP;
}

void CScene::ReleaseObjects()
{
	ReleaseShaderVariables();
	//게임 객체 리스트의 각 객체를 반환(Release)하고 리스트를 소멸시킨다.
	/*if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) m_ppObjects[j]->Release();
		delete[] m_ppObjects;
	}*/
}

void CScene::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));
	m_pLights->m_d3dxcGlobalAmbient = D3DXCOLOR(0.05f, 0.1f, 0.05f, 1.0f);

	//3개의 조명(점 광원, 스팟 광원, 방향성 광원)을 설정한다.
	m_pLights->m_pLights[0].m_bEnable = 1.0f;
	m_pLights->m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 300.0f;
	m_pLights->m_pLights[0].m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_d3dxcDiffuse = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_d3dxcSpecular = D3DXCOLOR(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[0].m_d3dxvPosition = D3DXVECTOR3(-20.0f, 20.0f, -20.0f);
	m_pLights->m_pLights[0].m_d3dxvDirection = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[0].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.001f, 0.0001f);
	m_pLights->m_pLights[1].m_bEnable = 1.0f;
	m_pLights->m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[1].m_fRange = 700.0f;
	m_pLights->m_pLights[1].m_d3dxcAmbient = D3DXCOLOR(0.0f, 0.001f, 0.0f, 1.0f);
	m_pLights->m_pLights[1].m_d3dxcDiffuse = D3DXCOLOR(0.6f, 0.05f, 0.1f, 1.0f);
	m_pLights->m_pLights[1].m_d3dxcSpecular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[1].m_d3dxvPosition = D3DXVECTOR3(0.0f, 30.0f, 0.0f);
	m_pLights->m_pLights[1].m_d3dxvDirection = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	m_pLights->m_pLights[1].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[1].m_fFalloff = 18.0f;
	m_pLights->m_pLights[1].m_fPhi = (float)cos(D3DXToRadian(50.0f));
	m_pLights->m_pLights[1].m_fTheta = (float)cos(D3DXToRadian(20.0f));
	m_pLights->m_pLights[2].m_bEnable = 1.0f;
	m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[2].m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[2].m_d3dxcDiffuse = D3DXCOLOR(0.2f, 0.2f, 0.1f, 1.0f);
	m_pLights->m_pLights[2].m_d3dxcSpecular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[2].m_d3dxvDirection = D3DXVECTOR3(-5.0f, -5.0f, 5.0f);
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

	m_pLights->m_pLights[4].m_bEnable = 0.0f;
	m_pLights->m_pLights[4].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[4].m_d3dxcAmbient = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[4].m_d3dxcDiffuse = D3DXCOLOR(0.5f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[4].m_d3dxcSpecular = D3DXCOLOR(0.7f, 0.1f, 0.1f, 8.0f);
	m_pLights->m_pLights[4].m_d3dxvDirection = D3DXVECTOR3(3.0f, -1.0f, -3.0f);

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
}

void CScene::ReleaseShaderVariables()
{
	if (m_pLights) delete m_pLights;
	if (m_pd3dcbLights) m_pd3dcbLights->Release();
}

void CScene::UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, LIGHTS *pLights)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbLights, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	LIGHTS *pcbLight = (LIGHTS *)d3dMappedResource.pData;
	memcpy(pcbLight, pLights, sizeof(LIGHTS));
	pd3dDeviceContext->Unmap(m_pd3dcbLights, 0);
	pd3dDeviceContext->PSSetConstantBuffers(PS_SLOT_LIGHT, 1, &m_pd3dcbLights);
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		default:
			break;
		}
		m_pPlayersMgrInform->GetMyPlayer()->m_bIsPushed = false;
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'a': case 'A':
			if (!m_pPlayersMgrInform->GetMyPlayer()->m_bIsPushed)
			{
				m_pPlayersMgrInform->GetMyPlayer()->PushPlayer(m_pTreasureChest);
				m_pPlayersMgrInform->GetMyPlayer()->PushPlayers(m_pPlayersMgrInform->m_ppPlayers, m_pPlayersMgrInform->m_iPlayersNum);
				m_pPlayersMgrInform->GetMyPlayer()->m_bIsPushed = true;
			}
			break;
		case '1': case '2': case '3': case '4': case '5':
			if (!m_pPlayersMgrInform->m_ppPlayers[m_pPlayersMgrInform->m_iMyPlayerID]->m_bIsDigOrInstall) m_pPlayersMgrInform->m_ppPlayers[m_pPlayersMgrInform->m_iMyPlayerID]->m_iVoxelPocketSlotIdx = wParam - '0' - 1;

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
	static D3DXVECTOR3 d3dxvSearchDir[23] =
	{
		//D3DXVECTOR3(0.0f, 0.0f, 1.0f),               // 상
		//D3DXVECTOR3(0.0f, 0.0f, -1.0f),               // 하
		//D3DXVECTOR3(1.0f, 0.0f, 0.0f),               // 우
		//D3DXVECTOR3(-1.0f, 0.0f, 0.0f),               // 좌

		D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DXVECTOR3(0.0f, -1.0f, 0.0f),

		D3DXVECTOR3(0.0f, 0.0f, 1.0f),               // 상
		D3DXVECTOR3(0.0f, 0.0f, -1.0f),               // 하
		D3DXVECTOR3(1.0f, 0.0f, 0.0f),               // 우
		D3DXVECTOR3(-1.0f, 0.0f, 0.0f),               // 좌

		D3DXVECTOR3(1.0f, -1.0f, -1.0f),   D3DXVECTOR3(0.0f, -1.0f, -1.0f),   D3DXVECTOR3(-1.0f, -1.0f, -1.0f),
		D3DXVECTOR3(1.0f, -1.0f, 0.0f),                                 D3DXVECTOR3(-1.0f, -1.0f, 0.0f),
		D3DXVECTOR3(1.0f, -1.0f, 1.0f),      D3DXVECTOR3(0.0f, -1.0f, 1.0f),      D3DXVECTOR3(-1.0f, -1.0f, 1.0f),

		D3DXVECTOR3(0.0f, 1.0f, 0.0f),
		D3DXVECTOR3(1.0f, 1.0f, -1.0f),      D3DXVECTOR3(0.0f, 1.0f, -1.0f),      D3DXVECTOR3(-1.0f, 1.0f, -1.0f),
		D3DXVECTOR3(1.0f, 1.0f, 0.0f),                                 D3DXVECTOR3(-1.0f, 1.0f, 0.0f),
		D3DXVECTOR3(1.0f, 1.0f, 1.0f),      D3DXVECTOR3(0.0f, 1.0f, 1.0f),      D3DXVECTOR3(-1.0f, 1.0f, 1.0f)
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
	D3DXVECTOR3 d3dxvCollisionPivot = D3DXVECTOR3(pPlayer->GetPosition().x, pPlayer->GetPosition().y + pPlayer->m_pMesh->m_AABB.m_d3dxvMin.y + 0.05f, pPlayer->GetPosition().z);
	D3DXVec3Transform(&d3dxvColLayerRow, &d3dxvCollisionPivot, &d3dxmtxFromPosToIdx);

	d3dxvColLayerRow.x = round(d3dxvColLayerRow.x);
	d3dxvColLayerRow.y = round(d3dxvColLayerRow.y);
	d3dxvColLayerRow.z = round(d3dxvColLayerRow.z);

	CVoxel *pCollidedVoxel;
	D3DXVECTOR3 d3dxvInspectionIdx;
	bool bIsSlopeVoxel = false;
	D3DXVECTOR3 d3dxvSlopeRearNormal;
	bool bIsSwamp = false;
	bool bIsFloor = false;
	bool bIsJumped = false;

	for (int i = 0; i < 23; i++)
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
				if (CPhysicalCollision::IsCollided(
					&CPhysicalCollision::MoveAABB(pPlayer->m_pMesh->m_AABB, pPlayer->GetPosition()),
					&CPhysicalCollision::MoveAABB(pCollidedVoxel->m_pMesh->m_AABB, pCollidedVoxel->GetPosition())))
				{
					D3DXVECTOR3 d3dxvRevision(0.0f, 0.0f, 0.0f);

					if (i < 2)
					{
						if (pPlayer->m_d3dxvVelocity.y > 0.0f)
						{
							i = 1;
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
							if (i == 0 || i == 1)
							{
								d3dxvSlopeRearNormal = pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxRearNormal;
								bIsSlopeVoxel = true;
							}

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
						i = 1;
						//break;
					}
					else if (i < 6)
					{
						if (bIsSwamp)
						{
							i = 5;
							continue;
						}
						if (!pCollidedVoxel->m_bIsSlope)
						{
							if (i < 4)
							{
								if (bIsSlopeVoxel && 0.98 < D3DXVec3Dot(&D3DXVECTOR3(0.0f, 0.0f, d3dxvSearchDir[i].z), &d3dxvSlopeRearNormal)) continue;
								pPlayer->m_d3dxvVelocity = pPlayer->m_d3dxvVelocity - 2.0f * (D3DXVec3Dot(&pPlayer->m_d3dxvVelocity, &D3DXVECTOR3(0.0f, 0.0f, -d3dxvSearchDir[i].z)) * D3DXVECTOR3(0.0f, 0.0f, -d3dxvSearchDir[i].z));
								pPlayer->m_d3dxvVelocity.z *= pCollidedVoxel->m_fReflection;
								//d3dxvPostV.y = m_d3dxvVelocity.y;
								//d3dxvPostV.y *= pCollidedVoxel->m_fReflection;

								d3dxvRevision.z = ((pCollidedVoxel->GetPosition().z - d3dxvSearchDir[i].z * m_pVoxelTerrain->m_fCubeDepth / 2.0f)
									- (pPlayer->GetPosition().z + d3dxvSearchDir[i].z * pPlayer->m_pMesh->m_AABB.m_d3dxvMax.z));
								//d3dxvPostV.z = 0.0f;
							}
							else
							{
								if (bIsSlopeVoxel && 0.98 < D3DXVec3Dot(&D3DXVECTOR3(-d3dxvSearchDir[i].x, 0.0f, 0.0f), &d3dxvSlopeRearNormal)) continue;
								pPlayer->m_d3dxvVelocity = pPlayer->m_d3dxvVelocity - 2.0f * (D3DXVec3Dot(&pPlayer->m_d3dxvVelocity, &D3DXVECTOR3(-d3dxvSearchDir[i].x, 0.0f, 0.0f)) * D3DXVECTOR3(-d3dxvSearchDir[i].x, 0.0f, 0.0f));
								pPlayer->m_d3dxvVelocity.x *= pCollidedVoxel->m_fReflection;
								//d3dxvPostV.y = m_d3dxvVelocity.y;
								//d3dxvPostV.y *= pCollidedVoxel->m_fReflection;

								d3dxvRevision.x = ((pCollidedVoxel->GetPosition().x + d3dxvSearchDir[i].x * m_pVoxelTerrain->m_fCubeWidth / 2.0f)
									- (pPlayer->GetPosition().x - d3dxvSearchDir[i].x * pPlayer->m_pMesh->m_AABB.m_d3dxvMax.x));
								//d3dxvPostV.x = 0.0f;
							}
							/*Moving(d3dxvRevision);
							m_CameraOperator.MoveCameraOperator(d3dxvRevision);*/
						}
						else
						{
							if (i < 4)         // 하
							{
								/*printf("pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxRearNormal (%f, %f, %f) \n",
								pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxRearNormal.x,
								pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxRearNormal.y,
								pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxRearNormal.z);*/
								if (bIsSlopeVoxel && 0.98 < D3DXVec3Dot(&D3DXVECTOR3(0.0f, 0.0f, d3dxvSearchDir[i].z), &d3dxvSlopeRearNormal)) continue;
								if (0.98f < D3DXVec3Dot(&D3DXVECTOR3(0.0f, 0.0f, -d3dxvSearchDir[i].z), &pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxRearNormal))
								{
									pPlayer->m_d3dxvVelocity = pPlayer->m_d3dxvVelocity - 2.0f * (D3DXVec3Dot(&pPlayer->m_d3dxvVelocity, &D3DXVECTOR3(0.0f, 0.0f, -d3dxvSearchDir[i].z)) * D3DXVECTOR3(0.0f, 0.0f, -d3dxvSearchDir[i].z));
									pPlayer->m_d3dxvVelocity.z *= pCollidedVoxel->m_fReflection;
									//d3dxvPostV.y = m_d3dxvVelocity.y;
									//d3dxvPostV.y *= pCollidedVoxel->m_fReflection;

									d3dxvRevision.z = ((pCollidedVoxel->GetPosition().z - d3dxvSearchDir[i].z * m_pVoxelTerrain->m_fCubeDepth / 2.0f)
										- (pPlayer->GetPosition().z + d3dxvSearchDir[i].z * pPlayer->m_pMesh->m_AABB.m_d3dxvMax.z));
									//d3dxvPostV.z = 0.0f;
								}
							}
							else                // 좌
							{
								if (bIsSlopeVoxel && 0.98 < D3DXVec3Dot(&D3DXVECTOR3(-d3dxvSearchDir[i].x, 0.0f, 0.0f), &d3dxvSlopeRearNormal)) continue;
								if (0.98f < D3DXVec3Dot(&D3DXVECTOR3(d3dxvSearchDir[i].x, 0.0f, 0.0f), &pCollidedVoxel->m_pMesh->m_CubeFaceNormal.m_d3dxRearNormal))
								{
									pPlayer->m_d3dxvVelocity = pPlayer->m_d3dxvVelocity - 2.0f * (D3DXVec3Dot(&pPlayer->m_d3dxvVelocity, &D3DXVECTOR3(-d3dxvSearchDir[i].x, 0.0f, 0.0f)) * D3DXVECTOR3(-d3dxvSearchDir[i].x, 0.0f, 0.0f));
									pPlayer->m_d3dxvVelocity.x *= pCollidedVoxel->m_fReflection;
									//d3dxvPostV.y = m_d3dxvVelocity.y;
									//d3dxvPostV.y *= pCollidedVoxel->m_fReflection;

									d3dxvRevision.x = ((pCollidedVoxel->GetPosition().x + d3dxvSearchDir[i].x * m_pVoxelTerrain->m_fCubeWidth / 2.0f)
										- (pPlayer->GetPosition().x - d3dxvSearchDir[i].x * pPlayer->m_pMesh->m_AABB.m_d3dxvMax.x));
								}
							}
						}
						pPlayer->Moving(d3dxvRevision);
						pPlayer->m_CameraOperator.MoveCameraOperator(d3dxvRevision);
					}
					else if (!bIsFloor && i < 14)      // 평범한 복셀에 한해서
					{
						if (pPlayer->m_d3dxvVelocity.y > 0.0f)
						{
							i = 13;
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
						i = 13;
						//break;
					}
					else if (!bIsFloor && i < 23)
					{
						if (pPlayer->m_d3dxvVelocity.y < 0.0f)
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

										printf("d3dxvFrontNormalXZ.x y : %f %f\n ", d3dxvFrontNormalXZ.x, d3dxvFrontNormalXZ.y);
										printf("d3dxvRevision : (%f, %f, %f)\n", d3dxvRevision.x, d3dxvRevision.y, d3dxvRevision.z);
										pPlayer->Moving(d3dxvRevision);
										pPlayer->m_CameraOperator.MoveCameraOperator(d3dxvRevision);
										//if (m_d3dxvMoveDir.y != 0.0f) m_d3dxvVelocity.y = 4.95f;

										bIsSlopeVoxel = true;
									}
								}
							}
								bIsJumped = false;
								//break;
								break;
						}
						else break;
					}

				}
			}
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
				if (pPlayer->GetPosition().y > m_pShips[i]->GetPosition().y + m_pShips[i]->m_pMesh->m_AABB.m_d3dxvMax.y)
				{

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
				}

			}
		}
	}
	//printf("Player Bottom : %f \n", GetPosition().y + m_pMesh->m_AABB.m_d3dxvMin.y);
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
	if (d3dxvPostV.y > PLAYER_MAX_VELOCITY_MAGNITUDE)
	{
		d3dxvPostV.y = PLAYER_MAX_VELOCITY_MAGNITUDE;
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
		d3dxvPostV.y = pPlayer->m_d3dxvVelocity.y = 4.95;//4.95f;// +d3dxvPostV.y;
	}

	// 최대 속도 제한
	if (D3DXVec2Length(&d3dxvVelocityXZ) > PLAYER_MAX_VELOCITY_MAGNITUDE)
	{
		D3DXVec2Normalize(&d3dxvVelocityXZ, &d3dxvVelocityXZ);
		d3dxvVelocityXZ *= PLAYER_MAX_VELOCITY_MAGNITUDE;
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

	// 이동 방향 초기화
	pPlayer->m_d3dxvMoveDir = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	//d3dxvVelocityXZ = D3DXVECTOR2(m_d3dxvVelocity.x, m_d3dxvVelocity.z);
	//printf("Player Velocity : %f\n", D3DXVec2Length(&d3dxvVelocityXZ));
	//printf("Velocity : (%.5f, %.5f, %.5f) \n", m_d3dxvVelocity.x, m_d3dxvVelocity.y, m_d3dxvVelocity.z);
	//printf("-----------------------------\n");
}

void CScene::AnimateObjects(ID3D11DeviceContext*pd3dDeviceContext, float fTimeElapsed)
{
	if(m_pWaveEffect) m_pWaveEffect->m_pMesh->Animate(pd3dDeviceContext, fTimeElapsed);						// 클라 영역

	for (int i = 0; i < m_pPlayersMgrInform->m_iPlayersNum; i++)
	{
		if (m_pPlayersMgrInform->m_ppPlayers[i]->m_bIsActive)
		{
			m_pPlayersMgrInform->m_ppPlayers[i]->RotateMoveDir(fTimeElapsed);

			if (CPhysicalCollision::IsCollided(
				&CPhysicalCollision::MoveAABB(m_pPlayersMgrInform->m_ppPlayers[i]->m_pMesh->m_AABB, m_pPlayersMgrInform->m_ppPlayers[i]->GetPosition()),
				&CPhysicalCollision::MoveAABB(m_pTreasureChest->m_pMesh->m_AABB, m_pTreasureChest->GetPosition())))
			{
				if (m_pPlayersMgrInform->m_ppPlayers[i]->m_d3dxvVelocity.y <= 0.0f && m_pPlayersMgrInform->m_ppPlayers[i]->GetPosition().y > m_pTreasureChest->GetPosition().y + m_pTreasureChest->m_pMesh->m_AABB.m_d3dxvMax.y)
				{
					//if (pPlayer->m_d3dxvVelocity.y > 0.0f) 
					D3DXVECTOR3 d3dxvRevision = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
					d3dxvRevision.y = m_pTreasureChest->GetPosition().y + m_pTreasureChest->m_pMesh->m_AABB.m_d3dxvMax.y - (m_pPlayersMgrInform->m_ppPlayers[i]->GetPosition().y + m_pPlayersMgrInform->m_ppPlayers[i]->m_pMesh->m_AABB.m_d3dxvMin.y);
					m_pPlayersMgrInform->m_ppPlayers[i]->Moving(d3dxvRevision);
					m_pPlayersMgrInform->m_ppPlayers[i]->m_CameraOperator.MoveCameraOperator(d3dxvRevision);
					m_pPlayersMgrInform->m_ppPlayers[i]->m_d3dxvVelocity.y = 0.0;
					if (m_pPlayersMgrInform->m_ppPlayers[i]->m_d3dxvMoveDir.y != 0.0f) m_pPlayersMgrInform->m_ppPlayers[i]->m_d3dxvVelocity.y = 4.95;
				}
				else CPhysicalCollision::ImpurseBasedCollisionResolution(m_pPlayersMgrInform->m_ppPlayers[i], m_pTreasureChest);
			}

			// i 번째 플레이어를 기준으로 한 플레이어 간 충격량 기반 충돌 체크
			for (int j = i + 1 ; j < m_pPlayersMgrInform->m_iPlayersNum; j++)
			{
				if (m_pPlayersMgrInform->m_ppPlayers[j]->m_bIsActive)
				{
					CPhysicalCollision::ImpurseBasedCollisionResolution(m_pPlayersMgrInform->m_ppPlayers[i], m_pPlayersMgrInform->m_ppPlayers[j]);
				}
			}

			// 복셀 터레인 및 씬의 환경 변수에 기반한 충돌 체크 및 물리 움직임
			MoveObjectUnderPhysicalEnvironment(m_pPlayersMgrInform->m_ppPlayers[i], fTimeElapsed);

			if (m_pPlayersMgrInform->m_ppPlayers[i]->GetPosition().y < -1.0f)
			{
				printf("플레이어가 물에 빠져버렸습니다!");
				m_RespawnManager.RegisterRespawnManager(m_pPlayersMgrInform->m_ppPlayers[i], true);
			}
		}
		else continue;
	}
	//printf("Pos (%.2f, %.2f, %.2f)\n", m_pPlayersMgrInform->GetMyPlayer()->GetPosition().x, m_pPlayersMgrInform->GetMyPlayer()->GetPosition().y, m_pPlayersMgrInform->GetMyPlayer()->GetPosition().z);
	//MoveObjectUnderPhysicalEnvironment(m_pTreasureChest, fTimeElapsed);

	if (m_pTreasureChest->GetPosition().y < -1.0f)
	{
		printf("보물상자가 물에 빠져버렸습니다!");
		InitScene();
	}

	m_RespawnManager.UpdateRespawnManager(fTimeElapsed);

	for (int i = 0; i < 2; i++)
	{
		if (CPhysicalCollision::IsCollided(
			&CPhysicalCollision::MoveAABB(m_pShips[i]->m_pMesh->m_AABB, m_pShips[i]->GetPosition()),
			&CPhysicalCollision::MoveAABB(m_pTreasureChest->m_pMesh->m_AABB, m_pTreasureChest->GetPosition())))
		{
			if (m_pTreasureChest->GetPosition().y > m_pShips[i]->GetPosition().y + m_pShips[i]->m_pMesh->m_AABB.m_d3dxvMax.y)
			{
				//printf("%d 팀이 승리하였습니다.\n", i);
			}
		}
	}
}

void CScene::Render(ID3D11DeviceContext*pd3dDeviceContext, CCamera *pCamera)
{
	if (m_pLights && m_pd3dcbLights) UpdateShaderVariable(pd3dDeviceContext, m_pLights);
	
	/*for (int i = 0; i < m_nObjects; i++)
	{
		m_ppObjects[i]->Render(pd3dDeviceContext);
	}*/
	for (int i = 0; i < 2; i++)
	{
		m_pShips[i]->Render(pd3dDeviceContext);
	}
	m_pTreasureChest->Render(pd3dDeviceContext);

	m_pVoxelTerrain->Render(pd3dDeviceContext, pCamera);
	if (m_pWaveEffect) m_pWaveEffect->Render(pd3dDeviceContext);
}
