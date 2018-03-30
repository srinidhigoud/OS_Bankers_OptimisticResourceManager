#ifndef TASKS_H
#define TASKS_H
#include <vector>
#include <tuple> 
#include <string>
using namespace std;
class Tasks{
public:
	vector<tuple<string,int,int,int,int> > Act;
	int iterator;
	int id;
	int finishTime;
	int delay_time;
	int waitTime;
	bool aborted;

	Tasks(int i = 0);
	void reset(int i);
	bool hasNextActivity();
	tuple<string, int,int,int,int> getNext();
	bool isSuspended();
	bool notYetSuspended();
	bool isResourceNotAvail(vector<int> res);
	void incIterator();
};
#endif
