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
#include <memory>
#include "interrupts.hpp"

namespace MemoryStructures {
        void addPCBEntry(std::shared_ptr<pcb_t>& head,
        __uint128_t pid,
        __uint128_t cpuTime,
        __uint128_t partitionNum,
        __uint128_t memoryAllocated)    
    {
            //Create the shared pointer
            auto newNode = std::make_shared<pcb_t>(pid,cpuTime,partitionNum,memoryAllocated,NULL);
            if (!head) {
                //If the passed node is empty
                head = newNode;
            } else {
                //make the newNode the new head o(1)
                newNode->nextNode = head;
                head = newNode;
            }
    }
}

namespace Parsing {

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

    void executeInstruction(std::ofstream* output, Parsing::instr* instruction, std::shared_ptr<MemoryStructures::pcb_t> pcb, MemoryStructures::File* files) {
        if (!Parsing::orders::CPU.compare(instruction->argName)) {
            executeCPU(output,instruction->args[0]);
        } else if (!Parsing::orders::SYSCALL.compare(instruction->argName)) {
            systemCall(output,instruction->args[1],instruction->args[0]);
        } else if (!Parsing::orders::END_IO.compare(instruction->argName)) {
            interrupt(output,instruction->args[1],instruction->args[0]);
        } else if (!Parsing::orders::FORK.compare(instruction->argName)) {
            fork(output,instruction->args[0]);
        } else if (!Parsing::orders::EXEC.compare(instruction->argName)) {
            interrupt(output,instruction->args[1],instruction->args[0]);
        }
    }

    void fork(std::ofstream* output, int duration, std::shared_ptr<MemoryStructures::pcb_t> pcb, MemoryStructures::File* files) {
        std::default_random_engine generator; // generates uniformly distributed numbers
        generator.seed(time(0)); //Give the generator a seed
        std::uniform_int_distribution<int> forkTimeDistribution(1,10); //Create a distribution

        //a. simulates system call - access vector table
        accessVectorTable(output, 2);
        //b. Copy parent PCB to child process
        writeExecutionStep(output,forkTimeDistribution(generator),"Copy parent PCB to child PCB"); //TODO: access pcb from here

        //c. Call the routing scheduler (all it should display for now is 'scheduler called')
        writeExecutionStep(output,forkTimeDistribution(generator),"Scheduler called.");
        //d. return from the ISR
        writeExecutionStep(output,1,"IRET"); // Interrupt return.
    }

    void exec(std::ofstream* output, std::string filename, int duration, std::shared_ptr<MemoryStructures::pcb_t> pcb, MemoryStructures::File* files) {
        std::default_random_engine generator; // generates uniformly distributed numbers
        generator.seed(time(0)); //Give the generator a seed
        std::uniform_int_distribution<int> execTimeDistribution(1,10); //Create a distribution
        //a. simulate a system call - access the vector table
        accessVectorTable(output, 3);
        //TODO: b. find the file in the list, and set the size in the PCB
        //TODO: c. find an empty partition where the program fits, with the best fit policy
        //TODO: d. mark the partition as occupied
        //TODO: e. update PCB
        //f. do the routing scheduler thing again
        writeExecutionStep(output,execTimeDistribution(generator),"Scheduler called.");
        //g. return normally
        writeExecutionStep(output,1,"IRET"); // Interrupt return.
    }
}

int main(int argc, char* argv[]) {
    //Initialize memory partitions with the proper sizes.
    using namespace MemoryStructures;
    Partition* memory = new Partition[6];
    for (int i = 0; i < sizeof(PARTITION_SIZES)/sizeof(int); i++) {
        memory[i] = (Partition) {.size = PARTITION_SIZES[i], .partitionNum = i, .code = "free"};
    } 
    //Initialize partition 6 with the PCB
    memory[5].code = "init";

    //initialize pcb entry with smart pointer
    std::shared_ptr<PcbEntry> pcb = std::make_shared<PcbEntry>(0,0,6,1,NULL);

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

    //Cleanup 
    delete[] memory;

    return 0;
}
