#pragma once

class Thread
{
	//queue for reading into threads
	//thread start function
	//initialize?
	//handle thread closing
	//get function for retrieving a thread handle.
	//closeall function to close out all threads
	bool isClosing = false;
	int threadNum;

public:
	bool Close(int threadNum);
	bool CloseAll();
	void Start(int numThreads, )

};

bool Thread::Close(int threadNum)
{

}

bool Thread::CloseAll()
{

}