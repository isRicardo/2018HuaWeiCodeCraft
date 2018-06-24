#include "include\com_def.h"
#include "include\predict.h"

int main(){
      
    // ‰»Î ˝æ›
    int inputBuffCnt = 0;
    char* inputBuff[MAX_INFO_NUM];
    /*inputBuff = (char** )malloc(MAX_INFO_NUM * sizeof(char*));*/

    int trainBuffCnt = 0;
    char* trainBuff[MAX_DATA_NUM];
    //trainBuff = (char** )malloc(MAX_DATA_NUM * sizeof(char*));

    inputBuffCnt = read_file(inputBuff, MAX_INFO_NUM, INPUT_DATA_PATH);
    trainBuffCnt = read_file(trainBuff, MAX_DATA_NUM, TRAIN_DATA_PATH);

    predict_server(inputBuff, trainBuff, trainBuffCnt, OUTPUT_DATA_PATH);
       
    release_buff(inputBuff, inputBuffCnt);
    release_buff(trainBuff, trainBuffCnt);

}