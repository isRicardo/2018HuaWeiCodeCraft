#ifndef __REFORM_DATA_H__
#define __REFORM_DATA_H__

extern int get_train_data(char* data[MAX_DATA_NUM], int data_num, vector<TRAIN_DATA>& trainDatas);

extern int get_input_data(char * info[MAX_INFO_NUM], INPUT_DATA& inputData);

extern int get_day_data(vector<TRAIN_DATA>& trainDatas, int timeSeqData[TIME_SEQ_NUM][FLAVOR_NUM],int& daysNum);

extern int get_part_data( int dayData[TIME_SEQ_NUM][FLAVOR_NUM], int dayDataNum, int predictStep, int partData[TIME_SEQ_NUM][FLAVOR_NUM], int& partDayNum);

extern int get_predict_data(int dayData[TIME_SEQ_NUM][FLAVOR_NUM], int days, int predictStep, int sumData[TIME_SEQ_NUM][FLAVOR_NUM], int& sumDataNum);

extern void release_time_seq_data(int** timeSeqData, const int valid_item_num);

#endif