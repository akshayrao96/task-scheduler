#include <iostream>
#include <fstream>
#include <queue>
#include <cstdlib>
#include <sstream>
#include <list>
#include <map>

using namespace std;

// creates a Process, with the relevant information about the process
struct Process
{
  int processID;
  int burstTime;
  int burstTimeLeft;
  int arrivalTime;
  int terminationTime;
  int turnaroundTime;
  int waitingTime;
  int queueIn;
};

// compares 2 processes based on arrival time first, then it's burst time, then ID
class cmpProcessArrivalTime
{
public:
  int operator()(const Process &p1, const Process &p2)
  {
    if (p1.arrivalTime == p2.arrivalTime)
    {
      if (p1.burstTime == p2.burstTime)
      {
        return p1.processID > p2.processID;
      }
      else
      {
        return p1.burstTime > p2.processID;
      }
    }
    else
    {
      return p1.arrivalTime > p2.arrivalTime;
    }
  }
};

// compares 2 processes based on id
class cmpProcessID
{
public:
  int operator()(const Process &p1, const Process &p2)
  {
    return p1.processID > p2.processID;
  }
};

// gets the process with the lowest burst time left
Process *getLowestBurstTimeProcess(vector<Process> &readyList)
{
  int i;
  Process *processPtr = &readyList[0];
  for (i = 1; i < readyList.size(); i++)
  {
    if (readyList[i].burstTimeLeft < processPtr->burstTimeLeft ||
        (readyList[i].burstTimeLeft == processPtr->burstTimeLeft && readyList[i].processID < processPtr->processID))
    {
      processPtr = &readyList[i];
    }
  }
  return processPtr;
}

// removes a finished process from ready list
void removeFinishedProcess(vector<Process> &readyList)
{
  int i;
  for (i = 0; i < readyList.size(); i++)
  {
    if (readyList[i].burstTimeLeft == 0)
    {
      readyList.erase(readyList.begin() + i);
      break;
    }
  }
}

// prints the order of which processes were being executed on, along with its termination time
void printOrder(queue<int> &orderOfProcesses, map<int, int> IDFinishTime)
{
  cout << "---ORDER OF PROCESS---" << endl;
  cout << orderOfProcesses.front() << "(" << IDFinishTime[orderOfProcesses.front()] << ")";
  orderOfProcesses.pop();
  while (!orderOfProcesses.empty())
  {
    cout << "," << orderOfProcesses.front() << "(" << IDFinishTime[orderOfProcesses.front()] << ")";
    orderOfProcesses.pop();
  }
  cout << endl
       << endl;
}

// prints the process id's turnaround and waiting times, in order of id
void updateTimes(vector<Process> &allProcessID, map<int, int> IDFinishTime)
{
  cout << endl;
  cout << setw(4) << "ID" << setw(15) << "Arrival Time" << setw(13) << "Burst Time" << setw(15) << "Finished Time" << setw(18) << "Turnaround Time" << setw(15) << "Waiting Time" << endl;
  int i;
  for (i = 0; i < allProcessID.size(); i++)
  {
    allProcessID[i].terminationTime = IDFinishTime[allProcessID[i].processID];
    allProcessID[i].turnaroundTime = allProcessID[i].terminationTime - allProcessID[i].arrivalTime;
    allProcessID[i].waitingTime = allProcessID[i].turnaroundTime - allProcessID[i].burstTime;
    cout << setw(4) << allProcessID[i].processID << setw(11) << allProcessID[i].arrivalTime << setw(12) << allProcessID[i].burstTime << setw(13) << allProcessID[i].terminationTime << setw(16) << allProcessID[i].turnaroundTime << setw(17) << allProcessID[i].waitingTime << endl;
  }
  cout << endl;
}

// prints the average turnaround and waiting time of all processes
void printAverageTimes(vector<Process> allProcessID)
{
  int i;
  int totalWaitingTime = 0;
  int totalTurnaroundTime = 0;
  for (i = 0; i < allProcessID.size(); i++)
  {
    totalWaitingTime += allProcessID[i].waitingTime;
    totalTurnaroundTime += allProcessID[i].turnaroundTime;
  }
  float avgWaitingTime = (float)totalWaitingTime / (float)i;
  float avgTurnaroundTime = (float)totalTurnaroundTime / (float)i;

  cout << "Average turnaround time for all processes: " << avgTurnaroundTime << endl;
  cout << "Average waiting time for all processes:    " << avgWaitingTime << endl;
  cout << endl;
}

// prints an error message
void errorMessage()
{
  cout << "USAGE: Executable file must take in 1 valid file path" << endl;
  cout << "EXAMPLE: ./question2 <file.txt>" << endl;
}

int main(int argc, char **argv)
{
  // throws error if not given exactly 1 argument
  if (argc != 2)
  {
    cout << "ERROR: Invalid number of arguments given" << endl;
    errorMessage();
    exit(EXIT_FAILURE);
  }

  priority_queue<Process, vector<Process>, cmpProcessArrivalTime> arrivalQueue; // In order of arrival time, breaking ties with burst time, then ID
  priority_queue<Process, vector<Process>, cmpProcessID> processIDqueue;        // In order of ID

  // checks if given argument is a file
  ifstream schedulerFile;
  schedulerFile.open(argv[1]);

  // throws error if argument given is not a valid file
  if (!schedulerFile.is_open())
  {
    cout << "ERROR: Invalid file argument given" << endl;
    errorMessage();
    exit(EXIT_FAILURE);
  }

  // holds current line of given txt file
  string line = "";

  // reads line of text file, creates a Process from the information in the line, and added to 2 priority queues
  // 1 queue holds process in order of ID, other queue holds process in order of arrival time
  while (getline(schedulerFile, line))
  {
    int processID;
    int burstTime;
    int arrivalTime;
    string placeholder;

    stringstream currentLine(line);

    getline(currentLine, placeholder, ',');
    processID = atoi(placeholder.c_str());

    getline(currentLine, placeholder, ',');
    burstTime = atoi(placeholder.c_str());

    getline(currentLine, placeholder, ',');
    arrivalTime = atoi(placeholder.c_str());

    Process p;
    p.processID = processID;
    p.arrivalTime = arrivalTime;
    p.burstTime = burstTime;
    p.burstTimeLeft = burstTime;
    arrivalQueue.push(p);
    processIDqueue.push(p);
  }

  schedulerFile.close(); // finished reading file, close file

  vector<Process> allProcessArrivalTime; // order of arrival time, for scheduling purposes
  vector<Process> allProcessID;          // order of ID, to iterate for termination, turnaround and waiting time

  // transfers the priority queues to a list, for iterative purposes when scheduling processes
  // maintains order held in priority queues
  while (!arrivalQueue.empty())
  {
    allProcessArrivalTime.push_back(arrivalQueue.top()); // add to all processes
    arrivalQueue.pop();                                  // remove process 1 by 1 from priority queue
  }

  while (!processIDqueue.empty())
  {
    allProcessID.push_back(processIDqueue.top());
    processIDqueue.pop();
  }

  vector<Process> readyList; // list of processes that have arrived and are ready to be processed

  map<int, int> IDFinishTime; // maps id to termination time

  Process *current = NULL; // points to current process being executed
  Process *prev = NULL;    // indicates previous process, null means current process is likely to be different

  queue<int> orderOfProcesses; // queue holding order of when process allowed to be executed

  int counter = -1; // counter used to simulate a timer in scheduling

  // schedules processes until all processes executed
  while (!readyList.empty() || !allProcessArrivalTime.empty())
  {
    counter += 1;
    if (current != NULL)
    {
      counter == current->arrivalTime ? current->burstTimeLeft -= 0 : current->burstTimeLeft -= 1; // executes current process

      // if current process is complete, get termination time and remove it from ready list
      if (current->burstTimeLeft == 0)
      {
        IDFinishTime[current->processID] = counter;
        current = NULL;
        prev = NULL;
        removeFinishedProcess(readyList);
      }
    }

    // add process to ready list based on its arrival time
    while (!allProcessArrivalTime.empty() && allProcessArrivalTime.front().arrivalTime == counter)
    {
      readyList.push_back(allProcessArrivalTime.front());
      allProcessArrivalTime.erase(allProcessArrivalTime.begin());
    }

    if (readyList.empty()) // no processes to execute at this time, go to next iteration
    {
      continue;
    }
    current = getLowestBurstTimeProcess(readyList);

    // check if current process is different to previous, then add it order of execution
    if (current != prev && (prev == NULL || current->processID != prev->processID))
    {
      orderOfProcesses.push(current->processID);
      prev = current;
    }
    // if process is complete, get termination time, remove it from readyList
  }

  // print out all processes executed and relevant information about the scheduling performed
  updateTimes(allProcessID, IDFinishTime);
  printAverageTimes(allProcessID);
  printOrder(orderOfProcesses, IDFinishTime);
  allProcessID.clear();
  IDFinishTime.clear();

  return 0;
}
