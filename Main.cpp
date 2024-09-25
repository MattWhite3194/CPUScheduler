#include <stdio.h>
#include <queue>
#include <iostream>
#include <list>

struct PCB{
    std::vector<int> burstTimes;
    int burstCounter = 0;
    int processNum;
    int waitTime = 0;
    int turnAroundTime = 0;
    int responseTime = 0;
    int lastWaitingEnterTime = 0;
    int lastRunningEnterTime = 0;
};

std::list<PCB*> GetProcessControlBlocks() {
    std::list<PCB*> PCBs;
    
    std::vector<std::vector<int>> processBurstTimes = {
        {5, 27, 3, 31, 5, 43, 4, 18, 6, 22, 4, 26, 3, 24, 4},
        {4, 48, 5, 44, 7, 42, 12, 37, 9, 76, 4, 41, 9, 31, 7, 43, 8},
        {8, 33, 12, 41, 18, 65, 14, 21, 4, 61, 15, 18, 14, 26, 5, 31, 6},
        {3, 35, 4, 41, 5, 45, 3, 51, 4, 61, 5, 54, 6, 82, 5, 77, 3},
        {16, 24, 17, 21, 5, 36, 16, 26, 7, 31, 13, 28, 11, 21, 6, 13, 3, 11, 4},
        {11, 22, 4, 8, 5, 10, 6, 12, 7, 14, 9, 18, 12, 24, 15, 30, 8},
        {14, 46, 17, 41, 11, 42, 15, 21, 4, 32, 7, 19, 16, 33, 10},
        {4, 14, 5, 33, 6, 51, 14, 73, 16, 87, 6}
    };

    int numProcesses = 8; 
    for (int i = 0; i < numProcesses; i++) {
        PCB* process = new PCB;
        process->burstTimes = processBurstTimes[i];
        process->processNum = i+1;
        PCBs.push_back(process);
    }

    return PCBs;
}

void FCFS() {
    std::list<PCB*> readyList = GetProcessControlBlocks();
    std::list<PCB*> waitingList;
    std::list<PCB*> finishedProcesses;
    PCB* running = NULL;
    int systemTime = 0;
    while(true) {

        //if there is no process running and a process in the ready queue, move that process to running
        if (running == NULL) {
            if (readyList.size() != 0) {
                running = readyList.front();
                readyList.pop_front();
                if (running->burstCounter == 0) {
                    running->responseTime = systemTime;
                    running->lastRunningEnterTime = systemTime;
                }
            }
        }
        else {
            if (systemTime - running->lastRunningEnterTime == running->burstTimes[running->burstCounter]) {
                running->burstCounter = running->burstCounter + 1;
                running->lastWaitingEnterTime = systemTime;

                //remove process if that was its last burst
                if (running->burstCounter >= running->burstTimes.size()) {
                    running->turnAroundTime = systemTime;
                    finishedProcesses.push_back(running);
                }
                else
                    waitingList.push_back(running);
                running = NULL;
            }
        }

        for (std::list<PCB*>::iterator it = waitingList.begin(); it != waitingList.end();) {
            if (systemTime - (*it)->lastWaitingEnterTime == (*it)->burstTimes[(*it)->burstCounter]) {
                PCB* process = *it;
                process->burstCounter = process->burstCounter + 1;
                readyList.push_back(process);
                it = waitingList.erase(it);
            }
            else {
                it++;
            }
        }
        //increment time of all processes in the ready queue, and the systen time
        for (std::list<PCB*>::iterator it = readyList.begin(); it != readyList.end(); ++it){
            (*it)->waitTime = (*it)->waitTime + 1;
        }
        systemTime++;

        //exit if their are no processes left
        if (running == NULL && readyList.size() == 0 && waitingList.size() == 0)
            break;
    }

    for (std::list<PCB*>::iterator it = finishedProcesses.begin(); it != finishedProcesses.end(); ++it) {
        std::cout<<"Process "<<(*it)->processNum<<":\n";
        std::cout<<"\t Wait Time: "<<(*it)->waitTime<<"\n";
        std::cout<<"\t Turnaround Time: "<<(*it)->turnAroundTime<<"\n";
        std::cout<<"\t Response Time: "<<(*it)->responseTime<<"\n\n";
    }

}

void SJF() {

}

void MLQF() {

}

int main() {
    FCFS();
    return 0;
}