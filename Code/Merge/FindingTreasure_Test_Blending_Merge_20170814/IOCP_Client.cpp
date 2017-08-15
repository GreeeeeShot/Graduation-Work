#include "stdafx.h"
#include "IOCP_Init.h"
#include "IOCP_Client.h"
#include "GameManager.h"
#include "SoundManager.h"
#include "FindingTreasure_Test_Blending.h"



SOCKET g_mysocket;
WSABUF	send_wsabuf;
char 	send_buffer[BUF_SIZE];
WSABUF	recv_wsabuf;
char	recv_buffer[BUF_SIZE];
char	packet_buffer[BUF_SIZE];
DWORD		in_packet_size = 0;
int		saved_packet_size = 0;
int		g_myid;

int		g_left_x = 0;
int     g_top_y = 0;

int my_team;
int my_id;
int user_num=0;

State state;

Client_Data player;

void clienterror()
{
	exit(-1);
}

void ProcessPacket(char *ptr)
{
	static bool first_time = true;
	int x = 0, cameraY = 0, z = 0, id = -1;
	float px = 0.0, py = 0.0, pz = 0.0;
	switch (ptr[1])
	{
	case SC_PUT_PLAYER:
	{
		sc_packet_put_player *my_packet = reinterpret_cast<sc_packet_put_player *>(ptr);
		//CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_iPlayersNum++;

		for (int i = 0; i < 7; ++i)
		{
			if (!waitingplayer[i].connect)
			{
				id = i;
				break;
			}
		}

		waitingplayer[id].connect = true;

		waitingplayer[id].ready = false;
		waitingplayer[id].team = (BELONG_TYPE)my_packet->team;
		waitingplayer[id].id = my_packet->id;
		user_num++;
		break;
	}
	case SC_INIT_TEAM:
	{
		sc_packet_initteam *my_packet = reinterpret_cast<sc_packet_initteam *>(ptr);
		my_team = my_packet->team;
		my_id = my_packet->id;
		break;
	}
	case SC_POS:
	{
		sc_packet_pos *my_packet = reinterpret_cast<sc_packet_pos *>(ptr);
		//CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[my_packet->id]->SetPosition(D3DXVECTOR3(my_packet->x, my_packet->y, my_packet->z));
		x = my_packet->MoveX;
		z = my_packet->MoveZ;
		cameraY = my_packet->CameraY;
		id = my_packet->id;
		//CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->SetMove(x, y, z);
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_MoveX = x;
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_MoveZ = z;
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraY = cameraY;
		break;
	}
	case SC_JUMP:
	{
		sc_packet_jump *my_packet = reinterpret_cast<sc_packet_jump *>(ptr);
		id = my_packet->id;
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_d3dxvMoveDir.y = 5.0f;
		break;
	}
	case SC_SETPOS:
	{
		sc_packet_put_player *my_packet = reinterpret_cast<sc_packet_put_player *>(ptr);
		//CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[my_packet->id]->SetPosition(D3DXVECTOR3(my_packet->x, my_packet->y, my_packet->z));
		break;
	}
	case SC_REMOVE_PLAYER:
	{
		sc_packet_remove_player *my_packet = reinterpret_cast<sc_packet_remove_player *>(ptr);
		break;
	}
	case SC_INIT:
	{
		sc_packet_init *my_packet = reinterpret_cast<sc_packet_init *>(ptr);
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_iMyPlayerID = my_packet->id;
		id = my_packet->id;

		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->SetPosition(D3DXVECTOR3(my_packet->Posx, my_packet->Posy, my_packet->Posz));
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.m_Camera.SetPosition(D3DXVECTOR3(my_packet->Lookx, my_packet->Looky, my_packet->Lookz));
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.InitCameraOperator(CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]);

		break;
	}
	case SC_BOX:
	{
		sc_packet_box *my_packet = reinterpret_cast<sc_packet_box *>(ptr);
		CGameManager::GetInstance()->m_pGameFramework->m_pScene->m_pTreasureChest->m_SyncPosition = D3DXVECTOR3(my_packet->Posx, my_packet->Posy, my_packet->Posz);
		if (my_packet->Posy - CGameManager::GetInstance()->m_pGameFramework->m_pScene->m_pTreasureChest->GetPosition().y > 2.0f || my_packet->Posy - CGameManager::GetInstance()->m_pGameFramework->m_pScene->m_pTreasureChest->GetPosition().y < -2.0f)
		{
			CGameManager::GetInstance()->m_pGameFramework->m_pScene->m_pTreasureChest->SetPosition(D3DXVECTOR3(my_packet->Posx, my_packet->Posy, my_packet->Posz));
			printf("순간이동!");
		}
		else if (my_packet->Posx - CGameManager::GetInstance()->m_pGameFramework->m_pScene->m_pTreasureChest->GetPosition().x > 5.0f || my_packet->Posx - CGameManager::GetInstance()->m_pGameFramework->m_pScene->m_pTreasureChest->GetPosition().x < -5.0f)
		{
			CGameManager::GetInstance()->m_pGameFramework->m_pScene->m_pTreasureChest->SetPosition(D3DXVECTOR3(my_packet->Posx, my_packet->Posy, my_packet->Posz));
			printf("순간이동!");
		}
		else if (my_packet->Posz - CGameManager::GetInstance()->m_pGameFramework->m_pScene->m_pTreasureChest->GetPosition().z > 5.0f || my_packet->Posz- CGameManager::GetInstance()->m_pGameFramework->m_pScene->m_pTreasureChest->GetPosition().z < -5.0f)
		{
			CGameManager::GetInstance()->m_pGameFramework->m_pScene->m_pTreasureChest->SetPosition(D3DXVECTOR3(my_packet->Posx, my_packet->Posy, my_packet->Posz));
			printf("순간이동!");
		}
		break;
	}
	case SC_SYNC:
	{
		sc_packet_sync *my_packet = reinterpret_cast<sc_packet_sync *>(ptr);
		id = my_packet->id;
		D3DXVECTOR3 pos = CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->GetPosition();
		if (CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_bIsDigOrInstall)
			break;
		if (pos.y < -1)
		{
			CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_SyncPosition = D3DXVECTOR3(my_packet->Posx, my_packet->Posy, my_packet->Posz);
			CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->SetPosition(D3DXVECTOR3(my_packet->Posx, my_packet->Posy, my_packet->Posz));
			CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_d3dxvVelocity = D3DXVECTOR3(my_packet->Velx, my_packet->Vely, my_packet->Velz);
			
			CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetPosition(D3DXVECTOR3(my_packet->Lookx, my_packet->Looky, my_packet->Lookz));
			CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetOperatorPosition(D3DXVECTOR3(my_packet->Operx, my_packet->Opery, my_packet->Operz));
			CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetDir(D3DXVECTOR3(my_packet->Dirx, my_packet->Diry, my_packet->Dirz));
			CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetDistance(my_packet->distance);
			
			CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetLook(D3DXVECTOR3(my_packet->oLookx, my_packet->oLooky, my_packet->oLookz));
			CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetRight(D3DXVECTOR3(my_packet->Rightx, my_packet->Righty, my_packet->Rightz));
			
			CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.InitCameraOperator(CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]);
		}
		else if ((pos.y-my_packet->Posy < -1.0f || pos.y - my_packet->Posy > 1.0f) || (pos.z - my_packet->Posz < -1.9f || pos.z - my_packet->Posz > 1.9f) || (pos.x - my_packet->Posx < -1.9f || pos.x - my_packet->Posx > 1.9f))
		{
			CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->SetPosition(D3DXVECTOR3(my_packet->Posx, my_packet->Posy, my_packet->Posz));
			CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_SyncPosition = D3DXVECTOR3(my_packet->Posx, my_packet->Posy, my_packet->Posz);
			CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_d3dxvVelocity = D3DXVECTOR3(my_packet->Velx, my_packet->Vely, my_packet->Velz);

			
			CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetPosition(D3DXVECTOR3(my_packet->Lookx, my_packet->Looky, my_packet->Lookz));
			CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetOperatorPosition(D3DXVECTOR3(my_packet->Operx, my_packet->Opery, my_packet->Operz));
			CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetDir(D3DXVECTOR3(my_packet->Dirx, my_packet->Diry, my_packet->Dirz));
			CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetDistance(my_packet->distance);
			CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetLook(D3DXVECTOR3(my_packet->oLookx, my_packet->oLooky, my_packet->oLookz));
			CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetRight(D3DXVECTOR3(my_packet->Rightx, my_packet->Righty, my_packet->Rightz));
			
			CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.m_pastYpos = my_packet->Looky;
		}
		else
		{
			float pasty = CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.m_pastYpos;
			float currenty = CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.GetPosition().y;
			float x = CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.GetPosition().x;
			float z = CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.GetPosition().z;
			
			CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_SyncPosition = D3DXVECTOR3(my_packet->Posx, my_packet->Posy, my_packet->Posz);
			CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_d3dxvVelocity = D3DXVECTOR3(my_packet->Velx, my_packet->Vely, my_packet->Velz);


			if(!(my_packet->Looky - pasty < 0.5 && my_packet->Looky - pasty > -0.5))
				CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetPosition(D3DXVECTOR3(my_packet->Lookx, my_packet->Looky, my_packet->Lookz));
			else if(!(my_packet->Lookx - x < 0.7 && my_packet->Lookx - x > -0.7) || !(my_packet->Lookz - z < 0.7 && my_packet->Lookz - z > -0.7))
				CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetPosition(D3DXVECTOR3(my_packet->Lookx, currenty, my_packet->Lookz));
			CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetOperatorPosition(D3DXVECTOR3(my_packet->Operx, my_packet->Opery, my_packet->Operz));
			CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetDir(D3DXVECTOR3(my_packet->Dirx, my_packet->Diry, my_packet->Dirz));
			CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetDistance(my_packet->distance);
			CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetLook(D3DXVECTOR3(my_packet->oLookx, my_packet->oLooky, my_packet->oLookz));
			CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetRight(D3DXVECTOR3(my_packet->Rightx, my_packet->Righty, my_packet->Rightz));
			
			CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.m_pastYpos = my_packet->Looky;
		}
		//CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetCameraVector(D3DXVECTOR3(my_packet->Posx, my_packet->Posy, my_packet->Posz));
		//CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetLook(D3DXVECTOR3(my_packet->Lookx, my_packet->Looky, my_packet->Lookz));
		//CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetCameraPos(D3DXVECTOR3(my_packet->Lookx, my_packet->Looky, my_packet->Lookz));
		
		break;
	}
	case SC_RESPAWN:
	{
		sc_packet_respawn *my_packet = reinterpret_cast<sc_packet_respawn *>(ptr);
		id = my_packet->id;
		//printf("id : %d", id);
		float y = CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->GetPosition().y;
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_SyncPosition = D3DXVECTOR3(my_packet->Posx, my_packet->Posy, my_packet->Posz);
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->SetPosition(D3DXVECTOR3(my_packet->Posx, my_packet->Posy, my_packet->Posz));
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.m_Camera.SetPosition(D3DXVECTOR3(my_packet->Lookx, my_packet->Looky, my_packet->Lookz));
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetOperatorPosition(D3DXVECTOR3(my_packet->Operx, my_packet->Opery, my_packet->Operz));
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetDir(D3DXVECTOR3(my_packet->Dirx, my_packet->Diry, my_packet->Dirz));
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetDistance(my_packet->distance);
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetLook(D3DXVECTOR3(my_packet->oLookx, my_packet->oLooky, my_packet->oLookz));
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetRight(D3DXVECTOR3(my_packet->Rightx, my_packet->Righty, my_packet->Rightz));

		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.InitCameraOperator(CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]);

		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_serverActive = true;
		break;
	}
	case SC_DEAD:
	{
		CSoundManager::FMODSoundPlayAndRepeat(SOUND_TYPE_EFFECT_SPLASH);
		sc_packet_sync *my_packet = reinterpret_cast<sc_packet_sync *>(ptr);
		id = my_packet->id;
		CGameManager::GetInstance()->m_pGameFramework->m_pScene->m_RespawnManager.RegisterRespawnManager(CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id], true);
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_serverActive = false;
		break;
	}
	case SC_INSVOX:
	{
		sc_packet_vox *my_packet = reinterpret_cast<sc_packet_vox *>(ptr);
		id = my_packet->id;
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_SyncPosition = D3DXVECTOR3(my_packet->Posx, my_packet->Posy, my_packet->Posz);
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->SetPosition(D3DXVECTOR3(my_packet->Posx, my_packet->Posy, my_packet->Posz));
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetPosition(D3DXVECTOR3(my_packet->Lookx, my_packet->Looky, my_packet->Lookz));
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetOperatorPosition(D3DXVECTOR3(my_packet->Operx, my_packet->Opery, my_packet->Operz));
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetDir(D3DXVECTOR3(my_packet->Dirx, my_packet->Diry, my_packet->Dirz));
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetDistance(my_packet->distance);
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetLook(D3DXVECTOR3(my_packet->oLookx, my_packet->oLooky, my_packet->oLookz));
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetRight(D3DXVECTOR3(my_packet->Rightx, my_packet->Righty, my_packet->Rightz));


		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_iVoxelPocketSlotIdx = my_packet->pocket;
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_Insvoxel = true;
		break;
	}
	case SC_DELVOX:
	{
		sc_packet_vox *my_packet = reinterpret_cast<sc_packet_vox *>(ptr);
		id = my_packet->id;
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_SyncPosition = D3DXVECTOR3(my_packet->Posx, my_packet->Posy, my_packet->Posz);
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->SetPosition(D3DXVECTOR3(my_packet->Posx, my_packet->Posy, my_packet->Posz));
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetPosition(D3DXVECTOR3(my_packet->Lookx, my_packet->Looky, my_packet->Lookz));
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetOperatorPosition(D3DXVECTOR3(my_packet->Operx, my_packet->Opery, my_packet->Operz));
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetDir(D3DXVECTOR3(my_packet->Dirx, my_packet->Diry, my_packet->Dirz));
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetDistance(my_packet->distance);
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetLook(D3DXVECTOR3(my_packet->oLookx, my_packet->oLooky, my_packet->oLookz));
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetRight(D3DXVECTOR3(my_packet->Rightx, my_packet->Righty, my_packet->Rightz));


		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_iVoxelPocketSlotIdx = my_packet->pocket;
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_Delvoxel = true;
		break;
	}
	case SC_CANCLEVOX:
	{
		sc_packet_voxcancle *my_packet = reinterpret_cast<sc_packet_voxcancle *>(ptr);
		id = my_packet->id;
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_Insvoxel = false;
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_Delvoxel = false;
		break;
	}
	case SC_LIFTBOX:
	{
		sc_packet_lift *my_packet = reinterpret_cast<sc_packet_lift *>(ptr);
		id = my_packet->id;
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_SyncPosition = D3DXVECTOR3(my_packet->Posx, my_packet->Posy, my_packet->Posz);
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->SetPosition(D3DXVECTOR3(my_packet->Posx, my_packet->Posy, my_packet->Posz));
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetPosition(D3DXVECTOR3(my_packet->Lookx, my_packet->Looky, my_packet->Lookz));
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetOperatorPosition(D3DXVECTOR3(my_packet->Operx, my_packet->Opery, my_packet->Operz));
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetDir(D3DXVECTOR3(my_packet->Dirx, my_packet->Diry, my_packet->Dirz));
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetDistance(my_packet->distance);
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetLook(D3DXVECTOR3(my_packet->oLookx, my_packet->oLooky, my_packet->oLookz));
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetRight(D3DXVECTOR3(my_packet->Rightx, my_packet->Righty, my_packet->Rightz));


		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_IsLift=true;
		break;
	}
	case SC_OUTBOX:
	{
		sc_packet_lift *my_packet = reinterpret_cast<sc_packet_lift *>(ptr);
		id = my_packet->id;
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_SyncPosition = D3DXVECTOR3(my_packet->Posx, my_packet->Posy, my_packet->Posz);
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->SetPosition(D3DXVECTOR3(my_packet->Posx, my_packet->Posy, my_packet->Posz));
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetPosition(D3DXVECTOR3(my_packet->Lookx, my_packet->Looky, my_packet->Lookz));
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetOperatorPosition(D3DXVECTOR3(my_packet->Operx, my_packet->Opery, my_packet->Operz));
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetDir(D3DXVECTOR3(my_packet->Dirx, my_packet->Diry, my_packet->Dirz));
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetDistance(my_packet->distance);
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetLook(D3DXVECTOR3(my_packet->oLookx, my_packet->oLooky, my_packet->oLookz));
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_CameraOperator.SetRight(D3DXVECTOR3(my_packet->Rightx, my_packet->Righty, my_packet->Rightz));


		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_IsLift = false;
		break;
	}
	case SC_MISSBOX:
	{
		sc_packet_missbox *my_packet = reinterpret_cast<sc_packet_missbox *>(ptr);
		id = my_packet->id;
		CGameManager::GetInstance()->m_pGameFramework->m_pScene->m_pTreasureChest->m_SyncPosition = D3DXVECTOR3(my_packet->Posx, my_packet->Posy, my_packet->Posz);
		CGameManager::GetInstance()->m_pGameFramework->m_pScene->m_pTreasureChest->SetPosition(D3DXVECTOR3(my_packet->Posx, my_packet->Posy, my_packet->Posz));
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_bIsPushed = false;
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->m_IsLift = false;
		CGameManager::GetInstance()->m_pGameFramework->m_pScene->m_pTreasureChest->BeRelievedFromLiftingPlayer();

		break;
	}
	case SC_TEAM_CHANGE:
	{
		sc_packet_waiting *my_packet = reinterpret_cast<sc_packet_waiting *>(ptr);
		id = my_packet->id;
		for (int i = 0; i < 7; ++i)
		{
			if (waitingplayer[i].id == id)
			{
				waitingplayer[i].team = (BELONG_TYPE)((int)(waitingplayer[i].team + 1) % 2);
				break;
			}
		}
		break;
	}
	case SC_CHARAC_CHANGE:
	{
		sc_packet_waiting *my_packet = reinterpret_cast<sc_packet_waiting *>(ptr);
		id = my_packet->id;
		for (int i = 0; i < 7; ++i)
		{
			if (waitingplayer[i].id == id)
			{
				waitingplayer[i].charac = (CHARACTER)((int)(waitingplayer[i].charac + 1) % 2);
				break;
			}
		}
		break;
	}
	case SC_READY:
	{
		sc_packet_waiting *my_packet = reinterpret_cast<sc_packet_waiting *>(ptr);
		id = my_packet->id;
		for (int i = 0; i < 7; ++i)
		{
			if (waitingplayer[i].id == id)
			{
				waitingplayer[i].ready = (waitingplayer[i].ready + 1) % 2;
				break;
			}
		}
		break;
	}
	case SC_EXIT:
	{
		sc_packet_waiting *my_packet = reinterpret_cast<sc_packet_waiting *>(ptr);
		id = my_packet->id;
		for (int i = 0; i < 7; ++i)
		{
			if (waitingplayer[i].id == id)
			{
				waitingplayer[i].connect = false;
				waitingplayer[i].team = BELONG_TYPE_INDIVIDUAL;
				user_num--;
				break;
			}
		}
		break;
	}
	case SC_GAME_START:
	{
		sc_packet_waiting *my_packet = reinterpret_cast<sc_packet_waiting *>(ptr);
		state = play;
		break;
	}
	case SC_GAMETIME:
	{
		sc_packet_gametime *my_packet = reinterpret_cast<sc_packet_gametime *>(ptr);
		break;
	}
	case SC_WIN:
	{
		sc_packet_result *my_packet = reinterpret_cast<sc_packet_result *>(ptr);
		break;
	}
	case SC_DRAW:
	{
		sc_packet_result *my_packet = reinterpret_cast<sc_packet_result *>(ptr);
		break;
	}
	case SC_DEFEAT:
	{
		sc_packet_result *my_packet = reinterpret_cast<sc_packet_result *>(ptr);
		break;
	}
	/*
	case SC_CHAT:
	{
	sc_packet_chat *my_packet = reinterpret_cast<sc_packet_chat *>(ptr);
	int other_id = my_packet->id;
	if (other_id == g_myid) {
	wcsncpy_s(player.message, my_packet->message, 256);
	player.message_time = GetTickCount();
	}
	else if (other_id < NPC_START) {
	wcsncpy_s(skelaton[other_id].message, my_packet->message, 256);
	skelaton[other_id].message_time = GetTickCount();
	}
	else {
	wcsncpy_s(npc[other_id - NPC_START].message, my_packet->message, 256);
	npc[other_id - NPC_START].message_time = GetTickCount();
	}
	break;

	}*/
	default:
		printf("Unknown PACKET type [%d]\n", ptr[1]);
		break;
	}
}

void ReadPacket(SOCKET sock)
{
	DWORD io_byte, io_flag = 0;

	int ret = WSARecv(sock, &recv_wsabuf, 1, &io_byte, &io_flag, NULL, NULL);
	if (ret) {
		int err_code = WSAGetLastError();
		printf("Recv Error [%d]\n", err_code);
	}

	BYTE *ptr = reinterpret_cast<BYTE *>(recv_buffer);

	while (0 != io_byte) {
		if (0 == in_packet_size) in_packet_size = ptr[0];
		if (io_byte + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			io_byte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, io_byte);
			saved_packet_size += io_byte;
			io_byte = 0;
		}
	}
}

void ClientMain(HWND main_window_handle, const char* serverip)
{
	WSADATA	wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	g_mysocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(MY_SERVER_PORT);
	ServerAddr.sin_addr.s_addr = inet_addr(serverip);

	int Result = WSAConnect(g_mysocket, (sockaddr *)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);

	WSAAsyncSelect(g_mysocket, main_window_handle, WM_SOCKET, FD_CLOSE | FD_READ);

	send_wsabuf.buf = send_buffer;
	send_wsabuf.len = BUF_SIZE;
	recv_wsabuf.buf = recv_buffer;
	recv_wsabuf.len = BUF_SIZE;
	//ReadPacket(g_mysocket);

	for (int i = 0; i < 7; ++i)
	{
		waitingplayer[i].connect = false;
		waitingplayer[i].team = BELONG_TYPE_INDIVIDUAL;
		waitingplayer[i].ready = false;
		waitingplayer[i].charac = pirate;
		waitingplayer[i].id = -1;
	}
}

void SetPacket(PACKETTYPE type, int Posx, int Posz, int Look)
{
	int ret = 0;
	int id = CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_iMyPlayerID;
	cs_packet_up *my_packet = reinterpret_cast<cs_packet_up *>(send_buffer);
	cs_packet_up *my_packet_jump = reinterpret_cast<cs_packet_up *>(send_buffer);
	cs_packet_camera *my_packet_camera = reinterpret_cast<cs_packet_camera *>(send_buffer);
	cs_packet_vox *my_packet_voxel = reinterpret_cast<cs_packet_vox *>(send_buffer);
	cs_packet_lift *my_packet_lift = reinterpret_cast<cs_packet_lift *>(send_buffer);
	DWORD iobyte;
	switch (type)
	{
	case POSMOVE:
		my_packet->size = sizeof(my_packet);
		send_wsabuf.len = sizeof(my_packet);
		my_packet->type = CS_POSMOVE;
		my_packet->Movex = Posx;
		my_packet->Movez = Posz;
		ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
		if (ret) {
			int error_code = WSAGetLastError();
			printf("Error while sending packet [%d]", error_code);
		}
		break;
	case JUMP:
		my_packet_jump->size = sizeof(my_packet_jump);
		send_wsabuf.len = sizeof(my_packet_jump);
		my_packet_jump->type = CS_JUMP;
		WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
		break;
	case CAMERAMOVE:
		my_packet_camera->size = sizeof(my_packet_camera);
		send_wsabuf.len = sizeof(my_packet_camera);

		my_packet_camera->type = CS_CAMERAMOVE;
		my_packet_camera->Look = Look;
		ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
		if (ret) {
			int error_code = WSAGetLastError();
			printf("Error while sending packet [%d]", error_code);
			break;
		}
		break;
	case VOXELDEL:
		my_packet_voxel->size = sizeof(my_packet_voxel);
		send_wsabuf.len = sizeof(my_packet_voxel);
		my_packet_voxel->type = CS_DELVOX;
		my_packet_voxel->pocket = CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->GetMyPlayer()->m_iVoxelPocketSlotIdx;
		WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
		if (ret) {
			int error_code = WSAGetLastError();
			printf("Error while sending packet [%d]", error_code);
			break;
		}
		break;
	case VOXELINS:
		my_packet_voxel->size = sizeof(my_packet_voxel);
		send_wsabuf.len = sizeof(my_packet_voxel);
		my_packet_voxel->type = CS_INSVOX;
		my_packet_voxel->pocket = CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->GetMyPlayer()->m_iVoxelPocketSlotIdx;
		WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
		if (ret) {
			int error_code = WSAGetLastError();
			printf("Error while sending packet [%d]", error_code);
			break;
		}
		break;
	case VOXELCANCLE:
		my_packet_voxel->size = sizeof(my_packet_voxel);
		send_wsabuf.len = sizeof(my_packet_voxel);
		my_packet_voxel->type = CS_CANCLEVOX;
		WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
		if (ret) {
			int error_code = WSAGetLastError();
			printf("Error while sending packet [%d]", error_code);
			break;
		}
		break;
	case LIFTBOX:
		my_packet_lift->size = sizeof(my_packet_lift);
		send_wsabuf.len = sizeof(my_packet_lift);
		my_packet_lift->type = CS_LIFTBOX;
		WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
		if (ret) {
			int error_code = WSAGetLastError();
			printf("Error while sending packet [%d]", error_code);
			break;
		}
		break;
	case OUTBOX:
		my_packet_lift->size = sizeof(my_packet_lift);
		send_wsabuf.len = sizeof(my_packet_lift);
		my_packet_lift->type = CS_OUTBOX;
		WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
		if (ret) {
			int error_code = WSAGetLastError();
			printf("Error while sending packet [%d]", error_code);
			break;
		}
		break;
	default:
		break;
	}
}

void WaitingPacket(PACKETTYPE type)
{
	int ret = 0;
	int id = CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_iMyPlayerID;
	cs_packet_waiting *my_packet = reinterpret_cast<cs_packet_waiting *>(send_buffer);
	DWORD iobyte;
	switch (type)
	{
	case TEAMCHANGE:
		my_packet->size = sizeof(my_packet);
		send_wsabuf.len = sizeof(my_packet);
		my_packet->type = CS_TEAM_CHANGE;
		WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
		if (ret) {
			int error_code = WSAGetLastError();
			printf("Error while sending packet [%d]", error_code);
			break;
		}
		break;
	case READY:
		my_packet->size = sizeof(my_packet);
		send_wsabuf.len = sizeof(my_packet);
		my_packet->type = CS_READY;
		WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
		if (ret) {
			int error_code = WSAGetLastError();
			printf("Error while sending packet [%d]", error_code);
			break;
		}
		break;
	case CHARACHANGE:
		my_packet->size = sizeof(my_packet);
		send_wsabuf.len = sizeof(my_packet);
		my_packet->type = CS_CHARAC_CHANGE;
		WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
		if (ret) {
			int error_code = WSAGetLastError();
			printf("Error while sending packet [%d]", error_code);
			break;
		}
		break;
	case EXIT:
		my_packet->size = sizeof(my_packet);
		send_wsabuf.len = sizeof(my_packet);
		my_packet->type = CS_EXIT;
		WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
		if (ret) {
			int error_code = WSAGetLastError();
			printf("Error while sending packet [%d]", error_code);
			break;
		}
		break;
	case GAMESET:
		my_packet->size = sizeof(my_packet);
		send_wsabuf.len = sizeof(my_packet);
		my_packet->type = CS_GAMEREADY;
		WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
		if (ret) {
			int error_code = WSAGetLastError();
			printf("Error while sending packet [%d]", error_code);
			break;
		}
	default:
		break;
	}
}