//
// Created by Yile Hu on 4/4/24.
//

#ifndef GITLET_OBJECT_H
#define GITLET_OBJECT_H
#include "common.h"
#include "Object.h"

class Object {
public:
    virtual std::string hash() const = 0;
    virtual std::string string_rep() const = 0;
};


#endif //GITLET_OBJECT_H
