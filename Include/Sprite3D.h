#ifndef SPRITE3D_H
#define SPRITE3D_H

#include <WinSock2.h>
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <DirectXCollision.h>
#include <string>
#include <fstream>
#include <sstream>
#include <mutex>

#include "GameObject.h"
#include "d3dApp.h"
#include "ConstantBuffer.h"
#include "Vertex.h"
#include "wterr.h"
#include "WarTimesObject.h"
#include "LightHelper.h"

enum class ModelMode
{
	BOX,              //立方体
	SPHERE,           //球体
	CYLINDER,         //圆柱体
	PLANE,            //三维平面
	FROM_FILE         //从文件读取
};

class isEqual
{
public:
	isEqual(DirectX::XMFLOAT3& vertexs);
	bool operator()(const std::vector<DirectX::XMFLOAT3>::value_type& value);
private:
	DirectX::XMFLOAT3 Vertexs;
};

class Sprite3D : public GameObject
{
public:
	//构造函数
	Sprite3D(
		ComPtr<ID3D11VertexShader> VertexShader3D,
		ComPtr<ID3D11PixelShader> PixelShader3D,
		ComPtr<ID3D11InputLayout> vertexLayout3D,
		ComPtr<ID3D11Device> device,
		ModelMode mode,
		std::string SourceDir = "",
		ComPtr<ID3D11SamplerState> samplerState = nullptr);
	Sprite3D() = default;

	//析构函数
	~Sprite3D();

	//二段构造
	WTRESULT init();
	WTRESULT init(DirectX::XMFLOAT3 center, DirectX::XMFLOAT2 planeSize, DirectX::XMFLOAT2 maxTexCoord);
	WTRESULT init(float centerX, float centerY, float centerZ, float width, float depth, float texU, float texV);
	WTRESULT init(float radius, int levels, int slices);
	WTRESULT init(float width, float height, float depth);
	WTRESULT init(float radius, float height, int slices);


	//输入布局、着色器绑定、顶点/索引缓冲区重新设置
	WTRESULT Update();

	//精灵自身的旋转
	WTRESULT RotationX(float angle);
	WTRESULT RotationY(float angle);
	WTRESULT RotationZ(float angle);

	//精灵绕其他轴旋转
	WTRESULT RotationAround(DirectX::XMFLOAT3 axis, DirectX::XMFLOAT3 axisPos, float angle, float Max = -1, DirectX::XMFLOAT3 ReferenceVector = { 0.0f,0.0f,0.0f });

	//精灵缩放
	WTRESULT Scaling(DirectX::XMFLOAT3 scale);

	//精灵移动
	WTRESULT Translation(DirectX::XMFLOAT3 translation);

	//设置精灵位置
	WTRESULT SetPosition(const DirectX::XMFLOAT3& pos);

	//获取位置向量
	DirectX::XMFLOAT3 GetLook();
	DirectX::XMFLOAT3 GetUp();
	DirectX::XMFLOAT3 GetRight();
	DirectX::XMFLOAT3 GetFront();

	// 绘制
	void Draw();

	//设置缓冲区
	WTRESULT SetBuffer();

	//重设网格
	WTRESULT ResetMesh();

	WTRESULT SetCanSee(bool canSee);

	bool GetCanSee();

	//检测是否被击中
	bool CheckShooted(DirectX::XMFLOAT3 Origin, DirectX::XMFLOAT3 Direction);

	//检测碰撞
	bool CheckCollision(Sprite3D& Other);

	DirectX::BoundingBox GetCollisionBox();

	//添加碰撞体
	WTRESULT SetCollisionBox();

	//同步数据给可传输的数据对象
	WTRESULT DataSync(std::shared_ptr<WarTimes::CObject> pObject);

	//从可传输的数据对象中获取数据
	WTRESULT DataGet(std::shared_ptr<WarTimes::CObject> pObject);

private:
	// 创建球体模型数据，levels和slices越大，精度越高。
	void CreateSphere(float radius, int levels, int slices);

	// 创建立方体模型数据
	void CreateBox(float width, float height, float depth);

	// 创建圆柱体模型数据，slices越大，精度越高。
	void CreateCylinder(float radius, float height, int slices);

	// 创建圆柱体只有侧面的模型数据，slices越大，精度越高
	MeshData CreateCylinderNoCap(float radius, float height, int slices);

	// 创建一个平面
	void CreatePlane(const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT2& planeSize, const DirectX::XMFLOAT2& maxTexCoord);
	void CreatePlane(float centerX, float centerY, float centerZ, float width, float depth, float texU, float texV);

	std::wstring toWString(std::string str);

	// 从文件中直接加载保存的内存数据
	// 文件只能是 SaveVertexToFile 所保存的文件
	WTRESULT ReadVertexFromFile(std::string dir, std::string filename);
	// 从MTL中读取DDS材质 文件名不以(反)斜杠开头, 文件夹路径以(反)斜杠结尾
	WTRESULT ReadVertexAttrFromMTL(std::string dir, std::string filename);

	// 根据MTL中的信息读取文件 文件夹路径以(反)斜杠结尾, 表示MTL去读取材质时所用的相对路径
	WTRESULT CreateTextureView(std::string dir);

	//纹理资源视图
	std::vector<ComPtr<ID3D11ShaderResourceView>> mTexture;

	//着色器
	ComPtr<ID3D11VertexShader> mVertexShader3D;				// 用于3D的顶点着色器
	ComPtr<ID3D11PixelShader> mPixelShader3D;				// 用于3D的像素着色器

	//材质属性
	std::vector<Material> mMaterial;
	std::vector<ComPtr<ID3D11Buffer>> mMaterials;

	//采样器
	ComPtr<ID3D11SamplerState> mSamplerState;

	//输入布局
	ComPtr<ID3D11InputLayout> mVertexLayout3D;

	//子模型
	struct Submodel {
		uint32_t start;
		uint32_t size;
		size_t texIndex;
		std::string mName;     //用于识别是哪个部件
	};
	std::vector<Submodel> mModelIndexRanges;

	//模型网格数据
	VertexModel meshData;                                  //用于从文件读取的模型网格
	MeshData mMeshData;                                    //用于自己生成的模型网格

	//模型材质
	std::vector<SubModelAttr> mModelAttr;

	//精灵位置向量
	DirectX::XMFLOAT3 mRight;
	DirectX::XMFLOAT3 mUp;
	DirectX::XMFLOAT3 mLook;
	DirectX::XMFLOAT3 mFront;

	//精灵的模式
	ModelMode mode;

	//是否可见
	bool isCanSee;

	//碰撞体
	DirectX::BoundingBox mCollisionBox;
};

#endif //SPRITE3D_H