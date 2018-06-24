#ifndef _PUSH_H_
#define _PUSH_H_

extern int push_vm_to_service(INPUT_DATA inputData, vector<VM>& predictVM, vector<SERVICE>& services);

extern int reform_output_file(char * result_file, INPUT_DATA& inputData, vector<VM>& predictVM, int& index,vector<SERVICE>& services);

#endif
