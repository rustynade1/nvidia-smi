// CSOPESYWK4_Hinolan.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <tabulate/table.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <chrono>
#include <ctime>
using namespace std;
using namespace tabulate;

void print_datetime() {
    auto timenow = chrono::system_clock::now();
    time_t formatted_time = chrono::system_clock::to_time_t(timenow);

    
    tm local_time;
    localtime_s(&local_time, &formatted_time);  

    // Format
    cout << put_time(&local_time, "%a %m  %d %H:%M:%S %Y") << endl;
}

string tailText(const string& text, size_t max_width = 45) {
    if (text.length() > max_width) {
        return "..." + text.substr(text.length() - (max_width - 3), max_width - 3);
    }
    return text;
}

size_t longest_line_width(const string& cellText) {
    stringstream ss(cellText);
    string line;
    size_t maxWidth = 0;

    // Split content based on \n and return the longest length
    while (getline(ss, line, '\n')) {
        maxWidth = max(maxWidth, line.length());
    }

    return maxWidth;
}


//All format-related code for the tables
void formatTables(Table headerTable, Table sumTable, Table processHeaderTable, Table processHeaderTable2, Table processTable) {

    //format row to remove bottom borders
    headerTable[0].format().border_bottom("").corner_bottom_left("").corner_bottom_right("");
    headerTable[0].format().padding_bottom(0);
    //format custom top border to be like actual nvidia-smi
    sumTable[0].format().border_top("=");
    //align 3rd column to the right
    for (int i = 0; i < sumTable.size(); i++) {
        sumTable[i][2].format().font_align(FontAlign::right);
    }

    //manually find total width of the table
    size_t total_width = 0;
    for (size_t i = 0; i < sumTable[0].size(); i++) {
        size_t max_column_width = 0;
        for (size_t row = 0; row < sumTable.size(); ++row) {
            max_column_width = max(max_column_width, longest_line_width(sumTable[row][i].get_text()));
        }
        total_width += max_column_width + 2;  //take into account padding
    }

    //Adjust total width to consider borders
    total_width += sumTable[0].size() + 1;
    total_width -= 2;


    headerTable[0][0].format().width(total_width);

    //Create vector that determines the ratio of each column 
    vector<int> column_ratios = { 1, 1, 3, 1 };
    int total_ratio = 0;

    // Calculate the total ratio
    for (const auto& ratio : column_ratios) {
        total_ratio += ratio;
    }

    // Calculate individual column widths based on the max width
    vector<int> columnWidths;
    for (const auto& ratio : column_ratios) {
        columnWidths.push_back((ratio * total_width) / total_ratio);
    }

    /*PROCESSHEADER*/

    //format row to remove bottom borders
    processHeaderTable[0].format().border_bottom("").corner_bottom_left("").corner_bottom_right("");
    processHeaderTable[0].format().padding_bottom(0);

    processHeaderTable[0][0].format().width(total_width);

    /*PROCESSHEADER2*/
    
    //Formatting to match nvidia-smi (as close as possible)
    processHeaderTable2.format().border_bottom("=").border_top("").corner_top_right("").corner_top_left("").corner_bottom_left("|").corner_bottom_right("|");
    processHeaderTable2[0][0].format().corner_bottom_left("|");
    processHeaderTable2[0].format().corner_bottom_left("=").border_left(" ");
    processHeaderTable2[0].format().padding_bottom(0);
    //adjust header column width
    for (int i = 0; i < processHeaderTable2[0].size(); i++) {
        processHeaderTable2[0][i].format().width(columnWidths[i]).font_align(FontAlign::center);
    }
    processHeaderTable2[0][2].format().font_align(FontAlign::left);

    /*PROCESSTABLE*/

    //remove top border to connect with header table, fix missing border left edge
    processTable.format().border_top("").corner_top_left("").corner_bottom_left("-").border_left("");
    for (int i = 0; i < processTable.size(); i++) {
		processTable[i][0].format().border_left("|   ");
    }

    //Centers text and checks if a string is longer than the max width (set to 45 character limit)
    for (int i = 0; i < processTable.size(); i++) {
        for (int j = 0; j < processTable[i].size(); j++) {
            processTable[i][j].format().font_align(FontAlign::center);
            string origText = processTable[i][j].get_text();
            // Apply the tailText function
            string truncText = tailText(origText);
            // Set the new truncated text back to the cell
            processTable[i][j].set_text(truncText);
        }
    }


    //adjust process column width
    for (int i = 0; i < processTable.size(); i++) {
        for (int j = 0; j < processTable[i].size(); j++) {
            processTable[i][j].format().width(columnWidths[j]);
        }
    }
}
void createTables() {
    Table headerTable; //only contains the header (driver version, cuda version)
    Table sumTable; //contains the summary of the GPU
    Table processHeaderTable;
    Table processHeaderTable2;
    Table processTable;


    headerTable.add_row({ " NVIDIA-SMI 560.94                 Driver Version: 560.94         CUDA Version: 12.6     " });
    
    //Add summary of GPU
    sumTable.add_row({ "GPU  Name                  Driver-Model\nFan  Temp   Perf          Pwr:Usage/Cap","Bus-Id          Disp.A\nMemory-Usage","Volatile Uncorr. ECC\nGPU-Util  Compute M.\nMIG M." });
    sumTable.add_row({ "   0  NVIDIA GeForce RTX 3050 ...  WDDM  \n N/A   49C    P8              9W /   75W \n","   00000000:01:00.0 Off \n     320MiB /   4096MiB \n","                  N/A \n      6%      Default \nN/A" });
	
    processHeaderTable.add_row({ "Processes:" });
    processHeaderTable2.add_row({ "PID","Type","Process Name","GPU Memory Usage" });
    
    //inserting list of processes
    processTable.add_row({ "0","C+G","C:\\Users\\Charles\\Desktop\\COLLEGE\\CSOPESY\\CSOPESYWK4_Hinolan\\x64\\Release\\CSOPESYWK4_Hinolan.exe","N/A" });
    processTable.add_row({ "1","C+G","C:\\Users\\Charles\\IdeaProjects\\lz4-java-master\\src\\java.exe","N/A" });
    processTable.add_row({ "2","C+G","C:\\Users\\Charles\\Desktop\\COLLEGE\\personal_projects\\cplusplus\\randomProcess.exe","N/A" });
    processTable.add_row({ "3","C+G","C:\\Users\\Charles\\Desktop\\misc\\3ds-related\\backups\\really\\reallly\\long\\adresss\\helloworld.exe","N/A" });
    processTable.add_row({ "4","C+G","C:\\Users\\Charles\\Documents\\GitHub\\csnetwk-mco\\client.exe","N/A" });

    formatTables(headerTable, sumTable, processHeaderTable, processHeaderTable2, processTable);

    cout << headerTable << endl;
    cout << sumTable << '\n' << endl;
    cout << processHeaderTable << endl;
    cout << processHeaderTable2 << endl;
    cout << processTable << endl;
}



int main()
{
	print_datetime();
	createTables();

    return 0;
}

