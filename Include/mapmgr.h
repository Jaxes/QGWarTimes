/***
 *地图碰撞检测
 *MapMgr是Map Manager的简写
 ***/
#ifndef __MAPMGR_H__
#define __MAPMGR_H__

#include <ostream>
#include <vector>
#include <functional>

class MapMgr {
public:
	MapMgr() = delete;
	// 从文件中创建地图
	// 文件内容要求由多行组成, 每行都由空格分隔开的数字组成
	MapMgr(const char* filename);
	// 创建空地图
	MapMgr(int row, int col);
	// 检测该位置是否可站人
	int at(int row, int col);
	// 检测该位置是否可站人
	int at(float y, float x);
	~MapMgr();
	// 遍历每个位置, 对每个位置调用函数
	void for_each(std::function<void(int row, int col, int dat)> func);
	// 地图宽
	int max_row();
	// 地图高
	int max_col();
	// 生成随机迷宫
	void random(int seed);
	// 检测该位置是否出界
	bool out(float y, float x);
	// 获取出生点
	void getInitPos(int& y, int& x);
private:
	std::vector<std::vector<int>*> rowdata;
	int max_row_once;
	int max_col_once;
	int initrow;
	int initcol;
};

#endif
