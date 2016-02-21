#pragma once

//카메라의 종류(모드: Mode)를 나타내는 상수를 다음과 같이 선언한다.
#define THIRD_PERSON_CAMERA	0x03

//프레임 버퍼의 크기와 종횡비(Aspect Ratio)를 나타내는 상수를 다음과 같이 선언한다.
#define FRAME_BUFFER_WIDTH	640
#define FRAME_BUFFER_HEIGHT	480
#define ASPECT_RATIO	(float(FRAME_BUFFER_WIDTH)/float(FRAME_BUFFER_HEIGHT))





struct VS_CB_VIEWPROJECTION_MATRIX
{
	D3DXMATRIX m_d3dxmtxView;
	D3DXMATRIX m_d3dxmtxProjection;
};



class CCamera
{
public:
	//CCamera 클래스의 기본 생성자를 다음과 같이 변경한다.
	CCamera();
	~CCamera();

	void SetMode(DWORD nMode) { m_nMode = nMode; }
	DWORD GetMode() { return(m_nMode); }

	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle);

	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext);

	void SetViewport(ID3D11DeviceContext *pd3dDeviceContext, DWORD xStart, DWORD yStart, DWORD nWidth, DWORD nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f);
	D3D11_VIEWPORT GetViewport() { return(m_d3dViewport); }

	D3DXMATRIX GetViewMatrix() { return(m_d3dxmtxView); }
	D3DXMATRIX GetProjectionMatrix() { return(m_d3dxmtxProjection); }
	ID3D11Buffer *GetCameraConstantBuffer() { return(m_pd3dcbCamera); }

	void SetPosition(D3DXVECTOR3 d3dxvPosition);
	D3DXVECTOR3& GetPosition();

	void SetLookAtPosition(D3DXVECTOR3 d3dxvLookAtWorld) { m_d3dxvLookAtWorld = d3dxvLookAtWorld; }
	D3DXVECTOR3& GetLookAtPosition() { return(m_d3dxvLookAtWorld); }

	D3DXVECTOR3& GetRightVector() { return(m_d3dxvRight); }
	D3DXVECTOR3& GetUpVector() { return(m_d3dxvUp); }
	D3DXVECTOR3& GetLookVector() { return(m_d3dxvLook); }

	float& GetPitch() { return(m_fPitch); }
	float& GetRoll() { return(m_fRoll); }
	float& GetYaw() { return(m_fYaw); }

	void SetOffset(D3DXVECTOR3 d3dxvOffset) { m_d3dxvOffset = d3dxvOffset; SetPosition(GetPosition() + d3dxvOffset); }
	D3DXVECTOR3& GetOffset() { return(m_d3dxvOffset); }

	void SetTimeLag(float fTimeLag) { m_fTimeLag = fTimeLag; }
	float GetTimeLag() { return(m_fTimeLag); }

	//카메라를 d3dxvShift 만큼 이동하는 가상함수이다.
	virtual void Move(const D3DXVECTOR3& d3dxvShift) { SetPosition(GetPosition() + d3dxvShift); }
	//카메라를 x-축, y-축, z-축으로 회전하는 가상함수이다.
	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f) { }
	//카메라의 이동, 회전에 따라 카메라의 정보를 갱신하는 가상함수이다.
	virtual void Update(const D3DXVECTOR3 *pd3dxvPosition, float fTimeElapsed) {}
	/*3인칭 카메라에서 카메라가 바라보는 지점을 설정하는 가상함수이다. 일반적으로 플레이어를 바라보도록 설정한다.*/
	virtual void SetLookAt(D3DXVECTOR3& vLookAt) { }

protected:
	////카메라의 위치(월드좌표계) 벡터이다.
	//D3DXVECTOR3 m_d3dxvPosition;
	// 카메라의 로컬 x - 축(Right), y - 축(Up), z - 축(Look)을 나타내는 벡터이다.
	D3DXVECTOR3 m_d3dxvRight;
	D3DXVECTOR3 m_d3dxvUp;
	D3DXVECTOR3 m_d3dxvLook;

	//카메라가 x-축, z-축, y-축으로 얼마만큼 회전했는 가를 나타내는 각도이다.
	float m_fPitch;
	float m_fRoll;
	float m_fYaw;

	//카메라의 종류(1인칭 카메라, 스페이스-쉽 카메라, 3인칭 카메라)를 나타낸다.
	DWORD m_nMode;

	//카메라가 바라보는 점(월드좌표계)을 나타내는 벡터이다.
	D3DXVECTOR3 m_d3dxvLookAtWorld;
	//플레이어와 카메라의 오프셋을 나타내는 벡터이다. 주로 3인칭 카메라에서 사용된다.
	D3DXVECTOR3 m_d3dxvOffset;
	//플레이어가 회전할 때 얼마만큼의 시간을 지연시킨 후 카메라를 회전시킬 것인가를 나타낸다.
	float m_fTimeLag;


	//카메라 변환 행렬과 투영 변환 행렬을 나타내는 멤버 변수를 선언한다.
	D3DXMATRIX m_d3dxmtxView;
	D3DXMATRIX m_d3dxmtxProjection;

	//뷰-포트를 나타내는 멤버 변수를 선언한다.
	D3D11_VIEWPORT m_d3dViewport;

	//카메라 변환 행렬과 투영 변환 행렬을 위한 상수 버퍼 인터페이스 포인터를 선언한다.
	ID3D11Buffer *m_pd3dcbViewProjection;
	ID3D11Buffer *m_pd3dcbCamera;

public:
	//카메라 변환행렬을 생성한다.
	void GenerateViewMatrix();
	// 카메라가 여러번 회전을 하게 되면 누적된 실수연산의 부적확성 때문에
	// 카메라의 로컬 x-축(Right), y-축(Up), z-축(LookAt)이 서로 직교하지 않을 수 있다.
	// 카메라의 로컬 x-축(Right), y-축(Up), z-축(LookAt)이 서로 직교하도록 만들어준다.
	void RegenerateViewMatrix();
	void GenerateViewMatrix(D3DXVECTOR3 d3dxvEyePosition, D3DXVECTOR3 d3dxvLookAt, D3DXVECTOR3 d3dxvUp);

};


class CThirdPersonCamera : public CCamera
{
public:
	CThirdPersonCamera();

	virtual void Update(const D3DXVECTOR3 *pd3dxvPosition, float fTimeElapsed);
	virtual void SetLookAt(D3DXVECTOR3& vLookAt);
};