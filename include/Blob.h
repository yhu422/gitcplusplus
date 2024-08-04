//
// Created by Yile Hu on 4/5/24.
//

#ifndef GITLET_BLOB_H
#define GITLET_BLOB_H

#include "common.h"
#include "Object.h"
#include "Util.h"
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
class Blob{
public:
    Blob()=default;
    explicit Blob(const filesystem::path& filename){
        name_ = filename.filename().string();
        content_ = readContent(filename);
    }
    void save (const filesystem::path& p) const {
        std::ofstream t(p, std::ios::binary);
        boost::archive::binary_oarchive oa(t);
        // write class instance to archive
        oa << *this;
        // archive and stream closed when destructors are called
    }
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & name_;
        ar & content_;
    }

    std::string getName() const{
        return name_;
    }

    std::string getContent() const{
        return content_;
    }

    std::string hash() const {
        return sha256(name_ + content_);
    }

private:
    std::string name_;
    std::string content_;
};
#endif //GITLET_BLOB_H
