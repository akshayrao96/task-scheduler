#include <iostream>
#include <fstream>
#include <queue>
#include <cstdlib>
#include <sstream>
#include <list>
#include <map>

using namespace std;

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

class cmpProcessID
{
public:
  int operator()(const Process &p1, const Process &p2)
  {
    return p1.processID > p2.processID;
  }
};

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

void printOrder(queue<int> orderOfProcesses, map<int, int> IDFinishTime)
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

void updateTimes(vector<Process> &allProcessID, map<int, int> IDFinishTime)
{
  cout << endl;
  cout << setw(4) << "ID" << setw(18) << "Turnaround time" << setw(15) << "Waiting time" << endl;
  int i;
  for (i = 0; i < allProcessID.size(); i++)
  {
    allProcessID[i].terminationTime = IDFinishTime[allProcessID[i].processID];
    allProcessID[i].turnaroundTime = allProcessID[i].terminationTime - allProcessID[i].arrivalTime;
    allProcessID[i].waitingTime = allProcessID[i].turnaroundTime - allProcessID[i].burstTime;
    cout << setw(4) << allProcessID[i].processID << setw(12) << allProcessID[i].turnaroundTime << setw(17) << allProcessID[i].waitingTime << endl;
  }
  cout << endl;
}

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

void errorMessage()
{
  cout << "USAGE: Executable file must take in 1 valid file path" << endl;
  cout << "EXAMPLE: ./question2 <file.txt>" << endl;
}

int main(int argc, char **argv)
{
  // Throws error if not given exactly 1 argument
  if (argc != 2)
  {
    cout << "ERROR: Invalid number of arguments given" << endl;
    errorMessage();
    exit(EXIT_FAILURE);
  }

  priority_queue<Process, vector<Process>, cmpProcessArrivalTime> arrivalQueue; // In order of arrival time, breaking ties with burst time, then ID
  priority_queue<Process, vector<Process>, cmpProcessID> processIDqueue;        // In order of ID

  ifstream schedulerFile;
  schedulerFile.open(argv[1]);

  // Throws error if argument given is not a valid file
  if (!schedulerFile.is_open())
  {
    cout << "ERROR: Invalid file argument given" << endl;
    errorMessage();
    exit(EXIT_FAILURE);
  }

  string line = "";

  // Reads line of text file, creates a new Process object and added to priority queue
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

  schedulerFile.close(); // Finished reading file, close file

  vector<Process> allProcessArrivalTime; // In order of arrival time, for scheduling purposes
  vector<Process> allProcessID;          // In order of ID, to iterate for termination, turnaround and waiting time

  // Populates allProcessArrivalTime in order of arrival times
  while (!arrivalQueue.empty())
  {
    allProcessArrivalTime.push_back(arrivalQueue.top()); // add to all processes
    arrivalQueue.pop();                                  // remove process 1 by 1 from priority queue
  }

  // Populates processList in order of ID
  while (!processIDqueue.empty())
  {
    allProcessID.push_back(processIDqueue.top());
    processIDqueue.pop();
  }

  vector<Process> readyList; // list of processes that have arrived

  map<int, int> IDFinishTime; // maps id to termination time

  Process *current = NULL; // points to current process being executed
  Process *prev = NULL;    // indicates previous process, null means current has changed

  queue<int> orderOfProcesses; // order of when process allowed to be executed

  int counter = -1;

  while (!readyList.empty() || !allProcessArrivalTime.empty())
  {
    counter += 1;
    if (current != NULL)
    {
      counter == current->arrivalTime ? current->burstTimeLeft -= 0 : current->burstTimeLeft -= 1; // decrement process counter

      if (current->burstTimeLeft == 0)
      {
        IDFinishTime[current->processID] = counter;
        current = NULL;
        prev = NULL;
        removeFinishedProcess(readyList);
      }
    }

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

    // check if current process is different to process last iteration
    if (current != prev && (prev == NULL || current->processID != prev->processID))
    {
      orderOfProcesses.push(current->processID);
      prev = current;
    }
    // if process is complete, get termination time, remove it from readyList
  }

  updateTimes(allProcessID, IDFinishTime);
  printAverageTimes(allProcessID);
  printOrder(orderOfProcesses, IDFinishTime);

  return 0;
}
