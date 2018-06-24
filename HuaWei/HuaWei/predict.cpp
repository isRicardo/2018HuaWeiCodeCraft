#include "com_def.h"
#include "predict.h"
#include "reform_data.h"
#include "push.h"
#include "ransac.h"

int get_flavor_history_data(int timeSeqData[TIME_SEQ_NUM][FLAVOR_NUM], int daysNum, int flavor, int* historyData);
int updata_time_seq_data(int timeSeqData[MAX_DATA_NUM][FLAVOR_NUM], int& dayNum, INPUT_DATA inputData, vector<int>& predictResults);
int store_predict_result(INPUT_DATA& inputData, vector<int> predictResults, vector<VM>& predictVMs);
int filter_on_four_pos(int data[TIME_SEQ_NUM][FLAVOR_NUM], int nums);
int filter_on_standard_deviation(int data[TIME_SEQ_NUM][FLAVOR_NUM], int nums);
bool comp_y(POINT a, POINT b);
int cal_average_no_this_point(vector<POINT> fitPoint, int errorIndex, int& average);
int predict_linear_fit(int testLevel, INPUT_DATA inputData, int data[TIME_SEQ_NUM][FLAVOR_NUM], int nums, vector<int>& predictResults);

//��Ҫ��ɵĹ��������
void predict_server(char * info[MAX_INFO_NUM], char * data[MAX_DATA_NUM], int data_num, char * filename)
{	   
    int ret = COM_OK;

    vector<TRAIN_DATA> trainDatas;
    INPUT_DATA inputData;
    int totalDays = 0;
    int timeSeqData[TIME_SEQ_NUM][FLAVOR_NUM] = {0};
    int partDayNum = 0;
    int partDayData[TIME_SEQ_NUM][FLAVOR_NUM] = {0};
    int perdictDataNum = PREDICT_DATA_NUM;
    int predictData[PREDICT_DATA_NUM][FLAVOR_NUM] = {0};
    vector<int> predictResults;
    vector<VM> predictVMs;
    vector<SERVICE> services;
    int outputLen;
    int testLevel = 0;

    /* ������������ */
    ret = get_input_data(info, inputData);
    if (!ret)
    {
        return ;
    }

    /* ����ѵ������ */
    ret = get_train_data(data, data_num, trainDatas);
    if (!ret)
    {
        return ;
    }

    /* ����ͳ����������� */
    ret = get_day_data(trainDatas, timeSeqData, totalDays);
    trainDatas.clear();
    
    if (inputData.predictStep < 8)
    {
        /* ���м� */
        testLevel = 0;
    }
    else
    {
        /* �߼� */
        testLevel = 1;
    }

    /* ����ҪԤ����������ۼ� */
    ret = get_part_data(timeSeqData, totalDays, inputData.predictStep, partDayData, partDayNum);

    ret = filter_on_four_pos(partDayData, partDayNum);

    //ret = filter_on_standard_deviation(partD, partDN);

    ret = get_predict_data(partDayData, partDayNum, inputData.predictStep, predictData, perdictDataNum);

    ret = predict_linear_fit(testLevel, inputData, predictData, perdictDataNum, predictResults);

    ret = store_predict_result(inputData, predictResults, predictVMs);

    ret = push_vm_to_service(inputData, predictVMs, services);

    /* �ݴ�������� */
    char * tmp_result_file = NULL;
    tmp_result_file = (char *)malloc(sizeof(char) * (inputData.vmNum * + services.size()) * 200);
    ret = reform_output_file(tmp_result_file, inputData, predictVMs, outputLen, services);

    char * result_file = NULL;
    result_file = (char *)malloc(sizeof(char) * outputLen);
    memcpy(result_file, tmp_result_file, sizeof(char) * outputLen);
    
    write_result(result_file, filename);

    free(result_file);
    free(tmp_result_file);
    predictResults.clear();
    services.clear();
    predictVMs.clear();
}

/* ȡ����Ӧ������ʷ���� */
int get_flavor_history_data(int timeSeqData[MAX_DATA_NUM][FLAVOR_NUM], int daysNum, int flavor, int* historyData)
{
    int ret = COM_OK;

    for (int i = 0; i < daysNum; i++)
    {
        historyData[i] = timeSeqData[i][flavor - 1];
    }
    return ret;
}

/* Ԥ��õ������ݷ������� */
int updata_time_seq_data(int timeSeqData[MAX_DATA_NUM][FLAVOR_NUM], int& dayNum, INPUT_DATA inputData, vector<int>& predictResults)
{
    int ret = COM_OK;
  
    for (int i = 0; i < inputData.vmNum; i++)
    {
        timeSeqData[dayNum][inputData.vms[i].flavor - 1] = predictResults[i];
    }
    dayNum ++;

    return ret;
}

/* ����Ԥ���� */
int store_predict_result(INPUT_DATA& inputData, vector<int> predictResults, vector<VM>& predictVMs)
{
    int ret = COM_OK;
    VM predictVM;

    for (int i = 0; i < inputData.vmNum; i++)
    {
        predictVM = inputData.vms[i];
        for (int j = 0; j < predictResults[i]; j++)
        {
            predictVMs.push_back(predictVM);
        }
    }

    return ret;
}

/* 
   �������Ԥ��
   ����predictResults����������input�ļ����������Ĺ��ֵ��������
*/
int predict_linear_fit(int testLevel, INPUT_DATA inputData, int data[TIME_SEQ_NUM][FLAVOR_NUM], int nums, vector<int>& predictResults)
{   
    int ret = COM_OK;

    POINT* points = NULL;
    points = (POINT*)malloc(sizeof(POINT) * nums);

    LINE line;
    int predictResult;

    /* ÿ�ֹ��ֿ���� */
    for (int i = 0; i < inputData.vmNum; i++)
    {

        /* ����������� */
        for (int j = 0; j < nums; j++)
        {
            points[j].x = j;
            points[j].y = data[j][inputData.vms[i].flavor - 1];
        }

        /* ��� */
        //ret = predict_ransac(testLevel, points, nums, line);//�������
        ret = least_square_linear_fit(points, nums, line); //���ó������
        predictResult = (int)(line.a * nums + line.b);

        if (0 == testLevel)
        {
            predictResult = (int)(predictResult + inputData.predictStep + 7);
        }
        else if (1 == testLevel)
        {
            predictResult = (int)(predictResult + inputData.predictStep + 17);
        }
 

        if (predictResult < 0)
        {
            predictResult = 0;
        }
        predictResults.push_back(predictResult);

    }

    free(points);
    return ret;
}

/* �ķ�λ��ȥ�� */
int filter_on_four_pos(int data[TIME_SEQ_NUM][FLAVOR_NUM], int nums)
{
    int ret = COM_OK;
    vector<POINT> fitPoint;
    POINT p;
    int topForthPoint = 0;      //��4��λ��
    int dowmForthPoint = 0;     //��4��λ��
    int topBounder = 0;
    int downBounder = 0;
    int average = 0;

    for (int flavor = 0; flavor < FLAVOR_NUM; flavor++)
    {
        fitPoint.clear();
        for (int i = 0; i < nums; i++)
        {
            p.x = i;
            p.y = data[i][flavor];
            fitPoint.push_back(p);
        }

        /* ���� */
        sort(fitPoint.begin(), fitPoint.end(), comp_y);

        topForthPoint = fitPoint[fitPoint.size() * 3 / 4 - 1].y;

        dowmForthPoint = fitPoint[fitPoint.size() * 1 / 4 - 1].y;

        topBounder = topForthPoint + ((topForthPoint - dowmForthPoint) * 3 / 2);

        downBounder = dowmForthPoint - ((topForthPoint - dowmForthPoint) * 3 / 2);

        for (size_t i = 0; i < fitPoint.size(); i++)
        {
            if (fitPoint[i].y > topBounder)
            {
                fitPoint[i].y = topBounder;
            }
            if (fitPoint[i].y < downBounder)
            {
                fitPoint[i].y = downBounder;
            }
        }

        /* ����д������ */
        for (int i = 0; i < fitPoint.size(); i++)
        {
            data[fitPoint[i].x][flavor] = fitPoint[i].y; 
        }
    }

    return ret;
}

/* 3����׼��ȥ�� */
int filter_on_standard_deviation(int data[TIME_SEQ_NUM][FLAVOR_NUM], int nums)
{
    int ret = COM_OK;
    double ave = 0;
    double deviation = 0;
    double standardDeviation = 0;
    double thr = 0;

    for (int flavor = 0; flavor < FLAVOR_NUM; flavor++)
    {
        /* ���ֵ */
        for (int i = 0; i < nums; i++)
        {
            ave += data[i][flavor]; 
        }
        ave /= nums;

        /* ���׼�� */
        for (int i = 0; i < nums; i++)
        {
            deviation += pow((data[i][flavor] - ave), 2.0);
        }
        deviation /= nums;

        standardDeviation = sqrt(deviation);
        thr = ave + 3 * standardDeviation;

        /* ȥ�� */
        for (int i = 0; i < nums; i++)
        {
            if (data[i][flavor] > thr)
            {
                data[i][flavor] = ave;
            }
        }
    }

    return ret;
}

int cal_average_no_this_point(vector<POINT> fitPoint, int errorIndex, int& average)
{
    int ret = COM_OK;
    int sum = 0;

    for (int i = 0; i < fitPoint.size(); i++)
    {
        sum += fitPoint[i].y;        
    }
    average = sum / fitPoint.size();

    return ret;
}

bool comp_y(POINT a, POINT b)
{
    return a.y < b.y;
}