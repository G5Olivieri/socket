#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <exception>
#include <vector>

#include "ppm.h"

// iniciando com valores padoes
void ppm::init()
{
  width = 0;
  height = 0;
  max_col_val = 255;
}

// criando um objeto PPM
ppm::ppm()
{
  init();
}

// criando um vazio PPM
ppm::ppm(const unsigned int _width, const unsigned int _height)
{
  init();
  this->width = _width;
  this->height = _height;
  this->nr_lines = this->height;
  this->nr_columns = this->width;
  this->size = this->width * this->height;

  r.resize(size);
  g.resize(size);
  b.resize(size);

  for(unsigned int i = 0; i < size; ++i)
    {
      r[i] = 0;
      g[i] = 0;
      b[i] = 0;
    }
}

// lendo PPM de fname
void ppm::read(const std::string &fname)
{
  std::ifstream inp(fname.c_str(), std::ios::in | std::ios::binary);
  if(inp.is_open())
    {
      std::string line;
      std::getline(inp, line);
      if(line != "P6")
	{
	  std::cout << "Error. Unrecognized file format." << std::endl;
	  return;
	}
      std::getline(inp, line);
      while(line[0] == '#')
	{
	  std::getline(inp, line);
	}
      std::stringstream dimensions(line);
      try {
	  dimensions >> width;
	  dimensions >> height;
	  nr_lines = height;
	  nr_columns = width;
      } catch(std::exception &e) {
	std::cerr << "Header file format error." << e.what() << std::endl;
	return;
      }
      size = width * height;
      r.reserve(size);
      g.reserve(size);
      b.reserve(size);

      char aux;
      for(unsigned int i = 0; i < size; i++)
	{
	  inp.read(&aux, 1);
	  r[i] = (unsined char) aux;
	  inp.read(&aux, 1);
	  g[i] = (unsined char) aux;
	  inp.read(&aux, 1);
	  b[i] = (unsined char) aux;
	}
    } else {
    std::cout << "Error. Unable to open " << fname << std::endl;
  }
  inp.close();
}

// write the PPM image in fname
void ppm::write(const std::string &fname)
{
  std::ofstream inp(fname.c_str(), std::ios::out | std::ios::binary);
  if(inp.is_open())
    {
      inp << "P6\n";
      inp << width;
      inp << " ";
      inp << height << "\n";
      inp << max_col_val << "\n";

      char aux;
      for(unsigned int i = 0; i < size; ++i)
	{
	  aux = (char) r[i];
	  inp.write(&aux, 1);
	  aux = (char) g[i];
	  inp.write(&aux, 1);
	  aux = (char) b[i];
	  inp.write(&aux, 1);
	}
    } else {
    std::cout << "Error. Unable to open " << fname << std::endl;
  }
  inp.close();
}