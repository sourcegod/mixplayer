/*
 * file: main.cpp
 * Main file
  * Compile: 
 * g++ -std=c++11 -Wall -pedantic myfile.cpp -o myfile
 * Date: Tue, 02/03/2021
 * Author: Coolbrother
*/

#include <iostream> // stdin, stdout
#include <string> // string
#include "alsa_driver.hpp"

#define APPNAME "MixPlayer"
using namespace std;
//----------------------------------------------------------
//========================================


int test() {
    cout << "Test on " << APPNAME << "\n\n";
    int err =0;
    if ( (err = openDevice()) ) {
        return EXIT_FAILURE;
    }
    writeFile(); 
    
    closeDevice();
    cout << endl;
  
    return 0;
}
//----------------------------------------------------------

int main() {
    test();
  cout << "Finished...\n" << endl;

  return 0;
}
//----------------------------------------------------------
