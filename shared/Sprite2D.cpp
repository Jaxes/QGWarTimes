#include "Sprite2D.h"
using namespace DirectX;

Sprite2D::Sprite2D(ComPtr<ID3D11VertexShader> VertexShader2D, 
	ComPtr<ID3D11PixelShader> PixelShader2D, 
	ComPtr<ID3D11InputLayout> vertexLayout2D,
	ComPtr<ID3D11Device> device, 
	PictureMode mode,
	ComPtr<ID3D11SamplerState> samplerState, 
	std::wstring strTexture)
	:GameObject(device)
{
	if (strTexture != L"")
	{
		if (mode == PictureMode::FROM_FILE)
		{
			// 初始化纹理
			ComPtr<ID3D11ShaderResourceView> tempTexture;
			if (strTexture.find(L".dds") != std::wstring::npos)
			{
				HR(CreateDDSTextureFromFile(md3dDevice.Get(), strTexture.c_str(), nullptr, tempTexture.GetAddressOf()));
			}
			else
			{
				HR(CreateWICTextureFromFile(md3dDevice.Get(), strTexture.c_str(), nullptr, tempTexture.GetAddressOf()));
			}
			mTexture.push_back(tempTexture);
		}
		else
		{
			WTWARN(L"%s", WTStrErr(WTRESULT_NotASpriteFromFile));
		}
	}
	this->mVertexShader2D = VertexShader2D;
	this->mPixelShader2D = PixelShader2D;
	this->mSamplerState = samplerState;
	this->TexIndex = 0;
	this->mVertexLayout2D = vertexLayout2D;
	this->isCanSee = true;
	this->mode = mode;
}

Sprite2D::~Sprite2D()
{

}

WTRESULT Sprite2D::AddTexture(std::wstring strTexture)
{
	if (this->mode == PictureMode::FROM_FILE)
	{
		if (strTexture != L"")
		{
			// 初始化纹理
			ComPtr<ID3D11ShaderResourceView> tempTexture;
			HR(CreateWICTextureFromFile(md3dDevice.Get(), strTexture.c_str(), nullptr, tempTexture.GetAddressOf()));
			mTexture.push_back(tempTexture);
			return WTRESULT_OK;
		}
		else
		{
			return WTRESULT_TextureAddFailed;
		}
	}
	else
	{
		return WTRESULT_NotASpriteFromFile;
	}
}

WTRESULT Sprite2D::Init(float x, float y, float width, float height, 
	float deepth, 
	float colorR, float colorG, float colorB, float colorA)
{
	Create2DShow(x, y, width / 2.0f, height / 2.0f, deepth, colorR, colorG, colorB, colorA);
	if (mode == PictureMode::FROM_FILE)
	{
		this->meshDataBackups = this->meshData;
	}
	else
	{
		this->mMeshDataBack = this->mMeshData;
	}
	this->mRotationCenterPos.x = x;
	this->mRotationCenterPos.y = y;
	WTR(SetBuffer());
	return WTRESULT_OK;
}

WTRESULT Sprite2D::Update()
{
	md3dImmediateContext->IASetInputLayout(mVertexLayout2D.Get());
	ResetMesh();
	md3dImmediateContext->VSSetShader(mVertexShader2D.Get(), nullptr, 0);
	md3dImmediateContext->PSSetShader(mPixelShader2D.Get(), nullptr, 0);
	// 顶点/索引缓冲区输入装配
	UINT strides;
	if (mode == PictureMode::FROM_FILE)
	{
		strides = sizeof(VertexPosNormalTex);
	}
	else
	{
		strides = sizeof(VertexPosNormalColor);
	}
	UINT offsets = 0;
	md3dImmediateContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &strides, &offsets);
	md3dImmediateContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	return WTRESULT_OK;
}

WTRESULT Sprite2D::Rotation(float angle,float aspectRatio)
{
	XMFLOAT2 scaling = { 1.0f, aspectRatio };
	XMFLOAT2 notScaling = { 1.0f,1.0f };
	XMFLOAT2 Translation = { 0.0f,0.0f };
	XMMATRIX ScalingMatrix = XMMatrixAffineTransformation2D(XMLoadFloat2(&scaling), XMLoadFloat2(&mRotationCenterPos), 0.0f, XMLoadFloat2(&Translation));
	XMMATRIX RotationMatrix = XMMatrixAffineTransformation2D(XMLoadFloat2(&notScaling), XMLoadFloat2(&mRotationCenterPos), angle, XMLoadFloat2(&Translation));
	if (mode == PictureMode::FROM_FILE)
	{
		for (auto& i : meshData.vertexVec)
		{
			XMStoreFloat3(&(i.pos), XMVector3Transform(XMLoadFloat3(&i.pos), XMMatrixInverse(nullptr, ScalingMatrix)*RotationMatrix*ScalingMatrix));
		}
	}
	else
	{
		for (auto& i : mMeshData.vertexVec)
		{
			XMStoreFloat3(&(i.pos), XMVector3Transform(XMLoadFloat3(&i.pos), XMMatrixInverse(nullptr, ScalingMatrix)*RotationMatrix*ScalingMatrix));
		}
	}
	
	return WTRESULT_OK;
}

WTRESULT Sprite2D::setRotationCenter(float RotationCenterX, float RotationCenterY)
{
	mRotationCenterPos.x = RotationCenterX;
	mRotationCenterPos.y = RotationCenterY;
	return WTRESULT_OK;
}

WTRESULT Sprite2D::Scaling(XMFLOAT2 scale)
{
	XMFLOAT2 Translation = { 0.0f,0.0f };
	XMVECTOR RotationCenterPos = XMLoadFloat2(&mRotationCenterPos);
	XMMATRIX TransformationMatrix = XMMatrixAffineTransformation2D(XMLoadFloat2(&scale), RotationCenterPos, 0.0f, XMLoadFloat2(&Translation));
	XMMATRIX TranslationMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&GetPosition()));
	XMMATRIX TranslationMatrixInverse = XMMatrixInverse(nullptr, TranslationMatrix);
	if (mode == PictureMode::FROM_FILE)
	{
		for (auto& i : meshData.vertexVec)
		{
			XMStoreFloat3(&(i.pos), XMVector3Transform(XMLoadFloat3(&i.pos), TranslationMatrixInverse*TransformationMatrix*TranslationMatrix));
		}
	}
	else
	{
		for (auto& i : mMeshData.vertexVec)
		{
			XMStoreFloat3(&(i.pos), XMVector3Transform(XMLoadFloat3(&i.pos), TranslationMatrixInverse*TransformationMatrix*TranslationMatrix));
		}
	}
	
	return WTRESULT_OK;
}

WTRESULT Sprite2D::Translation(XMFLOAT2 translation)
{
	XMFLOAT2 scaling = { 1.0f,1.0f };
	XMMATRIX TransformationMatrix = XMMatrixAffineTransformation2D(XMLoadFloat2(&scaling), XMLoadFloat2(&mRotationCenterPos), 0.0f, XMLoadFloat2(&translation));
	if (mode == PictureMode::FROM_FILE)
	{
		for (auto& i : meshData.vertexVec)
		{
			XMStoreFloat3(&(i.pos), XMVector3Transform(XMLoadFloat3(&i.pos), TransformationMatrix));
		}
	}
	else
	{
		for (auto& i : mMeshData.vertexVec)
		{
			XMStoreFloat3(&(i.pos), XMVector3Transform(XMLoadFloat3(&i.pos), TransformationMatrix));
		}
	}
	return WTRESULT_OK;
}

WTRESULT Sprite2D::SetTexIndex(UINT index)
{
	this->TexIndex = index;
	return WTRESULT_OK;
}

WTRESULT Sprite2D::Draw()
{
	//创建并设置常量缓冲区
	ComPtr<ID3D11Buffer> cBuffer = nullptr;
	md3dImmediateContext->VSGetConstantBuffers(0, 1, cBuffer.GetAddressOf());

	CBChangesEveryDrawing mCBDrawing;
	mCBDrawing.world = XMLoadFloat4x4(&mWorldMatrix);
	mCBDrawing.worldInvTranspose = XMMatrixTranspose(XMMatrixInverse(nullptr, mCBDrawing.world));
	mCBDrawing.texTransform = XMLoadFloat4x4(&mTexTransform);
	md3dImmediateContext->UpdateSubresource(cBuffer.Get(), 0, nullptr, &mCBDrawing, 0, 0);

	// 设置纹理
	if (this->mode == PictureMode::FROM_FILE)
	{
		md3dImmediateContext->PSSetShaderResources(0, 1, mTexture[TexIndex].GetAddressOf());
	}
	//设置采样器
	md3dImmediateContext->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());
	// 可以开始绘制
	md3dImmediateContext->DrawIndexed(mIndexCount, 0, 0);
	return WTRESULT_OK;
}

void Sprite2D::Create2DShow(const DirectX::XMFLOAT2 & center, const DirectX::XMFLOAT2 & scale, const float deepth, const DirectX::XMFLOAT4& color)
{
	WTR(Create2DShow(center.x, center.y, scale.x, scale.y, deepth, color.x, color.y, color.z, color.w));
}

WTRESULT Sprite2D::Create2DShow(float centerX, float centerY, float scaleX, float scaleY, float deepth,float colorR,float colorG,float colorB,float colorA)
{
	if (this->mode == PictureMode::FROM_FILE)
	{
		meshData.vertexVec.push_back({ XMFLOAT3(centerX - scaleX, centerY - scaleY, deepth), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) });
		meshData.vertexVec.push_back({ XMFLOAT3(centerX - scaleX, centerY + scaleY, deepth), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) });
		meshData.vertexVec.push_back({ XMFLOAT3(centerX + scaleX, centerY + scaleY, deepth), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) });
		meshData.vertexVec.push_back({ XMFLOAT3(centerX + scaleX, centerY - scaleY, deepth), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) });

		meshData.indexVec = { 0, 1, 2, 2, 3, 0 };
	}
	else
	{
		mMeshData.vertexVec.push_back({ XMFLOAT3(centerX - scaleX, centerY - scaleY, deepth), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(colorR, colorG, colorB, colorA) });
		mMeshData.vertexVec.push_back({ XMFLOAT3(centerX - scaleX, centerY + scaleY, deepth), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(colorR, colorG, colorB, colorA) });
		mMeshData.vertexVec.push_back({ XMFLOAT3(centerX + scaleX, centerY + scaleY, deepth), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(colorR, colorG, colorB, colorA) });
		mMeshData.vertexVec.push_back({ XMFLOAT3(centerX + scaleX, centerY - scaleY, deepth), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(colorR, colorG, colorB, colorA) });

		mMeshData.indexVec = { 0, 1, 2, 2, 3, 0 };
	}
	return WTRESULT_OK;
}

WTRESULT Sprite2D::SetBuffer()
{
	// 释放旧资源
	mVertexBuffer.Reset();
	mIndexBuffer.Reset();

	// 设置顶点缓冲区描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DEFAULT;
	if (this->mode == PictureMode::FROM_FILE)
	{
		vbd.ByteWidth = (UINT)meshData.vertexVec.size() * sizeof(VertexPosNormalTex);
	}
	else
	{
		vbd.ByteWidth = (UINT)mMeshData.vertexVec.size() * sizeof(VertexPosNormalColor);
	}
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	if (this->mode == PictureMode::FROM_FILE)
	{
		InitData.pSysMem = meshData.vertexVec.data();
	}
	else
	{
		InitData.pSysMem = mMeshData.vertexVec.data();
	}
	HR(md3dDevice->CreateBuffer(&vbd, &InitData, mVertexBuffer.GetAddressOf()));


	// 设置索引缓冲区描述
	if (this->mode == PictureMode::FROM_FILE)
	{
		mIndexCount = (int)meshData.indexVec.size();
	}
	else
	{
		mIndexCount = (int)mMeshData.indexVec.size();
	}
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(WORD) * mIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	// 新建索引缓冲区
	if (this->mode == PictureMode::FROM_FILE)
	{
		InitData.pSysMem = meshData.indexVec.data();
	}
	else
	{
		InitData.pSysMem = mMeshData.indexVec.data();
	}
	HR(md3dDevice->CreateBuffer(&ibd, &InitData, mIndexBuffer.GetAddressOf()));
	return WTRESULT_OK;
}

void Sprite2D::ResetMesh()
{
	WTR(SetBuffer());
}

WTRESULT Sprite2D::ResetPosition()
{
	if (mode == PictureMode::FROM_FILE)
	{
		this->meshData = this->meshDataBackups;
	}
	else
	{
		this->mMeshData = this->mMeshDataBack;
	}
	return WTRESULT_OK;
}

WTRESULT Sprite2D::SetCanSee(bool canSee)
{
	this->isCanSee = canSee;
	return WTRESULT_OK;
}

bool Sprite2D::GetCanSee()
{
	return this->isCanSee;
}
