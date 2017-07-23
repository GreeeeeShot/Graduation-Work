// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �Ǵ� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

// TODO : ���α׷��� �ʿ��� �߰� #define���� ���⿡�� �����մϴ�.
#define FRAME_BUFFER_WIDTH	1280//1920
#define FRAME_BUFFER_HEIGHT	720//1080
#define VS_SLOT_CAMERA				0x00
#define VS_SLOT_LIGHT_EMIT			0x00			// ��������� pass0���� ���
#define VS_SLOT_WORLD_MATRIX			0x01
#define VS_SLOT_SHADOW_MATRIX	0x02

#define PS_SLOT_COLOR	0x00
//����� ������ �����ϱ� ���� ��� ������ ���� ��ȣ�� �����Ѵ�. 
#define PS_SLOT_LIGHT			0x00
#define PS_SLOT_MATERIAL		0x01
#define PS_SLOT_FOG				0x02


#define PS_SLOT_TEXTURE		0x00				// �ؽ��� �̹����� ����� PS �Է� ���� ��ȣ
#define PS_SLOT_SAMPLER_STATE		0x00		// �ش� �ؽ��� �̹������� ǥ���� ������ ǥ�� ������̴�.
#define PS_SLOT_SHADOW_TEXTURE 0x01
#define PS_SLOT_LIGHT_TEXTURE		0x00
#define PS_SLOT_LIGHT_SHAFT_TEXTURE 0x00
#define PS_SLOT_PCF_SAMPLER_STATE		0x01		// �ش� �ؽ��� �̹������� ǥ���� ������ ǥ�� ������̴�.

// Windows ��� ����:
#include <windows.h>

// C ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx9.h>
#include <mmsystem.h>		//�ð��� ���õ� ��Ƽ�̵�� ���̺귯�� �Լ��� ����ϱ� ���Ͽ� ���� ��� ������ �߰��Ѵ�.
#include <math.h>			//���� ���̺귯�� �Լ��� ����ϱ� ���Ͽ� ���� ��� ������ �߰��Ѵ�.
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <d3dcompiler.h>  	//���̴� ������ �Լ��� ����ϱ� ���� ��� ����
#include <D3DX10Math.h>	//Direct3D ���� �Լ��� ����ϱ� ���� ��� ����
#include <vector>
#include <time.h>
#include "resource.h"
#include "GameManager.h"