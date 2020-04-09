
#include <ctime>
#include "time.h"
#include <map>
#include <string>

using namespace std; 

typedef struct 
{
	clock_t start;
	clock_t end;
	double time;
	string name;
}interval; 


class Benchmarking
{



public:
	Benchmarking();
	~Benchmarking();
	int getInterval(string id);
	void start(int id);
	void finish(int id);
	double getTime(int id);
	void printresult();
	timespec diff(timespec start, timespec end);



private:
	map<int, interval> benchmark;


};




