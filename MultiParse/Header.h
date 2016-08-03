#pragma once
#include <iostream>
#include <fstream>
#include <ctime>
#include <thread>

const int MAX_SEARCH = 100; //Maximum length of search term
const int MAX_STRLENGTH = 500; //Maximum line length for one line in file
const bool LOGGING = false; //Verbose logging on or off
const int MAX_THREADS = 4;
const bool VERBOSE = false;

void Extract(char searchString[], int & numMatches); //main parsing program
//void Export(char exportLine[], std::ofstream & exporter); //writes out matches to output.txt
void checkMatch(char searchTerm[], char toBeSearched[], bool& match); //Returns true if searchTerm is somewhere in toBeSearched
void readIn(char myString[], std::ifstream& reader); //reads lines into different strings for searching later.
void writeOut(char myString[], std::ofstream& writer, int& numMatches); //looks at matches and writes matching lines to output.
//void test(int test1[], int test2); //testing
void Clean(char toClean[], int strLength);