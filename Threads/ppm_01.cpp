#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <ctime>
#include "ppm.h"

std::vector<int> bounds(int parts, int mem)
{
  std::vector<int> bnd;
  int delta = mem / parts;
  int reminder = mem % parts;
  int N1 = 0;
  int N2 = 0;
  bnd.push_back(N1);
  for(int i = 0; i < parts; ++i)
    {
      N2 = N1 + delta;
      if(i == parts - 1)
  N2 += reminder;
      bnd.push_back(N2);
      N1 = N2;
    }
  return bnd;
}

bool border(int ii, int jj, int nr_lines, int nr_columns)
{
  if(ii >= 0 && ii < nr_lines && jj >= 0 && jj < nr_columns)
    return true;
  else
    return false;
}

void process(ppm &image, ppm &image2, int i, int j)
{
    int ii;
    int jj;
    int nr_lines;
    int nr_columns;
    int indx;
    unsigned int r;
    unsigned int g;
    unsigned int b;
    float r_sum;
    float g_sum;
    float b_sum;

    float filter[] = {
      0.10179640718562874, 0.11377245508982035, 0.10179640718562874,
      0.11377245508982035, 0.1377245508982036, 0.11377245508982035,
      0.10179640718562874, 0.11377245508982035, 0.10179640718562874
    };

    nr_lines = image.height;
    nr_columns = image.width;
    // aplicar o filtro
    r_sum = 0;
    g_sum = 0;
    b_sum = 0;

    // check norte oeste
    ii = i - 1;
    jj = j - 1;
    if(border(ii, jj, nr_lines, nr_columns))
    {
      indx = ii * image.width + jj;

      r = (unsigned int) image.r[indx];
      g = (unsigned int) image.g[indx];
      b = (unsigned int) image.b[indx];

      r_sum += r * filter[0];
      g_sum += g * filter[0];
      b_sum += b * filter[0];
    }

    // check norte
    ii = i - 1;
    jj = j;

    if(border(ii, jj, nr_lines, nr_columns))
      {
        indx = ii * image.width + jj;

        r = (unsigned int) image.r[indx];
        g = (unsigned int) image.g[indx];
        b = (unsigned int) image.b[indx];

        r_sum += r * filter[1];
        g_sum += g * filter[1];
        b_sum += b * filter[1];
      }

      // norte leste
      ii = i - 1;
      jj = j + 1;
    
      if(border(ii, jj, nr_lines, nr_columns))
      {
          indx = ii * image.width + jj;

          r = (unsigned int) image.r[indx];
          g = (unsigned int) image.g[indx];
          b = (unsigned int) image.b[indx];

          r_sum += r * filter[2];
          g_sum += g * filter[2];
          b_sum += b * filter[2];
      }

      // oeste
      ii = i;
      jj = j - 1;
       // check norte oeste
      ii = i - 1;
      jj = j - 1;
      if(border(ii, jj, nr_lines, nr_columns))
      {
          indx = ii * image.width + jj;

          r = (unsigned int) image.r[indx];
          g = (unsigned int) image.g[indx];
          b = (unsigned int) image.b[indx];

          r_sum += r * filter[3];
          g_sum += g * filter[3];
          b_sum += b * filter[3];
      }

      // centro
      ii = i;
      jj = j;
      
      indx = ii * image.width + jj;

      r = (unsigned int) image.r[indx];
      g = (unsigned int) image.g[indx];
      b = (unsigned int) image.b[indx];

      r_sum += r * filter[4];
      g_sum += g * filter[4];
      b_sum += b * filter[4];

      // leste
      ii = i;
      jj = j + 1;
      if(border(ii, jj, nr_lines, nr_columns))
      {
          indx = ii * image.width + jj;

          r = (unsigned int) image.r[indx];
          g = (unsigned int) image.g[indx];
          b = (unsigned int) image.b[indx];

          r_sum += r * filter[5];
          g_sum += g * filter[5];
          b_sum += b * filter[5];
      }

      // sul oeste
      ii = i + 1;
      jj = j - 1;
      if(border(ii, jj, nr_lines, nr_columns))
      {
          indx = ii * image.width + jj;

          r = (unsigned int) image.r[indx];
          g = (unsigned int) image.g[indx];
          b = (unsigned int) image.b[indx];

          r_sum += r * filter[6];
          g_sum += g * filter[6];
          b_sum += b * filter[6];
      }

      // sul
      ii = i + 1;
      jj = j;
      if(border(ii, jj, nr_lines, nr_columns))
      {
          indx = ii * image.width + jj;

          r = (unsigned int) image.r[indx];
          g = (unsigned int) image.g[indx];
          b = (unsigned int) image.b[indx];

          r_sum += r * filter[7];
          g_sum += g * filter[7];
          b_sum += b * filter[7];
      }

      // sul leste
      ii = i + 1;
      jj = j + 1;
      if(border(ii, jj, nr_lines, nr_columns))
      {
          indx = ii * image.width + jj;

          r = (unsigned int) image.r[indx];
          g = (unsigned int) image.g[indx];
          b = (unsigned int) image.b[indx];

          r_sum += r * filter[8];
          g_sum += g * filter[8];
          b_sum += b * filter[8];
      }

      // salvando mudança valores de pixel em image2
      indx = i * image.width; + j;
      image2.r[indx] = (unsigned char) r_sum;
      image2.g[indx] = (unsigned char) g_sum;
      image2.b[indx] = (unsigned char) b_sum;
}

void tst(ppm &image, ppm &image2, int left, int right)
{
  for(int i = left; i < right; i++)
  {
    int ii = i / image.width;
    int jj = i - ii * image.width;
    process(image, image2, ii, jj);
  }
}

int main()
{
  std::string fname = std::string("arara.ppm");
  
  ppm image(fname);
  ppm image2(image.width, image.height);
  
  int parts = 4;
  
  std::vector<int>bnd = bounds(parts, image.size);
  std::vector<std::thread> tt;
  
  time_t start, end;
  time(&start):

  for(int i = 0; i < parts - 1; ++i)
  {
    tt.push_back(std::thread(tst, std::ref(image), std::ref(image2), bnd[i], bnd[i + 1]));
  }

  for(int i = parts - 1; i < parts; ++i)
  {
    tst(image, image2, bnd[i], bnd[i + 1]);
  }
  for(auto &e : tt)
  {
    e.join();
  }

  time(&end);
  std::cout << difftime(end, start) << " seconds" << std::endl;

  image2.write("test.ppm");

  return 0;
}