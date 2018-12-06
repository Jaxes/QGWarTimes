#include "GameApp.h"
#include <filesystem>
using namespace DirectX;
using namespace std::experimental;

CInitSock Initsock;

static float totDeltaTime;
static UINT mCurrFrame = 1;
static bool GunisSelect = false;
static bool ModeisSelect = false;
static bool isCity = false;
static UINT chooseRoom = 0;
static bool isMove = false;
static bool isWin = true;

GameApp::GameApp(HINSTANCE hInstance)
	: D3DApp(hInstance)
{
}

GameApp::~GameApp()
{
}

bool GameApp::Init()
{
	if (!D3DApp::Init())
		return false;

	if (!InitEffect())
		return false;

	if (!InitResource())
		return false;

	if (!InitClient())
		return false;

	// ��ʼ����꣬���̲���Ҫ
	mMouse->SetWindow(mhMainWnd);
	mMouse->SetMode(DirectX::Mouse::MODE_ABSOLUTE);

	isStart = false;
	isReady = false;

	return true;
}

void GameApp::OnResize()
{
	assert(md2dFactory);
	assert(mdwriteFactory);
	// �ͷ�D2D�������Դ
	mColorBrush1.Reset();
	mColorBrush2.Reset();
	md2dRenderTarget.Reset();

	D3DApp::OnResize();

	// ΪD2D����DXGI������ȾĿ��
	ComPtr<IDXGISurface> surface;
	HR(mSwapChain->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<void**>(surface.GetAddressOf())));
	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
	HR(md2dFactory->CreateDxgiSurfaceRenderTarget(surface.Get(), &props, md2dRenderTarget.GetAddressOf()));

	surface.Reset();
	// �����̶���ɫˢ���ı���ʽ
	HR(md2dRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Yellow),
		mColorBrush1.GetAddressOf()));
	HR(md2dRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::White),
		mColorBrush2.GetAddressOf()));
	HR(mdwriteFactory->CreateTextFormat(L"����", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 15, L"zh-cn",
		mTextFormat.GetAddressOf()));
	
	
	// ����������ʾ
	if (mConstantBuffers[2] != nullptr)
	{
		mCamera->SetFrustum(XM_PIDIV2, AspectRatio(), 0.5f, 1000.0f);
		mCBOnReSize.proj = mCamera->GetProj();
		md3dImmediateContext->UpdateSubresource(mConstantBuffers[2].Get(), 0, nullptr, &mCBOnReSize, 0, 0);
		md3dImmediateContext->VSSetConstantBuffers(2, 1, mConstantBuffers[2].GetAddressOf());
	}
}

void GameApp::UpdateScene(float dt)
{
	static bool AddSpeed = false;
	// ��������¼�����ȡ���ƫ����
	Mouse::State mouseState = mMouse->GetState();
	Mouse::State lastMouseState = mMouseTracker.GetLastState();
	mMouseTracker.Update(mouseState);

	Keyboard::State keyState = mKeyboard->GetState();
	mKeyboardTracker.Update(keyState);

	// ����������1��60֡
	totDeltaTime += dt;
	if (totDeltaTime > 1.0f / 60)
	{
		totDeltaTime -= 1.0f / 60;
		mCurrFrame = (mCurrFrame + 1) % 60;
	}

	switch (mDirector->GetSence())
	{
		//��Ϸ��ʼ����
	case SENCE::START:
		if (mouseState.leftButton == false && mMouseTracker.leftButton == mMouseTracker.RELEASED)
		{
			if (mouseState.x > 355.5f && mouseState.x< 605.5f && mouseState.y>391.0f && mouseState.y < 491.0f)
			{
				mDirector->SetSence(SENCE::LOADING);
				WTR(Anim->Init(
					[](size_t frame)->DirectX::XMFLOAT2 {return XMFLOAT2(0.0040f, 0.0f); },
					[&](DirectX::XMFLOAT2&& move)->void {Loading[0]->Translation(move); },
					10, 500));
				WTR(Anim->Run());
			}
			Start[3]->SetTexIndex(0);
		}

		if (mouseState.leftButton == true && mMouseTracker.leftButton == mMouseTracker.HELD && mouseState.x > 355.5f && mouseState.x< 605.5f && mouseState.y>391.0f && mouseState.y < 491.0f)
		{
			Start[3]->SetTexIndex(1);
		}

		break;

		//ģʽѡ�����
	case SENCE::MODE_SELECT:
		if (!ModeisSelect && mouseState.leftButton == false && mMouseTracker.leftButton == mMouseTracker.UP)
		{
			if (mouseState.x > 72.5f && mouseState.x< 198.5f && mouseState.y>63.0f && mouseState.y < 129.0f)
			{
				modeSelect[3]->SetTexIndex(1);
				modeSelect[10]->SetCanSee(true);
				modeSelect[10]->SetTexIndex(0);
			}
			else if (mouseState.x > 72.5f && mouseState.x< 198.5f && mouseState.y>153.0f && mouseState.y < 219.0f)
			{
				modeSelect[4]->SetTexIndex(1);
				modeSelect[10]->SetCanSee(true);
				modeSelect[10]->SetTexIndex(1);
			}
			else
			{
				modeSelect[3]->SetTexIndex(0);
				modeSelect[4]->SetTexIndex(0);
				modeSelect[10]->SetCanSee(false);
			}
		}

		if (mouseState.leftButton == false && mMouseTracker.leftButton == mMouseTracker.RELEASED)
		{
			if (mouseState.x > 72.5f && mouseState.x< 198.5f && mouseState.y>63.0f && mouseState.y < 129.0f)
			{
				modeSelect[3]->SetTexIndex(1);
				modeSelect[4]->SetTexIndex(0);
				modeSelect[0]->SetTexIndex(0);
				roomSelect[5]->SetTexIndex(0);
				modeSelect[10]->SetTexIndex(0);
				modeSelect[10]->SetCanSee(true);
				modeSelect[11]->SetCanSee(false);
				modeSelect[12]->SetCanSee(false);
				ModeisSelect = true;
				isCity = true;
				mDirector->SetSence(SENCE::ROOM_SELECT);
				mClientInterface->PullGameData();
			}
			else if (mouseState.x > 72.5f && mouseState.x< 198.5f && mouseState.y>153.0f && mouseState.y < 219.0f)
			{
				modeSelect[3]->SetTexIndex(0);
				modeSelect[4]->SetTexIndex(1);
				modeSelect[0]->SetTexIndex(1);
				modeSelect[10]->SetTexIndex(1);
				roomSelect[5]->SetTexIndex(1);
				modeSelect[10]->SetCanSee(true);
				modeSelect[11]->SetCanSee(false);
				modeSelect[12]->SetCanSee(false);
				ModeisSelect = true;
				isCity = false;
				mDirector->SetSence(SENCE::ROOM_SELECT);
				mClientInterface->PullGameData();
			}
		}
		break;

		//����ѡ�����
	case SENCE::ROOM_SELECT:
		if (mouseState.leftButton == false && mMouseTracker.leftButton == mMouseTracker.RELEASED)
		{
			if (this->pGame->room.size() >= 1 && mouseState.x > 377.5f && mouseState.x<506.5f  && mouseState.y>132.5f && mouseState.y < 261.5f)
			{
				mDirector->SetSence(SENCE::POSITION_SELECT);
				chooseRoom = 1;
				roomSelect[2]->SetCanSee(true);
			}
			else if (this->pGame->room.size() >= 2 && mouseState.x > 550.5f  && mouseState.x< 679.5f && mouseState.y>132.5f && mouseState.y < 261.5f)
			{
				mDirector->SetSence(SENCE::POSITION_SELECT);
				chooseRoom = 2;
				roomSelect[3]->SetCanSee(true);
			}

			//�������䰴ť
			if (this->pGame->room.size() < 2 && mouseState.x > 420.0f && mouseState.x< 496.0f && mouseState.y>24.0f && mouseState.y < 44.0f)
			{
				mClientInterface->AddRoom();
				chooseRoom = static_cast<UINT>(this->pGame->room.size());
				roomSelect[chooseRoom + 1 ]->SetCanSee(true);
				modeSelect[5]->SetTexIndex(3);
				mDirector->SetSence(SENCE::POSITION_SELECT);
			}
			else
			{
				modeSelect[5]->SetTexIndex(0);
			}
		}

		if (mouseState.leftButton == false && mMouseTracker.leftButton == mMouseTracker.UP)
		{
			if (mouseState.x > 420.0f && mouseState.x< 496.0f && mouseState.y>24.0f && mouseState.y < 44.0f)
			{
				modeSelect[5]->SetTexIndex(1);
			}
			else
			{
				modeSelect[5]->SetTexIndex(0);
			}
		}

		if (mouseState.leftButton == true && mMouseTracker.leftButton == mMouseTracker.HELD)
		{
			if (mouseState.x > 420.0f && mouseState.x< 496.0f && mouseState.y>24.0f && mouseState.y < 44.0f)
			{
				modeSelect[5]->SetTexIndex(2);
			}
		}


	case SENCE::POSITION_SELECT:
		//���½ǵ�����ѡ�����
		if (!GunisSelect && mouseState.leftButton == false && mMouseTracker.leftButton == mMouseTracker.UP)
		{
			if (mouseState.x > 27.5f && mouseState.x< 241.5f && mouseState.y>334.0f && mouseState.y < 398.0f)
			{
				roomSelect[0]->SetTexIndex(1);
				roomSelect[4]->SetTexIndex(1);
				roomSelect[4]->SetCanSee(true);
			}
			else if (mouseState.x > 27.5f && mouseState.x< 241.5f && mouseState.y>420.5f && mouseState.y < 484.5f)
			{
				roomSelect[1]->SetTexIndex(1);
				roomSelect[4]->SetTexIndex(0);
				roomSelect[4]->SetCanSee(true);
			}
			else
			{
				roomSelect[0]->SetTexIndex(0);
				roomSelect[1]->SetTexIndex(0);
				roomSelect[4]->SetCanSee(false);
			}
		}

		if (mouseState.leftButton == false && mMouseTracker.leftButton == mMouseTracker.RELEASED)
		{
			if (mouseState.x > 27.5f && mouseState.x< 241.5f && mouseState.y>334.0f && mouseState.y < 398.0f)
			{
				roomSelect[0]->SetTexIndex(1);
				roomSelect[1]->SetTexIndex(0);
				roomSelect[4]->SetTexIndex(1);
				GunisSelect = true;
			}
			else if (mouseState.x > 27.5f && mouseState.x< 241.5f && mouseState.y>420.5f && mouseState.y < 484.5f)
			{
				roomSelect[0]->SetTexIndex(0);
				roomSelect[1]->SetTexIndex(1);
				roomSelect[4]->SetTexIndex(0);
				GunisSelect = true;
			}
			if (GunisSelect)
			{
				roomSelect[4]->SetCanSee(true);
			}
		}

		//��������ʵʱ����
		mClientInterface->PullGameData();
		UINT i;
		for (i = 2; i < 2 + this->pGame->room.size(); i++)
		{
			if (!roomSelect[i]->GetCanSee())
			{
				roomSelect[i]->SetCanSee(true);
			}
		}
		for (; i < 4; i++)
		{
			if (roomSelect[i]->GetCanSee())
			{
				roomSelect[i]->SetCanSee(false);
			}
		}

		//λ������ʵʱ����
		if (chooseRoom != 0)
		{
			int k = 0;
			for (auto& i : this->pGame->room[chooseRoom - 1]->team)
			{
				switch (i.size())
				{
				case 1:
					for (auto& j : i)
					{
						if (j.first != 1)
						{
							posSelect[2]->SetTexIndex(0);
							posSelect[9]->SetCanSee(true);

							posSelect[3]->SetTexIndex(1);
							posSelect[10]->SetCanSee(false);
						}
						else
						{
							posSelect[2]->SetTexIndex(1);
							posSelect[9]->SetCanSee(false);

							posSelect[3]->SetTexIndex(0);
							posSelect[10]->SetCanSee(true);
						}
					}
					break;
				case 2:
					for (int i = 2 + k * 2; i < 4 + k * 2; i++)
					{
						posSelect[i]->SetTexIndex(1);
						posSelect[i + 7]->SetCanSee(false);
					}
				case 0:
					for (int i = 2 + k * 2; i < 4 + k * 2; i++)
					{
						posSelect[i]->SetTexIndex(0);
						posSelect[i + 7]->SetCanSee(true);
					}
					break;
				}
				k++;
			}
		}
		
		//λ��ѡ�����
		if (mouseState.leftButton == false && mMouseTracker.leftButton == mMouseTracker.UP)
		{
			//�˳����䰴ť
			if (mouseState.x > 918.0f && mouseState.x< 942.0f && mouseState.y>23.2f && mouseState.y < 44.8f)
			{
				posSelect[6]->SetTexIndex(1);
			}
			else
			{
				posSelect[6]->SetTexIndex(0);
			}

		}
		if (mouseState.leftButton == true && mMouseTracker.leftButton == mMouseTracker.HELD)
		{
			//�˳����䰴ť
			if (mouseState.x > 918.0f && mouseState.x< 942.0f && mouseState.y>23.2f && mouseState.y < 44.8f)
			{
				posSelect[6]->SetTexIndex(1);
			}
		}
		if (mouseState.leftButton == false && mMouseTracker.leftButton == mMouseTracker.RELEASED)
		{
			//�˳����䰴ť
			if (mouseState.x > 918.0f && mouseState.x< 942.0f && mouseState.y>23.2f && mouseState.y < 44.8f)
			{
				mDirector->SetSence(SENCE::ROOM_SELECT);
				mClientInterface->ExitRoom(chooseRoom);
				chooseRoom = 0;
			}
			//����λ�ð�ť
			else if (mouseState.x > 746.0f && mouseState.x< 810.0f && mouseState.y>99.0f && mouseState.y < 163.0f)
			{
				posSelect[2]->SetTexIndex(1);
				posSelect[9]->SetCanSee(false);
				mClientInterface->AddPlayer(chooseRoom, 1, 1, this->pGame);
				mDirector->SetSence(SENCE::WAIT_OTHERS);
				WaitOthers[2]->SetTexIndex(1);
				Man = Player[0];
				Gun = SubmachineGun[0];
				Man->SetCanSee(true);
				Gun->SetCanSee(true);
			}
			else if (mouseState.x > 746.0f && mouseState.x< 810.0f && mouseState.y>212.0f && mouseState.y < 276.0f)
			{
				posSelect[3]->SetTexIndex(1);
				posSelect[10]->SetCanSee(false);
				mClientInterface->AddPlayer(chooseRoom, 1, 2, this->pGame);
				mDirector->SetSence(SENCE::WAIT_OTHERS);
				WaitOthers[3]->SetTexIndex(1);
				Man = Player[1];
				Gun = SubmachineGun[1];
				Man->SetCanSee(true);
				Gun->SetCanSee(true);
			}
			else if (mouseState.x > 859.0f && mouseState.x< 923.0f && mouseState.y>99.0f && mouseState.y < 163.0f)
			{
				
				posSelect[4]->SetTexIndex(1);
				posSelect[11]->SetCanSee(false);
				mClientInterface->AddPlayer(chooseRoom, 2, 1, this->pGame);
				mDirector->SetSence(SENCE::WAIT_OTHERS);
				WaitOthers[7]->SetTexIndex(1);
				Man = Player[2];
				Gun = SubmachineGun[2];
				Man->SetCanSee(true);
				Gun->SetCanSee(true);
			}
			else if (mouseState.x > 859.0f && mouseState.x< 923.0f && mouseState.y>212.0f && mouseState.y < 276.0f)
			{
				posSelect[5]->SetTexIndex(1);
				posSelect[12]->SetCanSee(false);
				mClientInterface->AddPlayer(chooseRoom, 2, 2, this->pGame);
				mDirector->SetSence(SENCE::WAIT_OTHERS);
				WaitOthers[8]->SetTexIndex(1);
				Man = Player[3];
				Gun = SubmachineGun[3];
				Man->SetCanSee(true);
				Gun->SetCanSee(true);
			}
		}

		break;

		//�����ȴ�����
	case SENCE::WAIT_OTHERS:
		//��������ʵʱ����
		mClientInterface->PullGameData();

		if (chooseRoom != 0)
		{
			int k = 0;
			for (auto& i : this->pGame->room[chooseRoom - 1]->team)
			{
				switch (i.size())
				{
				case 1:
					for (auto& j : i)
					{
						if (j.first != 1)
						{
							WaitOthers[2 + k * 5]->SetTexIndex(0);

							WaitOthers[3 + k * 5]->SetTexIndex(1);
						}
						else
						{
							WaitOthers[2 + k * 5]->SetTexIndex(1);

							WaitOthers[3 + k * 5]->SetTexIndex(0);
						}
					}
					break;
				case 2:
					for (int i = 2 + k * 5; i < 4 + k * 5; i++)
					{
						WaitOthers[i]->SetTexIndex(1);
					}
				case 0:
					for (int i = 2 + k * 5; i < 4 + k * 5; i++)
					{
						WaitOthers[i]->SetTexIndex(0);
					}
					break;
				}
				k++;
			}
		}
		if (isStart)
		{
			mDirector->SetSence(SENCE::LOADING);
			WTR(Anim->Init(
				[](size_t frame)->DirectX::XMFLOAT2 {return XMFLOAT2(0.0040f, 0.0f); },
				[&](DirectX::XMFLOAT2&& move)->void {Loading[0]->Translation(move); },
				10, 500));
			WTR(Anim->Run());
			mClientInterface->PushPlayerData(this->pGame, Man, Gun);
		}
		break;

		//���ؽ���
	case SENCE::LOADING:
		if (Anim->isEnd())
		{
			if (!isStart)
			{
				Loading[0]->ResetPosition();
				mDirector->SetSence(SENCE::MODE_SELECT);
			}
			else
			{
				Loading[0]->ResetPosition();
				mMouse->SetMode(DirectX::Mouse::MODE_RELATIVE);
				mDirector->SetSence(SENCE::FIGHT);
				mClientInterface->PushReady();

				XMFLOAT3 CameraPos = Man->GetPosition();
				CameraPos.y += 1.0f;

				std::dynamic_pointer_cast<FirstPersonCamera>(mCamera)->LookTo(CameraPos, Man->GetLook(), XMFLOAT3(0.0f, 1.0f, 0.0f));
				mCBFrame.view = mCamera->GetView();
				XMStoreFloat4(&mCBFrame.eyePos, mCamera->GetPositionXM());

				// ��ʼ�����ڴ��ڴ�С�䶯ʱ�޸ĵ�ֵ
				mCamera->SetFrustum(XM_PI / 3, AspectRatio(), 0.5f, 1000.0f);
				mCBOnReSize.proj = mCamera->GetProj();
				md3dImmediateContext->UpdateSubresource(mConstantBuffers[2].Get(), 0, nullptr, &mCBOnReSize, 0, 0);

				mClientInterface->PullGameData();
			}
		}
		break;

		//ս������㳡��
	case SENCE::SETTLEMENT:
		mMouse->SetMode(DirectX::Mouse::MODE_ABSOLUTE);

		if (mouseState.leftButton == false && mMouseTracker.leftButton == mMouseTracker.UP)
		{
			if (mouseState.x > 304.5f && mouseState.x< 429.5f && mouseState.y>460.5f && mouseState.y < 510.5f)
			{
				Settlement[3]->SetTexIndex(1);
			}
			else if (mouseState.x > 530.5f && mouseState.x< 655.5f && mouseState.y>460.5f && mouseState.y < 510.5f)
			{
				Settlement[4]->SetTexIndex(1);
			}
			else
			{
				Settlement[3]->SetTexIndex(0);
				Settlement[4]->SetTexIndex(0);
			}
		}

		if (mouseState.leftButton == true && mMouseTracker.leftButton == mMouseTracker.HELD)
		{
			if (mouseState.x > 304.5f && mouseState.x< 429.5f && mouseState.y>460.5f && mouseState.y < 510.5f)
			{
				Settlement[3]->SetTexIndex(2);
			}
			else if (mouseState.x > 530.5f && mouseState.x< 655.5f && mouseState.y>460.5f && mouseState.y < 510.5f)
			{
				Settlement[4]->SetTexIndex(2);
			}
		}

		if (mouseState.leftButton == false && mMouseTracker.leftButton == mMouseTracker.RELEASED)
		{
			if (mouseState.x > 304.5f && mouseState.x< 429.5f && mouseState.y>460.5f && mouseState.y < 510.5f)
			{
				Settlement[3]->SetTexIndex(3);
				mDirector->SetSence(SENCE::LOADING);
				isStart = true;
				mMouse->SetMode(DirectX::Mouse::MODE_RELATIVE);
				WTR(Anim->Init(
					[](size_t frame)->DirectX::XMFLOAT2 {return XMFLOAT2(0.0040f, 0.0f); },
					[&](DirectX::XMFLOAT2&& move)->void {Loading[0]->Translation(move); },
					10, 500));
				WTR(Anim->Run());
				mClientInterface->PushPlayerData(this->pGame, Man, Gun);
			}
			else if (mouseState.x > 530.5f && mouseState.x< 655.5f && mouseState.y>460.5f && mouseState.y < 510.5f)
			{
				Settlement[4]->SetTexIndex(3);
				mDirector->SetSence(SENCE::LOADING);
				mMouse->SetMode(DirectX::Mouse::MODE_ABSOLUTE);
			}
			else
			{
				Settlement[3]->SetTexIndex(0);
				Settlement[4]->SetTexIndex(0);
			}
		}
	case SENCE::FIGHT:
	{
		if (mCurrFrame % 15 == 0)
		{
			this->UI[27]->SetCanSee(false);
			if (mDirector->GetSence() != SENCE::SETTLEMENT)
			{
				int r, t, s;
				r = this->pGame->PlayerRoomInfo[this->Name].x;
				t = this->pGame->PlayerRoomInfo[this->Name].y;
				s = this->pGame->PlayerRoomInfo[this->Name].z;
				uint32_t blood = pGame->room[r - 1]->team[t - 1][s]->mBlood;
				uint32_t bullet = std::dynamic_pointer_cast<WarTimes::CWeaponSubmachineGun>(pGame->room[r - 1]->team[t - 1][s]->mpWeapon)->mBullet;
				if (blood > 0 && blood <= 100)
				{
					for (int i = 9; i >= 0; i--)
					{
						if (this->UI[i]->GetCanSee())
						{
							if (static_cast<uint32_t>((i + 1) * 10) > blood)
							{
								this->UI[i]->SetCanSee(false);
							}
							else
							{
								break;
							}
						}
					}
				}
				if (bullet > 0 && bullet <= 10)
				{
					for (int i = 19; i >= 10; i--)
					{
						if (this->UI[i]->GetCanSee())
						{
							if (static_cast<uint32_t>(i + 1 - 10) > bullet)
							{
								this->UI[i]->SetCanSee(false);
							}
							else
							{
								break;
							}
						}
					}
				}
			}
		}
		
		// ��ȡ����
		auto cam1st = std::dynamic_pointer_cast<FirstPersonCamera>(mCamera);
		auto cam3rd = std::dynamic_pointer_cast<ThirdPersonCamera>(mCamera);


		if (mCameraMode == CameraMode::FirstPerson || mCameraMode == CameraMode::Free)
		{
			// ��һ�˳�/����������Ĳ���

			// �����ƶ�
			if (keyState.IsKeyDown(Keyboard::W))
			{
				if (mCameraMode == CameraMode::FirstPerson)
				{
					cam1st->Walk(dt * (AddSpeed ? 10.0f : 5.0f));
					XMVECTOR move = XMVectorReplicate(dt * (AddSpeed ? 10.0f : 5.0f))*XMVector3Normalize(XMVector3Cross(cam1st->GetRightXM(), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));
					XMFLOAT3 moveFloat3;
					XMStoreFloat3(&moveFloat3, move);
					Man->Translation(moveFloat3);
				}
				else
					cam1st->MoveForward(dt * (AddSpeed ? 10.0f : 5.0f));
			}
			if (keyState.IsKeyDown(Keyboard::S))
			{
				if (mCameraMode == CameraMode::FirstPerson)
				{
					cam1st->Walk(dt * -(AddSpeed ? 10.0f : 5.0f));
					XMVECTOR move = XMVectorReplicate(dt * -(AddSpeed ? 10.0f : 5.0f))*XMVector3Normalize(XMVector3Cross(cam1st->GetRightXM(), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));
					XMFLOAT3 moveFloat3;
					XMStoreFloat3(&moveFloat3, move);
					Man->Translation(moveFloat3);
				}
				else
					cam1st->MoveForward(dt * -(AddSpeed ? 10.0f : 5.0f));
			}
			if (keyState.IsKeyDown(Keyboard::A))
			{
				cam1st->Strafe(dt * -(AddSpeed ? 10.0f : 5.0f));
				if (mCameraMode == CameraMode::FirstPerson)
				{
					XMVECTOR move = XMVectorReplicate(dt * -(AddSpeed ? 10.0f : 5.0f))*XMVector3Normalize(cam1st->GetRightXM());
					XMFLOAT3 moveFloat3;
					XMStoreFloat3(&moveFloat3, move);
					Man->Translation(moveFloat3);
				}
			}
			if (keyState.IsKeyDown(Keyboard::D))
			{
				cam1st->Strafe(dt * (AddSpeed ? 10.0f : 5.0f));
				if (mCameraMode == CameraMode::FirstPerson)
				{
					XMVECTOR move = XMVectorReplicate(dt * (AddSpeed ? 10.0f : 5.0f))*XMVector3Normalize(cam1st->GetRightXM());
					XMFLOAT3 moveFloat3;
					XMStoreFloat3(&moveFloat3, move);
					Man->Translation(moveFloat3);
				}
			}
			if (keyState.IsKeyDown(Keyboard::LeftShift))
				AddSpeed = true;
			if (keyState.IsKeyUp(Keyboard::LeftShift))
				AddSpeed = false;

			if (mDirector->GetSence() != SENCE::SETTLEMENT && !isMove && mouseState.x - lastMouseState.x != 0 && mouseState.y - lastMouseState.y != 0)
			{
				isMove = true;
			}

			if (isMove && mDirector->GetSence() != SENCE::SETTLEMENT)
			{
				// ��Ұ��ת����ֹ��ʼ�Ĳ�ֵ�����µ�ͻȻ��ת
				cam1st->Pitch(mouseState.y * dt * 0.85f);
				cam1st->RotateY(mouseState.x * dt * 0.85f);
				Man->RotationY(mouseState.x * dt * 0.85f);
				Gun->RotationAround({ 0.0f,1.0f,0.0f }, Man->GetPosition(), mouseState.x * dt * 0.85f);
				Gun->RotationAround(Man->GetRight(), Man->GetPosition(), mouseState.y * dt * 0.85f, XM_2PI / 9, Man->GetFront());
				
				//���
				if (mCurrFrame % 15 == 0 && mouseState.leftButton == true && mMouseTracker.leftButton == mMouseTracker.HELD)
				{
					int r, t, s;
					r = this->pGame->PlayerRoomInfo[this->Name].x;
					t = this->pGame->PlayerRoomInfo[this->Name].y;
					s = this->pGame->PlayerRoomInfo[this->Name].z;
					bool isShoot = false;
					for (int i = 0; i < 2; i++)
					{
						for (auto& j : this->pGame->room[r - 1]->team[i])
						{
							if (j.second->mName != this->Name)
							{
								if (this->Player[j.first - 1 + 2 * i]->CheckShooted(Man->GetPosition(), Man->GetLook()))
									mClientInterface->Shoot(j.second->mName, 10);
								isShoot = true;
								break;
							}
						}
						if (isShoot)
						{
							this->pGame->room[r - 1]->team[t - 1][s]->mScore += 3;
							break;
						}
					}
					this->pGame->room[r - 1]->team[t - 1][s]->mpWeapon->Attack();
				}
			}

			// ��λ��������[-20.0f, 20.0f]��������
			// ��������
			XMFLOAT3 adjustedPos, adjustedPosForMan;
			XMStoreFloat3(&adjustedPos, XMVectorClamp(cam1st->GetPositionXM(), XMVectorSet(-20.0f, -0.4f, -20.0f, 3.0f), XMVectorReplicate(20.0f)));
			XMStoreFloat3(&adjustedPosForMan, XMVectorClamp(XMLoadFloat3(&(Man->GetPosition())), XMVectorSet(-20.0f, -0.4f, -20.0f, 3.0f), XMVectorReplicate(20.0f)));
			cam1st->SetPosition(adjustedPos);
			Man->SetPosition(adjustedPosForMan);
			XMFLOAT3 r = Man->GetRight();
			XMFLOAT3 u = Man->GetUp();
			XMFLOAT3 l = Man->GetLook();
			Gun->SetPosition(Man->GetPosition());
			Gun->Translation(XMFLOAT3(r.x*0.5f, r.y*0.5f, r.z*0.5f));
			Gun->Translation(XMFLOAT3(u.x*0.65f, u.y*0.65f, u.z*0.65f));
			Gun->Translation(XMFLOAT3(l.x*0.6f, l.y*0.6f, l.z*0.6f));
		}
		else if (mCameraMode == CameraMode::ThirdPerson)
		{
			// �����˳�������Ĳ���

			cam3rd->SetTarget(Man->GetPosition());

			// ��������ת
			cam3rd->RotateX(mouseState.y * dt * 1.25f);
			cam3rd->RotateY(mouseState.x * dt * 1.25f);
			cam3rd->Approach(-mouseState.scrollWheelValue / 120 * 1.0f);
		}

		// ���¹۲����
		mCamera->UpdateViewMatrix();
		XMStoreFloat4(&mCBFrame.eyePos, mCamera->GetPositionXM());
		mCBFrame.view = mCamera->GetView();

		// ���ù���ֵ
		mMouse->ResetScrollWheelValue();

		// �����ģʽ�л�
		if (mKeyboardTracker.IsKeyPressed(Keyboard::D1) && mCameraMode != CameraMode::FirstPerson)
		{
			if (!cam1st)
			{
				cam1st.reset(new FirstPersonCamera);
				cam1st->SetFrustum(XM_PIDIV2, AspectRatio(), 0.5f, 1000.0f);
				mCamera = cam1st;
			}
			XMFLOAT3 pos = Man->GetPosition();
			pos.y += 1.0f;
			cam1st->LookTo(pos, Man->GetFront(), XMFLOAT3(0.0f, 1.0f, 0.0f));


			mCameraMode = CameraMode::FirstPerson;
		}
		else if (mKeyboardTracker.IsKeyPressed(Keyboard::D2) && mCameraMode != CameraMode::ThirdPerson)
		{
			if (!cam3rd)
			{
				cam3rd.reset(new ThirdPersonCamera);
				cam3rd->SetFrustum(XM_PIDIV2, AspectRatio(), 0.5f, 1000.0f);
				mCamera = cam3rd;
			}
			XMFLOAT3 target = Man->GetPosition();
			cam3rd->SetTarget(target);
			cam3rd->SetDistance(8.0f);
			cam3rd->SetDistanceMinMax(3.0f, 20.0f);

			mCameraMode = CameraMode::ThirdPerson;
		}
		else if (mKeyboardTracker.IsKeyPressed(Keyboard::D3) && mCameraMode != CameraMode::Free)
		{
			if (!cam1st)
			{
				cam1st.reset(new FirstPersonCamera);
				cam1st->SetFrustum(XM_PIDIV2, AspectRatio(), 0.5f, 1000.0f);
				mCamera = cam1st;
			}
			// �����Ϸ���ʼ
			XMFLOAT3 pos = Man->GetPosition();
			XMFLOAT3 up{ 0.0f, 1.0f, 0.0f };
			pos.y += 3;
			cam1st->LookTo(pos, Man->GetFront(), up);

			mCameraMode = CameraMode::Free;
		}

		mClientInterface->PushPlayerData(this->pGame, Man, Gun);

		md3dImmediateContext->UpdateSubresource(mConstantBuffers[1].Get(), 0, nullptr, &mCBFrame, 0, 0);
	}
	    break;
	}
	
	// �˳���������Ӧ�򴰿ڷ���������Ϣ
	if (keyState.IsKeyDown(Keyboard::Escape))
		SendMessage(MainWnd(), WM_DESTROY, 0, 0);
}

void GameApp::DrawScene()
{
	assert(md3dImmediateContext);
	assert(mSwapChain);

	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView.Get(), reinterpret_cast<const float*>(&Colors::Black));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	

	WTR(mDirector->DirectorDrawSence());

	if (mDirector->GetSence() == SENCE::SETTLEMENT)
	{
		std::wstringstream wss1, wss2;
		std::wstring text1, text2;
		int r, t, s;
		r = this->pGame->PlayerRoomInfo[this->Name].x;
		t = this->pGame->PlayerRoomInfo[this->Name].y;
		s = this->pGame->PlayerRoomInfo[this->Name].z;
		uint32_t score1 = this->pGame->room[r - 1]->team[t - 1][s]->mScore;
		uint32_t score2;
		for (auto& i : this->pGame->room[r - 1]->team[(3 - t) - 1])
		{
			score2 = i.second->mScore;
		}
		if (t == 1)
		{
			wss1 << L"Player_one";
			wss2 << L"Player_two";
		}
		else
		{
			wss1 << L"Player_two";
			wss2 << L"Player_one";
		}
		wss1 << L"         ";
		wss1 << score1;
		if (score1 < 10)
		{
			wss1 << L" ";
		}
		wss1 << L"                 ���ǹ                  ";

		wss2 << L"         ";
		wss2 << score2;
		if (score2 < 10)
		{
			wss2 << L" ";
		}
		wss2 << L"                 ���ǹ                  ";
		if (isWin)
		{
			wss1 << L"1";
			text1 = wss1.str();
			wss2 << L"0";
			text2 = wss2.str();
			md2dRenderTarget->BeginDraw();
			md2dRenderTarget->DrawTextW(text1.c_str(), (UINT32)text1.length(), mTextFormat.Get(),
				D2D1_RECT_F{ 203.0f, 220.0f, 904.5f, 240.0f }, mColorBrush1.Get());
			md2dRenderTarget->DrawTextW(text2.c_str(), (UINT32)text2.length(), mTextFormat.Get(),
				D2D1_RECT_F{ 203.0f, 260.0f, 904.5f, 380.0f }, mColorBrush2.Get());
			HR(md2dRenderTarget->EndDraw());
		}
		else
		{
			wss1 << L"0";
			text2 = wss1.str();
			wss2 << L"1";
			text1 = wss2.str();
			md2dRenderTarget->BeginDraw();
			md2dRenderTarget->DrawTextW(text1.c_str(), (UINT32)text1.length(), mTextFormat.Get(),
				D2D1_RECT_F{ 203.0f, 220.0f, 904.5f, 240.0f }, mColorBrush2.Get());
			md2dRenderTarget->DrawTextW(text2.c_str(), (UINT32)text2.length(), mTextFormat.Get(),
				D2D1_RECT_F{ 203.0f, 260.0f, 904.5f, 380.0f }, mColorBrush1.Get());
			HR(md2dRenderTarget->EndDraw());
		}
		
	}

	HR(mSwapChain->Present(0, 0));
}

HRESULT GameApp::CreateShaderFromFile(const WCHAR * objFileNameInOut, const WCHAR * hlslFileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob ** ppBlobOut)
{
	HRESULT hr = S_OK;

	// Ѱ���Ƿ����Ѿ�����õĶ�����ɫ��
	if (objFileNameInOut && filesystem::exists(objFileNameInOut))
	{
		HR(D3DReadFileToBlob(objFileNameInOut, ppBlobOut));
	}
	else
	{
		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
		// ���� D3DCOMPILE_DEBUG ��־���ڻ�ȡ��ɫ��������Ϣ���ñ�־���������������飬
		// ����Ȼ������ɫ�������Ż�����
		dwShaderFlags |= D3DCOMPILE_DEBUG;

		// ��Debug�����½����Ż��Ա������һЩ����������
		dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		ComPtr<ID3DBlob> errorBlob = nullptr;
		hr = D3DCompileFromFile(hlslFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, shaderModel,
			dwShaderFlags, 0, ppBlobOut, errorBlob.GetAddressOf());
		if (FAILED(hr))
		{
			if (errorBlob != nullptr)
			{
				OutputDebugStringA(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
			}
			return hr;
		}

		// ��ָ��������ļ���������ɫ����������Ϣ���
		if (objFileNameInOut)
		{
			HR(D3DWriteBlobToFile(*ppBlobOut, objFileNameInOut, FALSE));
		}
	}

	return hr;
}


bool GameApp::InitEffect()
{
	ComPtr<ID3DBlob> blob;

	// ����������ɫ��(2D)
	HR(CreateShaderFromFile(L"HLSL\\VS_2D.vso", L"...\\Shader\\VS_2D.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(md3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, mVertexShader2D.GetAddressOf()));
	// �������㲼��(2D)
	HR(md3dDevice->CreateInputLayout(VertexPosNormalTex::inputLayout, ARRAYSIZE(VertexPosNormalTex::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), mVertexLayout2D.GetAddressOf()));

	// ����������ɫ��(2D)
	HR(CreateShaderFromFile(L"HLSL\\PS_2D.pso", L"...\\Shader\\PS_2D.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(md3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, mPixelShader2D.GetAddressOf()));

	// ����������ɫ��(3D)
	HR(CreateShaderFromFile(L"HLSL\\VS_3D.vso", L"...\\Shader\\VS_3D.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(md3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, mVertexShader3D.GetAddressOf()));
	// �������㲼��(3D)
	HR(md3dDevice->CreateInputLayout(VertexPosNormalTex::inputLayout, ARRAYSIZE(VertexPosNormalTex::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), mVertexLayout3D.GetAddressOf()));

	// ����������ɫ��(3D)
	HR(CreateShaderFromFile(L"HLSL\\PS_3D.pso", L"...\\Shader\\PS_3D.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(md3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, mPixelShader3D.GetAddressOf()));

	return true;
}

bool GameApp::InitResource()
{
	// ******************
	// ���ó�������������
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = 0;
	// �½�����VS��PS�ĳ���������
	cbd.ByteWidth = sizeof(CBChangesEveryDrawing);
	HR(md3dDevice->CreateBuffer(&cbd, nullptr, mConstantBuffers[0].GetAddressOf()));
	cbd.ByteWidth = sizeof(CBChangesEveryFrame);
	HR(md3dDevice->CreateBuffer(&cbd, nullptr, mConstantBuffers[1].GetAddressOf()));
	cbd.ByteWidth = sizeof(CBChangesOnResize);
	HR(md3dDevice->CreateBuffer(&cbd, nullptr, mConstantBuffers[2].GetAddressOf()));
	cbd.ByteWidth = sizeof(CBNeverChange);
	HR(md3dDevice->CreateBuffer(&cbd, nullptr, mConstantBuffers[3].GetAddressOf()));
	cbd.ByteWidth = sizeof(CBMaterial);
	HR(md3dDevice->CreateBuffer(&cbd, nullptr, mConstantBuffers[4].GetAddressOf()));

	// ******************
	// ��ʼ��������״̬
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HR(md3dDevice->CreateSamplerState(&sampDesc, mSamplerState.GetAddressOf()));

	// ******************
	//��ʼ����Ϸ����
	mDirector = std::shared_ptr<Director>(new Director(md3dImmediateContext));
	//������Ҫ
	mDirector->SetSence(SENCE::START);

	// ******************
	//��ʼ������
	this->Anim = std::shared_ptr<Animator<DirectX::XMFLOAT2>>(new Animator<DirectX::XMFLOAT2>(this->mhMainWnd));
	

	// ******************
	// ��ʼ������

	//ս��������ʼ��
	if (!InitFightSence())
		return false;
	
	//��ʼ������ʼ��
	if (!InitStartSence())
		return false;
	
	//ģʽѡ�񳡾���ʼ��
	if (!InitModeSelectSence())
		return false;
	
	//����ѡ�񳡾���ʼ��
	if (!InitRoomSelectSence())
		return false;

	//λ��ѡ�񳡾���ʼ��
	if (!InitPositionSelectSence())
		return false; 

	//�����ȴ�������ʼ��
	if (!InitWaitOthersSence())
		return false;

	//���������ʼ��
	if (!InitLoadingSence())
		return false;

	//��������ʼ��
	if (!InitSettlementSence())
		return false;

	// ******************
	// ��ʼ��������������ֵ
	// ��ʼ��ÿ֡���ܻ�仯��ֵ
	mCameraMode = CameraMode::FirstPerson;
	auto camera = std::shared_ptr<FirstPersonCamera>(new FirstPersonCamera);
	mCamera = camera;

	// ******************
	// ��ʼ��������Ⱦ״̬
	RenderStates::InitAll(md3dDevice);

	// ******************
	// ��ʼ������仯��ֵ
	// ������
	mCBNeverChange.dirLight[0].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mCBNeverChange.dirLight[0].Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	mCBNeverChange.dirLight[0].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mCBNeverChange.dirLight[0].Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	// �ƹ�
	mCBNeverChange.pointLight[0].Position = XMFLOAT3(0.0f, 50.0f, 0.0f);
	mCBNeverChange.pointLight[0].Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mCBNeverChange.pointLight[0].Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mCBNeverChange.pointLight[0].Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mCBNeverChange.pointLight[0].Att = XMFLOAT3(0.0f, 0.05f, 0.0f);
	mCBNeverChange.pointLight[0].Range = 100.0f;
	mCBNeverChange.numDirLight = 1;
	mCBNeverChange.numPointLight = 1;
	mCBNeverChange.numSpotLight = 0;
	// ��ʼ������
	mMaterial.material.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mMaterial.material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mMaterial.material.Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 5.0f);

	// ���²����ױ��޸ĵĳ�����������Դ
	
	md3dImmediateContext->UpdateSubresource(mConstantBuffers[3].Get(), 0, nullptr, &mCBNeverChange, 0, 0);
	md3dImmediateContext->UpdateSubresource(mConstantBuffers[4].Get(), 0, nullptr, &mMaterial, 0, 0);

	// ����ͼԪ���ͣ��趨���벼��
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Ԥ�Ȱ󶨸�������Ļ�����������ÿ֡���µĻ�������Ҫ�󶨵�������������
	md3dImmediateContext->VSSetConstantBuffers(0, 1, mConstantBuffers[0].GetAddressOf());
	md3dImmediateContext->VSSetConstantBuffers(1, 1, mConstantBuffers[1].GetAddressOf());
	md3dImmediateContext->VSSetConstantBuffers(2, 1, mConstantBuffers[2].GetAddressOf());

	md3dImmediateContext->PSSetConstantBuffers(1, 1, mConstantBuffers[1].GetAddressOf());
	md3dImmediateContext->PSSetConstantBuffers(3, 1, mConstantBuffers[3].GetAddressOf());
	md3dImmediateContext->PSSetConstantBuffers(4, 1, mConstantBuffers[4].GetAddressOf());
	md3dImmediateContext->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());
	return true;
}

bool GameApp::InitFightSence()
{
	std::shared_ptr<Sence> pSence;
	pSence = std::shared_ptr<Sence>(new Sence(SENCE::FIGHT));

	//��Ϸ���󲿷�
	for (int i = 0; i < 4; i++) {
		mWalls[i] = std::shared_ptr<Sprite3D>(new Sprite3D(mVertexShader3D, mPixelShader3D, mVertexLayout3D, md3dDevice, ModelMode::PLANE, "Texture\\brick.dds", mSamplerState));
	}
	for (int i = 0; i < 4; ++i)
	{
		mWalls[i]->init(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(40.0f, 10.0f), XMFLOAT2(5.0f, 1.5f));
		mWalls[i]->SetBuffer();
		XMMATRIX world = XMMatrixRotationX(-XM_PIDIV2) * XMMatrixRotationY(XM_PIDIV2 * i)
			* XMMatrixTranslation(i % 2 ? -20.0f * (i - 2) : 0.0f, 3.0f, i % 2 == 0 ? -20.0f * (i - 1) : 0.0f);
		mWalls[i]->SetWorldMatrix(world);
		mWalls[i]->SetTexTransformMatrix(XMMatrixIdentity());
		pSence->AddSprite3D(mWalls[i]);
	}

	GameMap = std::shared_ptr<Sprite3D>(new Sprite3D(
		mVertexShader3D, mPixelShader3D, mVertexLayout3D, md3dDevice, ModelMode::FROM_FILE, "�ִ���ͼ"));
	GameMap->Scaling({ 1 / 25.0f,1 / 25.0f,1 / 25.0f });
	GameMap->init();
	GameMap->SetTexTransformMatrix(XMMatrixIdentity());
	GameMap->SetWorldMatrix(XMMatrixIdentity());
	GameMap->Translation(XMFLOAT3(0.0f, 4.0f, 0.0f));
	pSence->AddSprite3D(GameMap);

	for (int i = 0; i < 4; i++)
	{
		Player[i] = std::shared_ptr<Sprite3D>(new Sprite3D(
			mVertexShader3D, mPixelShader3D, mVertexLayout3D, md3dDevice, ModelMode::FROM_FILE, "ԭʼ��"));
		Player[i]->Scaling({ 1 / 100.0f,1 / 100.0f ,1 / 100.0f });
		Player[i]->init();
		Player[i]->SetTexTransformMatrix(XMMatrixIdentity());
		Player[i]->SetWorldMatrix(XMMatrixIdentity());
		pSence->AddSprite3D(Player[i]);
	}
	Player[0]->SetPosition({ 18.0f,-0.4f,5.0f });
	Player[1]->SetPosition({ 18.0f,-0.4f,3.0f });
	Player[2]->SetPosition({ -18.0f,-0.4f,-5.0f });
	Player[2]->RotationY(XM_PI);
	Player[3]->SetPosition({ -18.0f,-0.4f,-3.0f });
	Player[3]->RotationY(XM_PI);
	for (int i = 0; i < 4; i++)
	{
		Player[i]->SetCollisionBox();
		Player[i]->SetCanSee(false);
	}

	//���ǹ
	for (int i = 0; i < 4; i++)
	{
		SubmachineGun[i] = std::shared_ptr<Sprite3D>(new Sprite3D(
			mVertexShader3D, mPixelShader3D, mVertexLayout3D, md3dDevice, ModelMode::FROM_FILE, "���ǹ"));
		SubmachineGun[i]->Scaling({ 1 / 1500.0f,1 / 1500.0f ,1 / 1500.0f });
		SubmachineGun[i]->init();
		SubmachineGun[i]->SetTexTransformMatrix(XMMatrixIdentity());
		SubmachineGun[i]->SetWorldMatrix(XMMatrixIdentity());
		SubmachineGun[i]->SetPosition(Player[i]->GetPosition());
		XMFLOAT3 r = Player[i]->GetRight();
		XMFLOAT3 u = Player[i]->GetUp();
		XMFLOAT3 l = Player[i]->GetLook();
		SubmachineGun[i]->Translation(XMFLOAT3(r.x*0.5f, r.y*0.5f, r.z*0.5f));
		SubmachineGun[i]->Translation(XMFLOAT3(u.x*0.65f, u.y*0.65f, u.z*0.65f));
		SubmachineGun[i]->Translation(XMFLOAT3(l.x*0.6f, l.y*0.6f, l.z*0.6f));
		float* Angle = XMVector3AngleBetweenVectors(XMLoadFloat3(&Player[i]->GetFront()), XMLoadFloat3(&SubmachineGun[i]->GetLook())).m128_f32;
		SubmachineGun[i]->RotationY(*Angle);
		pSence->AddSprite3D(SubmachineGun[i]);
		SubmachineGun[i]->SetCanSee(false);
	}
	//����ǹ��ģ�Ͷ��㷭ת�ˣ��ز���Ҫ������
	//for (int i = 0; i < 4; i++)
	//{
	//	ShotGun[i] = std::shared_ptr<Sprite3D>(new Sprite3D(
	//		mVertexShader3D, mPixelShader3D, mVertexLayout3D, md3dDevice, ModelMode::FROM_FILE, "ɢ��ǹ"));
	//	ShotGun[i]->Scaling({ 1 / 1500.0f,1 / 1500.0f ,1 / 1500.0f });
	//	ShotGun[i]->init();
	//	ShotGun[i]->SetTexTransformMatrix(XMMatrixIdentity());
	//	ShotGun[i]->SetWorldMatrix(XMMatrixIdentity());
	//	ShotGun[i]->SetPosition(Player[i]->GetPosition());
	//	XMFLOAT3 r = Player[i]->GetRight();
	//	XMFLOAT3 u = Player[i]->GetUp();
	//	XMFLOAT3 l = Player[i]->GetLook();
	//	ShotGun[i]->Translation(XMFLOAT3(r.x*0.5f, r.y*0.5f, r.z*0.5f));
	//	ShotGun[i]->Translation(XMFLOAT3(u.x*0.65f, u.y*0.65f, u.z*0.65f));
	//	ShotGun[i]->Translation(XMFLOAT3(l.x*0.6f, l.y*0.6f, l.z*0.6f));
	//	float* Angle = XMVector3AngleBetweenVectors(-XMLoadFloat3(&Player[i]->GetFront()), XMLoadFloat3(&ShotGun[i]->GetLook())).m128_f32;
	//	ShotGun[i]->RotationY(*Angle);
	//	pSence->AddSprite3D(ShotGun[i]);
	//	//ShotGun[i]->SetCanSee(false);
	//}

	//UI����
	std::wstring strSenceTex[28] = {
		L"Ѫ��.dds",L"Ѫ��.dds" ,L"Ѫ��.dds" ,L"Ѫ��.dds" ,L"Ѫ��.dds" ,
		L"Ѫ��.dds" ,L"Ѫ��.dds" ,L"Ѫ��.dds" ,L"Ѫ��.dds" ,L"Ѫ��.dds",
		L"�ӵ����������ӵ���.png",L"�ӵ����������ӵ���.png",L"�ӵ����������ӵ���.png",L"�ӵ����������ӵ���.png",L"�ӵ����������ӵ���.png",
		L"�ӵ����������ӵ���.png",L"�ӵ����������ӵ���.png",L"�ӵ����������ӵ���.png",L"�ӵ����������ӵ���.png",L"�ӵ����������ӵ���.png",
		L"Ĭ��ͷ���п�.png",L"Ĭ��ͷ���п�.png",L"Ĭ��ͷ���п�.png",L"Ĭ��ͷ���п�.png",L"YourTeam.png",L"Opponent.png",
		L"׼��.png",L"������.png" };
	for (int i = 0; i < 28; i++)
	{
		UI[i] = std::shared_ptr<Sprite2D>(new Sprite2D(
			mVertexShader2D, mPixelShader2D, mVertexLayout2D, md3dDevice, PictureMode::FROM_FILE, mSamplerState, L"Texture\\" + strSenceTex[i]));
		if (i >= 20 && i <= 23)
		{
			UI[i]->AddTexture(L"Texture\\Ĭ��ͷ��������.png");
		}
		UI[i]->SetTexTransformMatrix(XMMatrixIdentity());
		UI[i]->SetWorldMatrix(XMMatrixIdentity());
		pSence->AddSprite2D(UI[i]);
	}
	//Ѫ��
	for (int i = 0; i < 10; i++)
	{
		UI[i]->Init((-703.5f + i * 25) / 960, 451.0f / 540, 0.026042f, 0.018518f, 0.0f);
	}

	for (int i = 10; i < 20; i++)
	{
		UI[i]->Init((-703.5f + (i-10) * 25) / 960, 389.0f / 540, 0.010417f, 0.070370f, 0.0f);
	}

	//ͷ��
	UI[20]->Init(-0.8625f, 0.755555f, 0.133333f, 0.237037f, 0.0f);
	UI[21]->Init(-0.8625f, 0.314814f, 0.133333f, 0.237037f, 0.0f);
	UI[22]->Init(0.86458f, 0.577777f, 0.133333f, 0.237037f, 0.0f);
	UI[23]->Init(0.86458f, 0.314814f, 0.133333f, 0.237037f, 0.0f);
	UI[24]->Init(-0.8625f, 0.475925f, 0.25f, 0.25f * 200 / 320, 0.0f);
	UI[25]->Init(0.86458f, 0.779629f, 0.25f, 0.25f * 200 / 400, 0.0f);
	UI[26]->Init(0.0f, 0.0f, 0.052083f, 0.092592f, 0.0f);
	UI[27]->Init();
	UI[27]->SetCanSee(false);

	mDirector->AddSence(pSence);
	pSence.reset();
	return true;
}

bool GameApp::InitStartSence()
{
	std::shared_ptr<Sence> pSence;
	std::wstring strSenceTex[4] = { L"������ģ����.png" ,L"����.png" ,L"����.png",L"��ʼ��ť.png" };
	pSence = std::shared_ptr<Sence>(new Sence(SENCE::START));

	for (int i = 0; i < 4; i++)
	{
		Start[i] = std::shared_ptr<Sprite2D>(new Sprite2D(
			mVertexShader2D, mPixelShader2D, mVertexLayout2D, md3dDevice, PictureMode::FROM_FILE, mSamplerState, L"Texture\\" + strSenceTex[i]));
		Start[i]->SetTexTransformMatrix(XMMatrixIdentity());
		Start[i]->SetWorldMatrix(XMMatrixIdentity());
		pSence->AddSprite2D(Start[i]);
	}
	Start[3]->AddTexture(L"Texture\\��ʼ��ť�������.png");

	Start[0]->Init();
	Start[1]->Init(0.0f, 0.617592f, 1.472916f, 1.472916f * 277.0f / 1440.0f * AspectRatio(), 0.01f);
	Start[2]->Init(0.0f, -0.250229f, 0.520833f, 0.520833f*636.0f / 547.0f*AspectRatio(), 0.01f);
	Start[3]->Init(0.0f, -0.63f, 0.520833f, 0.520833f*239.0f / 514.0f*AspectRatio(), 0.02f);

	mDirector->AddSence(pSence);
	pSence.reset();
	return true;
}

bool GameApp::InitModeSelectSence()
{
	std::shared_ptr<Sence> pSence;
	std::wstring strSenceTex[13] = {
		L"����ѡ�񱳾�-�ִ�����.png",
		L"ģʽѡ�񱳾�.png",
		L"����ѡ�񱳾�.png",
		L"��δѡ���ִ�����.png",
		L"��δѡ��ԭʼ��Ұ.png",
		L"�������䰴ť����ʼ״̬��������ɫΪ#ffffff��.png",
		L"����.png",
		L"ģʽѡ��.png",
		L"����ѡ��.png",
		L"����ѡ��.png",
		L"��ģʽѡ���ִ�����.png",
		L"��ѡ��ģʽ.png",
		L"��ѡ��ģʽ.png" };
	pSence = std::shared_ptr<Sence>(new Sence(SENCE::MODE_SELECT));

	for (int i = 0; i < 13; i++)
	{
		modeSelect[i] = std::shared_ptr<Sprite2D>(new Sprite2D(
			mVertexShader2D, mPixelShader2D, mVertexLayout2D, md3dDevice, PictureMode::FROM_FILE, mSamplerState, L"Texture\\" + strSenceTex[i]));
		modeSelect[i]->SetTexTransformMatrix(XMMatrixIdentity());
		modeSelect[i]->SetWorldMatrix(XMMatrixIdentity());
		pSence->AddSprite2D(modeSelect[i]);
	}
	modeSelect[0]->AddTexture(L"Texture\\����ѡ�񱳾�-ԭʼ��Ұ.png");
	modeSelect[3]->AddTexture(L"Texture\\�ִ���ͼ.png");
	modeSelect[4]->AddTexture(L"Texture\\ԭʼ��Ұ.png");
	modeSelect[5]->AddTexture(L"Texture\\�������䰴ť����껬��ʱ״̬��������ɫΪ#ccffff��.png");
	modeSelect[5]->AddTexture(L"Texture\\�������䰴ť�����ʱ״̬��������ɫΪ#ccffff��.png");
	modeSelect[5]->AddTexture(L"Texture\\�������䰴ť�������״̬����ɫΪ#ffffff��.png");
	modeSelect[10]->AddTexture(L"Texture\\��ģʽѡ��ԭʼ��Ұ.png");
	modeSelect[10]->SetCanSee(false);

	modeSelect[0]->Init();
	modeSelect[1]->Init(-0.71875f, 0.5f, 0.5625f, 0.5625f*AspectRatio(), 0.01f);
	modeSelect[2]->Init(-0.71875f, -0.5f, 0.5625f, 0.5625f*AspectRatio(), 0.01f);
	modeSelect[3]->Init(-0.71875f, 0.64f, 0.268518f, 0.267085f, 0.02f);
	modeSelect[4]->Init(-0.71875f, 0.31f, 0.268518f, 0.267085f, 0.02f);
	modeSelect[5]->Init(-0.081481f, 0.874074f, 0.158332f, 0.074074f, 0.01f);
	modeSelect[6]->Init(0.86770f, 0.870370f, 0.023958f, 0.044444f, 0.01f);
	modeSelect[7]->Init(-0.71875f, 0.903703f, 0.15f, 0.15f * 41 / 160 * AspectRatio(), 0.02f);
	modeSelect[8]->Init(-0.71875f, -0.096296f, 0.15f, 0.15f * 41 / 160 * AspectRatio(), 0.02f);
	modeSelect[9]->Init(-0.30231f, 0.874074f, 0.15f, 0.15f * 41 / 160 * AspectRatio(), 0.02f);
	modeSelect[10]->Init(-0.71875f, 0.090740f, 0.11666f, 0.11666f * 33 / 124 * AspectRatio(), 0.02f);
	modeSelect[11]->Init(-0.71875f, -0.5f, 0.175f, 0.175f * 36 / 189 * AspectRatio(), 0.02f);
	modeSelect[12]->Init(0.29166f, 0.0f, 0.175f, 0.175f * 36 / 189 * AspectRatio(), 0.02f);

	mDirector->AddSence(pSence);
	pSence.reset();
	return true;
}

bool GameApp::InitRoomSelectSence()
{
	std::shared_ptr<Sence> pSence;
	std::wstring strSenceTex[6] = { L"����ǹ��δѡ��.png",L"���ǹ��δѡ��.png",L"�����.png",L"�����.png",L"���ǹ.png",L"������ѡ���ִ�����.png" };
	pSence = std::shared_ptr<Sence>(new Sence(SENCE::ROOM_SELECT));
	for (int i = 0; i < 11; i++)
	{
		pSence->AddSprite2D(modeSelect[i]);
	}

	for (int i = 0; i < 6; i++)
	{
		roomSelect[i] = std::shared_ptr<Sprite2D>(new Sprite2D(
			mVertexShader2D, mPixelShader2D, mVertexLayout2D, md3dDevice, PictureMode::FROM_FILE, mSamplerState, L"Texture\\" + strSenceTex[i]));
		roomSelect[i]->SetTexTransformMatrix(XMMatrixIdentity());
		roomSelect[i]->SetWorldMatrix(XMMatrixIdentity());
		pSence->AddSprite2D(roomSelect[i]);
	}
	roomSelect[0]->AddTexture(L"Texture\\ѡ������ǹ.png");
	roomSelect[1]->AddTexture(L"Texture\\ѡ����ǹ.png");
	roomSelect[4]->AddTexture(L"Texture\\����ǹ.png");
	roomSelect[5]->AddTexture(L"Texture\\������ѡ��ԭʼ��Ұ.png");

	roomSelect[0]->Init(-0.719791f, -0.35f, 0.44583f, 0.237037f, 0.02f);
	roomSelect[1]->Init(-0.719791f, -0.67592f, 0.44583f, 0.237037f, 0.02f);
	roomSelect[2]->Init(-0.07916f, 0.27037f, 0.26875f, 0.47f, 0.02f);
	roomSelect[2]->SetCanSee(false);
	roomSelect[3]->Init(0.28125f, 0.27037f, 0.26875f, 0.47f, 0.02f);
	roomSelect[3]->SetCanSee(false);
	roomSelect[4]->Init(-0.71875f, -0.90926f, 0.11666f, 0.051851f, 0.02f);
	roomSelect[4]->SetCanSee(false);
	roomSelect[5]->Init(0.28125f, 0.67222f, 0.2f, 0.08888f, 0.02f);

	mDirector->AddSence(pSence);
	pSence.reset();
	return true;
}

bool GameApp::InitPositionSelectSence()
{
	std::shared_ptr<Sence> pSence;
	std::wstring strSenceTex[13] = { 
		L"�ڸ���.dds",
		L"���䱳��.dds",
		L"�����λͷ��.png",
		L"�����λͷ��.png",
		L"�����λͷ��.png",
		L"�����λͷ��.png",
		L"�رհ�ť����ʼ�������״̬��.png",
		L"TEAM ONE.png",
		L"TEAM TWO.png",
		L"����.png",
		L"����.png",
		L"����.png",
		L"����.png" };

	pSence = std::shared_ptr<Sence>(new Sence(SENCE::POSITION_SELECT));

	for (int i = 0; i < 11; i++)
	{
		pSence->AddSprite2D(modeSelect[i]);
	}
	for (int i = 0; i < 6; i++)
	{
		pSence->AddSprite2D(roomSelect[i]);
	}

	for (int i = 0; i < 13; i++)
	{
		posSelect[i] = std::shared_ptr<Sprite2D>(new Sprite2D(
			mVertexShader2D, mPixelShader2D, mVertexLayout2D, md3dDevice, PictureMode::FROM_FILE, mSamplerState, L"Texture\\" + strSenceTex[i]));
		if (i < 6 && i > 1)
		{
			posSelect[i]->AddTexture(L"Texture\\Ĭ��ͷ��.png");
		}
		posSelect[i]->SetTexTransformMatrix(XMMatrixIdentity());
		posSelect[i]->SetWorldMatrix(XMMatrixIdentity());
		pSence->AddSprite2D(posSelect[i]);
	}
	posSelect[6]->AddTexture(L"Texture\\�رհ�ť����껬�������ʱ״̬��.png");

	posSelect[0]->Init(0.28125f, 0.0f, 1.4375f, 2.0f, 0.03f);
	posSelect[1]->Init(0.75f, 0.0f, 0.5f, 2.0f, 0.04f);

	posSelect[2]->Init(0.62083f, 0.51481f, 0.13f, 0.23703f, 0.05f);
	//����
	posSelect[3]->Init(0.62083f, 0.1f, 0.13f, 0.23703f, 0.05f);
	posSelect[4]->Init(0.85625f, 0.51481f, 0.13f, 0.23703f, 0.05f);
	posSelect[5]->Init(0.85625f, 0.1f, 0.13f, 0.23703f, 0.05f);

	posSelect[6]->Init(0.9375f, 0.874074f, 0.05f, 0.08f, 0.05f);

	posSelect[7]->Init(0.62083f, 0.69259f, 0.197916f, 0.11851f, 0.05f);
	posSelect[8]->Init(0.85625f, 0.69259f, 0.197916f, 0.11851f, 0.05f);

	posSelect[9]->Init(0.62083f, 0.342592f, 0.1f, 0.08888f, 0.05f);
	//����
	posSelect[10]->Init(0.62083f, -0.07592f, 0.1f, 0.08888f, 0.05f);
	posSelect[11]->Init(0.85625f, 0.342592f, 0.1f, 0.08888f, 0.05f);
	posSelect[12]->Init(0.85625f, -0.07592f, 0.1f, 0.08888f, 0.05f);


	mDirector->AddSence(pSence);
	pSence.reset();
	return true;
}

bool GameApp::InitWaitOthersSence()
{
	std::shared_ptr<Sence> pSence = std::shared_ptr<Sence>(new Sence(SENCE::WAIT_OTHERS));
	std::wstring strSenceTex[12] = { L"����.png",L"VS.png",
		L"��λͷ���п�.png",L"��λͷ���п�.png" ,L"��λͷ���п�.png" ,L"��λͷ���п�.png" ,L"��λͷ���п�.png" ,
		L"��λͷ���п�.png",L"��λͷ���п�.png" ,L"��λͷ���п�.png" ,L"��λͷ���п�.png" ,L"��λͷ���п�.png"};

	for (int i = 0; i < 12; i++)
	{
		WaitOthers[i] = std::shared_ptr<Sprite2D>(new Sprite2D(
			mVertexShader2D, mPixelShader2D, mVertexLayout2D, md3dDevice, PictureMode::FROM_FILE, mSamplerState, L"Texture\\" + strSenceTex[i]));
		if (i >= 2 && i <= 11)
		{
			WaitOthers[i]->AddTexture(L"Texture\\Ĭ��ͷ���п�.png");
		}
		WaitOthers[i]->SetTexTransformMatrix(XMMatrixIdentity());
		WaitOthers[i]->SetWorldMatrix(XMMatrixIdentity());
		pSence->AddSprite2D(WaitOthers[i]);
	}
	WaitOthers[0]->Init();
	WaitOthers[1]->Init(0.0f, 0.0f, 1.757292f, 1.575925f, 0.01f);

	//��λͷ��
	WaitOthers[2]->Init(0.077083f, -0.688888f, 0.208333f, 0.370370f, 0.03f);
	WaitOthers[3]->Init(0.405208f, -0.031481f, 0.208333f, 0.370370f, 0.03f);
	WaitOthers[4]->Init(0.695833f,  0.555555f, 0.208333f, 0.370370f, 0.03f);
	WaitOthers[5]->Init(0.583333f, -0.688888f, 0.208333f, 0.370370f, 0.03f);
	WaitOthers[6]->Init(0.825f   , -0.203703f, 0.208333f, 0.370370f, 0.03f);

	WaitOthers[7]->Init(-0.077083f, 0.688888f, 0.208333f, 0.370370f, 0.03f);
	WaitOthers[8]->Init(-0.405208f, 0.031481f, 0.208333f, 0.370370f, 0.03f);
	WaitOthers[9]->Init(-0.695833f, -0.555555f, 0.208333f, 0.370370f, 0.03f);
	WaitOthers[10]->Init(-0.583333f, 0.688888f, 0.208333f, 0.370370f, 0.03f);
	WaitOthers[11]->Init(-0.825f, 0.203703f, 0.208333f, 0.370370f, 0.03f);

	mDirector->AddSence(pSence);
	pSence.reset();
	return true;
}

bool GameApp::InitLoadingSence()
{
	std::shared_ptr<Sence> pSence = std::shared_ptr<Sence>(new Sence(SENCE::LOADING));
	std::wstring strSenceTex[2] = { L"���ͼƬ.png",L"������·��.png" };
	for (int i = 0; i < 2; i++)
	{
		Loading[i] = std::shared_ptr<Sprite2D>(new Sprite2D(
			mVertexShader2D, mPixelShader2D, mVertexLayout2D, md3dDevice, PictureMode::FROM_FILE, mSamplerState, L"Texture\\" + strSenceTex[i]));
		Loading[i]->SetTexTransformMatrix(XMMatrixIdentity());
		Loading[i]->SetWorldMatrix(XMMatrixIdentity());
		pSence->AddSprite2D(Loading[i]);
	}
	Loading[0]->Init(-2.0f, 0.0f, 2.0f, 2.0f, 0.0f);
	Loading[1]->Init();

	mDirector->AddSence(pSence);
	pSence.reset();
	return true;
}

bool GameApp::InitSettlementSence()
{
	std::shared_ptr<Sence> pSence = std::shared_ptr<Sence>(new Sence(SENCE::SETTLEMENT));
	for (int i = 0; i < 4; i++)
	{
		pSence->AddSprite3D(mWalls[i]);
	}
	pSence->AddSprite3D(GameMap);
	for (int i = 0; i < 4; i++)
	{
		pSence->AddSprite3D(Player[i]);
		pSence->AddSprite3D(SubmachineGun[i]);
	}

	std::wstring strSenceTex[5] = { 
		L"lost����.png",L"win����.png",L"���а�.png",L"���¿��ְ�ť����ʼ״̬ ��ɫΪ#b6ffa8��.png",L"���ذ�ť����ʼ״̬ ��ɫΪ#a8e1ff��.png" };
	for (int i = 0; i < 5; i++)
	{
		Settlement[i] = std::shared_ptr<Sprite2D>(new Sprite2D(
			mVertexShader2D, mPixelShader2D, mVertexLayout2D, md3dDevice, PictureMode::FROM_FILE, mSamplerState, L"Texture\\" + strSenceTex[i]));
		Settlement[i]->SetTexTransformMatrix(XMMatrixIdentity());
		Settlement[i]->SetWorldMatrix(XMMatrixIdentity());
		pSence->AddSprite2D(Settlement[i]);
	}
	Settlement[0]->SetCanSee(false);

	Settlement[3]->AddTexture(L"Texture\\���¿��ְ�ť����껬��״̬ ��ɫΪ#66ff66��.png");
	Settlement[3]->AddTexture(L"Texture\\���¿��ְ�ť�����ʱ״̬ ��ɫΪ#66ff66��.png");
	Settlement[3]->AddTexture(L"Texture\\���¿��ְ�ť�������״̬ ��ɫΪ#b6ffa8��.png");

	Settlement[4]->AddTexture(L"Texture\\���ذ�ť����껬��״̬ ��ɫΪ#66ccff��.png");
	Settlement[4]->AddTexture(L"Texture\\���ذ�ť�����ʱ״̬ ��ɫΪ#66ccff��.png");
	Settlement[4]->AddTexture(L"Texture\\���ذ�ť�������״̬ ��ɫΪ#a8e1ff��.png");

	Settlement[0]->Init(0.0f, 0.620370f, 1.053125f, 0.374074f, 0.0f);
	Settlement[1]->Init(0.0f, 0.620370f, 0.917708f, 0.374074f, 0.0f);
	Settlement[2]->Init(0.0f, -0.139814f, 2.0f, 0.953703f, 0.0f);
	Settlement[3]->Init(-0.235417f, -0.798148f, 0.260416f, 0.185185f, 0.0f);
	Settlement[4]->Init(0.235417f, -0.798148f, 0.260416f, 0.185185f, 0.0f);

	mDirector->AddSence(pSence);
	pSence.reset();
	return true;
}

bool GameApp::InitClient()
{
	srand((unsigned)time(NULL));
	char buffer[256]{};
	char ipAddr[16]{};

	std::wstringstream ws;
	// ȡ�ñ�����������
	::gethostname(buffer, 256);
	// ͨ���������õ�IP��ַ��Ϣ
	ADDRINFO* info;
	ADDRINFO hints{};
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	::getaddrinfo(buffer, nullptr, &hints, &info);
	sockaddr_in* pSin = (sockaddr_in*)info->ai_addr;
	inet_ntop(AF_INET, &pSin->sin_addr, ipAddr, sizeof(ipAddr));

	/*ws << ipAddr;
	ws >> Name;*/
	freeaddrinfo(info);
	
	ws << rand() % 100000;
	ws >> this->Name;

	std::string ServerIpAddr;
	std::ifstream in("ipAddr.txt");
	getline(in, ServerIpAddr);
	in.close();

	// ������ַ
	sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(23320);
	InetPton(AF_INET, toWString(ServerIpAddr).c_str(), &addr.sin_addr);

	mClient = std::shared_ptr<Client>(new Client());
	if (mClient->init(reinterpret_cast<sockaddr*>(&addr)))
		return false;

	mClient->run();

	mClientInterface = std::shared_ptr<ClientInterface>(new ClientInterface(mClient, Name));

	mClientInterface->Say(L"Hello");

	// ������Ϸ
	std::shared_ptr<WarTimes::CWarTimes> pGame(new WarTimes::CWarTimes());
	this->pGame = pGame;

	mClientInterface->mCallbacks[L"ServiceInterface::Shoot"] =
		[&,this](std::wistream& win) -> std::wstring {
		uint32_t Decrease;
		win >> Decrease;
		int r, t, s;
		{
			std::mutex Map_Lock;
			std::lock_guard<std::mutex> Map_Lock_helper(Map_Lock);
			r = this->pGame->PlayerRoomInfo[this->Name].x; 
			t = this->pGame->PlayerRoomInfo[this->Name].y;
			s = this->pGame->PlayerRoomInfo[this->Name].z;
			this->pGame->room[r - 1]->team[t - 1][s]->mBlood -= Decrease;
		}
		if (this->pGame->room[r - 1]->team[t - 1][s]->mBlood == 0)
		{
			isWin = false;
			this->Settlement[0]->SetCanSee(true);
			this->Settlement[1]->SetCanSee(false);
			this->mClientInterface->GameEnding(r);
		}
		this->UI[27]->SetCanSee(true);
		return L"";
	};

	mClientInterface->mCallbacks[L"ServiceInterface::PushGameData"] =
		[&,this](std::wistream& win) -> std::wstring {
		this->pGame->fromString(win);
		if (this->isReady)
		{
			int r;
			r = this->pGame->PlayerRoomInfo[this->Name].x;
			for (int i = 0; i < 2; i++)
			{
				for (auto& j : this->pGame->room[r - 1]->team[i])
				{
					if (j.second->mName != this->Name)  
					{
						Player[j.first - 1 + 2 * i]->DataGet(j.second);
						Player[j.first - 1 + 2 * i]->SetCanSee(true);
						SubmachineGun[j.first - 1 + 2 * i]->DataGet(j.second->mpWeapon);
						SubmachineGun[j.first - 1 + 2 * i]->SetCanSee(true);
					}
				}
			}
		}
		return L"";
	};

	mClientInterface->mCallbacks[L"ServiceInterface::GameStart"] =
		[&,this](std::wistream& win) -> std::wstring {
		this->isStart = true;
		return L"";
	};

	mClientInterface->mCallbacks[L"ServiceInterface::AllReady"] =
		[&, this](std::wistream& win) -> std::wstring {
		this->isReady = true;
		return L"";
	};

	mClientInterface->mCallbacks[L"ServiceInterface::GameEnding"] =
		[&, this](std::wistream& win) -> std::wstring {
		this->isStart = false;
		this->mDirector->SetSence(SENCE::SETTLEMENT);
		return L"";
	};

	return true;
}