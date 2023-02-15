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

  Process *current = NULL;

  int counter = -1;

  while (!readyList.empty() || !allProcessArrivalTime.empty())
  {
    counter += 1;

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
    counter == current->arrivalTime ? current->burstTimeLeft -= 0 : current->burstTimeLeft -= 1; // decrement process counter
    // if process is complete, get termination time, remove it from readyList
    if (current->burstTimeLeft == 0)
    {
      IDFinishTime[current->processID] = counter;
      removeFinishedProcess(readyList);
      current = NULL;
    }
  }
  return 0;
}
