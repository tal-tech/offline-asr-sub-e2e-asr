#include<iostream>
#include "include/tal_asr_api.h"
#include "include/tal_asr_define.h"
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <base/base64.h>
#include <vector>
#include <fstream>
#include <algorithm>
void * asr_resource = nullptr;
void * GetDecModule()
{
    void * dec;
    int ret = -1;
    
    if((ret = TALASRInstanceCreate(asr_resource, &dec)) || !dec)
    {
        std::cout << "asr create failed:" << ret << std::endl;
        return nullptr;
    }
    if((ret = TalASRInstanceStart(dec, ""))){
        std::cout << "asr started failed:" << ret << std::endl;
        return nullptr;
    }
    return dec;
}
void Init() 
{
    const char *mod_dir = "../../../res";
    int ret = -1;
    if((ret = TalASRResourceImport(mod_dir, &asr_resource)) || !asr_resource) {
        std::cout << "failed to load alg mod";
        exit(1);
    }

    // 资源初始化状态, 0表示初始化成功, 其它表示异常
    ret = TalASRResourceImport(mod_dir, &asr_resource);
    if(ret != 0 || !asr_resource) 
    {
        std::cout << "failed to load alg mod ret: " << ret << std::endl;
        exit(1);
    }
    std::cout << "Init succeed" << std::endl;
}

void getResult(std::string voicePath,std::string &result_list)
{
    void *dec = GetDecModule();
    if(!dec)
    {
        std::cout << "got empty instance" << std::endl;
        return;
    }
    int ret = -1;

    std::ifstream audioFileStream(voicePath, std::ios::binary);
    if (!audioFileStream) 
    {
        std::cerr << "Failed to open audio file." << std::endl;
        return;
    }
    std::string audioData((std::istreambuf_iterator<char>(audioFileStream)),std::istreambuf_iterator<char>());
    std::string b64Data;
    base::Base64Encode(audioData, &b64Data);
    //std::cout << b64Data;
    std::string data_str = "";
    // Base64解码
    base::Base64Decode(b64Data, &data_str);
    if((ret = TalASRInstanceRecognize(dec, const_cast<char*>(data_str.c_str()),data_str.length(), result_list)))
    {
        std::cout << "none 0 after asr:" << std::to_string(ret) << std::endl;
        return;
    }
}
std::string extractResult(const std::string& input) 
{
    std::string resultField = "\"result\":\"";
    size_t startPos = input.find(resultField);
    if (startPos == std::string::npos) 
    {
        return "";  
    }
    startPos += resultField.length();
    size_t endPos = input.find("\"", startPos);
    if (endPos == std::string::npos) 
    {
        return "";  
    }

    return input.substr(startPos, endPos - startPos);
}
int main()
{
    std::string path = "../../../cs.wav";
    std::string result;

    Init();
    getResult(path, result);
    result = extractResult(result);
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    std::cout << result << std::endl;
    return 0;
}