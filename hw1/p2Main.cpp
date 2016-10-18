#include <iostream>


#include <string>
#include <sstream>
#include "p2Table.h"
#include <cstdlib>
#include <climits>

using namespace std;
bool line_input(string & command, string & params);
int main()
{
    Table table;

    // TODO: read in the csv file
    string csvFile;
    cout << "Please enter the file name: ";
    cin >> csvFile;
    if (table.read(csvFile))
        cout << "File \"" << csvFile << "\" was read in successfully." << endl;
    else exit(-1); // csvFile does not exist.

    cin.ignore();

    // TODO read and execute commands
    string line;
    while (1) {
        getline(cin, line);
        istringstream iss(line);
        string command, params;
        iss >> command;
        if (line.length()<command.length()+1){
            if(command=="PRINT"){
                table.print();       
            }else if(command=="EXIT"){
                break;
            }
        }else{
            iss.str(line.substr(command.length()+1));
            if(command=="ADD"){
                getline(iss, params, '\r');
                table.parsing(params,2);
            }else{
                string col_id;
                iss >> col_id;
                if(command=="SUM"){
                    cout << "The summation of data in column #"<<col_id <<" is "<<table.sum(atoi(col_id.c_str()))<<"."<<endl;
                }else if(command=="MAX"){
                    cout << "The maximum of data in column #"<<col_id <<" is "<<table.max(atoi(col_id.c_str()))<<"."<<endl;
                }else if(command=="MIN"){
                    cout << "The minimum of data in column #"<<col_id <<" is "<<table.min(atoi(col_id.c_str()))<<"."<<endl;
                }else if(command=="COUNT"){
                    cout << "The distinct count of data in column #"<<col_id <<" is "<<table.count(atoi(col_id.c_str()))<<"."<<endl;
                }else if(command=="AVE"){
                    cout << "The average of data in column #"<<col_id <<" is "<<fixed<<setprecision(1)<<table.ave(atoi(col_id.c_str()))<<"."<<endl;
                }
            }
        } 
    }
}
bool line_input(string & command, string & params){
    return true;
}