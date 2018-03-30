#ifndef OPTIM_H
#define OPTIM_H
#include <vector>
#include <unordered_map>
#include "Tasks.h"
using namespace std;
class OptimRes{
public:
	int numb_tasks;
	int numb_resources; 
	vector<int> maxRes;
	unordered_map<int, vector<int> > allocated;
	OptimRes(int tackC = 0, int resC = 0);
	void run(vector<Tasks>& tasks);
};
#endif