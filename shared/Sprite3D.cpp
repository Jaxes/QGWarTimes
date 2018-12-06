#include "Sprite3D.h"
using std::ifstream;
using std::string;
using namespace DirectX;

Sprite3D::Sprite3D(
	ComPtr<ID3D11VertexShader> VertexShader3D, 
	ComPtr<ID3D11PixelShader> PixelShader3D,
	ComPtr<ID3D11InputLayout> vertexLayout3D,
	ComPtr<ID3D11Device> device, 
	ModelMode mode,
	string SourceDir,
	ComPtr<ID3D11SamplerState> samplerState)
	:GameObject(device)
{
	std::string TextureDir = "Texture/" + SourceDir + "/";
	std::string TextureName = SourceDir + ".wtv";

	// 初始化纹理
	ComPtr<ID3D11ShaderResourceView> tempTexture;

	if (!samplerState)
	{
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
	}
	else
	{
		this->mSamplerState = samplerState;
	}
	
	switch (mode)
	{
	case ModelMode::FROM_FILE:
		//读取顶点
		WTR(ReadVertexFromFile(TextureDir, TextureName));
		WTR(ReadVertexAttrFromMTL(TextureDir, SourceDir + ".mtl"));
		//读取材质
		WTR(CreateTextureView(TextureDir));
		break;
	case ModelMode::BOX:
	case ModelMode::CYLINDER:
	case ModelMode::PLANE:
	case ModelMode::SPHERE:
		HR(CreateDDSTextureFromFile(md3dDevice.Get(), toWString(SourceDir).c_str(), nullptr, tempTexture.GetAddressOf()));
		mTexture.push_back(tempTexture);
		break;
	}
	this->mVertexShader3D = VertexShader3D;
	this->mPixelShader3D = PixelShader3D;
	this->mode = mode;
	this->mUp = { 0.0f,1.0f,0.0f };
	this->mLook = { -1.0f,0.0f,0.0f };
	this->mRight = { 0.0f,0.0f,1.0f };
	this->mFront = this->mLook;
	this->mVertexLayout3D = vertexLayout3D;
	this->isCanSee = true;
}

Sprite3D::~Sprite3D()
{
}

WTRESULT Sprite3D::init()
{
	return SetBuffer();
}

WTRESULT Sprite3D::init(DirectX::XMFLOAT3 center, DirectX::XMFLOAT2 planeSize, DirectX::XMFLOAT2 maxTexCoord)
{
	CreatePlane(center, planeSize, maxTexCoord);
	ResetMesh();
	md3dImmediateContext->VSSetShader(mVertexShader3D.Get(), nullptr, 0);
	md3dImmediateContext->PSSetShader(mPixelShader3D.Get(), nullptr, 0);
	//输入装配阶段的顶点/索引缓冲区设置
	UINT strides = sizeof(VertexPosNormalTex);
	UINT offsets = 0;
	md3dImmediateContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &strides, &offsets);
	md3dImmediateContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	return WTRESULT_OK;
}

WTRESULT Sprite3D::init(float centerX, float centerY, float centerZ, float width, float depth, float texU, float texV)
{
	CreatePlane(centerX, centerY, centerZ, width, depth, texU, texV);
	ResetMesh();
	md3dImmediateContext->VSSetShader(mVertexShader3D.Get(), nullptr, 0);
	md3dImmediateContext->PSSetShader(mPixelShader3D.Get(), nullptr, 0);
	//输入装配阶段的顶点/索引缓冲区设置
	UINT strides = sizeof(VertexPosNormalTex);
	UINT offsets = 0;
	md3dImmediateContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &strides, &offsets);
	md3dImmediateContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	return WTRESULT_OK;
}

WTRESULT Sprite3D::init(float radius, int levels, int slices)
{
	CreateSphere(radius, levels, slices);
	ResetMesh();
	md3dImmediateContext->VSSetShader(mVertexShader3D.Get(), nullptr, 0);
	md3dImmediateContext->PSSetShader(mPixelShader3D.Get(), nullptr, 0);
	//输入装配阶段的顶点/索引缓冲区设置
	UINT strides = sizeof(VertexPosNormalTex);
	UINT offsets = 0;
	md3dImmediateContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &strides, &offsets);
	md3dImmediateContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	return WTRESULT_OK;
}

WTRESULT Sprite3D::init(float width, float height, float depth)
{
	CreateBox(width, height, depth);
	ResetMesh();
	md3dImmediateContext->VSSetShader(mVertexShader3D.Get(), nullptr, 0);
	md3dImmediateContext->PSSetShader(mPixelShader3D.Get(), nullptr, 0);
	//输入装配阶段的顶点/索引缓冲区设置
	UINT strides = sizeof(VertexPosNormalTex);
	UINT offsets = 0;
	md3dImmediateContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &strides, &offsets);
	md3dImmediateContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	return WTRESULT_OK;
}

WTRESULT Sprite3D::init(float radius, float height, int slices)
{
	CreateCylinder(radius, height, slices);
	ResetMesh();
	md3dImmediateContext->VSSetShader(mVertexShader3D.Get(), nullptr, 0);
	md3dImmediateContext->PSSetShader(mPixelShader3D.Get(), nullptr, 0);
	//输入装配阶段的顶点/索引缓冲区设置
	UINT strides = sizeof(VertexPosNormalTex);
	UINT offsets = 0;
	md3dImmediateContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &strides, &offsets);
	md3dImmediateContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	return WTRESULT_OK;
}

WTRESULT Sprite3D::Update()
{
	md3dImmediateContext->IASetInputLayout(mVertexLayout3D.Get());
	//ResetMesh();
	md3dImmediateContext->VSSetShader(mVertexShader3D.Get(), nullptr, 0);
	md3dImmediateContext->PSSetShader(mPixelShader3D.Get(), nullptr, 0);
	// 输入装配阶段的顶点/索引缓冲区设置
	UINT stride = sizeof(VertexPosNormalTex);	// 跨越字节数
	UINT offset = 0;							// 起始偏移量
	md3dImmediateContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	return WTRESULT_OK;
}

WTRESULT Sprite3D::RotationX(float angle)
{
	XMMATRIX WorldMatrix = XMLoadFloat4x4(&(this->mWorldMatrix));
	XMMATRIX TranslationMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&GetPosition()));
	XMMATRIX TranslationMatrixInverse = XMMatrixInverse(nullptr, TranslationMatrix);
	XMMATRIX RotationMatrix = TranslationMatrixInverse * XMMatrixRotationX(angle)*TranslationMatrix;
	XMStoreFloat4x4(&(this->mWorldMatrix), WorldMatrix*RotationMatrix);

	XMMATRIX R = XMMatrixRotationX(angle);
	XMStoreFloat3(&mRight, XMVector3TransformNormal(XMLoadFloat3(&mRight), R));
	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));

	XMVECTOR Right = XMLoadFloat3(&mRight);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Front = XMVector3Normalize(XMVector3Cross(Right, Up));

	XMStoreFloat3(&(this->mFront), Front);

	return WTRESULT_OK;
}

WTRESULT Sprite3D::RotationY(float angle)
{
	XMMATRIX WorldMatrix = XMLoadFloat4x4(&(this->mWorldMatrix));
	XMMATRIX TranslationMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&GetPosition()));
	XMMATRIX TranslationMatrixInverse = XMMatrixInverse(nullptr, TranslationMatrix);
	XMMATRIX RotationMatrix = TranslationMatrixInverse * XMMatrixRotationY(angle) * TranslationMatrix;
	XMStoreFloat4x4(&(this->mWorldMatrix), WorldMatrix*RotationMatrix);

	XMMATRIX R = XMMatrixRotationY(angle);
	XMStoreFloat3(&mRight, XMVector3TransformNormal(XMLoadFloat3(&mRight), R));
	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));

	XMVECTOR Right = XMLoadFloat3(&mRight);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Front = XMVector3Normalize(XMVector3Cross(Right, Up));

	XMStoreFloat3(&(this->mFront), Front);

	return WTRESULT_OK;
}

WTRESULT Sprite3D::RotationZ(float angle)
{
	XMMATRIX WorldMatrix = XMLoadFloat4x4(&(this->mWorldMatrix));
	XMMATRIX TranslationMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&GetPosition()));
	XMMATRIX TranslationMatrixInverse = XMMatrixInverse(nullptr, TranslationMatrix);
	XMMATRIX RotationMatrix = TranslationMatrixInverse * XMMatrixRotationZ(angle)*TranslationMatrix;
	XMStoreFloat4x4(&(this->mWorldMatrix), WorldMatrix*RotationMatrix);

	XMMATRIX R = XMMatrixRotationZ(angle);
	XMStoreFloat3(&mRight, XMVector3TransformNormal(XMLoadFloat3(&mRight), R));
	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));

	XMVECTOR Right = XMLoadFloat3(&mRight);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Front = XMVector3Normalize(XMVector3Cross(Right, Up));

	XMStoreFloat3(&(this->mFront), Front);
	return WTRESULT_OK;
}

WTRESULT Sprite3D::RotationAround(XMFLOAT3 axis, XMFLOAT3 axisPos, float angle, float Max, XMFLOAT3 ReferenceVector)
{
	if (Max != -1)
	{
		float* Angle = XMVector3AngleBetweenVectors(XMLoadFloat3(&ReferenceVector), XMLoadFloat3(&this->mLook)).m128_f32;
		if (angle < 0 && this->mLook.y > 0 && -angle + *Angle > Max)
		{
			angle = -(Max - *Angle);
		}
		else if (angle > 0 && this->mLook.y < 0 && angle + *Angle > Max)
		{
			angle = Max - *Angle;
		}
	}
	XMMATRIX WorldMatrix = XMLoadFloat4x4(&(this->mWorldMatrix));
	XMMATRIX TranslationMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&axisPos));
	XMMATRIX TranslationMatrixInverse = XMMatrixInverse(nullptr, TranslationMatrix);
	XMMATRIX RotationMatrix = TranslationMatrixInverse * XMMatrixRotationAxis(XMLoadFloat3(&axis), angle) * TranslationMatrix;
	XMStoreFloat4x4(&(this->mWorldMatrix), WorldMatrix*RotationMatrix);
	
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&axis), angle);
	XMStoreFloat3(&mRight, XMVector3TransformNormal(XMLoadFloat3(&mRight), R));
	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));

	XMVECTOR Right = XMLoadFloat3(&mRight);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Front = XMVector3Normalize(XMVector3Cross(Right, Up));

	XMStoreFloat3(&(this->mFront), Front);
	return WTRESULT_OK;
}

WTRESULT Sprite3D::Scaling(XMFLOAT3 scale)
{
	//对模型进行放缩
	float sum[3] = { 0 };
	for (auto & i : meshData.vertexs) {
		sum[0] += i.pos.x;
		sum[1] += i.pos.y;
		sum[2] += i.pos.z;
	}
	for (auto& i : sum) {
		i /= meshData.vertexs.size();
	}

	for (auto & i : meshData.vertexs) {
		i.pos.x -= sum[0];
		i.pos.x *= scale.x;
		i.pos.y -= sum[1];
		i.pos.y *= scale.x;
		i.pos.z -= sum[2];
		i.pos.z *= scale.y;
	}
	return WTRESULT_OK;
}

WTRESULT Sprite3D::ReadVertexFromFile(string dir, string filename)
{
	FILE* infile;
	errno_t r;
	uint64_t digit;
	r = fopen_s(&infile, (dir + filename).c_str(), "rb");
	if (r) {
		wchar_t wpstr[64];
		_wcserror_s(wpstr, r);
		WTERROR("文件打开失败: %s", wpstr);
		return WTRESULT_FileOpenFailed;
	}

	fread(&digit, sizeof(digit), 1, infile);
	meshData.vertexs.resize(digit);
	fread(meshData.vertexs.data(), sizeof(VertexPosNormalTex), meshData.vertexs.size(), infile);

	fread(&digit, sizeof(digit), 1, infile);
	meshData.indexes.resize(digit);
	fread(meshData.indexes.data(), sizeof(uint32_t), meshData.indexes.size(), infile);

	fread(&digit, sizeof(digit), 1, infile);
	meshData.subModels.resize(digit);
	for (auto & i : meshData.subModels) {
		fread(&digit, sizeof(digit), 1, infile);
		i.name.resize(digit);
		fread(&i.name[0], sizeof(char), digit, infile);

		fread(&digit, sizeof(digit), 1, infile);
		i.usemtl.resize(digit);
		fread(&i.usemtl[0], sizeof(char), digit, infile);

		fread(&i.startIndex, sizeof(i.startIndex), 1, infile);
		fread(&i.count, sizeof(i.count), 1, infile);
	}

	uint32_t sum = 0;
	for (auto const& i : meshData.subModels) {
		sum += i.count;
	}
	if (sum != meshData.indexes.size()) {
		WTERROR("读取错误: sum = %d, indexes.count = %d", sum, meshData.indexes.size());
	}

	fclose(infile);
	return WTRESULT_OK;
}

WTRESULT Sprite3D::ReadVertexAttrFromMTL(string dir, string filename) {
	std::ifstream infile;
	infile.open(dir + filename, std::ios::in);
	ASSERT_RETURN(infile.fail() == false, WTRESULT_FileOpenFailed);
	while (infile.good()) {
		std::string lineStr;
		getline(infile, lineStr);
		std::istringstream line(lineStr);
		std::string firstWord;
		line >> firstWord;
		if (firstWord.size() == 0) {
			continue;
		}
		else if (firstWord[0] == '#') {
			continue;
		}
		else if (firstWord == "newmtl") {
			mModelAttr.push_back({});
			line.ignore(1);
			getline(line, mModelAttr.back().name);
		}
		else if (firstWord == "Ka") {
			if (!(line >> mModelAttr.back().Ka.x >> mModelAttr.back().Ka.y >> mModelAttr.back().Ka.z)) {
				WTERROR(L"Ka 读取错误");
			}
			mModelAttr.back().KaValid = true;
		}
		else if (firstWord == "Kd") {
			if (!(line >> mModelAttr.back().Kd.x >> mModelAttr.back().Kd.y >> mModelAttr.back().Kd.z)) {
				WTERROR(L"Kd 读取错误");
			}
			mModelAttr.back().KdValid = true;
		}
		else if (firstWord == "Ks") {
			if (!(line >> mModelAttr.back().Ks.x >> mModelAttr.back().Ks.y >> mModelAttr.back().Ks.z)) {
				WTERROR(L"Ks 读取错误");
			}
			mModelAttr.back().KsValid = true;
		}
		else if (firstWord == "illum") {
			if (!(line >> mModelAttr.back().illum)) {
				WTERROR(L"illum 读取错误");
			}
			mModelAttr.back().illum = true;
		}
		else if (firstWord == "Ns") {
			if (!(line >> mModelAttr.back().Ns)) {
				WTERROR(L"Ns 读取错误");
			}
			mModelAttr.back().NsValid = true;
		}
		else if (firstWord == "ColorFilePath") {
			line.ignore(1);
			getline(line, mModelAttr.back().colorFilePath);
		}
		else if (firstWord == "DiffuseFilePath") {
			line.ignore(1);
			getline(line, mModelAttr.back().diffuseFilePath);
		}
		else if (firstWord == "ReflexFilePath") {
			line.ignore(1);
			getline(line, mModelAttr.back().reflexFilePath);
		}
	}
	return 0;
}

WTRESULT Sprite3D::CreateTextureView(string dir) {
	for (auto const& i : mModelAttr)
	{
		ComPtr<ID3D11ShaderResourceView> tempTexture;

		if (i.colorFilePath.size() > 0) {
			std::wstring colorFilePath = toWString(dir + i.colorFilePath);
			WTINFO("读取文件: %s", colorFilePath.c_str());
			HR(CreateDDSTextureFromFile(md3dDevice.Get(), colorFilePath.c_str(), nullptr, tempTexture.GetAddressOf()));
			mTexture.push_back(tempTexture);
		}
		else {
			mTexture.push_back(nullptr);
		}
		// 加载材质属性
		CBMaterial materialMap;
		ZeroMemory(&materialMap, sizeof(materialMap));
		if (i.KaValid) {
			materialMap.material.Ambient = DirectX::XMFLOAT4(i.Ka.x, i.Ka.y, i.Ka.z, 1.0f);
		}
		else {
			materialMap.material.Ambient = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		if (i.KdValid) {
			materialMap.material.Diffuse = DirectX::XMFLOAT4(i.Kd.x, i.Kd.y, i.Kd.z, 1.0f);
		}
		else {
			materialMap.material.Diffuse = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		if (i.KsValid) {
			materialMap.material.Specular = DirectX::XMFLOAT4(i.Ks.x, i.Ks.y, i.Ks.z, 1.0f);
		}
		else {
			materialMap.material.Specular = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		if (i.NsValid) {
			materialMap.material.Specular.w = static_cast<float>(i.Ns);
		}
		mMaterial.push_back(materialMap.material);
		D3D11_BUFFER_DESC bd = { 0 };
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(CBMaterial);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		D3D11_SUBRESOURCE_DATA sd = { 0 };
		sd.pSysMem = &materialMap;
		ComPtr<ID3D11Buffer> material;
		HR(md3dDevice->CreateBuffer(&bd, &sd, &material));
		mMaterials.push_back(material);
	}
	for (auto const & i : meshData.subModels) {
		if (i.count == 0)continue;
		size_t texIndex = -1;
		for (size_t j = 0; j < mModelAttr.size(); j++) {
			if (i.usemtl == mModelAttr[j].name) {
				texIndex = j;
				break;
			}
		}
		if (texIndex != (size_t)(-1)) {
			mModelIndexRanges.push_back({ i.startIndex, i.count, texIndex ,i.name});
		}
		else {
			WTERROR("找不到材质: %s", i.usemtl.c_str());
		}
	}
	return WTRESULT_OK;
}

WTRESULT Sprite3D::Translation(DirectX::XMFLOAT3 translation)
{
	XMMATRIX WorldMatrix = XMLoadFloat4x4(&(this->mWorldMatrix));
	XMMATRIX TranslationMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&translation));
	WorldMatrix *= TranslationMatrix;
	XMStoreFloat4x4(&(this->mWorldMatrix), WorldMatrix);
	return WTRESULT_OK;
}

WTRESULT Sprite3D::SetPosition(const DirectX::XMFLOAT3 & pos)
{
	this->mWorldMatrix.m[3][0] = pos.x;
	this->mWorldMatrix.m[3][1] = pos.y;
	this->mWorldMatrix.m[3][2] = pos.z;
	return WTRESULT_OK;
}

XMFLOAT3 Sprite3D::GetLook()
{
	return this->mLook;
}

DirectX::XMFLOAT3 Sprite3D::GetUp()
{
	return this->mUp;
}

DirectX::XMFLOAT3 Sprite3D::GetRight()
{
	return this->mRight;
}

DirectX::XMFLOAT3 Sprite3D::GetFront()
{
	return this->mFront;
}

void Sprite3D::Draw()
{
	md3dImmediateContext->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());
	if (this->mode == ModelMode::FROM_FILE)
	{
		for (auto const& i : mModelIndexRanges) {

			// 设置材质属性
			md3dImmediateContext->PSSetConstantBuffers(4, 1, mMaterials[i.texIndex].GetAddressOf());
			XMVECTOR Translation = XMVectorSet(0.0f, 0.4f, 0.0f, 0.0f);

			//设置三大变换矩阵
			ComPtr<ID3D11Buffer> cBuffer = nullptr;
			md3dImmediateContext->VSGetConstantBuffers(0, 1, cBuffer.GetAddressOf());
			//todo:到时候再把相应的缓冲区添加上去
			CBChangesEveryDrawing mCBDrawing;
			if (i.mName != "手臂.1")
			{
				mCBDrawing.world = XMLoadFloat4x4(&mWorldMatrix);
			}
			else
			{
				XMMATRIX WorldMatrix = XMLoadFloat4x4(&(this->mWorldMatrix));
				XMMATRIX TranslationMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&GetPosition()));
				XMMATRIX TranslationMatrixInverse = XMMatrixInverse(nullptr, TranslationMatrix);
				XMMATRIX RotationMatrix = TranslationMatrixInverse * XMMatrixRotationAxis(XMLoadFloat3(&(this->mRight)),XM_PIDIV2)*TranslationMatrix;
				mCBDrawing.world = WorldMatrix * RotationMatrix * XMMatrixTranslationFromVector(XMVectorSet(0.0f, 0.4f, 0.0f, 0.0f));
			}
			mCBDrawing.worldInvTranspose = XMMatrixTranspose(XMMatrixInverse(nullptr, mCBDrawing.world));
			mCBDrawing.texTransform = XMLoadFloat4x4(&mTexTransform);

			md3dImmediateContext->UpdateSubresource(cBuffer.Get(), 0, nullptr, &mCBDrawing, 0, 0);

			// 设置纹理
			md3dImmediateContext->PSSetShaderResources(0, 1, mTexture[i.texIndex].GetAddressOf());
			md3dImmediateContext->DrawIndexed(i.size, i.start, 0);
		}
	}
	else
	{
		// 设置顶点/索引缓冲区
		UINT strides = sizeof(VertexPosNormalTex);
		UINT offsets = 0;
		md3dImmediateContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &strides, &offsets);
		md3dImmediateContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

		// 获取之前已经绑定到渲染管线上的常量缓冲区并进行修改
		ComPtr<ID3D11Buffer> cBuffer = nullptr;
		md3dImmediateContext->VSGetConstantBuffers(0, 1, cBuffer.GetAddressOf());
		CBChangesEveryDrawing mCBDrawing;
		mCBDrawing.world = XMLoadFloat4x4(&mWorldMatrix);
		mCBDrawing.worldInvTranspose = XMMatrixTranspose(XMMatrixInverse(nullptr, mCBDrawing.world));
		mCBDrawing.texTransform = XMLoadFloat4x4(&mTexTransform);

		// 更改材质
		ComPtr<ID3D11Buffer> cBuffer2 = nullptr;
		md3dImmediateContext->PSGetConstantBuffers(4, 1, cBuffer2.GetAddressOf());
		CBMaterial materialDrawing;
		materialDrawing.material.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		materialDrawing.material.Diffuse = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
		materialDrawing.material.Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 50.0f);

		md3dImmediateContext->UpdateSubresource(cBuffer.Get(), 0, nullptr, &mCBDrawing, 0, 0);
		md3dImmediateContext->UpdateSubresource(cBuffer2.Get(), 0, nullptr, &materialDrawing, 0, 0);
		// 设置纹理
		md3dImmediateContext->PSSetShaderResources(0, 1, mTexture[0].GetAddressOf());
		// 可以开始绘制
		md3dImmediateContext->DrawIndexed(mIndexCount, 0, 0);
	}
}

WTRESULT Sprite3D::SetBuffer()
{
	// 释放旧资源
	mVertexBuffer.Reset();
	mIndexBuffer.Reset();

	// 设置顶点缓冲区描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DEFAULT;
	if (this->mode == ModelMode::FROM_FILE)
	{
		vbd.ByteWidth = (UINT)meshData.vertexs.size() * sizeof(VertexPosNormalTex);
	}
	else
	{
		vbd.ByteWidth = (UINT)mMeshData.vertexVec.size() * sizeof(VertexPosNormalTex);
	}
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	if (this->mode == ModelMode::FROM_FILE) 
	{
		InitData.pSysMem = meshData.vertexs.data();
	}
	else
	{
		InitData.pSysMem = mMeshData.vertexVec.data();
	}
	HR(md3dDevice->CreateBuffer(&vbd, &InitData, mVertexBuffer.GetAddressOf()));

	// 设置索引缓冲区描述
	if (this->mode == ModelMode::FROM_FILE)
	{
		mIndexCount = (int)meshData.indexes.size();
	}
	else
	{
		mIndexCount = (int)mMeshData.indexVec.size();
	}
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(uint32_t) * mIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	// 新建索引缓冲区
	if (this->mode == ModelMode::FROM_FILE)
	{
		InitData.pSysMem = meshData.indexes.data();
	}
	else
	{
		InitData.pSysMem = mMeshData.indexVec.data();
	}
	HR(md3dDevice->CreateBuffer(&ibd, &InitData, mIndexBuffer.GetAddressOf()));
	return WTRESULT_OK;
}

WTRESULT Sprite3D::ResetMesh()
{
	return SetBuffer();
}

WTRESULT Sprite3D::SetCanSee(bool canSee)
{
	this->isCanSee = canSee;
	return WTRESULT_OK;
}

bool Sprite3D::GetCanSee()
{
	return this->isCanSee;
}

bool Sprite3D::CheckShooted(XMFLOAT3 Origin, XMFLOAT3 Direction)
{
	float x;
	FXMVECTOR O = XMLoadFloat3(&Origin);
	FXMVECTOR Dir = XMLoadFloat3(&Direction);
	/*for (auto const& i : this->CollisionBox)
	{
		if (i.Intersects(O, Dir, x))
		{
			WTINFO(L"被击中！");
			return true;
		}
	}*/
	DirectX::BoundingBox collisionbox;
	this->mCollisionBox.Transform(collisionbox, XMLoadFloat4x4(&(this->mWorldMatrix)));
	if (collisionbox.Intersects(O, Dir, x))
	{
		WTINFO(L"被击中！");
		return true;
	}
	WTINFO(L"未击中！");
	return false;
}

bool Sprite3D::CheckCollision(Sprite3D& Other)
{
	/*for (auto const& i : this->CollisionBox)
	{
		for (auto const& j : Other.GetCollisionBox())
		{
			if (i.Intersects(j))
			{
				WTINFO(L"碰撞");
				return true;
			}
		}
	}*/
	DirectX::BoundingBox collisionbox;
	this->mCollisionBox.Transform(collisionbox, XMLoadFloat4x4(&(this->mWorldMatrix)));
	if (collisionbox.Intersects(Other.GetCollisionBox()))
	{
		WTINFO(L"碰撞");
		return true;
	}
	return false;
}

DirectX::BoundingBox Sprite3D::GetCollisionBox()
{
	DirectX::BoundingBox collisionbox;
	this->mCollisionBox.Transform(collisionbox, XMLoadFloat4x4(&(this->mWorldMatrix)));
	return collisionbox;
}

WTRESULT Sprite3D::SetCollisionBox()
{
	/*std::vector<DirectX::XMFLOAT3> pTemp;
	this->CollisionBox.resize(mModelIndexRanges.size());
	int j;
	for (int i = 0; i < mModelIndexRanges.size(); i++)
	{
		pTemp.clear();
		for (j = mModelIndexRanges[i].start; j < mModelIndexRanges[i].start + mModelIndexRanges[i].size; j++)
		{
			assert(j >= 0);

			DirectX::XMFLOAT3 Vertexs = this->meshData.vertexs[this->meshData.indexes[j]].pos;
			if (find_if(pTemp.begin(), pTemp.end(), isEqual(Vertexs)) == pTemp.end())
			{
				pTemp.push_back(Vertexs);
			}
		}
		if (i == 32)
			WTINFO(L"j的值为%d,size为%d,vertexs大小为%d", j, mModelIndexRanges[i].start + mModelIndexRanges[i].size, this->meshData.vertexs.size());
		WTINFO(L"i的值为%d,size为%d,vertexs大小为%d", i, pTemp.size(), this->meshData.vertexs.size());
		BoundingBox::CreateFromPoints(this->CollisionBox[i], pTemp.size(), pTemp.data(), sizeof(XMFLOAT3));
	}*/
	std::vector<DirectX::XMFLOAT3> pPoint;
	for (int i = 0; i < this->meshData.vertexs.size(); i++)
	{
		pPoint.push_back(this->meshData.vertexs[i].pos);
	}
	BoundingBox::CreateFromPoints(this->mCollisionBox, pPoint.size(), pPoint.data(), sizeof(XMFLOAT3));
	return WTRESULT_OK;
}

WTRESULT Sprite3D::DataSync(std::shared_ptr<WarTimes::CObject> pObject)
{
	std::mutex Sync_lock;
	std::lock_guard<std::mutex> Sync_lock_helper(Sync_lock);
	pObject->mFront = XMLoadFloat3(&(this->mFront));
	pObject->mLook = XMLoadFloat3(&(this->mLook));
	pObject->mUp = XMLoadFloat3(&(this->mUp));
	pObject->mRight = XMLoadFloat3(&(this->mRight));
	pObject->mWorldMatrix = this->mWorldMatrix;
	return WTRESULT_OK;
}

WTRESULT Sprite3D::DataGet(std::shared_ptr<WarTimes::CObject> pObject)
{
	if (!XMVector3Equal(pObject->mFront, XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)))
	{
		XMStoreFloat3(&(this->mFront), pObject->mFront);
	}
	if (!XMVector3Equal(pObject->mLook, XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)))
	{
		XMStoreFloat3(&(this->mLook), pObject->mLook);
	}
	if (!XMVector3Equal(pObject->mRight, XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)))
	{
		XMStoreFloat3(&(this->mRight), pObject->mRight);
	}
	if (!XMVector3Equal(pObject->mUp, XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)))
	{
		XMStoreFloat3(&(this->mUp), pObject->mUp);
	}
	if (pObject->mWorldMatrix._11 != 0)
	{
		this->mWorldMatrix = pObject->mWorldMatrix;
	}
	return WTRESULT_OK;
}

std::wstring Sprite3D::toWString(std::string str) 
{
	int size = MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.size(), 0, 0);
	if (size <= 0) {
		WTERROR("多字节转Unicode失败");
		return L"";
	}
	std::wstring wstr;
	wstr.resize(size + 1);
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.size(), &wstr[0], (int)wstr.size());
	wstr.back() = L'\0';
	if (wstr.front() == 0xFEFF) {
		wstr.erase(wstr.front());
	}
	return wstr;
}

void Sprite3D::CreateSphere(float radius, int levels, int slices)
{
	float phi = 0.0f, theta = 0.0f;
	float per_phi = XM_PI / levels;
	float per_theta = XM_2PI / slices;
	float x, y, z;

	// 放入顶端点
	mMeshData.vertexVec.push_back({ XMFLOAT3(0.0f, radius, 0.0f) , XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) });


	for (int i = 1; i < levels; ++i)
	{
		phi = per_phi * i;
		for (int j = 0; j < slices; ++j)
		{
			theta = per_theta * j;
			x = radius * sinf(phi) * cosf(theta);
			y = radius * cosf(phi);
			z = radius * sinf(phi) * sinf(theta);
			// 计算出局部坐标、法向量和纹理坐标
			XMFLOAT3 pos = XMFLOAT3(x, y, z), normal;
			XMStoreFloat3(&normal, XMVector3Normalize(XMLoadFloat3(&pos)));
			mMeshData.vertexVec.push_back({ pos, normal, XMFLOAT2(theta / XM_2PI, phi / XM_PI) });
		}
	}
	// 放入底端点
	mMeshData.vertexVec.push_back({ XMFLOAT3(0.0f, -radius, 0.0f) , XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) });

	// 逐渐放入索引
	if (levels > 1)
	{
		for (int j = 1; j <= slices; ++j)
		{
			mMeshData.indexVec.push_back(0);
			mMeshData.indexVec.push_back(j % slices + 1);
			mMeshData.indexVec.push_back(j);

		}
	}


	for (int i = 1; i < levels - 1; ++i)
	{
		for (int j = 1; j <= slices; ++j)
		{

			mMeshData.indexVec.push_back((i - 1) * slices + j);
			mMeshData.indexVec.push_back((i - 1) * slices + j % slices + 1);
			mMeshData.indexVec.push_back(i * slices + j % slices + 1);

			mMeshData.indexVec.push_back(i * slices + j % slices + 1);
			mMeshData.indexVec.push_back(i * slices + j);
			mMeshData.indexVec.push_back((i - 1) * slices + j);

		}

	}

	// 逐渐放入索引
	if (levels > 1)
	{
		for (int j = 1; j <= slices; ++j)
		{
			mMeshData.indexVec.push_back((levels - 2) * slices + j);
			mMeshData.indexVec.push_back((levels - 2) * slices + j % slices + 1);
			mMeshData.indexVec.push_back((WORD)(mMeshData.vertexVec.size() - 1));
		}
	}
	WTINFO(L"%s", WTStrErr(WTRESULT_OK));
}

void Sprite3D::CreateBox(float width, float height, float depth)
{
	float w2 = width / 2, h2 = height / 2, d2 = depth / 2;

	mMeshData.vertexVec.resize(24);
	// 顶面
	mMeshData.vertexVec[0].pos = XMFLOAT3(-w2, h2, -d2);
	mMeshData.vertexVec[1].pos = XMFLOAT3(-w2, h2, d2);
	mMeshData.vertexVec[2].pos = XMFLOAT3(w2, h2, d2);
	mMeshData.vertexVec[3].pos = XMFLOAT3(w2, h2, -d2);
	// 底面
	mMeshData.vertexVec[4].pos = XMFLOAT3(w2, -h2, -d2);
	mMeshData.vertexVec[5].pos = XMFLOAT3(w2, -h2, d2);
	mMeshData.vertexVec[6].pos = XMFLOAT3(-w2, -h2, d2);
	mMeshData.vertexVec[7].pos = XMFLOAT3(-w2, -h2, -d2);
	// 左面
	mMeshData.vertexVec[8].pos = XMFLOAT3(-w2, -h2, d2);
	mMeshData.vertexVec[9].pos = XMFLOAT3(-w2, h2, d2);
	mMeshData.vertexVec[10].pos = XMFLOAT3(-w2, h2, -d2);
	mMeshData.vertexVec[11].pos = XMFLOAT3(-w2, -h2, -d2);
	// 右面
	mMeshData.vertexVec[12].pos = XMFLOAT3(w2, -h2, -d2);
	mMeshData.vertexVec[13].pos = XMFLOAT3(w2, h2, -d2);
	mMeshData.vertexVec[14].pos = XMFLOAT3(w2, h2, d2);
	mMeshData.vertexVec[15].pos = XMFLOAT3(w2, -h2, d2);
	// 背面
	mMeshData.vertexVec[16].pos = XMFLOAT3(w2, -h2, d2);
	mMeshData.vertexVec[17].pos = XMFLOAT3(w2, h2, d2);
	mMeshData.vertexVec[18].pos = XMFLOAT3(-w2, h2, d2);
	mMeshData.vertexVec[19].pos = XMFLOAT3(-w2, -h2, d2);
	// 正面
	mMeshData.vertexVec[20].pos = XMFLOAT3(-w2, -h2, -d2);
	mMeshData.vertexVec[21].pos = XMFLOAT3(-w2, h2, -d2);
	mMeshData.vertexVec[22].pos = XMFLOAT3(w2, h2, -d2);
	mMeshData.vertexVec[23].pos = XMFLOAT3(w2, -h2, -d2);

	for (int i = 0; i < 4; ++i)
	{
		mMeshData.vertexVec[i].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);		// 顶面
		mMeshData.vertexVec[i + 4].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);	// 底面
		mMeshData.vertexVec[i + 8].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);	// 左面
		mMeshData.vertexVec[i + 12].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);	// 右面
		mMeshData.vertexVec[i + 16].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);	// 背面
		mMeshData.vertexVec[i + 20].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);// 正面
	}

	for (int i = 0; i < 6; ++i)
	{
		mMeshData.vertexVec[i * 4].tex = XMFLOAT2(0.0f, 1.0f);
		mMeshData.vertexVec[i * 4 + 1].tex = XMFLOAT2(0.0f, 0.0f);
		mMeshData.vertexVec[i * 4 + 2].tex = XMFLOAT2(1.0f, 0.0f);
		mMeshData.vertexVec[i * 4 + 3].tex = XMFLOAT2(1.0f, 1.0f);
	}

	mMeshData.indexVec = {
		0, 1, 2, 2, 3, 0,		// 顶面
		4, 5, 6, 6, 7, 4,		// 底面
		8, 9, 10, 10, 11, 8,	// 左面
		12, 13, 14, 14, 15, 12,	// 右面
		16, 17, 18, 18, 19, 16, // 背面
		20, 21, 22, 22, 23, 20	// 正面
	};
	WTINFO(L"%s", WTStrErr(WTRESULT_OK));
}

void Sprite3D::CreateCylinder(float radius, float height, int slices)
{
	MeshData meshData = CreateCylinderNoCap(radius, height, slices);
	float h2 = height / 2;
	float theta = 0.0f;
	float per_theta = XM_2PI / slices;

	int offset = 2 * (slices + 1);
	// 放入顶端圆心
	meshData.vertexVec.push_back({ XMFLOAT3(0.0f, h2, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.5f, 0.5f) });
	// 放入顶端圆上各点
	for (int i = 0; i < slices; ++i)
	{
		theta = i * per_theta;
		meshData.vertexVec.push_back({ XMFLOAT3(cosf(theta), h2, sinf(theta)), XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT2(cosf(theta) / 2 + 0.5f, sinf(theta) / 2 + 0.5f) });
	}
	// 逐渐放入索引
	for (int i = 1; i <= slices; ++i)
	{
		meshData.indexVec.push_back(offset);
		meshData.indexVec.push_back(offset + i % slices + 1);
		meshData.indexVec.push_back(offset + i);
	}


	// 放入底部圆上各点
	for (int i = 0; i < slices; ++i)
	{
		theta = i * per_theta;
		meshData.vertexVec.push_back({ XMFLOAT3(cosf(theta), -h2, sinf(theta)), XMFLOAT3(0.0f, -1.0f, 0.0f),
			XMFLOAT2(cosf(theta) / 2 + 0.5f, sinf(theta) / 2 + 0.5f) });
	}
	// 放入底端圆心
	meshData.vertexVec.push_back({ XMFLOAT3(0.0f, -h2, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.5f, 0.5f) });

	// 逐渐放入索引
	offset += slices + 1;
	for (int i = 1; i <= slices; ++i)
	{
		meshData.indexVec.push_back(offset);
		meshData.indexVec.push_back(offset + i);
		meshData.indexVec.push_back(offset + i % slices + 1);
	}

	this->mMeshData = meshData;
	WTINFO(L"%s", WTStrErr(WTRESULT_OK));
}

MeshData Sprite3D::CreateCylinderNoCap(float radius, float height, int slices)
{
	MeshData meshData;
	float h2 = height / 2;
	float theta = 0.0f;
	float per_theta = XM_2PI / slices;

	// 放入侧面顶端点
	for (int i = 0; i <= slices; ++i)
	{
		theta = i * per_theta;
		meshData.vertexVec.push_back({ XMFLOAT3(radius * cosf(theta), h2, radius * sinf(theta)), XMFLOAT3(cosf(theta), 0.0f, sinf(theta)),
			XMFLOAT2(theta / XM_2PI, 0.0f) });
	}
	// 放入侧面底端点
	for (int i = 0; i <= slices; ++i)
	{
		theta = i * per_theta;
		meshData.vertexVec.push_back({ XMFLOAT3(radius * cosf(theta), -h2, radius * sinf(theta)), XMFLOAT3(cosf(theta), 0.0f, sinf(theta)),
			XMFLOAT2(theta / XM_2PI, 1.0f) });
	}

	// 放入索引
	for (int i = 0; i < slices; ++i)
	{
		meshData.indexVec.push_back(i);
		meshData.indexVec.push_back(i + 1);
		meshData.indexVec.push_back((slices + 1) + i + 1);

		meshData.indexVec.push_back((slices + 1) + i + 1);
		meshData.indexVec.push_back((slices + 1) + i);
		meshData.indexVec.push_back(i);
	}
	WTINFO(L"%s", WTStrErr(WTRESULT_OK));
	return meshData;
}

void Sprite3D::CreatePlane(const DirectX::XMFLOAT3 & center, const XMFLOAT2 & planeSize, const DirectX::XMFLOAT2 & maxTexCoord)
{
	CreatePlane(center.x, center.y, center.z, planeSize.x, planeSize.y, maxTexCoord.x, maxTexCoord.y);
}

void Sprite3D::CreatePlane(float centerX, float centerY, float centerZ, float width, float depth, float texU, float texV)
{
	mMeshData.vertexVec.push_back({ XMFLOAT3(centerX - width / 2, centerY, centerZ - depth / 2), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, texV) });
	mMeshData.vertexVec.push_back({ XMFLOAT3(centerX - width / 2, centerY, centerZ + depth / 2), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) });
	mMeshData.vertexVec.push_back({ XMFLOAT3(centerX + width / 2, centerY, centerZ + depth / 2), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(texU, 0.0f) });
	mMeshData.vertexVec.push_back({ XMFLOAT3(centerX + width / 2, centerY, centerZ - depth / 2), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(texU, texV) });

	mMeshData.indexVec = { 0, 1, 2, 2, 3, 0 };
	WTINFO(L"%s", WTStrErr(WTRESULT_OK));
}

isEqual::isEqual(DirectX::XMFLOAT3& vertexs)
{
	Vertexs = vertexs;
}

bool isEqual::operator()(const std::vector<DirectX::XMFLOAT3>::value_type & value)
{
	return (value.x == Vertexs.x) && (value.y == Vertexs.y) && (value.z == Vertexs.z);
}