//Multithreaded parsing variation on original.  Opens and searches file.  Outputs matching search lines to an output file.

#include "mThread.h"
#include "Header.h"

int main()
{
	char searchPhrase[MAX_SEARCH];
	int numMatches = 0;

	//TODO:start GUI

	std::cout << "Enter phrase for extraction: ";
	std::cin.width(MAX_SEARCH);
	std::cin >> searchPhrase;
	std::cin.ignore(1000, '\n');

	std::clock_t start = clock();

	Extract(searchPhrase, numMatches);

	if (numMatches)
		std::cout << "Parser found " << numMatches << " matches.  Extracted into output.csv" << std::endl;
	else
		std::cout << "No matches found, sorry." << std::endl;

	std::clock_t end = clock();
	std::cout << "Time elapsed for program: " << double(end - start) / CLOCKS_PER_SEC << std::endl;

	std::cin.ignore(100, '\n');

	//TODO:Close GUI


	return 0;
}

//Extract calls while loop to parse each line of the file and write it to the output file if the line has a match.
void Extract(char searchString[], int & numMatches) {
	//allocate a triple array (char mystrings[2][4][MAXSTRINGS]) for buffer to threads.  Also allocate an array of 8 bools. Read first 4 lines into first different arrays.
	//allocate single arrays with chars and offsets (easier with bools, how with chars?)
	//while loop
	//create 4 threads finding matches to search string in arrays passed in.  also pass in(by reference) one out of an array of bools.
	//while threads run, read in next 4 lines from input file.
	//join all 4 threads, write any matches into output file.
	//end while
	//clean up.

	std::ifstream parser;
	std::ofstream writer;
	//std::thread myThreads[MAX_THREADS];
	//char currLines[2][MAX_THREADS][MAX_STRLENGTH];
	//char allLines[2 * MAX_THREADS * MAX_STRLENGTH];
	char buffArray[MAX_THREADS * MAX_STRLENGTH];
	//bool isMatch[2 * MAX_THREADS];
	//bool switcher = false;
	//bool isOver = false;
	ThreadManager mManager;

	Clean(buffArray, MAX_THREADS * MAX_STRLENGTH);


	parser.open("File.csv");
	writer.open("output.csv");

	if (parser.is_open())
		for (int i = 0; i < MAX_THREADS; ++i)
			readIn(&buffArray[MAX_STRLENGTH * i], parser);
	else
		if (LOGGING) std::cout << "failed to open input file" << std::endl;

	/*
	while (!isOver) 
	{
		if (!parser || parser.eof()) 
			isOver = true;

		for (int i = 0; i < MAX_THREADS; i++) //create 4 threads and start executing comparisons
		{
			myThreads[i] = std::thread(
				checkMatch,
				searchString,
				&allLines[int(switcher) * MAX_STRLENGTH * MAX_THREADS + MAX_STRLENGTH * i], 
				std::ref(isMatch[int(switcher) * MAX_THREADS + i]));
		}

		readIn(&allLines[int(!switcher) * MAX_STRLENGTH * MAX_THREADS], parser);

		for (int i = 0; i < MAX_THREADS; i++) //join all of the threads.
			if (myThreads[i].joinable())
				myThreads[i].join();
			else
				if (LOGGING) std::cout << "One of the threads became unjoinable. ThreadId: " << myThreads[i].get_id() << '\n';

		//writeOut(currLines[int(switcher)], writer, isMatch[int(switcher)], numMatches);
		writeOut(&allLines[int(switcher) * MAX_STRLENGTH * MAX_THREADS], writer, &isMatch[int(switcher) * MAX_THREADS], numMatches);

		switcher = !switcher; //switch the 4 arrays being used.
	}
	*/

	//TODO: Start thread manager
	//while thread manager is running, while loop to check for thread updates needed
	//once thread manager finishes, close the threads
	mManager.Start(searchString, buffArray);

	for (int i = 0; i < MAX_THREADS; ++i)
		readIn(&buffArray[MAX_STRLENGTH * i], parser);

	while (parser && !parser.eof())
	{
		bool matched;

		for (int i = 0; i < MAX_THREADS; ++i)
		{
			if (mManager.needsQueue[i])
			{
				//matched = mManager.matches[i];
				if (mManager.matches[i])
					mManager.Write(i, writer, numMatches);
				mManager.reQueue(buffArray, i);//reset flag on thread and enter next string for processing
				//if(matched)
				//	writeOut(&buffArray[MAX_STRLENGTH * i], writer, numMatches);
				readIn(&buffArray[MAX_STRLENGTH * i], parser);
			}			
		}
	}

	mManager.CloseAll();

	parser.close();
	writer.close();
}

void Clean(char toClean[], int strLength)
{
	for (int i = 0; i < strLength; i++)
		toClean[i] = '\0';
}

void readIn(char myString[], std::ifstream& reader)
{
	Clean(myString, MAX_STRLENGTH);

	//for (int i = 0; i < MAX_THREADS; i++) 
	//{
		reader.get(myString, MAX_STRLENGTH, '\n');
		reader.ignore();

		while (reader.fail() && !reader.eof()) //repeat until successful read or until eof is reached.
		{
			if (LOGGING && reader.eof()) std::cout << "End of file bit set" << '\n';
			else if (LOGGING) std::cout << "line was empty" << '\n';
			reader.clear(); //clear failure
			reader.ignore(); //ignore newline
			reader.get(myString, MAX_STRLENGTH, '\n'); //read another line into that array.
			reader.ignore();
		}
		if (VERBOSE) {
			char temp[MAX_STRLENGTH];
			strcpy_s(temp, MAX_STRLENGTH, myString);
			std::cout << "Line read: " << temp << '\n';
		}
	//}

}

//writes out the result of the search to the output file in order of the searches.
/*void writeOut(char myStrings[], std::ofstream& writer, bool matches[], int& numMatches)
{
	char temp[MAX_STRLENGTH];

	for (int i = 0; i < MAX_THREADS; i++)
	{
		if (matches[i])
		{
			//writer << myStrings[i] << std::endl;
			strcpy_s(temp, MAX_STRLENGTH, &myStrings[MAX_STRLENGTH * i]);
			writer << temp << '\n';
			numMatches++;
		}
	}

}*/

void writeOut(char myString[], std::ofstream& writer, int& numMatches) 
{
	char temp[MAX_STRLENGTH];
	strcpy_s(temp, MAX_STRLENGTH, myString);
	writer << temp << '\n';
	numMatches++;
}

//Returns true if there is a match between the searchterm and the line to be searched.
void checkMatch(char searchTerm[], char toBeSearched[], bool& match) 
{

	int searchLength = strlen(searchTerm);
	int stringLength = strlen(toBeSearched);
	int consecMatching;
	//bool matchFound = false;

	match = false;

	if (stringLength < searchLength) {
		if (LOGGING) std::cout << "search term longer than searched line. Search term chars: " << searchLength
			<< " line chars: " << stringLength << ". Breaking from function." << '\n';
		return;
	}


	for (int i = 0; i < stringLength; i++) {
		if (stringLength - i < searchLength) {
			if (LOGGING) std::cout << "reached end of line before search term found" << '\n';
			break;
		}

		if (toBeSearched[i] == searchTerm[0])
		{
			consecMatching = 0;
			for (int j = 0; j < searchLength; j++)
			{
				if (toBeSearched[i + j] == searchTerm[j])
				{
					if (LOGGING) std::cout << "character matched: " << searchTerm[j] << '\n';
					consecMatching++;
				}
			}
			if (consecMatching == searchLength)
			{
				match = true;
				if (LOGGING) std::cout << "Match found" << '\n';
			}
		}
	}

	return;
}

/*pseudocode
while(thread close flag not set)
call checkmatch
set matched bool
set queued = false
while !queued
check thread close flag
end while
end while

queuing scheme:
string with all the queued strings.
while close flag not set, use for loop to loop through bool array with queued flags
if it comes across a set flag, call function to write out if match and read in new value on that position.
if read fails (end of file) set close thread flag.
*/