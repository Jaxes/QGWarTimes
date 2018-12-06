#ifndef WARTIMES_H
#define WARTIMES_H

#include <vector>
#include <map>
#include <memory>
#include <DirectXMath.h>
#include "Serialize.h"
#include "WarTimesPlayer.h"
#include "wterr.h"
#include "wtlog.h"

namespace WarTimes {

	class CObject;
	class CWeapon;
	class CPlayer;

	// 储存血量的数据类型
	//using Blood_t = uint32_t;
	enum EBlood {
		BLOOD_MIN = 0, // 最低血为0
		BLOOD_MAX = 100, // 最高血为100
	};

	// 储存分数
	//using Score_t = uint32_t;
	
	// 表示武器威力的类型
	//using WeaponPower_t = uint32_t;

	class CWarTimes : public ISerialize {
	private:
		static SerializeHelper<CWarTimes> mSerializeHelper;

	public:
		//房间信息
		using Seat = std::map<int, std::shared_ptr<CPlayer>>;
		struct Room
		{
			Room()
			{
				PlayerNum = 0;
				ReadyNum = 0;
				team.resize(2);
			}
			uint32_t PlayerNum;
			uint32_t ReadyNum;
			std::vector<Seat> team;
		};
		std::vector<std::shared_ptr<Room>> room;

		//玩家与房间的对应信息
		std::map<std::wstring, DirectX::XMINT3> PlayerRoomInfo;

		//更新PlayerNum的数据
		void update();

		//添加房间
		void AddRoom();

		// 应当实现的工厂方法, 构造空对象
		static std::shared_ptr<ISerialize> construct();
		// 从数据流中还原对象, 限空对象调用, 要求数据需为同一个类用 toString 生成的数据流
		std::wistream& fromString(std::wistream& in) override;
		// 将对象序列化输出到数据流中, 要求一行一个数据
		std::wostream& toString(std::wostream& out) const override;
	};
}

#endif