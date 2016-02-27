#include "stdafx.h"
#include "Player.h"


CPlayer::CPlayer()
{
	m_d3dxvPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_d3dxvRight = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_d3dxvUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_d3dxvLook = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	m_d3dxvVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_d3dxvGravity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fMaxVelocityXZ = 0.0f;
	m_fMaxVelocityY = 0.0f;
	m_fFriction = 0.0f;

	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;

	m_pPlayerUpdatedContext = NULL;
	m_pCameraUpdatedContext = NULL;

	m_pShader = NULL;
}

CPlayer::~CPlayer()
{
}

void CPlayer::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
}

void CPlayer::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext)
{
}

/*플레이어의 위치와 회전축으로부터 월드 변환 행렬을 생성하는 함수이다. 플레이어의 Right 벡터가 월드 변환 행렬의 첫 번째 행 벡터, Up 벡터가 두 번째 행 벡터, Look 벡터가 세 번째 행 벡터, 플레이어의 위치 벡터가 네 번째 행 벡터가 된다.*/
void CPlayer::RegenerateWorldMatrix()
{
	GetWorldMatrix()->_11 = m_d3dxvRight.x;
	GetWorldMatrix()->_12 = m_d3dxvRight.y;
	GetWorldMatrix()->_13 = m_d3dxvRight.z;
	GetWorldMatrix()->_21 = m_d3dxvUp.x;
	GetWorldMatrix()->_22 = m_d3dxvUp.y;
	GetWorldMatrix()->_23 = m_d3dxvUp.z;
	GetWorldMatrix()->_31 = m_d3dxvLook.x;
	GetWorldMatrix()->_32 = m_d3dxvLook.y;
	GetWorldMatrix()->_33 = m_d3dxvLook.z;
	GetWorldMatrix()->_41 = m_d3dxvPosition.x;
	GetWorldMatrix()->_42 = m_d3dxvPosition.y;
	GetWorldMatrix()->_43 = m_d3dxvPosition.z;
}

/*플레이어의 위치를 변경하는 함수이다. 플레이어의 위치는 기본적으로 사용자가 플레이어를 이동하기 위한 키보드를 누를 때 변경된다. 플레이어의 이동 방향(dwDirection)에 따라 플레이어를 fDistance 만큼 이동한다.*/
void CPlayer::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity)
{
	if (dwDirection)
	{
		D3DXVECTOR3 d3dxvShift = D3DXVECTOR3(0, 0, 0);
		//화살표 키 ‘↑’를 누르면 로컬 z-축 방향으로 이동(전진)한다. ‘↓’를 누르면 반대 방향으로 이동한다.
		if (dwDirection & DIR_FORWARD) d3dxvShift += m_d3dxvLook * fDistance;
		if (dwDirection & DIR_BACKWARD) d3dxvShift -= m_d3dxvLook * fDistance;
		//화살표 키 ‘→’를 누르면 로컬 x-축 방향으로 이동한다. ‘←’를 누르면 반대 방향으로 이동한다.
		if (dwDirection & DIR_RIGHT) d3dxvShift += m_d3dxvRight * fDistance;
		if (dwDirection & DIR_LEFT) d3dxvShift -= m_d3dxvRight * fDistance;
		//‘Page Up’을 누르면 로컬 y-축 방향으로 이동한다. ‘Page Down’을 누르면 반대 방향으로 이동한다.
		if (dwDirection & DIR_UP) d3dxvShift += m_d3dxvUp * fDistance;
		if (dwDirection & DIR_DOWN) d3dxvShift -= m_d3dxvUp * fDistance;

		//플레이어를 현재 위치 벡터에서 d3dxvShift 벡터 만큼 이동한다.
		Move(d3dxvShift, bUpdateVelocity);
	}
}

void CPlayer::Move(const D3DXVECTOR3& d3dxvShift, bool bUpdateVelocity)
{
	//bUpdateVelocity가 참이면 플레이어를 이동하지 않고 속도 벡터를 변경한다.
	if (bUpdateVelocity)
	{
		m_d3dxvVelocity += d3dxvShift;
	}
	else
	{
		//플레이어를 현재 위치 벡터에서 d3dxvShift 벡터 만큼 이동한다.
		D3DXVECTOR3 d3dxvPosition = m_d3dxvPosition + d3dxvShift;
		m_d3dxvPosition = d3dxvPosition;
		RegenerateWorldMatrix();
	}
}

//플레이어를 로컬 x-축, y-축, z-축을 중심으로 회전한다.
void CPlayer::Rotate(float x, float y, float z)
{
	D3DXMATRIX mtxRotate;

	// 회전으로 인해 플레이어의 로컬 x-축, y-축, z-축이 서로 직교하지 않을 수 있으므로
	// z-축(LookAt 벡터)을 기준으로 하여 서로 직교하고 단위벡터가 되도록 한다.
	D3DXVec3Normalize(&m_d3dxvLook, &m_d3dxvLook);
	D3DXVec3Cross(&m_d3dxvRight, &m_d3dxvUp, &m_d3dxvLook);
	D3DXVec3Normalize(&m_d3dxvRight, &m_d3dxvRight);
	D3DXVec3Cross(&m_d3dxvUp, &m_d3dxvLook, &m_d3dxvRight);
	D3DXVec3Normalize(&m_d3dxvUp, &m_d3dxvUp);

	RegenerateWorldMatrix();
}


void CPlayer::Update(float fTimeElapsed)
{
	// 플레이어의 속도 벡터를 중력 벡터와 더한다.
	// 중력 벡터에 fTimeElapsed를 곱하는 것은 중력을 시간에 비례하도록 적용한다는 의미이다.
	m_d3dxvVelocity += m_d3dxvGravity * fTimeElapsed;

	// 플레이어의 속도 벡터의 XZ-성분의 크기를 구한다.
	// 이것이 XZ-평면의 최대 속력보다 크면 속도 벡터의 x와 z-방향 성분을 조정한다.
	float fLength = sqrtf(m_d3dxvVelocity.x * m_d3dxvVelocity.x + m_d3dxvVelocity.z * m_d3dxvVelocity.z);
	if (fLength > m_fMaxVelocityXZ)
	{
		m_d3dxvVelocity.x *= (m_fMaxVelocityXZ / fLength);
		m_d3dxvVelocity.z *= (m_fMaxVelocityXZ / fLength);
	}

	// 플레이어의 속도 벡터의 Y-성분의 크기를 구한다.
	// 이것이 Y 축 방향의 최대 속력보다 크면 속도 벡터의 y-방향 성분을 조정한다.
	fLength = sqrtf(m_d3dxvVelocity.y * m_d3dxvVelocity.y);
	if (fLength > m_fMaxVelocityY) m_d3dxvVelocity.y *= (m_fMaxVelocityY / fLength);

	/*플레이어를 속도 벡터 만큼 이동한다. 속도 벡터에 fTimeElapsed를 곱하는 것은 속도를 시간에 비례하도록 적용한다는 의미이다.*/
	Move(m_d3dxvVelocity * fTimeElapsed, false);

	// 플레이어의 위치가 변경될 때 추가로 수행할 작업을 수행한다.
	// 예를 들어, 플레이어의 위치가 변경되었지만 플레이어 객체에는 지형(Terrain)의 정보가 없다.
	// 플레이어의 새로운 위치가 유효한 위치가 아닐 수도 있고 또는 플레이어의 충돌 검사 등을 수행할 필요가 있다.
	// 이러한 상황에서 플레이어의 위치를 유효한 위치로 다시 변경할 수 있다.
	if (m_pPlayerUpdatedContext) OnPlayerUpdated(fTimeElapsed);

	// 플레이어의 속도 벡터가 마찰력 때문에 감속이 되어야 한다면 감속 벡터를 생성한다.
	// 속도 벡터의 반대 방향 벡터를 구하고 단위 벡터로 만든다. 마찰 계수를 시간에 비례하도록 하여 마찰력을 구한다.
	// 단위 벡터에 마찰력을 곱하여 감속 벡터를 구한다. 속도 벡터에 감속 벡터를 더하여 속도 벡터를 줄인다.
	// 마찰력이 속력보다 크면 속력은 0이 될 것이다.
	D3DXVECTOR3 d3dxvDeceleration = -m_d3dxvVelocity;
	D3DXVec3Normalize(&d3dxvDeceleration, &d3dxvDeceleration);
	fLength = D3DXVec3Length(&m_d3dxvVelocity);
	float fDeceleration = (m_fFriction * fTimeElapsed);
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_d3dxvVelocity += d3dxvDeceleration * fDeceleration;
}

void CPlayer::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	if (m_pShader)
	{
		m_pShader->UpdateShaderVariables(pd3dDeviceContext, GetWorldMatrix());
		m_pShader->Render(pd3dDeviceContext);
	}
	if (GetMesh()) GetMesh()->Render(pd3dDeviceContext);
}

void CPlayer::OnPlayerUpdated(float fTimeElapsed)
{
}

CAirplanePlayer::CAirplanePlayer(ID3D11Device *pd3dDevice)
{
	//비행기 메쉬를 생성한다.
	CMesh *pAirplaneMesh = new CAirplaneMesh(pd3dDevice, 20.0f, 20.0f, 4.0f, D3DCOLOR_XRGB(0, 255, 0));
	SetMesh(pAirplaneMesh);

	//플레이어(비행기) 메쉬를 렌더링할 때 사용할 쉐이더를 생성한다.
	m_pShader = new CPlayerShader();
	m_pShader->CreateShader(pd3dDevice);
	m_pShader->CreateShaderVariables(pd3dDevice);

	//플레이어를 위한 쉐이더 변수를 생성한다.
	CreateShaderVariables(pd3dDevice);
}

CAirplanePlayer::~CAirplanePlayer()
{
	if (m_pShader) delete m_pShader;
}

void CAirplanePlayer::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	D3DXMATRIX mtxRotate;
	// 3인칭 카메라일 때 플레이어 메쉬를 로컬 x-축을 중심으로 +90도 회전하고 렌더링한다.
	D3DXMatrixRotationYawPitchRoll(&mtxRotate, 0.0f, (float)D3DXToRadian(90.0f), 0.0f);
	*(GetWorldMatrix()) = mtxRotate * *(GetWorldMatrix());

	CPlayer::Render(pd3dDeviceContext);
}

