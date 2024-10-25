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
    int priority = 0;
};

void SortFinishedProcess(PCB* process, std::list<PCB*> &finishedProcesses);
std::list<PCB*> GetProcessControlBlocks();
void FCFS();
void SJF();
void MLQF();
void PrintResults(std::list<PCB*> finishedProcesses, int cpuTime, int systemTime);

int main() {
    std::cout<<"CPU Scheduling algorithms.\nOptions: \tFCFS\tSJF\tMLQF\n";

    std::string input;
    while (true) {
        std::cin >> input;
        for (int i = 0; i < input.size(); i++)
            input[i] = std::tolower(input[i]);
        
        if (input == "fcfs") {
            FCFS();
            break;
        }
        else if (input == "sjf") {
            SJF();
            break;
        }
        else if (input == "mlqf") {
            MLQF();
            break;
        }
        else
            std::cout<<"Invalid input.\tPlease type either FCFS, SJF, or MLQF\n";
    }
    
    return 0;
}

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
    PCB* running = NULL;
    std::list<PCB*> readyList = GetProcessControlBlocks();
    std::list<PCB*> waitingList;
    std::list<PCB*> finishedProcesses;
    int systemTime = 0;
    int cpuTime = 0;
    while(true) {

        if (running != NULL) {
            running->burstTimes[running->burstCounter] = running->burstTimes[running->burstCounter] - 1;
            cpuTime++;
            if (running->burstTimes[running->burstCounter] == 0) {
                running->burstCounter = running->burstCounter + 1;

                //remove process if that was its last burst
                if (running->burstCounter >= running->burstTimes.size()) {
                    running->turnAroundTime = systemTime;
                    SortFinishedProcess(running, finishedProcesses);
                }
                else
                    waitingList.push_back(running);
                running = NULL;
            }
        }
        if (readyList.size() != 0 && running == NULL) {
            running = readyList.front();
            readyList.pop_front();
            if (running->burstCounter == 0) {
                running->responseTime = systemTime;
            }
        }

        //increment time of all processes in the ready queue, and the systen time
        for (std::list<PCB*>::iterator it = readyList.begin(); it != readyList.end(); ++it){
            (*it)->waitTime = (*it)->waitTime + 1;
        }

        for (std::list<PCB*>::iterator it = waitingList.begin(); it != waitingList.end();) {
            (*it)->burstTimes[(*it)->burstCounter] = (*it)->burstTimes[(*it)->burstCounter] - 1;
            if ((*it)->burstTimes[(*it)->burstCounter] == 0) {
                PCB* process = *it;
                process->burstCounter = process->burstCounter + 1;
                readyList.push_back(process);
                it = waitingList.erase(it);
            }
            else {
                it++;
            }
        }
        
        

        //exit if their are no processes left
        if (running == NULL && readyList.size() == 0 && waitingList.size() == 0)
            break;
        
        systemTime++;
    }
    PrintResults(finishedProcesses, cpuTime, systemTime);
}

void SJF() {
    PCB* running = NULL;
    //all lists are implemented as linked lists using std::list
    std::list<PCB*> readyList = GetProcessControlBlocks();
    std::list<PCB*> waitingList;
    std::list<PCB*> finishedProcesses;
    int systemTime = 0;
    int cpuTime = 0;
    while(true) {

        if (running != NULL) {
            //decrement the running process' current cpu burst if there is a running process
            running->burstTimes[running->burstCounter] = running->burstTimes[running->burstCounter] - 1;
            cpuTime++;
            //if the cpu birst has reached zero, move it to the waiting list if it has a following io burst. Otherwise, move it to finished-processes
            if (running->burstTimes[running->burstCounter] == 0) {
                running->burstCounter = running->burstCounter + 1;

                //remove process if that was its last burst
                if (running->burstCounter >= running->burstTimes.size()) {
                    //since all processes entered at time 0, turnaround time is the toal time from when they first entered the readylist, to when the process compkletes and exits
                    running->turnAroundTime = systemTime;
                    SortFinishedProcess(running, finishedProcesses);
                }
                else
                    waitingList.push_back(running);
                running = NULL;
            }
        }
        if (readyList.size() != 0 && running == NULL) {
            //find the shortest job in the ready list
            PCB* shortestJob = readyList.front();
            for (std::list<PCB*>::iterator it = readyList.begin(); it != readyList.end(); ++it) {
                if ((*it)->burstTimes[(*it)->burstCounter] < shortestJob->burstTimes[shortestJob->burstCounter])
                    shortestJob = *it;
            }

            //remove it from the ready
            readyList.remove(shortestJob);
            running = shortestJob;
            shortestJob = NULL;

            if (running->burstCounter == 0) {
                running->responseTime = systemTime;
            }
        }


        //increment time of all processes in the ready list, and the systen time
        for (std::list<PCB*>::iterator it = readyList.begin(); it != readyList.end(); ++it){
            (*it)->waitTime = (*it)->waitTime + 1;
        }

        for (std::list<PCB*>::iterator it = waitingList.begin(); it != waitingList.end();) {
            (*it)->burstTimes[(*it)->burstCounter] = (*it)->burstTimes[(*it)->burstCounter] - 1;
            if ((*it)->burstTimes[(*it)->burstCounter] == 0) {
                PCB* process = *it;
                process->burstCounter = process->burstCounter + 1;
                readyList.push_back(process);
                it = waitingList.erase(it);
            }
            else {
                it++;
            }
        }
        systemTime++;
        

        //exit if their are no processes left
        if (running == NULL && readyList.size() == 0 && waitingList.size() == 0)
            break;
    }

    PrintResults(finishedProcesses, cpuTime, systemTime);
}

void MLQF() {
    //all processes start in the round-robin time 5 ready queue
    std::list<PCB*> rr5ReadyList = GetProcessControlBlocks();
    std::list<PCB*> rr10ReadyList;
    std::list<PCB*> fcfsReadyList;
    std::list<PCB*> waitingList;
    std::list<PCB*> finishedProcesses;
    PCB* running = NULL;
    int rr5Time = 0;
    int rr10Time = 0;
    int systemTime = 0;
    int cpuTime = 0;
    while (true) {

        if (running != NULL) {
            //decrement thr remaining time in the process burst time
            running->burstTimes[running->burstCounter] = running->burstTimes[running->burstCounter] - 1;
            cpuTime++;
            //if their is no time left in the burst, move it to the waiting list if their is another burst time, otherwise move it to finished processes
            if (running->burstTimes[running->burstCounter] == 0) {
                running->burstCounter = running->burstCounter + 1;
                //remove process if that was its last burst
                if (running->burstCounter >= running->burstTimes.size()) {
                    running->turnAroundTime = systemTime;
                    SortFinishedProcess(running, finishedProcesses);
                }
                else {
                    waitingList.push_back(running);
                }
                running = NULL;
                rr5Time = 0;
                rr10Time = 0;
            }
            else {
                //if the running process has not finished, check its process priority and check if the time quantum has expired
                switch (running->priority) {
                    case 0: {
                        rr5Time++;
                        if (rr5Time == 5) {
                            //if the time quantum has expired, increment the process priority and move it to the round-robin time 10 queue
                            running->priority = running->priority + 1;
                            rr10ReadyList.push_back(running);
                            running = NULL;
                            rr5Time = 0;
                        }
                        break;
                    }

                    case 1: {
                        rr10Time++;
                        if (rr10Time == 10) {
                            //increment the priority and move the process into the fcfs queue
                            running->priority = running->priority + 1;
                            fcfsReadyList.push_back(running);
                            running = NULL;
                            rr10Time = 0;
                        }
                        break;
                    }
                    //do nothing if the process is in the fcfs queue
                    default: 
                        break;
                }
            }
        }
        if (running == NULL) {
            //checks each queue in order of priority. the first one that is not empty will be selected
            std::list<PCB*>* current_queue = NULL;
            if (rr5ReadyList.size() != 0) 
                current_queue = &rr5ReadyList;
            else if (rr10ReadyList.size() != 0)
                current_queue = &rr10ReadyList;
            else if (fcfsReadyList.size() != 0)
                current_queue = &fcfsReadyList;
            
            if (current_queue != NULL) {
                //set running to the first process in the queue
                running = (*current_queue).front();
                (*current_queue).pop_front();
                if (running->burstCounter == 0) 
                    running->responseTime = systemTime;
            }
        }

        //increment time of all processes in the ready queue, and the system time
        for (std::list<PCB*>::iterator it = rr5ReadyList.begin(); it != rr5ReadyList.end(); ++it){
            (*it)->waitTime = (*it)->waitTime + 1;
        }
        for (std::list<PCB*>::iterator it = rr10ReadyList.begin(); it != rr10ReadyList.end(); ++it){
            (*it)->waitTime = (*it)->waitTime + 1;
        }
        for (std::list<PCB*>::iterator it = fcfsReadyList.begin(); it != fcfsReadyList.end(); ++it){
            (*it)->waitTime = (*it)->waitTime + 1;
        }

        for (std::list<PCB*>::iterator it = waitingList.begin(); it != waitingList.end();) {
            //decrement the bursat time for every process in the waiting list
            (*it)->burstTimes[(*it)->burstCounter] = (*it)->burstTimes[(*it)->burstCounter] - 1;
            //if the process has finished its burst time, move it back into a ready queue based on its priority
            if ((*it)->burstTimes[(*it)->burstCounter] == 0) {
                PCB* process = *it;
                process->burstCounter = process->burstCounter + 1;
                switch (process->priority) {
                    case 0: {
                        rr5ReadyList.push_back(process);
                        break;
                    }
                    case 1: {
                        rr10ReadyList.push_back(process);
                        break;
                    }
                    case 2: {
                        fcfsReadyList.push_back(process);
                        break;
                    }
                }
                it = waitingList.erase(it);
            }
            else {
                it++;
            }
        }

        
        systemTime++;
        

        //exit if their are no processes left
        if (running == NULL && rr5ReadyList.size() == 0 && rr10ReadyList.size() == 0 && fcfsReadyList.size() == 0 && waitingList.size() == 0)
            break;
    }

    PrintResults(finishedProcesses, cpuTime, systemTime);
}

void PrintResults(std::list<PCB*> finishedProcesses, int cpuTime, int systemTime) {
    std::cout<<"Total system time: "<<systemTime<<"\n";
    std::cout<<"CPU utilization: "<<((int)((cpuTime/(float)systemTime)*10000))/100.0f<<"%\n";


    float cumulativeWaitTime = 0, cumulativeResponseTime = 0, cumulativeTurnaroundTime = 0;
    for (std::list<PCB*>::iterator it = finishedProcesses.begin(); it != finishedProcesses.end(); ++it) {
        cumulativeWaitTime += (*it)->waitTime;
        cumulativeResponseTime += (*it)->responseTime;
        cumulativeTurnaroundTime += (*it)->turnAroundTime;
        std::cout<<"Process "<<(*it)->processNum<<":\n";
        std::cout<<"\t Wait Time: "<<(*it)->waitTime<<"\n";
        std::cout<<"\t Turnaround Time: "<<(*it)->turnAroundTime<<"\n";
        std::cout<<"\t Response Time: "<<(*it)->responseTime<<"\n\n";
        free(*it);
    }
    std::cout<<"Average Waiting Time (Tw): " << cumulativeWaitTime / (float)finishedProcesses.size() << "\n";
    std::cout<<"Average Response Time (Tr): " << cumulativeResponseTime / (float)finishedProcesses.size() << "\n";
    std::cout<<"Average Turnaround Time (Ttr): " << cumulativeTurnaroundTime / (float)finishedProcesses.size() << "\n";
}

void SortFinishedProcess(PCB* process, std::list<PCB*> &finishedProcesses) {
    std::list<PCB*>::iterator it = finishedProcesses.begin();
    while(it != finishedProcesses.end() && (*it)->processNum < process->processNum) {
        it++;
    }
    finishedProcesses.insert(it, process);
}
