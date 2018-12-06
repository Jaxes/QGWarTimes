#ifndef WARTIMESOBJECT_H
#define WARTIMESOBJECT_H

#include "Serialize.h"
#include <DirectXMath.h>
#include "wterr.h"

namespace WarTimes {
	// 运动学属性
	// 未来可能加入动画, 所以尽量用类方法改变对象
	class CObject : public ISerialize {
	public:
		//// 物体位置
		//DirectX::XMVECTOR mPosition;

		//// 物体速度(矢量)
		//DirectX::XMVECTOR mVelocity;

		//// 物体加速度(矢量)
		//DirectX::XMVECTOR mAcceleration;

		//// 物体旋转
		//DirectX::XMVECTOR mRollPitchYaw;

		//// 物体的正上方
		//DirectX::XMVECTOR mAbove;
		// 对象位置向量
		DirectX::XMVECTOR mRight;
		DirectX::XMVECTOR mUp;
		DirectX::XMVECTOR mLook;
		DirectX::XMVECTOR mFront;

		// 世界矩阵
		DirectX::XMFLOAT4X4 mWorldMatrix;

	public:
		//构造函数
		CObject() = default;
		// 给予初始矩阵初始化
		CObject(DirectX::XMFLOAT4X4& world);

		// 序列化接口
		static std::shared_ptr<ISerialize> construct();
		std::wistream& fromString(std::wistream& in) override;
		std::wostream& toString(std::wostream& out) const override;
	};
}

#endif
