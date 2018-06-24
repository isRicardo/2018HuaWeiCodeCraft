#ifndef _COM_DEF_H_
#define _COM_DEF_H_

#include<iostream>
#include<vector>
#include<time.h>
#include<algorithm>
#include<string.h>
using namespace std;

#define FLAVOR_NUM 15
#define TIME_SEQ_NUM 5000
#define PREDICT_DATA_NUM 20

typedef enum ErrorDefine
{
    COM_ERROR,
    COM_OK,
    COM_NULL_PTR
}ERROR_DEFINE;

typedef struct tagTime
{
    int year;
    int month;
    int day;

}TIME_DATA;

typedef struct tagVM
{
    int flavor;
    int cpu;
    int memory;
}VM;

typedef struct tagService
{
    int leftCpu;
    int leftMemory;
    vector<VM> vms;
}SERVICE;

typedef struct tagTrainData
{
    char id[40];
    int flavor;
    TIME_DATA time;
}TRAIN_DATA;

typedef struct tagInputData
{
    SERVICE service;
    int vmNum;
    vector<VM> vms;
    char source[4];
    TIME_DATA startTime;
    TIME_DATA endTime;
    int predictStep;
}INPUT_DATA;

typedef struct tagPoint
{
    int x;
    int y;
}POINT;

typedef struct tagLine
{
    double a;
    double b;
}LINE;

#endif
