#ifndef BANK_H
#define BANK_H
#include <vector>
#include <unordered_map>
#include "Tasks.h"
using namespace std;
class Bank{
public:
	int numb_tasks;
	int numb_resources; 
	vector<int> maxRes;
	unordered_map<int, vector<int> > allocated;
	unordered_map<int, vector<int> > claim;
	Bank(int tackC = 0, int resC = 0);
	bool isClaimable(vector<int>& avail_res,int idx);
	void run(vector<Tasks>& tasks);
	void resClaim(vector<int>& avail_res, int idx);
	void resReClaim(vector<int>& avail_res, int idx);
	bool Banker(vector<Tasks>& tasks, int idx, vector<int>& avail_res, vector<int> running);
};
#endif