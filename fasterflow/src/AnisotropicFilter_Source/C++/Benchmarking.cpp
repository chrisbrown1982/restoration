#include "Benchmarking.h"
#include <iostream>


using namespace std;
Benchmarking::Benchmarking()
{
	
}
Benchmarking::~Benchmarking()
{
	
}
int Benchmarking::getInterval(string name)
{

	int size;
	size = benchmark.size();
	interval bench;
	bench.name = name;

	benchmark.insert( pair<int,interval>(size,bench));

	return  benchmark.size()-1;

}

void Benchmarking::start(int id)
{
	benchmark[id].start = clock();
}

void Benchmarking::finish(int id)
{
	benchmark[id].end = clock();
	benchmark[id].time = (double(benchmark[id].end)-double(benchmark[id].start));
}

double Benchmarking::getTime(int id)
{
	return benchmark[id].time;

}

timespec Benchmarking::diff(timespec start, timespec end)
{
	timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

void Benchmarking::printresult()
{

	

	cout << "### begin intervals" << endl;
	cout << "[ 0," << benchmark.size() << ":" << endl;

	for (int i=0; i<benchmark.size();i++)
	{
		cout << benchmark[i].time << endl;
	}
	cout << "]" << endl;
	cout << "### begin intervals" << endl;
	cout << "### begin descriptions" << endl;
	cout << "[ 0," << benchmark.size() << ":" << endl;

	for (int i=0; i<benchmark.size();i++)
	{
		cout << "\"" << benchmark[i].name.c_str() << "\"" << endl;
	}
	cout << "]" << endl;
	cout << "### end descriptions" << endl;



}
