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
	//构造函数
	Sprite2D(ComPtr<ID3D11VertexShader> VertexShader2D,
		ComPtr<ID3D11PixelShader> PixelShader2D,
		ComPtr<ID3D11InputLayout> vertexLayout2D,
		ComPtr<ID3D11Device> device,
		PictureMode mode,
		ComPtr<ID3D11SamplerState> samplerState = nullptr,
		std::wstring strTexture = L"");
	Sprite2D() = default;

	//析构函数
	~Sprite2D();

	//增加纹理
	//strTexture：纹理文件路径
	WTRESULT AddTexture(std::wstring strTexture);

	//二段构造
	//x,y：精灵坐标
	//width,height：精灵宽高
	WTRESULT Init(
		float x = 0.0f, float y = 0.0f, float width = 2.0f, float height = 2.0f, 
		float deepth = 0.0f, 
		float colorR = 1.0f, float colorG = 1.0f, float colorB = 1.0f, float colorA = 1.0f);

	// 设置缓冲区
	WTRESULT SetBuffer();

	//输入布局、着色器绑定、顶点/索引缓冲区重新设置
	//vertexLayout2D：传入输入布局
	WTRESULT Update();

	//精灵旋转
	//angle：逆时针旋转角度
	//aspectRatio：屏幕宽高比，直接传入AspectRatio()即可
	WTRESULT Rotation(float angle, float aspectRatio);
	WTRESULT setRotationCenter(float RotationCenterX, float RotationCenterY);

	//精灵缩放
	//scale：x，y坐标缩放比例
	WTRESULT Scaling(DirectX::XMFLOAT2 scale);

	//精灵移动
	//translation：平移的向量
	WTRESULT Translation(DirectX::XMFLOAT2 translation);

	//设置要绘制的是哪张纹理（动画的可用接口）
	//index：第几张纹理（从0开始）
	WTRESULT SetTexIndex(UINT index);

	//绘制(通过TexIndex来识别绘制哪张纹理，默认为第一张)
	WTRESULT Draw();

	//重设网格
	void ResetMesh();

	//位置复原
	//用于配合动画类复原操作
	WTRESULT ResetPosition();

	WTRESULT SetCanSee(bool canSee);

	bool GetCanSee();

private:
	// 创建一个覆盖NDC屏幕的面
	void Create2DShow(const DirectX::XMFLOAT2& center = { 0.0f, 0.0f }, const DirectX::XMFLOAT2& scale = { 1.0f, 1.0f }, const float deepth = 0.0f, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });
	WTRESULT Create2DShow(float centerX = 0.0f, float centerY = 0.0f, float scaleX = 1.0f, float scaleY = 1.0f, float deepth = 0.0f, float colorR = 1.0f, float colorG = 1.0f, float colorB = 1.0f, float colorA = 1.0f);
	// 纹理
	std::vector<ComPtr<ID3D11ShaderResourceView>> mTexture;

	//着色器
	ComPtr<ID3D11VertexShader> mVertexShader2D;				// 用于2D的顶点着色器
	ComPtr<ID3D11PixelShader> mPixelShader2D;				// 用于2D的像素着色器

	//旋转中心坐标
	DirectX::XMFLOAT2 mRotationCenterPos;

	//采样器
	ComPtr<ID3D11SamplerState> mSamplerState;

	//输入布局
	ComPtr<ID3D11InputLayout> mVertexLayout2D;

	//网格
	MeshData meshData;
	PicturnData mMeshData;

	//网格备份
	MeshData meshDataBackups;
	PicturnData mMeshDataBack;

	//纹理集下标
	UINT TexIndex;

	//是否可见
	bool isCanSee;

	//精灵类型
	PictureMode mode;
};

#endif //SPRITE2D_H