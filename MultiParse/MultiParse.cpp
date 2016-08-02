//Multithreaded parsing variation on original.  Opens and searches file.  Outputs matching search lines to an output file.

#include <iostream>
#include <fstream>
#include <ctime>
#include <thread>
//#include "mThread.h"

const int MAX_SEARCH = 100; //Maximum length of search term
const int MAX_STRLENGTH = 500; //Maximum line length for one line in file
const bool LOGGING = true; //Verbose logging on or off
const int MAX_THREADS = 4;
const bool VERBOSE = true;

void Extract(char searchString[], int & numMatches); //main parsing program
//void Export(char exportLine[], std::ofstream & exporter); //writes out matches to output.txt
void checkMatch(char searchTerm[], char toBeSearched[], bool& match); //Returns true if searchTerm is somewhere in toBeSearched
void readIn(char myStrings[], std::ifstream& reader); //reads lines into different strings for searching later.
void writeOut(char myStrings[], std::ofstream& writer, bool matches[], int& numMatches); //looks at matches and writes matching lines to output.
void test(int test1[], int test2); //testing
void Clean(char toClean[], int strLength);

int main()
{
	char searchPhrase[MAX_SEARCH];
	int numMatches = 0;

	//start GUI

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

	//Close GUI


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
	std::thread myThreads[MAX_THREADS];
	//char currLines[2][MAX_THREADS][MAX_STRLENGTH];
	char allLines[2 * MAX_THREADS * MAX_STRLENGTH];
	bool isMatch[2 * MAX_THREADS];
	bool switcher = false;
	bool isOver = false;


	parser.open("File.csv");
	writer.open("output.csv");

	if (parser.is_open())
		//readIn(currLines[int(switcher)], parser);
		readIn(&allLines[int(switcher) * MAX_STRLENGTH * MAX_THREADS], parser);
	else
		if (LOGGING) std::cout << "failed to open input file" << std::endl;

	while (!isOver) 
	{
		if (!parser || parser.eof()) 
			isOver = true;

		for (int i = 0; i < MAX_THREADS; i++) //create 4 threads and start executing comparisons
		{
			//myThreads[i] = std::thread(checkMatch, searchString, currLines[int(switcher)][i], isMatch[int(switcher)][i]);
			//curpos = &allLines[int(switcher) * MAX_STRLENGTH * MAX_THREADS + MAX_STRLENGTH * MAX_THREADS];

			myThreads[i] = std::thread(
				checkMatch,
				searchString,
				&allLines[int(switcher) * MAX_STRLENGTH * MAX_THREADS + MAX_STRLENGTH * i], 
				//curpos,
				std::ref(isMatch[int(switcher) * MAX_THREADS + i]));
		}

		//readIn(currLines[int(!switcher)], parser);
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


	//cleanup
	//for (int i = 0; i < MAX_THREADS; i++)
	//	delete myThreads[i];
	parser.close();
	writer.close();


}

void Clean(char toClean[], int strLength)
{
	for (int i = 0; i < strLength; i++)
		toClean[i] = '\0';
}

void readIn(char myStrings[], std::ifstream& reader)
{
	Clean(myStrings, MAX_STRLENGTH * MAX_THREADS);

	for (int i = 0; i < MAX_THREADS; i++) 
	{
		reader.get(&myStrings[MAX_STRLENGTH * i], MAX_STRLENGTH, '\n');
		reader.ignore();

		while (reader.fail() && !reader.eof()) //repeat until successful read or until eof is reached.
		{
			if (LOGGING && reader.eof()) std::cout << "End of file bit set" << '\n';
			else if (LOGGING) std::cout << "line was empty" << '\n';
			reader.clear(); //clear failure
			reader.ignore(); //ignore newline
			reader.get(&myStrings[MAX_STRLENGTH * i], MAX_STRLENGTH, '\n'); //read another line into that array.
			reader.ignore();
		}
		if (VERBOSE) {
			char temp[MAX_STRLENGTH];
			strcpy_s(temp, MAX_STRLENGTH, &myStrings[MAX_STRLENGTH * i]);
			std::cout << "Line read: " << temp << '\n';
		}
	}

}

//writes out the result of the search to the output file in order of the searches.
void writeOut(char myStrings[], std::ofstream& writer, bool matches[], int& numMatches)
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

}

//Returns true if there is a match between the searchterm and the line to be searched.
void checkMatch(char searchTerm[], char toBeSearched[], bool& match) {

	int searchLength = strlen(searchTerm);
	int stringLength = strlen(toBeSearched);
	//int eolDist = stringLength;
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

		//eolDist--;
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