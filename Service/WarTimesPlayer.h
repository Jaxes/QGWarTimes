#ifndef WARTIMESPLAYER_H
#define WARTIMESPLAYER_H

#include "Serialize.h"
#include "WarTimes.h"
#include "WarTimesObject.h"
#include "WarTimesWeapon.h"

namespace WarTimes {

	class CWeapon;

	// 玩家
	class CPlayer : public CObject {
	private:
		static SerializeHelper<CPlayer> mSerializeHelper;

	public:
		// 名字
		std::wstring mName;

		// 血量
		uint32_t mBlood;

		// 分数
		uint32_t mScore;

		// 武器
		std::shared_ptr<WarTimes::CWeapon> mpWeapon;

	public:
		CPlayer(
			CObject const& objInfo,
			std::wstring const& name,
			std::shared_ptr<WarTimes::CWeapon> weapon,
			uint32_t blood);
		CPlayer() = default;
		CPlayer(std::wstring const& name);

		// 序列化接口
		static std::shared_ptr<ISerialize> construct();
		std::wistream& fromString(std::wistream& in) override;
		std::wostream& toString(std::wostream& out) const override;
	};

}

#endif
