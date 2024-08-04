//
// Created by Yile Hu on 5/13/24.
//

#ifndef GITLET_COMMIT_H
#define GITLET_COMMIT_H

#include <utility>
#include "common.h"
#include "Object.h"
#include "vars.h"
#include "Util.h"
#include "Blob.h"
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/map.hpp>
class Commit {
public:
    friend class boost::serialization::access;
    Commit()= default;
    Commit(std::string logMessage, std::string time, std::string parent, std::string secondParent)
    : message_(std::move(logMessage)),
      timeStamp_(std::move(time)),
      parentCommit_(std::move(parent)),
      secondParentCommit_(std::move(secondParent)){}

      void save () const {
        filesystem::path p = commit_path / this->hash();
        std::ofstream t(p, std::ios::binary);
        boost::archive::binary_oarchive oa(t);
        // write class instance to archive
        oa << *this;
        // archive and stream closed when destructors are called
      }
    [[nodiscard]] std::string hash() const{
        return sha256(parentCommit_ + message_ + timeStamp_);
    }
    void addReference(std::string key, std::string val){
        blobReference_[key] = val;
    }

    bool hasReference(const std::string& filename){
        return blobReference_.find(filename) != blobReference_.end();
    }

    bool hasBlob(const std::string& filename, const std::string& hash){
        return blobReference_[filename] == hash;
    }

    std::string findHash(const std::string& filename){
        return blobReference_[filename];
    }

    void rmReference(const std::string& filename){
        blobReference_.erase(filename);
    }

    void setReference(const std::map<string, string>& other){
        blobReference_ = std::move(other);
    }

    bool hasBlob(Blob& b){
        return blobReference_.find(b.hash()) != blobReference_.end();
    }

    std::map<string, string> getReference(){
        return blobReference_;
    }

    void displayReference(){
        for(auto& [key,val]:blobReference_){
            std::cout << key << " " << val << std::endl;
        }
    }

    std::string_view getTimeStamp() const{
        return timeStamp_;
    }

    std::string_view getMessage() const{
        return message_;
    }

    std::string_view getParent() const{
        return parentCommit_;
    }



    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & message_;
        ar & timeStamp_;
        ar & blobReference_;
        ar & parentCommit_;
        ar & secondParentCommit_;
    }
private:
    std::string message_;
    std::string timeStamp_;
    std::map<std::string, std::string> blobReference_;
    std::string parentCommit_;
    std::string secondParentCommit_;
};

BOOST_CLASS_VERSION(Commit, 1)



#endif //GITLET_COMMIT_H
