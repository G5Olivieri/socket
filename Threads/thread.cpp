#include <iostream>
#include <thread>

using namespace std;

void foo()
{
  puts("Potato\n");
}

int bar(int x)
{
  if(x > 1) return bar(x - 1) + bar(x - 2);
  else return 1;
}

void threadify(string message)
{
  cout << message << " ";
}

int main(int argc, char *argv[])
{
  thread hello_thread(threadify, "Hello");
  thread world_thread(threadify, "world");
  thread first(foo);
  thread second;
  long rc = second.create
  
  cout << "main, foo and bar now execute concurrently...\n";
  first.join();
  second.join();
  world_thread.join();
  hello_thread.join();
  puts("\nThread completed!");
  return 0;
}
