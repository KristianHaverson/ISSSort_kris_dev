// My code include.
#include "Common.hh"
#include "Converter.hh"
#include "TimeSorter.hh"
#include "Calibration.hh"
#include "EventBuilder.hh"

// ROOT include.
#include <TTree.h>
#include <TFile.h>

// C++ include.
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <sstream>

// Command line interface
#ifndef __COMMAND_LINE_INTERFACE
# include "CommandLineInterface.hh"
#endif


int main( int argc, char *argv[] ){
	
	// Default parameters and name
	std::string output_name = "output.root";
	std::string name_cal_file;
	std::vector<std::string> input_names;
	
	// select what steps of the analysis to go through
	bool flag_convert = false;
	bool flag_sort = false;
	bool flag_eventbuilder = false;
	
	// select what steps of the analysis to be forced
	bool force_convert = false;
	bool force_sort = false;
	
	// Flag for somebody needing help on command line
	bool help_flag = false;
	
	// Command line interface, stolen from MiniballCoulexSort
	CommandLineInterface *interface = new CommandLineInterface();

	interface->Add("-i", "List of input files", &input_names );
	interface->Add("-o", "Output file for events tree", &output_name );
	interface->Add("-f", "Flag to force new ROOT conversion", &flag_convert );
	interface->Add("-s", "Flag to sort file by time", &flag_sort );
	interface->Add("-e", "Flag to build physics events", &flag_eventbuilder );
	interface->Add("-c", "Calibration file", &name_cal_file );
	interface->Add("-h", "Print this help", &help_flag );

	interface->CheckFlags( argc, argv );
	if( help_flag ) {
		
		interface->CheckFlags( 1, argv );
		return 0;
		
	}

	// Check we have data files
	if( !input_names.size() ) {
			
			std::cout << "You have to provide at least one input file!" << std::endl;
			return 1;
			
	}
	
	// Check we have a calibration file
	if( name_cal_file.size() > 0 ) {
		
		std::cout << "Calibration file: " << name_cal_file << std::endl;
		
	}
	else {
		
		if( flag_sort || flag_eventbuilder )
			std::cout << "No calibration file provided. Using defaults." << std::endl;
		name_cal_file = "dummy";

	}
	
	Calibration *cal = new Calibration( name_cal_file );

	//------------------------//
	// Run conversion to ROOT //
	//------------------------//
	Converter conv;
	std::cout << "\n +++ ISS Analysis:: processing Converter +++" << std::endl;

	TFile rtest;
	std::ifstream ftest;
	std::string name_input_file;
	std::string name_output_file;
	std::string name_log_file;
	
	// Check each file
	for( unsigned int i = 0; i < input_names.size(); i++ ){
			
		name_input_file = input_names.at(i);
		name_output_file = input_names.at(i) + ".root";
		name_log_file = input_names.at(i) + ".log";

		// If it doesn't exist, we have to convert it anyway
		// The convert flag will force it to be converted
		ftest.open( name_output_file.data() );
		if( !ftest.is_open() ) force_convert = true;
		else {
			
			ftest.close();
			rtest.Open( name_output_file.data() );
			if( rtest.IsZombie() ) force_convert = true;
			else if( flag_convert )
				std::cout << name_output_file << " already sorted" << std::endl;
			rtest.Close();
			
		}

		if( flag_convert || force_convert ) {
			
			std::cout << name_input_file << " --> ";
			std::cout << name_output_file << std::endl;
			
			conv.ConvertFile( name_input_file, name_output_file, name_log_file );

			force_convert = false;
			
		}

	}
		

	//-------------------------//
	// Do time sorting of data //
	//-------------------------//
	TimeSorter sort( cal );
	std::cout << "\n +++ ISS Analysis:: processing TimeSorter +++" << std::endl;
	
	// Check each file
	for( unsigned int i = 0; i < input_names.size(); i++ ){
			
		name_input_file = input_names.at(i) + ".root";
		name_output_file = input_names.at(i) + "_sort.root";
		name_log_file = input_names.at(i) + ".log";

		// If it doesn't exist, we have to sort it anyway
		// But only if we want to  build events
		if( flag_eventbuilder ) {
			
			ftest.open( name_output_file.data() );
			if( !ftest.is_open() ) force_sort = true;
			else {
				
				ftest.close();
				rtest.Open( name_output_file.data() );
				if( rtest.IsZombie() ) force_sort = true;
				else if( flag_sort )
					std::cout << name_output_file << " already sorted" << std::endl;
				rtest.Close();
				
			}
			
		}

		if( flag_sort || force_sort ) {
		
			std::cout << name_input_file << " --> ";
			std::cout << name_output_file << std::endl;
			
			sort.SortFile( name_input_file, name_output_file, name_log_file );
		
			force_sort = false;

		}
	
	}
	
	
	//-----------------------//
	// Physics event builder //
	//-----------------------//
	if( !flag_eventbuilder ) return 0;
	std::cout << "\n +++ ISS Analysis:: processing EventBuilder +++" << std::endl;

	std::vector<string> name_event_files;
	name_output_file = output_name;
	//name_log_file = output_name.substr( 0, output_name.find_last_of(".") );
	//name_log_file += ".log";

	// We are going to chain all the files now
	for( unsigned int i = 0; i < input_names.size(); i++ ){
			
		name_input_file = input_names.at(i) + "_sort.root";
		name_event_files.push_back( name_input_file );
	
	}
	
	EventBuilder eb( cal, name_event_files, name_output_file );
	
	cout << "Finished!\n";
			
	return 0;
	
}