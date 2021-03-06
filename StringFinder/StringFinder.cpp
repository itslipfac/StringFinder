#include <iostream>
#include <omp.h>
#include <termcolor\termcolor.hpp>

#include "dataextractor.h"
#include "commandparser.h"

using namespace std;
using namespace termcolor;

int main(int argc, char *argv[])
{
    CommandParser cp;

    if ( cp.ValidateArguments(argc, argv) )
    {
        cout << green << "Arguments valid. Extracting data using <"<< NUM_THREADS <<"> threads ..." << reset << endl;

        DataExtractor& de = DataExtractor::instance( cp.searchString(), cp.location() );
        
        double time = omp_get_wtime();

        de.ExtractData();

        time = omp_get_wtime() - time;
        cout << "Data extracted in: <" << green << time << reset << "> sec." << reset << endl;
        
        de.DisplayData();
    }
    else
    {
        cout << red << "Invalid arguments. Cannot extract data." << reset << endl;
    }

    getchar();
}
