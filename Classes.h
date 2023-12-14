#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;

class Base64 {

public:
	Base64();

protected:
	string alphabet;
	string input;
	string output;
	string coded;
	ifstream inputFile;
	ofstream outputFile;
};

class Encoder :Base64 {

public:
	void EncodeTriplet(const string& triplet);
	void EncodeDuplet(const string& duplet);
	void EncodeSymbol(const char& symbol);
	void EncodeFile(const string& FilePath);
	void DataToFile(const string& FilePath);
	void Clear_Coded();
};


class Decoder :Base64 {

public:
	int DecodeTriplet(const string& triplet);
	int DecodeDuplet(const string& duplet);
	int DecodeSymbol(const string& symbol);
	int getPos(const char& symbol);
	int DecodeFile(const string& FilePath);
	void DataToFile(const string& FilePath);
	void Clear_Output();
};