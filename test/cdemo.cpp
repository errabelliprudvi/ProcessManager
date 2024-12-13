#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

int main(int argc, char* argv[])
{
  while(true)
  {
    cout<< "Welcome to c++ Programming..."<<endl;
    this_thread::sleep_for(chrono::milliseconds(5000));
  }
}