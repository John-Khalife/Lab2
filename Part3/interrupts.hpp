/**
 * This file contains code for the interrupt simulator
 * @date September 30th, 2024
 * @author John Khalife, Stavros Karamalis
*/

#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

//dependencies
#include <iostream>
#include <fstream>
#include <string>


//This namespace contains simulated memory structures used by the CPU.
namespace MemoryStructures {

    int PARTITION_SIZES[] = {40,25,15,10,8,2}; 

    //This structure represents a single partition
    struct Partition {
        __uint128_t partitionNum;
        __uint128_t size;
        std::string code;
    } typedef part_t;

    //This structure represents a single PCB entry.
    struct PcbEntry {
        __uint128_t pid;
        __uint128_t cpuTime;
        __uint128_t partitionNum; 
        __uint128_t memoryAllocated;
        std::shared_ptr<PcbEntry> nextNode;
        std::ifstream filespot; 
    } typedef pcb_t;

    //This structure represents a file in persistent memory
    struct extFile {
        char programName[20];
        __uint128_t size;
    };

    /**
     * This method is intended to be used to add on a new process to the pcb.
     * @param head - the starting node (containing the init process)
     * @param pid - the process id
     * @param cpuTime - the amount of time remaining for the process to execute
     * @param partitionNum - the number of the partition
     * @param memoryAllocated - the amount of memory in mB allocated to the process
    */
    void addPCBEntry(std::shared_ptr<pcb_t> head,
        __uint128_t pid,
        __uint128_t cpuTime,
        __uint128_t partitionNum,
        __uint128_t memoryAllocated);
}


//These functions and structures are responsible for Parsing.
namespace Parsing {

    const int MAX_PARAMETERS = 2; // Constant that holds the maximum number of parameters a command can have.

    // If ever a new instruction needs to be added - add the equivalent string here
    namespace orders {
        using namespace std;
        const string CPU = "CPU";
        const string SYSCALL = "SYSCALL";
        const string END_IO = "END_IO";
        const string FORK = "FORK";
        const string EXEC = "EXEC";
    }


    void readExtFiles(std::ifstream* file, MemoryStructures::extFile* head);

    //This structure holds parameters
    struct Parameter {
        bool isString = false; //This is a type tag
        union {
            int number;
            char* word;
        };
    };

    //This struct holds the contents of an instruction including command and arguments
    struct Instruction{
        Parameter args[MAX_PARAMETERS];
        std::string commandName;
    } typedef instr;

    /**
     * This function reads from a trace given and returns an instruction each time it is called.
     * @param file - an ifstream object that provides access to the trace.
     * @return an instruction object containing the neccessary information for the CPU to execute a command.
    */
    instr* readFromTrace(std::ifstream* file);

    /**
     * This function converts an integer to a hexidecimal string.
     * @param number - an integer that will be converted.
     * @return a hexidecimal string.
    */
    std::string integerToHexString(int number);
};

//All functions in this namespace are responsible for execution
namespace Execution {

    /**
     * This method represents the CPU instruction that can be given from the trace.
     * @param duration - an integer representing the duration of the command.
     * @param output - an output stream for writing to the execution file.
    */
    void executeCPU(std::ofstream* output, int duration);

    /**
     * This method is intended to process interrupts given by an I/O device connected to the CPU.
     * @param duration - an integer representing the duration of the command.
     * @param isrAddress - an integer representing the address of the ISR address in the vector table
     * @param output - an output stream for writing to the execution file.
    */
    void interrupt(std::ofstream* output, int duration,int isrAddress);

    /**
     * This method is meant to be used to access the vector table given an address and will output the ISR address it found.
     * @param isrAddress - an integer representing the address of the ISR address in the vector table
     * @param output - an output stream for writing to the execution file.
    */
    void accessVectorTable(std::ofstream* output, int isrAddress);

    /**
     * Method used to write CPU events to the output file
     * @param duration - An integer stating the timer taken for the CPU to complete the action
     * @param eventType - A string dictating the action of the CPU
     * @param output - an output stream for writing to the execution file.
     * 
    */
    void writeExecutionStep(std::ofstream* output, int duration, std::string eventType);

    /**
     * This method is intended to be used for a system call - it checks the input device.
     * @param duration - An integer stating the timer taken for the CPU to complete the action
     * @param isrAddress - An integer stating the memory address in the vector table for the ISR.
     * @param output - an output stream for writing to the execution file.
    */
    void systemCall(std::ofstream* output, int duration, int isrAddress);

    /**
     * This method is intended to handle the fork instruction 
     * @param output - an output stream for writing to the execution file.
     * @param duration - An integer stating the time taken for the CPu to complete the action
    */
    void fork(std::ofstream* output, int duration, std::shared_ptr<MemoryStructures::pcb_t> pcb);

   /**
    * This method handles the execute instruction
    * @param filename - a string representing the file name
    * @param duration - An integer stating the time taken for the CPU to complete the action
   */
    void exec(std::ofstream* output, std::string filename, int duration, std::shared_ptr<MemoryStructures::pcb_t> pcb, MemoryStructures::File* files);

    /**
     * This method is used to call the appropriate function based on the instrcution given.
     * @param instruction - a instr struct that contains the command and any parameters it may have
     * @param output - an output stream for writing to the execution file.
    */
    void executeInstruction(std::ofstream* output, Parsing::instr* instruction, std::shared_ptr<MemoryStructures::pcb_t> pcb, MemoryStructures::File* files);
};


#endif