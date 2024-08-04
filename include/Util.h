//
// Created by Yile Hu on 5/13/24.
//

#ifndef GITLET_UTIL_H
#define GITLET_UTIL_H

#include <boost/archive/binary_iarchive.hpp>
#include "common.h"
template<typename T>
std::optional<T> readObject(const T& obj, std::filesystem::path p){

    std::ifstream ifs2(p, std::ios::binary);
    if(!std::filesystem::exists(p)){
        return {};
    }
    boost::archive::binary_iarchive ia(ifs2);
    ia >> obj;
    return std::optional<T>(obj);
}
std::string sha256(const std::string& str);
std::string readContent(const std::filesystem::path& p);

#endif //GITLET_UTIL_H
