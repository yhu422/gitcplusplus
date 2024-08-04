//
// Created by Yile Hu on 5/13/24.
//
#include "../include/common.h"
using namespace std;
string sha256(const string& str){
    const size_t length = str.length();
    char* name = new char[length + 1];
    unsigned char obuf[32];
    strcpy(name, str.c_str());
    SHA256((unsigned char*)name, length, obuf);
    stringstream res;
    for(auto c:obuf){
        res << hex << (short) c;
    }
    return res.str();
}

string readContent(const filesystem::path& p){
    ifstream  ifs(p);
    return {(istreambuf_iterator<char>(ifs) ),
            (istreambuf_iterator<char>()    )};
}