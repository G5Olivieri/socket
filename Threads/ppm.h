#ifndef _PPM_H
#define _PPM_H

class ppm
{
  void init();
  // info sobre PPM file (altura e largura)
  unsigned int nr_lines;
  unsigned int nr_columns;

 public:
  // arrays para RGB valores
  std::vector<unsigned char> r;
  std::vector<unsigned char> g;
  std::vector<unsigned char> b;

  
  unsigned int height;
  unsigned int width;
  unsigned int max_col_val;
  // numero total de elementos (pixel)
  unsigned int size;

  ppm();
  // criando um PPM objeto e passando o nome do arquivo como parametro
  ppm(const std::string &fname);
  // criando um vazio PPM image com valores para width e height
  // o RGB array so  iniciados com zeros
  ppm(const unsigned int _width, const unsigned int _height);
  // lendo PPM imagem de fname
  void read(const std::string &fname);
  // escrevendo PPM em fname
  void write(const std::string &fname);
};

#endif
