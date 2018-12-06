#ifndef SPRITE2D_H
#define SPRITE2D_H

#include <WinSock2.h>
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>

#include "GameObject.h"
#include "d3dApp.h"
#include "ConstantBuffer.h"
#include "wterr.h"

enum class PictureMode
{
	FROM_FILE,
	SQUARE
};

class Sprite2D : public GameObject
{
public:
	//���캯��
	Sprite2D(ComPtr<ID3D11VertexShader> VertexShader2D,
		ComPtr<ID3D11PixelShader> PixelShader2D,
		ComPtr<ID3D11InputLayout> vertexLayout2D,
		ComPtr<ID3D11Device> device,
		PictureMode mode,
		ComPtr<ID3D11SamplerState> samplerState = nullptr,
		std::wstring strTexture = L"");
	Sprite2D() = default;

	//��������
	~Sprite2D();

	//��������
	//strTexture�������ļ�·��
	WTRESULT AddTexture(std::wstring strTexture);

	//���ι���
	//x,y����������
	//width,height��������
	WTRESULT Init(
		float x = 0.0f, float y = 0.0f, float width = 2.0f, float height = 2.0f, 
		float deepth = 0.0f, 
		float colorR = 1.0f, float colorG = 1.0f, float colorB = 1.0f, float colorA = 1.0f);

	// ���û�����
	WTRESULT SetBuffer();

	//���벼�֡���ɫ���󶨡�����/������������������
	//vertexLayout2D���������벼��
	WTRESULT Update();

	//������ת
	//angle����ʱ����ת�Ƕ�
	//aspectRatio����Ļ��߱ȣ�ֱ�Ӵ���AspectRatio()����
	WTRESULT Rotation(float angle, float aspectRatio);
	WTRESULT setRotationCenter(float RotationCenterX, float RotationCenterY);

	//��������
	//scale��x��y�������ű���
	WTRESULT Scaling(DirectX::XMFLOAT2 scale);

	//�����ƶ�
	//translation��ƽ�Ƶ�����
	WTRESULT Translation(DirectX::XMFLOAT2 translation);

	//����Ҫ���Ƶ����������������Ŀ��ýӿڣ�
	//index���ڼ���������0��ʼ��
	WTRESULT SetTexIndex(UINT index);

	//����(ͨ��TexIndex��ʶ�������������Ĭ��Ϊ��һ��)
	WTRESULT Draw();

	//��������
	void ResetMesh();

	//λ�ø�ԭ
	//������϶����ิԭ����
	WTRESULT ResetPosition();

	WTRESULT SetCanSee(bool canSee);

	bool GetCanSee();

private:
	// ����һ������NDC��Ļ����
	void Create2DShow(const DirectX::XMFLOAT2& center = { 0.0f, 0.0f }, const DirectX::XMFLOAT2& scale = { 1.0f, 1.0f }, const float deepth = 0.0f, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });
	WTRESULT Create2DShow(float centerX = 0.0f, float centerY = 0.0f, float scaleX = 1.0f, float scaleY = 1.0f, float deepth = 0.0f, float colorR = 1.0f, float colorG = 1.0f, float colorB = 1.0f, float colorA = 1.0f);
	// ����
	std::vector<ComPtr<ID3D11ShaderResourceView>> mTexture;

	//��ɫ��
	ComPtr<ID3D11VertexShader> mVertexShader2D;				// ����2D�Ķ�����ɫ��
	ComPtr<ID3D11PixelShader> mPixelShader2D;				// ����2D��������ɫ��

	//��ת��������
	DirectX::XMFLOAT2 mRotationCenterPos;

	//������
	ComPtr<ID3D11SamplerState> mSamplerState;

	//���벼��
	ComPtr<ID3D11InputLayout> mVertexLayout2D;

	//����
	MeshData meshData;
	PicturnData mMeshData;

	//���񱸷�
	MeshData meshDataBackups;
	PicturnData mMeshDataBack;

	//�����±�
	UINT TexIndex;

	//�Ƿ�ɼ�
	bool isCanSee;

	//��������
	PictureMode mode;
};

#endif //SPRITE2D_H