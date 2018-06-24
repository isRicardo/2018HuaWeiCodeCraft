#include "lib_io.h"
#include "com_def.h"

/************************************************************************/
/* 功能：对输入数据格式处理                                             */
/************************************************************************/

int get_day_num_of_month(int year,int month, int& days);

int get_train_data(char* data[MAX_DATA_NUM], int data_num, vector<TRAIN_DATA>& trainDatas)
{
    int ret = COM_OK;

    int col;
    int index = 0;

    if (NULL == data)
    {
        return COM_NULL_PTR;
    }

    TRAIN_DATA trainData;
    char time[20];

    for (int i = 0; i < data_num; i++)
    {        
        /* 写入ID */
        col = 0;
        index = 0;
        for (int j = 0; data[i][j] != '\t'; j++)
        {
            trainData.id[index] = data[i][j];
            index ++;
            col ++;
        }
        trainData.id[index] = '\0';

        col += 7;
        trainData.flavor = 0;
        for (int j = col; data[i][j] != '\t'; j++)
        {
            trainData.flavor = trainData.flavor * 10 + (data[i][j] - '0');
            col ++;
        }

        col ++;
        index = 0;
        for (int j = col; data[i][j] != ' '; j++)
        {          
            time[index] = data[i][j];
            index ++;
        }
        time[index] = '\0';        

        trainData.time.year = 0;
        trainData.time.month = 0;
        trainData.time.day = 0;
        for (int i = 0; i < 4; i++)
        {
            trainData.time.year = trainData.time.year * 10 + time[i] - '0';
        }
        for (int i = 5; i < 7; i++)
        {
            trainData.time.month = trainData.time.month * 10 + time[i] - '0';
        }
        for (int i = 8; i < 10; i++)
        {
            trainData.time.day = trainData.time.day * 10 + time[i] - '0';
        }

        trainDatas.push_back(trainData);
    }

    return ret;
}

int get_input_data(char* info[MAX_INFO_NUM], INPUT_DATA& inputData)
{
    int ret = COM_OK;
    int row = 0;
    int col = 0;
    int index = 0;
    int cnt = 0;
    VM vm;
    char startTime[20];
    char endTime[20];
    int monthDay = 0;

    if (NULL == info)
    {
        return COM_NULL_PTR;
    }

    /* 读取第一行的服务器资源信息 */
    row = 0;
    col = 0;  
    inputData.service.leftCpu = 0;
    inputData.service.leftMemory = 0;
    for (int i = 0; info[row][i] != ' '; i++)
    {
        inputData.service.leftCpu = inputData.service.leftCpu * 10 + (info[row][i] - '0');
        col ++;
    }

    col ++;
    for (int i = col; info[row][i] != ' '; i++)
    {
        inputData.service.leftMemory = inputData.service.leftMemory * 10 + (info[row][i] - '0');
    }
    inputData.service.leftMemory *= 1024;

    /* 读取第三行的虚拟器规格数量 */
    row = 2;
    {
        inputData.vmNum = 0;
        for (int i = 0; info[row][i] != '\r'; i++)
        {
            inputData.vmNum = inputData.vmNum * 10 + (info[row][i] - '0');
        }
    }

    /* 第四行开始为虚拟机类型 */
    row = 3;
    for (int j = row; j < row + inputData.vmNum; j++)
    {
        col = 6;
        vm.flavor = 0;
        vm.cpu = 0;
        vm.memory = 0;
        for (int i = col; info[j][i] != ' '; i++)
        {
            vm.flavor = vm.flavor * 10 + (info[j][i] - '0');
            col++;
        }

        col++;
        for (int i = col; info[j][i] != ' '; i++)
        {
            vm.cpu = vm.cpu * 10 + (info[j][i] - '0');
            col++;
        }

        col++;
        for (int i = col; info[j][i] != '\r'; i++)
        {
            vm.memory = vm.memory * 10 + (info[j][i] - '0');
            col++;
        }

        inputData.vms.push_back(vm);
    }

    /* 读取申请的资源 */
    row = row + inputData.vmNum + 1;
    {
        index = 0;
        for (int i = 0; info[row][i] != '\r'; i++)
        {
            inputData.source[index] = info[row][i];
            index ++;
        }
        inputData.source[index] = '\0';
    }

    /* 读取时间 */
    row = row + 2;
    {
        index = 0;
        for (int i = 0; info[row][i] != ' '; i++)
        {
            startTime[index] = info[row][i];
            index ++;
        }
        startTime[index] = '\0';
    }

    row = row + 1;
    {
        index = 0;
        for (int i = 0; info[row][i] != ' '; i++)
        {
            endTime[index] = info[row][i];
            index ++;
        }
        endTime[index] = '\0';
    }    

    inputData.startTime.year = 0;
    inputData.startTime.month = 0;
    inputData.startTime.day = 0;
    for (int i = 0; i < 4; i++)
    {
        inputData.startTime.year = inputData.startTime.year * 10 + startTime[i] - '0';
    }
    for (int i = 5; i < 7; i++)
    {
        inputData.startTime.month = inputData.startTime.month * 10 + startTime[i] - '0';
    }
    for (int i = 8; i < 10; i++)
    {
       inputData.startTime.day = inputData.startTime.day * 10 + startTime[i] - '0';
    }
    
    inputData.endTime.year = 0;
    inputData.endTime.month = 0;
    inputData.endTime.day = 0;
    for (int i = 0; i < 4; i++)
    {
        inputData.endTime.year = inputData.endTime.year * 10 + endTime[i] - '0';
    }
    for (int i = 5; i < 7; i++)
    {
        inputData.endTime.month = inputData.endTime.month * 10 + endTime[i] - '0';
    }
    for (int i = 8; i < 10; i++)
    {
        inputData.endTime.day = inputData.endTime.day * 10 + endTime[i] - '0';
    }

    if (inputData.startTime.month == inputData.endTime.month)
    {
        inputData.predictStep = inputData.endTime.day - inputData.startTime.day;
    }
    else
    {
        ret = get_day_num_of_month(inputData.startTime.year, inputData.startTime.month, monthDay);
        inputData.predictStep = monthDay - inputData.startTime.day + inputData.endTime.day;
    }

    return ret;

}

int get_day_data(vector<TRAIN_DATA>& trainDatas, int dayData[TIME_SEQ_NUM][FLAVOR_NUM], int& days)
{
    int ret = COM_OK;
    int pastDay = 0;
    int lastMonth = trainDatas[0].time.month;
    int lastMonthDay = 0;

    for (int i = 0; i < trainDatas.size(); i++)
    {
        if (trainDatas[i].time.month != lastMonth)
        {
            ret = get_day_num_of_month(trainDatas[i-1].time.year, trainDatas[i-1].time.month, lastMonthDay);
            pastDay += lastMonthDay;
            lastMonth = trainDatas[i].time.month;
        }
        days = pastDay + trainDatas[i].time.day;
        dayData[days - 1][trainDatas[i].flavor - 1] += 1;
    }

    return ret;
}

int get_part_data( int dayData[TIME_SEQ_NUM][FLAVOR_NUM], int dayDataNum, int predictStep, int partData[TIME_SEQ_NUM][FLAVOR_NUM], int& partDayNum)
{
    int ret = COM_OK;

    if (dayDataNum < (predictStep * PREDICT_DATA_NUM))
    {
        partDayNum = dayDataNum;
    }
    else
    {
        partDayNum = predictStep * PREDICT_DATA_NUM;
    }

    for (int flavor = 0; flavor < FLAVOR_NUM; flavor++)
    {
        for (int i = 0; i < partDayNum; i++)
        {
            partData[i][flavor] = dayData[dayDataNum - partDayNum + i][flavor];
        }
    }

    return ret;
}

int get_predict_data(int dayData[TIME_SEQ_NUM][FLAVOR_NUM], int days, int predictStep, int predictData[TIME_SEQ_NUM][FLAVOR_NUM], int& predictDataNum)
{
    int ret = COM_OK;
    int sum = 0;
    predictDataNum =  days / predictStep;
    int index = 0;

    for (int flavor = 0; flavor < FLAVOR_NUM; flavor++)
    {
        index = 0;
        for (int i = days - predictDataNum * predictStep; i < days; i += predictStep)
        {
            sum = 0;
            for (int j = i; j < i + predictStep; j++)
            {
                sum += dayData[j][flavor];
            }
            predictData[index][flavor] = sum;
            index ++;
        }

    }

    return ret;
}

void release_time_seq_data(int** timeSeqData, const int valid_item_num)
{
    for (int i = 0; i < valid_item_num; i++)
        free(timeSeqData[i]);
}

/* 根据月份求天数 */
int get_day_num_of_month(int year,int month, int& days)
{
    int ret = COM_OK;

    int leapYear[12]={31,29,31,30,31,30,31,31,30,31,30,31};
    int noLeapYear[12]={31,28,31,30,31,30,31,31,30,31,30,31};
    int isLeap=0;

    if (0 == month || 0 == year)
    {
        return COM_ERROR;
    }

    if((0 == year % 4 && 0 != year % 100) || (0 == year % 100 && 0 == year % 400))
    {
        isLeap=1;
    }

    if (1 == isLeap)
    {
        days = leapYear[month - 1];
    }
    else
    {
        days = noLeapYear[month -1];
    }

    return ret;
}