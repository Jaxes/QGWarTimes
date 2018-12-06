#ifndef WARTIMESGUN_H
#define WARTIMESGUN_H

#include "Serialize.h"
#include "WarTimes.h"
#include "WarTimesObject.h"

namespace WarTimes {

	// 武器
	class CWeapon : public CObject {
	protected:
		//攻击力（多少格血）
		uint32_t mPower;

	public:
		CWeapon(CObject objInfo, uint32_t power = 0);
		// 攻击, 成功返回 0, 否则返回非 0
		virtual int Attack() = 0;

		// 序列化接口
		std::wistream& fromString(std::wistream& in) override;
		std::wostream& toString(std::wostream& out) const override;
	};


	// 冲锋枪
	class CWeaponSubmachineGun : public CWeapon {
	private:
		static SerializeHelper<CWeaponSubmachineGun> mSerializeHelper;

	protected:
		//子弹数
		uint32_t mBullet;

	public:
		CWeaponSubmachineGun(CObject const& objInfo, uint32_t power = 1, uint32_t bullet = 10);
		// 攻击, 成功返回 0, 否则返回非 0
		int Attack() override;

		// 序列化接口
		static std::shared_ptr<ISerialize> construct();
		std::wistream& fromString(std::wistream& in) override;
		std::wostream& toString(std::wostream& out) const override;
	};


	// 霰弹枪
	class CWeaponShotGun : public CWeapon {
	private:
		static SerializeHelper<CWeaponShotGun> mSerializeHelper;

	protected:
		//子弹数
		uint32_t mBullet;

	public:
		CWeaponShotGun(CObject const& objInfo, uint32_t power = 1, uint32_t bullet = 10);
		// 攻击, 成功返回 0, 否则返回非 0
		int Attack() override;

		// 序列化接口
		static std::shared_ptr<ISerialize> construct();
		std::wistream& fromString(std::wistream& in) override;
		std::wostream& toString(std::wostream& out) const override;
	};
}

#endif
