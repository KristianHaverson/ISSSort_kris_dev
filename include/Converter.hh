#ifndef _Converter_hh
#define _Converter_hh

#include <bitset>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <string>

#include <TFile.h>
#include <TTree.h>
#include <TH1.h>

// Common header
#ifndef __COMMON_HH
# include "Common.hh"
#endif

class Converter {

public:
	
	Converter();
	~Converter();
	
	void ConvertFile( std::string input_file_name,
					  std::string output_file_name,
					  std::string log_file_name );
	void Initialise();
	void MakeHists();

private:
	
	unsigned long my_tm_stp;
	unsigned long my_info_field;
	unsigned long my_info_code;
	unsigned long my_type;
	unsigned long my_adc_data;
	unsigned long my_hit;
	unsigned long my_mod_id;
	unsigned long my_ch_id;
	unsigned long my_asic_id;
	
	common::info_data s_info;
	common::event_id  s_id;
	common::adc_data  s_adc;
	
	TH1F *hadc[common::n_module][common::n_asic][common::n_channel];
	
};

#endif