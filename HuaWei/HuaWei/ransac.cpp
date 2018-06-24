#include"com_def.h"
#include"ransac.h"

int least_square_linear_fit(POINT* fitPoint, const int num, LINE& fitLine);
int conduct_ransac(LINE currentLine, POINT* remainPoint, int testNum, POINT* testPoint, int dataNum, int remainNum, LINE& bestLine, double& bestError);
int cal_error(LINE currentLine, POINT* remainPoint, int testNum, POINT* testPoint, int dataNum, int remainNum, double& thisError);
int sample_no_repeat(int* indexData, int dataNum, int sampleNum, int* out, int outLen, vector<int>& sampleIndex);
int get_sample_remain_point(POINT* points, int dataNum, int it, int sampleNum, vector<int>& sampleIndex, POINT* samplePoint, POINT* remainPoint);

int predict_ransac(int testLevel, POINT* points, int dataNum, LINE& bestLine)
{
    int ret = COM_OK;
    double bestError;
    LINE fitLine;
    int sampleNum = 0;
    int testNum = 0;

    if (0 == testLevel)
    {
        sampleNum = 2;
        testNum = 3;
    }
    if (1 == testLevel)
    {
        sampleNum = 3;
        testNum = 2;
    }

    int* indexData = NULL;
    indexData = (int*)malloc(sizeof(int) * dataNum);
    for (int i = 0; i < dataNum; i++)
    {
        indexData[i] = i;
    }

    vector<int> sampleIndex;
    int* oneceSampleIndex = NULL;
    oneceSampleIndex = (int*)malloc(sizeof(int) * sampleNum);

    POINT* samplePoint = NULL;
    samplePoint = (POINT*)malloc(sizeof(POINT) * sampleNum);
    
    int  remainNum = dataNum - sampleNum;
    POINT* remainPoint = NULL;
    remainPoint = (POINT*)malloc(sizeof(POINT) * remainNum);
    
    POINT* testPoint = NULL;
    testPoint = (POINT*)malloc(sizeof(POINT) * testNum);
    int testPointIndex = 0;
    for (int i = dataNum - 1; i >= (dataNum - testNum); i--)
    {
        testPoint[testPointIndex++] = points[i];
    }

    /* 初始化 */
    ret = least_square_linear_fit(points, dataNum, bestLine);
    ret = cal_error(bestLine, points, testNum, testPoint, dataNum, remainNum, bestError);

    /* 抽样 */
    ret = sample_no_repeat(indexData, dataNum, sampleNum, oneceSampleIndex, sampleNum, sampleIndex);

    for (int it = 0; it < sampleIndex.size() / sampleNum; it += sampleNum)
    {
        ret = get_sample_remain_point(points, dataNum, it, sampleNum, sampleIndex, samplePoint, remainPoint);

        ret = least_square_linear_fit(samplePoint, sampleNum, fitLine);

        ret = conduct_ransac(fitLine, remainPoint, testNum, testPoint, dataNum, remainNum, bestLine, bestError);
    }

    free(indexData);
    free(oneceSampleIndex);
    free(samplePoint);
    free(remainPoint);
    free(testPoint);

    return ret;
}

int conduct_ransac(LINE currentLine, POINT* remainPoint, int testNum, POINT* testPoint, int dataNum, int remainNum, LINE& bestLine, double& bestError)
{
    int ret = COM_OK;

    double thisError = 0;

    ret = cal_error(currentLine, remainPoint, testNum, testPoint, dataNum, remainNum, thisError);

    /* 当前模型更优 */
    if (thisError <= bestError)
    {
        bestLine = currentLine;
        bestError = thisError;
    }
     
    return ret;
}

/* 最小二乘法直线拟合 y = a·x + b， 计算系数a 和 b */
int least_square_linear_fit(POINT* fitPoint, const int num, LINE& fitLine)
{
    double sumX2 = 0.0;
    double sumY  = 0.0;
    double sumX  = 0.0;
    double sumXY = 0.0;

    for (int i = 0; i < num; ++i) 
    {
        sumX2 += fitPoint[i].x * fitPoint[i].x;
        sumY  += fitPoint[i].y;
        sumX  += fitPoint[i].x;
        sumXY += fitPoint[i].x * fitPoint[i].y;
    }

    fitLine.a = (num * sumXY - sumX * sumY) / (num * sumX2 - sumX * sumX);
    fitLine.b = (sumX2 * sumY - sumX * sumXY) / (num * sumX2-sumX * sumX);

    return true;
}

/* 衡量误差 */
int cal_error(LINE currentLine, POINT* remainPoint, int testNum, POINT* testPoint, int dataNum, int remainNum, double& thisError)
{
    int ret = COM_OK;

    int currentY = 0;
    int errorOffset = 0;
    vector<POINT> consenPoint;

    /* 找出大致符合当前模型的点 */
    for (int i = 0; i < remainNum; i++)
    {
        /* 符合当前模型的点 */
        currentY = (int)(currentLine.a * remainPoint[i].x + currentLine.b + 0.5);
        errorOffset = abs(remainPoint[i].y - currentY);

        if( errorOffset < ERROR_THRSHOLD)
        {
            consenPoint.push_back(remainPoint[i]);
        }
    }

    for (int i = 0; i < testNum; i++)
    {
        currentY = (int)(currentLine.a * testPoint[i].x + currentLine.b);    
        errorOffset += abs(testPoint[i].y - currentY);
    }

    thisError = ((double)errorOffset / (double)consenPoint.size()) ;

    consenPoint.clear();
    return ret;
}

/* 不重复抽样 */
int sample_no_repeat(int* indexData, int dataNum, int sampleNum, int* out, int outLen, vector<int>& sampleIndex)
{
    int ret = COM_OK;

    if(sampleNum == 0)  
    {  
        for (int j = 0; j < outLen; j++)  
        {
            sampleIndex.push_back(out[j]);
        }
        return ret;  
    }  

    for (int i = dataNum; i >= sampleNum; --i)  //从后往前依次选定一个  
    {  
        out[sampleNum-1] = indexData[i-1]; //选定一个后  
        sample_no_repeat(indexData, i-1, sampleNum-1, out, outLen, sampleIndex); // 从前i-1个里面选取m-1个进行递归  
    }  

    return ret;
}

int get_sample_remain_point(POINT* points, int dataNum, int it, int sampleNum, vector<int>& sampleIndex, POINT* samplePoint, POINT* remainPoint)
{
    int ret = COM_OK;
    int num = 0;
    int repeatFlag = 0;
    vector<int> onceSampleIndex;
    vector<int> onceRemainIndex;

    onceSampleIndex.clear();
    onceRemainIndex.clear();
    for (int i = it; i < it + sampleNum; i++)
    {
        onceSampleIndex.push_back(sampleIndex[i]);
    }

    for (int i = 0; i < dataNum; i++)
    {
        repeatFlag = 0;
        for (int j = 0; j < sampleNum; j++)
        {
            if (i == onceSampleIndex[j])
            {
                repeatFlag = 1;
            }
        }
        if (0 == repeatFlag)
        {
            onceRemainIndex.push_back(i);
        }

    }

    /* 抽样出的点 */
    for (int i = 0; i < sampleNum; i++)
    {
        num = onceSampleIndex[i];
        samplePoint[i] = points[num];
    }

    for (int i = 0; i < (dataNum - sampleNum); i++)
    {
        num = onceRemainIndex[i];
        remainPoint[i] = points[num];
    }

    return ret;
}