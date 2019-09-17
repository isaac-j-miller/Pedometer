#define _CRT_SECURE_NO_WARNINGS

//from https://github.com/ben-strasser/fast-cpp-csv-parser

#include <iostream>
#include <string>
#include "csv.h"
#include "Pedometer.h"

Pedometer pedometer;

int main()
{
	//create a string with the full/absolute file path for the data to be read.
	std::string fileName = "14_steps_trimmed.csv";

	//create the reader object to parse a file with 3 columns
	io::CSVReader<3> in(fileName);

	//tell the class what are the names of the three columns
	in.read_header(io::ignore_extra_column, "x", "y", "z");

	//create variables to hold the data points for each row
	int x; int y; int z;

	//read through the file and do whatever you like with the data
	bool available = true;
	while (available)
	{
		available = in.read_row(x, y, z);
		pedometer.add_point(pedometer.read_accelerometer(x, y, z));
		pedometer.output_data();
		
		
	}

	return 0;
}
