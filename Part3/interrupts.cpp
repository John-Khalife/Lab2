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
#include <memory>

#include "interrupts.hpp"

namespace MemoryStructures {
    void copyPCBEntry(std::shared_ptr<pcb_t>& entry)    
    {
            if (!entry) {
                auto newNode = std::make_shared<pcb_t>(
                    highestPid,
                    "init",
                    6,
                    1,
                    NULL,
                    NULL,
                    true,
                    entry);
                highestPid++;
                entry = newNode;
            } else {
                //Create the shared pointer to the child process + increment highest pid
                auto newNode = std::make_shared<pcb_t>(
                    highestPid,
                    entry->programName,
                    entry->partitionNum,
                    entry->memoryAllocated,
                    NULL,
                    entry->filespot,
                    true,
                    entry);
                highestPid++;
                //Tell the parent process not to take the next exec command
                entry->doExec = false;
                //Get to the end of the list
                std::shared_ptr<pcb_t> temp = entry;
                while (temp->nextNode != NULL) {
                    temp = temp->nextNode;
                }
                temp->nextNode = newNode;
            }
    }

    void modifyPCBEntry(
        std::shared_ptr<pcb_t>& entry,
        char programName[20],
        __uint8_t partitionNum,
        __uint128_t memoryAllocated) {
        if (entry == NULL) {return;}
        std::strcpy(entry->programName, programName);
        entry->partitionNum = partitionNum;
        entry->memoryAllocated = memoryAllocated;
        entry->filespot = std::ifstream(std::string(programName) + ".txt");  
        if (entry->parent != NULL) {
            entry->parent->doExec = false;
        } 
        if (entry->doExec == false) {
            entry->doExec = true;
        }
    }

    std::string pcbToString(std::shared_ptr<pcb_t>& pcb) {
        
        std::shared_ptr<pcb_t> temp = pcb;
        while (temp->parent != NULL) {
            temp = temp->parent;
        }
        std::string output;
        output.append("+----------------------------------------------+\n");
        output.append("| PID | Program Name | Partition Number | Size |\n");
        output.append("+----------------------------------------------+\n");

        
        while (temp != NULL) {
            output.append("| "); 
            output.append(std::to_string((int) temp->pid));
            output + std::string(snprintf(0,0,"%+d",2 - ((int) temp->pid)-1),' ');
            output.append(" | ");
            output.append(temp->programName);
            output + std::string(snprintf(0,0,"%+d",13 - (int) strlen(temp->programName)),' ');
            output.append(" | ");
            output.append(std::to_string((int) temp->partitionNum));
            output + std::string(snprintf(0,0,"%+d",16 - ((int) temp->partitionNum)-1),' ');
            output.append(" | ");
            output.append(std::to_string((int) temp->memoryAllocated));
            output + std::string(snprintf(0,0,"%+d",4 - ((int) temp->memoryAllocated)-1),' ');
            output.append(" |\n");
            temp = temp->nextNode;
        }
        output.append("+----------------------------------------------+\n");
        return output;
    }

    int reserveMemory(Partition* memory, __uint8_t size, char* programName) {
        //*NOTE Partition sizes are ordered from largest to smallest - so best fit will be easy.
        //*That means however, this method may need to be updated in the future if different partitions are given.
        for (int i = sizeof(PARTITION_SIZES)/sizeof(int) - 1; i >= 0 ; i--) {
            if (std::strcmp(memory[i].code,"free") == 0) {
                if (size < memory[i].size) {
                    memory[i].code = programName;
                    return i;
                }
                
            }
        }
        return -1;
    }

    void freeMemory(Partition* memory, __uint8_t partitionNum) {
        memory[partitionNum].code = "free";
    }

    __uint8_t getFileSize(std::shared_ptr<MemoryStructures::extFile>& head, char* programName) {
        std::shared_ptr<MemoryStructures::extFile> temp = head;
        while(temp != NULL) {
            if (std::strcmp(programName,temp->programName) == 0) {
                return temp->size;
            }
            temp = temp->nextNode;
        }
        return 0;
    }

    std::shared_ptr<pcb_t> getRunningProcess(std::shared_ptr<pcb_t>& pcb) {
        std::shared_ptr<pcb_t> temp = pcb;
        while(temp->nextNode != NULL) {
            temp = temp->nextNode;
        }
        while (temp != NULL) {
            if (temp->filespot.is_open()) {
                return temp;
            }
            temp = temp->parent;
        }
        return 0;
        
    }
}

namespace Parsing {

    void readExtFiles(std::shared_ptr<MemoryStructures::extFile>& head)
    {
        std::ifstream file(PROGRAMS_LIST_FILE_NAME);
        while (!file.eof()) {
                std::shared_ptr<MemoryStructures::extFile> latestNode = head;
                std::shared_ptr<MemoryStructures::extFile> newNode = std::make_shared<MemoryStructures::extFile>();
                std::string text;
                getline(file,text);
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
                    newNode->programName[i] = temp[i];
                }
                getline(ss,temp,' ');
                newNode->size = stoi(temp);
                newNode->nextNode = NULL;

                if (!head) {
                    head = newNode;
                } else {
                    latestNode->nextNode = newNode;
                    latestNode = latestNode->nextNode;
                }
                
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
                getline(ss,operation->commandName,' ');// First parameter always command
                while (getline(ss,s,' ')) { //while for any args
                    //Check if the argument is a string or an integer
                    for (int i = 0 ; i < std::strlen(s.c_str()) ; i++) {
                        if (!isdigit(s[i])) {
                            operation->args[i].isString = true;
                            break;
                        }
                    }

                    if (operation->args[i].isString) {
                        std::strncpy(operation->args[i].word, s.c_str(), 20);
                    } else {
                        operation->args[i].number = stoi(s);
                    }
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

    bool setOutputFile(std::string filename) {
        output.open(filename);
        if (output.fail()) {return false;}
        return true;
    }

    void writeExecutionStep(int duration, std::string eventType) {
        if (output.fail()) {return;}
        output << timer << ", " << duration << ", " << eventType << std::endl; // Write the Execution message in the proper format
        timer += duration; //Add the amount of timer to CPU timer for the next write
    }

    void writePcbTable(std::shared_ptr<MemoryStructures::pcb_t> pcb) {
        static std::ofstream pcbOutput(PCB_OUTPUT_FILE_NAME);

        pcbOutput << "!-----------------------------------------------------------!" << std::endl;
        pcbOutput << "Save Time: " << timer << " ms" << std::endl;
        pcbOutput << MemoryStructures::pcbToString(pcb);
        pcbOutput << "!-----------------------------------------------------------!" << std::endl;
    }

    void systemCall(std::ofstream* output,int duration, int isrAddress) {
        accessVectorTable(isrAddress);

        std::default_random_engine generator; // generates uniformly distributed numbers
        generator.seed(time(0)); //Give the generator a seed
        std::uniform_real_distribution<> isrDistribution(0, 0.25); //probability distribution for 0-25% of the duration.
        int dataTransferTime = (int) duration*isrDistribution(generator) + 1; //time for data transfer
        int errorCheckingTime = (int) duration*isrDistribution(generator) + 1; //generate time for error checking

        //Call the device driver
        writeExecutionStep(duration - dataTransferTime - errorCheckingTime, "SYSCALL: Execute ISR.");
        writeExecutionStep(dataTransferTime,"Transfer data."); //transfer data
        writeExecutionStep(errorCheckingTime,"Check for errors."); // Error check
        writeExecutionStep(1,"IRET"); // Interrupt return.
    }

    void executeCPU(std::ofstream* output, int duration) {
        writeExecutionStep(duration,"CPU Execution.");
    }

    void interrupt(int duration, int isrAddress) {
        writeExecutionStep(1,"Check priority of interrupt.");
        writeExecutionStep(1,"Check if interrupt is masked.");
        accessVectorTable(isrAddress);
        writeExecutionStep(duration,"END_IO"); 
        writeExecutionStep(1,"IRET"); // Interrupt return.
    }

    void accessVectorTable(std::ofstream* output, int isrAddress) {
        std::default_random_engine generator; // generates uniformly distributed numbers
        generator.seed(time(0)); //Give the generator a seed
        std::uniform_int_distribution<int> contextSaveTimeDistribution(1,3); //Create a distribution

        //First switch to kernel mode
        writeExecutionStep(1,"Switch CPU to Kernel mode.");
        
        using namespace std;
        writeExecutionStep(contextSaveTimeDistribution(generator),"Context saved."); //Save context

        //Check vector table and call ISR
        ifstream file;
        string text;
        
        writeExecutionStep(1,"Find vector " + std::to_string(isrAddress) + " in memory position " + Parsing::integerToHexString(isrAddress) + ".");
        file.open("vector_table.txt");
        for (int i = 0 ; i <= isrAddress ; i++) {
            getline(file,text);
        }
        file.close(); // Now the text string should contain the ISR.
        writeExecutionStep(1, "Load address " + text + " into the PC."); //output the address being loaded
    }

void fork(int duration, std::shared_ptr<MemoryStructures::pcb_t>& currentProcess) {
        std::default_random_engine generator; // generates uniformly distributed numbers
        generator.seed(time(0)); //Give the generator a seed
        std::uniform_int_distribution<int> forkTimeDistribution(1,9); //Create a distribution

        //a. simulates system call - access vector table
        accessVectorTable(2);
        //b. Copy parent PCB to child process
        //*NOTE: The assignment stated a random time of 1-10 but also must sum to duration.
        //*Assumption: a fork call will not take less than 10ms.
        int forkTime = floor((forkTimeDistribution(generator)/100)*duration);
        writeExecutionStep(forkTime, "Copy parent PCB to child PCB"); 
        MemoryStructures::copyPCBEntry(currentProcess);
        //c. Call the routing scheduler (all it should display for now is 'scheduler called')
        writeExecutionStep(duration - forkTime,"Scheduler called.");
        //d. return from the ISR
        writeExecutionStep(1,"IRET"); // Interrupt return.
        writePcbTable(currentProcess);
    }

    void exec(char* filename, int duration, std::shared_ptr<MemoryStructures::pcb_t>& currentProcess, std::shared_ptr<MemoryStructures::extFile>& files,MemoryStructures::Partition* memory) {
        std::default_random_engine generator; // generates uniformly distributed numbers
        generator.seed(time(0)); //Give the generator a seed
        std::uniform_real_distribution<float> execTimeDistribution(0,0.2); //Create a distribution

        //a. simulate a system call - access the vector table
        accessVectorTable(3);
        //b. find the file in the list, and set the size in the PCB
        __uint8_t size = MemoryStructures::getFileSize(files,filename);
        if (size == 0) {
            writeExecutionStep(0,"ERROR: No file of that name exists");
            return;
        }
        int execTime = floor((0.2)*duration);
        writeExecutionStep(execTime,"EXEC: load " + std::string(filename) + " of size " + std::to_string(size) + "Mb.");
        //c. find an empty partition where the program fits, with the best fit policy
        int partitionNum = MemoryStructures::reserveMemory(memory,size,filename);
        if (partitionNum < 0) {
            writeExecutionStep(0,"ERROR: Unable to allocate memory for process");
            return;
        }
        int findTime = floor((execTimeDistribution(generator))*duration);
        writeExecutionStep(findTime,"Found partition " + std::to_string(partitionNum) + " with " + std::to_string(memory[partitionNum].size) + "Mb of space." );
        //d. mark the partition as occupied
        int occupiedTime = floor((execTimeDistribution(generator))*duration);
        writeExecutionStep(occupiedTime,"Marked partition " + std::to_string(partitionNum) + " as occupied.");
        //e. update PCB
        int updateTime = floor((execTimeDistribution(generator))*duration);
        writeExecutionStep(updateTime,"Updating PCB with new information.");
        MemoryStructures::modifyPCBEntry(currentProcess,filename,partitionNum,size);
        //f. do the routing scheduler thing again
        writeExecutionStep(duration - updateTime - occupiedTime - findTime - execTime,"Scheduler called.");
        //g. return normally
        writeExecutionStep(1,"IRET"); // Interrupt return.
        writePcbTable(currentProcess);
    }

    void executeInstruction(
        Parsing::instr* instruction,
        std::shared_ptr<MemoryStructures::pcb_t>& currentProcess,
       std::shared_ptr<MemoryStructures::extFile>& files,
        MemoryStructures::Partition* memory) {

        if (!Parsing::orders::CPU.compare(instruction->commandName)) {
            executeCPU(instruction->args[0].number);
        } else if (!Parsing::orders::SYSCALL.compare(instruction->commandName)) {
            systemCall(instruction->args[1].number,instruction->args[0].number);
        } else if (!Parsing::orders::END_IO.compare(instruction->commandName)) {
            interrupt(instruction->args[1].number,instruction->args[0].number);
        } else if (!Parsing::orders::FORK.compare(instruction->commandName)) {
            fork(instruction->args[0].number,currentProcess);
        } else if (!Parsing::orders::EXEC.compare(instruction->commandName)) {
            exec(instruction->args[0].word,instruction->args[1].number,currentProcess, files, memory);
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
    if (isdigit(fileNum[0])) { //If the value is a number, add it to the end of Execution.
        Execution::setOutputFile("Execution" + fileNum + ".txt");
    } else {
        Execution::setOutputFile("Execution.txt"); //otherwise just open Execution.txt as the output file.
    }
    std::ifstream input(argv[1]); //input file opened

    //Initialize memory partitions with the proper sizes.
    using namespace MemoryStructures;
    Partition* memory = new Partition[6];
    for (int i = 0; i < sizeof(PARTITION_SIZES)/sizeof(int); i++) {
        memory[i] = (Partition) {.size = PARTITION_SIZES[i], .partitionNum = i, .code = "free"};
    } 
    //Initialize partition 6 with the PCB
    memory[5].code = "init";
    //Initialize pcb entry
    std::shared_ptr<PcbEntry> pcb;
    MemoryStructures::copyPCBEntry(pcb);
    //Initialize the file list.
    std::shared_ptr<extFile> files;
    Parsing::readExtFiles(files);

    //Loop continues while there are still processes with open files in the pcb.
    //Get the process that should be run right now. from the pcb
    //Start executing that line by line (continue to check for new processes each line)
    std::shared_ptr<PcbEntry> currentProcess = getRunningProcess(pcb);
    while(currentProcess->filespot.is_open()){
        Parsing::instr* operation = Parsing::readFromTrace(&currentProcess->filespot);
        Execution::executeInstruction(operation,currentProcess,files,memory);
        currentProcess = getRunningProcess(pcb);
    }

    //Cleanup 
    delete[] memory;
    //Everything else uses smart pointers - deallocated automatically when they leave scope
    return 0;
}
