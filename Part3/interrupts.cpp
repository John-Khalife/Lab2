/**
 * This is the definition file for the methods belonging to the CPU class.
 * @date September 30th, 2024
 * @author John Khalife, Stavros Karamalis
*/

#include <iostream>
#include <fstream>
#include <random>
#include <sstream>
#include <string>
#include <iomanip>
#include "interrupts.hpp"

namespace Parsing {

    void readExtFiles(std::ifstream* file, MemoryStructures::extFile* node)
    {
        if(!file->eof()) {
                std::string text;
                getline(*(file),text);
                for(int i = 0, len = text.size(); i < len; i++){
                    if (text[i] == ','){
                        text.erase(i--, 1);
                        len = text.size();
                    }
                }
                
                std::stringstream ss(text);
                std::string temp;
                getline(ss, temp,' ');// First parameter always program name
                for (int i = 0, len = temp.size(); i < len; i++){
                    node->programName[i] = temp[i];
                }
                getline(ss,temp,' ');
                node->size = stoi(temp);
        }
    }


    instr* readFromTrace(std::ifstream* file)
        {
            instr* operation = new instr();
            if (!file->eof()) {
                std::string text;
                getline(*(file),text);
                for(int i = 0, len = text.size(); i < len; i++){
                    if (text[i] == ','){
                        text.erase(i--, 1);
                        len = text.size();
                    }
                }
                
                std::stringstream ss(text);
                int i = 0;
                std::string s;
                getline(ss,operation->argName,' ');// First parameter always command
                while (getline(ss,s,' ')) { //while for any args
                    operation->args[i] = stoi(s);
                    i++;
                }
            } else {
                file->close();
            }
            return operation;
        }

        std::string integerToHexString(int number) {
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(4) << std::hex << number;
        return "0x" + ss.str();
    }
}

namespace Execution {

    void writeExecutionStep(std::ofstream* file, int duration, std::string eventType) {
        static int timer = 0;
        (*file) << timer << ", " << duration << ", " << eventType << std::endl; // Write the Execution message in the proper format
        timer += duration; //Add the amount of timer to CPU timer for the next write
    }

    void systemCall(std::ofstream* output,int duration, int isrAddress) {
        accessVectorTable(output, isrAddress);

        std::default_random_engine generator; // generates uniformly distributed numbers
        generator.seed(time(0)); //Give the generator a seed
        std::uniform_real_distribution<> isrDistribution(0, 0.25); //probability distribution for 0-25% of the duration.
        int dataTransferTime = (int) duration*isrDistribution(generator) + 1; //time for data transfer
        int errorCheckingTime = (int) duration*isrDistribution(generator) + 1; //generate time for error checking

        //Call the device driver
        writeExecutionStep(output,duration - dataTransferTime - errorCheckingTime, "SYSCALL: Execute ISR.");
        writeExecutionStep(output,dataTransferTime,"Transfer data."); //transfer data
        writeExecutionStep(output,errorCheckingTime,"Check for errors."); // Error check
        writeExecutionStep(output,1,"IRET"); // Interrupt return.
    }

    void executeCPU(std::ofstream* output, int duration) {
        writeExecutionStep(output,duration,"CPU Execution.");
    }

    void interrupt(std::ofstream* output, int duration, int isrAddress) {
        writeExecutionStep(output ,1,"Check priority of interrupt.");
        writeExecutionStep(output,1,"Check if interrupt is masked.");
        accessVectorTable(output,isrAddress);
        writeExecutionStep(output,duration,"END_IO"); 
        writeExecutionStep(output,1,"IRET"); // Interrupt return.
    }

    void accessVectorTable(std::ofstream* output, int isrAddress) {
        std::default_random_engine generator; // generates uniformly distributed numbers
        generator.seed(time(0)); //Give the generator a seed
        std::uniform_int_distribution<int> contextSaveTimeDistribution(1,3); //Create a distribution

        //First switch to kernel mode
        writeExecutionStep(output, 1,"Switch CPU to Kernel mode.");
        
        using namespace std;
        writeExecutionStep(output, contextSaveTimeDistribution(generator),"Context saved."); //Save context

        //Check vector table and call ISR
        ifstream file;
        string text;
        
        writeExecutionStep(output, 1,"Find vector " + std::to_string(isrAddress) + " in memory position " + Parsing::integerToHexString(isrAddress) + ".");
        file.open("vector_table.txt");
        for (int i = 0 ; i <= isrAddress ; i++) {
            getline(file,text);
        }
        file.close(); // Now the text string should contain the ISR.
        writeExecutionStep(output, 1, "Load address " + text + " into the PC."); //output the address being loaded
    }

    void executeInstruction(std::ofstream* output, Parsing::instr* instruction) {
        if (!Parsing::orders::CPU.compare(instruction->argName)) {
            executeCPU(output,instruction->args[0]);
        } else if (!Parsing::orders::SYSCALL.compare(instruction->argName)) {
            systemCall(output,instruction->args[1],instruction->args[0]);
        } else if (!Parsing::orders::END_IO.compare(instruction->argName)) {
            interrupt(output,instruction->args[1],instruction->args[0]);
        }
    }
}

int main(int argc, char* argv[]) {
    //Check to make sure there are arguments
    if (argc <= 1 || argc > 2) {
        std::cout << "Improper argument number." << std::endl;
        return 1;
    }

    //Get the file number in the title of trace (if it is there)
    std::string fileNum = ((std::string) argv[1]).substr(((std::string) argv[1]).find_first_of("0123456789"),((std::string) argv[1]).find_last_of("0123456789") - ((std::string) argv[1]).find_first_of("0123456789") + 1); //Grab the value before .txt
    std::ofstream output;
    //Create input and output file objects
    if (isdigit(fileNum[0])) { //If the value is anumber, add it to the end of Execution.
        output.open("Execution" + fileNum + ".txt");
    } else {
        output.open("Execution.txt"); //otherwise just open Execution.txt as the output file.
    }
    std::ifstream input(argv[1]);
        while(input.is_open()){
            Parsing::instr* operation = Parsing::readFromTrace(&input);
            Execution::executeInstruction(&output,operation);
        }
    return 0;
}
