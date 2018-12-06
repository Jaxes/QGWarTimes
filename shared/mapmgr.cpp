#include <iostream>
#include <ios>
#include <fstream>
#include <vector>
#include <array>
#include <functional>
#include <errno.h>
#include "mapmgr.h"

MapMgr::MapMgr(const char* filename) :
	rowdata(0),
	max_row_once(0), 
	max_col_once(0),
	initrow(10),
	initcol(4) {
	using namespace std;
	int col = 0;
	int row = 0;
	std::ifstream infile;
	infile.open(filename);
	while(!infile.eof()) {
		char ch = infile.get();
		if(isdigit(ch)) {
			infile.unget();
		} else if(ch == '\n') {
			row++;
			col = 0;
			continue;
		} else {
			col++;
			continue;
		}
		while(rowdata.size() <= (size_t)row) {
			rowdata.push_back(new std::vector<int>(0));
		}
		while(rowdata[row]->size() <= (size_t)col) {
			rowdata[row]->push_back(0);
		}
		int data;
		infile >> data;
		(*rowdata[row])[col] = data;
	}
}

MapMgr::MapMgr(int row, int col) :
	rowdata(0), 
	max_row_once(0), 
	max_col_once(0), 
	initrow(1), 
	initcol(1) {
	for(int r = 0; r<row; r++) {
		rowdata.push_back(new std::vector<int>(col));
	}
}

MapMgr::~MapMgr() {
	for(auto const& i : rowdata) {
		if(i)delete i;
	}
}

int MapMgr::at(int row, int col) {
	if(row<0 || col<0) {
		return 1;
	}
	if((size_t)row >= rowdata.size())
		return 0;
	if((size_t)col >= rowdata[row]->size())return 0;
	return rowdata[row]->at(col);
}

int MapMgr::at(float y, float x) {
	if(x<0 || y<0) {
		return 1;
	}
	int j = (int)y;
	int i = (int)x;
	if(int res = at(j, i))
		return res;

	int pos[8][2] = {
		-1,0,
		0,-1,
		0,1,
		1,0,
		-1,-1,
		-1,1,
		1,-1,
		1,1,
	};

	const float dmove = 0.3f;

	for(int ind = 0; ind < 8; ind++) {
		int newj = j + pos[ind][0];
		int newi = i + pos[ind][1];
		if(at(newj, newi)) {
			float newy = y + dmove * pos[ind][0];
			float newx = x + dmove * pos[ind][1];
			if(int res = at((int)newy, (int)newx)) {
				return res;
			}
		}
	}
	return 0;
}

void MapMgr::for_each(std::function<void(int row, int col, int dat)> func) {
	int total = 0;
	for(size_t row = 0; row < rowdata.size(); row++) {
		for(size_t col = 0; col < rowdata[row]->size(); col++) {
			if(rowdata[row]->at(col)) {
				total++;
			}
			func(row, col, rowdata[row]->at(col));
		}
	}
	total;
}

inline int MapMgr::max_row() {
	if(max_row_once)return max_row_once;
	return max_row_once = rowdata.size();
}

inline int MapMgr::max_col() {
	if(max_col_once)return max_col_once;
	int max = 0;
	for(auto const& i : rowdata) {
		if(i->size() > (size_t)max) {
			max = i->size();
		}
	}
	return max_col_once = max;
}

void MapMgr::random(int seed) {
	initrow = 1;
	initcol = 1;
	int i = 16;
	srand(seed);
	std::vector<std::array<int, 2>> pass;
	for_each([&, this](int row, int col, int dat) {
		if(row == 0 || col == 0 || row == max_row() - 1 || col == max_col() - 1) {
			(*rowdata[row])[col] = 1;
		} else {
			switch(row % 2 + col % 2 * 2) {
			case 3:// O
				(*rowdata[row])[col] = i++;
				break;
			case 1:// Vpass
			case 2:// Hpass
				(*rowdata[row])[col] = 2;
				pass.push_back(std::array<int, 2>{row, col});
				break;
			case 0:// wall
				(*rowdata[row])[col] = 1;
				break;
			}
		}
		});
	size_t ind;
	while((ind = pass.size())>0) {
		ind = rand() % ind;
		int row = pass[ind][0];
		int col = pass[ind][1];
		int place1row = row;
		int place1col = col;
		int place2row = row;
		int place2col = col;
		if(!(row % 2)) {
			place1row -= 1;
			place2row += 1;
		} else {
			place1col -= 1;
			place2col += 1;
		}
		int dat1 = at(place1row, place1col);
		int dat2 = at(place2row, place2col);
		if(dat1 != dat2) {
			for(int r = 1; r<max_row(); r += 2) {
				for(int c = 1; c<max_col(); c += 2) {
					int tmpdat = at(r, c);
					if(tmpdat == dat1) {
						(*rowdata[r])[c] = dat2;
					}
				}
			}
			(*rowdata[row])[col] = 0;
		} else {
			(*rowdata[row])[col] = 1;
		}
		pass.erase(pass.begin() + ind);
	}

	for(int r = 1; r<max_row(); r += 2) {
		for(int c = 1; c<max_col(); c += 2) {
			(*rowdata[r])[c] = 0;
		}
	}
	(*rowdata[1])[1] = 0;
	(*rowdata[max_row()-1])[max_col()-2] = 0;
}

bool MapMgr::out(float y, float x) {
	if(y < 0 || x < 0) {
		return true;
	} else {
		size_t row = (size_t)y;
		size_t col = (size_t)x;
		if(row >= rowdata.size()) {
			return true;
		} else {
			if(col >= rowdata[row]->size()) {
				return true;
			} else {
				return false;
			}
		}
	}
}

void MapMgr::getInitPos(int & y, int & x) {
	y = initrow;
	x = initcol;
}
