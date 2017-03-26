// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.

// TODO : 프로그램에 필요한 추가 #define문은 여기에서 참조합니다.
#define FRAME_BUFFER_WIDTH	1280
#define FRAME_BUFFER_HEIGHT	720
#define VS_SLOT_CAMERA				0x00
#define VS_SLOT_WORLD_MATRIX			0x01
#define PS_SLOT_COLOR	0x00
//조명과 재질을 설정하기 위한 상수 버퍼의 슬롯 번호를 정의한다. 
#define PS_SLOT_LIGHT			0x00
#define PS_SLOT_MATERIAL		0x01


#define PS_SLOT_TEXTURE		0x00				// 텍스쳐 이미지가 저장될 PS 입력 슬롯 번호
#define PS_SLOT_SAMPLER_STATE		0x00		// 해당 텍스쳐 이미지에서 표본을 추출할 표본 추출기이다.


// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include <d3d11.h>
#include <d3dx11.h>
#include <mmsystem.h>		//시간과 관련된 멀티미디어 라이브러리 함수를 사용하기 위하여 다음 헤더 파일을 추가한다.
#include <math.h>			//수학 라이브러리 함수를 사용하기 위하여 다음 헤더 파일을 추가한다.
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <d3dcompiler.h>  	//쉐이더 컴파일 함수를 사용하기 위한 헤더 파일
#include <D3DX10Math.h>	//Direct3D 수학 함수를 사용하기 위한 헤더 파일

