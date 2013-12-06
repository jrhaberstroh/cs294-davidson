#include "LargeMatrix.H"
#include "Real.H"
#include "CH_Timer.H"
#include <boost/filesystem.hpp>
#include <gsl/gsl_rng.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

namespace Mtx
{

LargeMatrix::LargeMatrix(std::string a_filename, int a_num_rows, int a_iostate, int a_nums_in_memory)
	{
		m_filename = a_filename;
		m_iostate = a_iostate;
		m_num_rows = a_num_rows;
		assert(m_iostate | FRead);
		assert(m_iostate | FWrite);
		std::cout << "Succeeded assertions!"<<std::endl;
	}

int LargeMatrix::EstimateMaxSize(int a_num_rows)
	{
		return a_num_rows * a_num_rows * 32;
	}


void LargeMatrix::GenerateRandom(std::string a_filename, int a_num_rows, int a_ioflags)
	{
		//std::cout << "Nothing yet implemented for GenerateRandom" << std::endl;
		std::ofstream mtx_file;

		gsl_rng * r;
		const gsl_rng_type * T;
		gsl_rng_env_setup();
		T = gsl_rng_default;
		r = gsl_rng_alloc(T);
		gsl_rng_set(r, 90210);
		std::vector<Real> rand_row(a_num_rows, 0);
		std::cout << "Generator type: " << gsl_rng_name(r) << std::endl;
		
		mtx_file.open(a_filename.c_str());
		for (int i = 0 ; i < a_num_rows ; i++)
			{	
				for (int j = 0 ; j < a_num_rows ; j++)
					{
						rand_row[j] = gsl_rng_uniform(r);
					}
				for (int j = 0 ; j < a_num_rows ; j++)
					{
						mtx_file << rand_row[j] << ",";
					}
				mtx_file << std::endl;
			}
		mtx_file.close();

		gsl_rng_free(r);
	}

colVect LargeMatrix::operator*(colVect col)
	{
		CH_TIMERS("Multiplication");
		CH_TIMER("mult_file", t1);
		colVect dot(col.size(), 0);
		assert(col.size() == m_num_rows);

		std::ifstream mtx_file;
		CH_START(t1);
		mtx_file.open(m_filename.c_str());
		//std::cout << "MATRIX LOADING: " << std::endl;
		std::stringstream ss;
		for (int i = 0 ; i < m_num_rows ; i++)
			{
				rowVect row(m_num_rows,0);
				std::string line;

				if (mtx_file.is_open())
					{
						getline(mtx_file,line);
					}
				ss.str(line);

				std::string remain;
				for (int j = 0 ; j < m_num_rows ; j++)
					{
						ss >> row[j];
						//std::cout << row[j] << "  \t";
						ss.ignore(256,',');
					}
				//std::cout << std::endl;
				
				dot[i] = row*col;
			}
		CH_STOP(t1);
		return dot;
	}

}
