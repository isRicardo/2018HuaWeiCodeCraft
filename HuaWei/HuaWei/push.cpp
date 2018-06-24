#include "com_def.h"
#include "push.h"

int print_num(int numData, char numInChar[10], int& num);
int count_vm_num(INPUT_DATA& inputData, vector<VM>& predictVM, vector<int>& nums);
int count_vm_in_service(SERVICE& services, vector<int>& flavors, vector<int>& nums);
int sort_descend(vector<VM>& predictVM, const char* source);
bool comp_cpu(VM a, VM b);
bool comp_men(VM a, VM b);
bool comp_flavor(VM a, VM b);
int add_vm_to_service(INPUT_DATA inputData, vector<VM>& predictVM, vector<SERVICE>& services);

/************************************************************************/
/* ��Դ���������
* ̰�ĵطţ��Ȱ���Դ���ʹӴ�С����
* �ٰ���ȡ������ǰ���еķ������зŵ�����ţ�
* �Ų����¿�
/************************************************************************/
int push_vm_to_service(INPUT_DATA inputData, vector<VM>& predictVM, vector<SERVICE>& services)
{
    int ret = COM_OK;

    SERVICE tmpService;
    int hasPush = 0;
    ret = sort_descend(predictVM, inputData.source);

    tmpService = inputData.service;
    services.push_back(tmpService);

    for (size_t i = 0; i < predictVM.size(); i++)
    {
        hasPush = 0;
        for (size_t j = 0; j < services.size(); j++)
        {
            /* ��ǰ���еķ������Ƿ�ŵ��£� */
            if (0 == hasPush && predictVM[i].cpu < services[j].leftCpu && predictVM[i].memory < services[j].leftMemory)
            {               
                hasPush = 1;
                services[j].vms.push_back(predictVM[i]);
                services[j].leftCpu = services[j].leftCpu - predictVM[i].cpu;
                services[j].leftMemory = services[j].leftMemory - predictVM[i].memory;
            }
        }

        /* ���Ų���,�¿����������Ž�ȥ */
        if (0 == hasPush)
        {
            tmpService = inputData.service;
            tmpService.vms.push_back(predictVM[i]);
            tmpService.leftCpu = tmpService.leftCpu - predictVM[i].cpu;
            tmpService.leftMemory = tmpService.leftMemory - predictVM[i].memory;
            services.push_back(tmpService);
        }
    }

    if (1 < services.size())
    {
        ret = add_vm_to_service(inputData, predictVM, services);
    }

    return ret;
}

int add_vm_to_service(INPUT_DATA inputData, vector<VM>& predictVM, vector<SERVICE>& services)
{
    int ret = COM_OK;
    int addIndex = 0;
    VM tmpVM;

    /* ���һ��û�з����� ������ */
    while (true)
    {
        if (addIndex >= inputData.vmNum)
        {
            addIndex = 0;
        }
        tmpVM = inputData.vms[addIndex];

        /* ��ǰ���еķ������Ƿ�ŵ��£� */
        if (tmpVM.cpu < services.back().leftCpu 
            && tmpVM.memory < services.back().leftMemory)
        {               
            services.back().vms.push_back(tmpVM);
            services.back().leftCpu = services.back().leftCpu - tmpVM.cpu;
            services.back().leftMemory = services.back().leftMemory - tmpVM.memory;
            predictVM.push_back(tmpVM);
            addIndex ++;
        }
        else
        {
            break;
        }
    }

    return ret;
}

/* ��������ļ� */
int reform_output_file(char * result_file, INPUT_DATA& inputData, vector<VM>& predictVM, int& index,vector<SERVICE>& services)
{
    int ret = COM_OK;
    int vmNum = predictVM.size();
    char flavorName[] = "flavor";
    vector<int> flavorNums;
    vector<int> falvor;
    vector<int> numsInService;
    char numInChar[10] = {0};
    int num;
    index = 0;
    ret = count_vm_num(inputData, predictVM, flavorNums);

    /* Ԥ��õ���VM����*/
    ret = print_num(predictVM.size(), numInChar, num);
    for (int n = (num - 1); n >= 0 ; n--)
    {
        result_file[index++] = numInChar[n];
    }
    result_file[index++] = '\n';

    for (int i = 0; i < inputData.vmNum; i++)
    {
        for (int j = 0; flavorName[j] != '\0'; j++)
        {
            result_file[index++] = flavorName[j];
        }

        /* flavor��� */
        ret = print_num(inputData.vms[i].flavor, numInChar, num);
        for (int n = (num - 1); n >= 0 ; n--)
        {
            result_file[index++] = numInChar[n];
        }

        result_file[index++] = ' ';

        /* flavor������� */
        ret = print_num(flavorNums[i], numInChar, num);
        for (int n = (num - 1); n >= 0 ; n--)
        {
            result_file[index++] = numInChar[n];
        }
        result_file[index++] = '\n';
    }

    result_file[index++] = '\n';

    /* ����services���� */
    ret = print_num(services.size(), numInChar, num);
    for (int n = (num - 1); n >= 0 ; n--)
    {
        result_file[index++] = numInChar[n];
    }
    result_file[index++] = '\n';

    for (size_t i = 0; i < services.size(); i++)
    {
        falvor.clear();
        numsInService.clear();

        /* services��� */
        ret = print_num(i + 1, numInChar, num);
        for (int n = (num - 1); n >= 0 ; n--)
        {
            result_file[index++] = numInChar[n];
        }
        result_file[index++] = ' ';

        ret = count_vm_in_service(services[i], falvor, numsInService);

        for (size_t j = 0; j < falvor.size(); j++)
        {
            for (int n = 0; flavorName[n] != '\0'; n++)
            {
                result_file[index++] = flavorName[n];
            }

            ret = print_num(falvor[j], numInChar, num);
            for (int n = (num - 1); n >= 0 ; n--)
            {
                result_file[index++] = numInChar[n];
            }
            result_file[index++] = ' ';

            ret = print_num(numsInService[j], numInChar, num);
            for (int n = (num - 1); n >= 0 ; n--)
            {
                result_file[index++] = numInChar[n];
            }
            result_file[index++] = ' ';
        }
        if ((services.size() - 1) != i)
        {
            result_file[index++] = '\n';
        }
    }
    result_file[index++] = '\0';

    return ret;
}

int print_num(int numData, char numInChar[10], int& num)
{
    int ret = COM_OK;
    num = 0;

    for (int n = 0; n < 10; n++)
    {
        numInChar[n] = 0;
    }

    if (0 == numData)
    {
        numInChar[0]= 0 + '0';
        num = 1;
    }

    for (int i = 0; i < 10; i++)
    {
        if (0 != numData)
        {
            numInChar[i]= numData % 10 + '0';
            numData = numData / 10;
            num++;
        }
    }
    return ret;
}

/* ����Ӧ����ж��ٸ� */
int count_vm_num(INPUT_DATA& inputData, vector<VM>& predictVM, vector<int>& nums)
{
    int ret = COM_OK;

    for (int i = 0; i < inputData.vmNum; i++)
    {        
        nums.push_back(0);
    }

    for (int i = 0; i < inputData.vmNum; i++)
    {        

        for (size_t j = 0; j < predictVM.size(); j++)
        {
            if (inputData.vms[i].flavor == predictVM[j].flavor)
            {
                nums[i] ++;
            }
        }
    }

    return ret;
}

/* �������ڷ���������������� */
int count_vm_in_service(SERVICE& services, vector<int>& flavors, vector<int>& nums)
{
    int ret = COM_OK;
    int flag = 0;
    int cnt = 0;
    vector<VM> tmpVM;

    if (0 == services.vms.size())
    {
        flavors.push_back(0);
        nums.push_back(0);
        return ret;
    }
    for (size_t i = 0; i < services.vms.size(); i++)
    {
        tmpVM.push_back(services.vms[i]);
    }

    /* ��������� */
    sort(tmpVM.begin(), tmpVM.end(), comp_flavor);

    for (size_t i = 0; i < services.vms.size(); i++)
    {
        services.vms[i] = tmpVM[i];
    }
    tmpVM.clear();

    flavors.push_back(services.vms[0].flavor);
    nums.push_back(1);
    cnt = 0;
    for (size_t i = 1; i < services.vms.size(); i++)
    {      
        if (flavors[cnt] == services.vms[i].flavor)
        {
            nums[cnt] ++;
        }
        else
        {
            cnt ++;
            nums.push_back(1);
            flavors.push_back(services.vms[i].flavor);
        }
    }

    return ret;
}

/* �����ͽ������� */
int sort_descend(vector<VM>& predictVM, const char* source)
{
    int ret = COM_OK;
    char cpu[] = "CPU";
    char mem[] = "MEM";

    /* ��ָ����������� */
    if (0 == strcmp(source, cpu))
    {
        sort(predictVM.begin(), predictVM.end(), comp_cpu);
    }
    else if (0 == strcmp(source, mem))
    {
        sort(predictVM.begin(), predictVM.end(), comp_men);
    }

    return ret;
}

bool comp_cpu(VM a, VM b)
{
    return a.cpu > b.cpu;
}

bool comp_men(VM a, VM b)
{
    return a.memory > b.memory;
}

bool comp_flavor(VM a, VM b)
{
    return a.flavor < b.flavor;
}
