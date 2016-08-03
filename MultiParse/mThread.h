#pragma once
#include "Header.h"

class ThreadManager
{
	//queue for reading into threads
	//thread start function
	//initialize?
	//Function for reading into queue
	//handle thread closing
	//get function for retrieving a thread handle.
	//closeall function to close out all threads
	//int threadNum;
	bool threadClose[MAX_THREADS];
	char stringQueue[MAX_STRLENGTH * MAX_THREADS];
	std::thread allThreads[MAX_THREADS];


	void ThreadProgram(int threadN, char searchString[]);

	

public:

	bool needsQueue[MAX_THREADS];
	//bool isClosing = false;
	bool matches[MAX_THREADS];

	ThreadManager(); //initialize variables
	~ThreadManager();
	void reQueue(char buffArray[], int threadN);
	//bool Close(int threadNum);
	void CloseAll();
	void Start(char searchString[], char buffArray[]);
	void Write(int threadN, std::ofstream& writer, int& numMatches);
	//checkMatch()
};

ThreadManager::ThreadManager() //initializes the booleans and the queue string
{
	for (int i = 0; i < MAX_THREADS; ++i)
	{
		this->threadClose[i] = false;
		this->matches[i] = false;
		this->needsQueue[i] = false;
	}
	Clean(this->stringQueue, MAX_STRLENGTH * MAX_THREADS);
}

ThreadManager::~ThreadManager()
{
	for (int i = 0; i < MAX_THREADS; i++) //join all of the threads.
	{
		if (this->allThreads[i].joinable())
			this->allThreads[i].join();
		else if (LOGGING)
			std::cout << "One of the threads became unjoinable. ThreadId: " << this->allThreads[i].get_id() << '\n';
	}
	if(LOGGING)
		std::cout << "Threads closed\n";
}

void ThreadManager::Start(char searchString[], char buffArray[])
{
	for (int i = 0; i < MAX_STRLENGTH * MAX_THREADS; ++i)
		this->stringQueue[i] = buffArray[i];

	for (int i = 0; i < MAX_THREADS; ++i)
	{
		if (VERBOSE)
		{
			char temp[MAX_STRLENGTH];
			strcpy_s(temp, MAX_STRLENGTH, &this->stringQueue[MAX_STRLENGTH * i]);
			std::cout << "read into queue: " << temp << '\n';
		}
		this->allThreads[i] = std::thread(&ThreadManager::ThreadProgram, this, i, searchString);
		//this->allThreads[i] = std::thread(&ThreadManager::reQueue, this);
	}
}

/*bool ThreadManager::Close(int threadNum)
{
	return true;
}*/

void ThreadManager::CloseAll()
{
	for (int i = 0; i < MAX_THREADS; i++) //ask all of the threads to stop running.
		this->threadClose[i] = true;
}

void ThreadManager::ThreadProgram(int threadN, char searchString[])
{
/*
while(thread close flag not set)
call checkmatch
set matched bool
set queued = false
while !queued
check thread close flag
end while
end while
*/
	while (!threadClose[threadN])
	{
		checkMatch(searchString, &this->stringQueue[MAX_STRLENGTH * threadN], this->matches[threadN]);
		this->needsQueue[threadN] = true;
		while (this->needsQueue[threadN])//hang thread in while loop until queue is refreshed
			if (threadClose[threadN]) //check if thread needs to close while waiting
				break;
	}

}

void ThreadManager::reQueue(char buffArray[], int threadN)//starts thread back up by refreshing the queue string and clearing needsQueue flag
{
	int offset = MAX_STRLENGTH * threadN;
	strcpy_s(&this->stringQueue[offset], MAX_STRLENGTH, &buffArray[offset]);
	this->needsQueue[threadN] = false;
}

void ThreadManager::Write(int threadN, std::ofstream& writer, int& numMatches)
{
	writeOut(&this->stringQueue[MAX_STRLENGTH * threadN], writer, numMatches);
}