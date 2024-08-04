#include "include/common.h"
#include "include/Util.h"
#include "include/Commit.h"
#include "include/Blob.h"
#include "include/vars.h"
using namespace std;

int init();
int commit(const std::string& message,  const std::string& secondParent);
int status();
int branch(const string& name);
int add(const string& filename);
int rm(const string& filename);
int checkoutBranch(const string& branchName);
int checkoutCommit(const string& commitHash);
std::optional<Blob> findBlob(const filesystem::path& p);
void log();
void printLog(Commit& c);
int main(int argc, char* argv[]) {
    if(argc < 2){
        cout << "Need to Supply Arguments to Program";
        return 1;
    }
    string command = argv[1];
    if(command == "init") {
        init();
    }else if(command == "commit"){
        commit(string(argv[2]), string(""));
    }else if(command == "log"){
        log();
    }else if(command == "status"){
        status();
    }else if(command == "branch"){
        branch(string(argv[2]));
    }else if(command == "add"){
        add(string(argv[2]));
    }else if(command == "rm"){
        rm(string(argv[2]));
    }else if(command == "checkout"){
        if(string(argv[2]) == "-b"){
            checkoutBranch(string(argv[3]));
        }else if(string(argv[2]) == "-c"){
            checkoutCommit(string(argv[3]));
        }
    }

//    std::ofstream t("test");
//    Commit c("HelloWorld", "11234", "Parent1", "Parent2");
//    c.addReference("key1", "val1");
//    {
//        boost::archive::text_oarchive oa(t);
//        // write class instance to archive
//        oa << c;
//        // archive and stream closed when destructors are called
//    }
//    Commit new_c;
//    {
//        // create and open an archive for input
//        std::ifstream ifs("test");
//        boost::archive::text_iarchive ia(ifs);
//        // read class state from archive
//        ia >> new_c;
//        // archive and stream closed when destructors are called
//    }
//    cout << c.hash() << endl;
//    cout << new_c.hash() << endl;
//    c.displayReference();
//    new_c.displayReference();
    return 0;
}
Commit getHeadCommit(){
    std::string headBranch = readContent(head);
    std::string head_hash = readContent(branches_path / headBranch);
    Commit headCommit;
    {
        // create and open an archive for input
        try {
            std::ifstream ifs2(commit_path / head_hash,std::ios::binary);
            boost::archive::binary_iarchive ia(ifs2);
            ia >> headCommit;
        }catch(boost::archive::archive_exception& e){
            cout << e.what() << endl;
        }

        // read class state from archive;
        // archive and stream closed when destructors are called
    }
    return headCommit;
}
std::string make_daytime_string(){
    using namespace std;
    time_t now  = time(nullptr);
    return ctime(&now);
}

int init(){
    if(filesystem::exists(main_path)){
        cout << "Already Initialized" << endl;
        return 0;
    }
    filesystem::create_directory(main_path);
    filesystem::create_directory(staging_path);
    filesystem::create_directory(blob_path);
    filesystem::create_directory(add_path);
    filesystem::create_directory(remove_path);
    filesystem::create_directory(commit_path);
    filesystem::create_directory(branches_path);
    Commit initial("initial commit", make_daytime_string(), "None", "None");
    ofstream file(head);
    if(file.is_open()){
        file << "master";
        file.close();
    }else{
        cout << "ERROR: FAIL TO CREATE FILE";
        filesystem::remove_all(main_path);
    }
    ofstream master(branches_path / "master");
    if(master.is_open()){
        master << initial.hash();
        master.close();
    }else{
        cout << "ERROR: FAIL TO CREATE FILE";
        filesystem::remove_all(main_path);
    }
    initial.save();
    return 0;
}

int add(const string& filename){
    if(!filesystem::exists(cwd/filename)){
        cout << "File does not exist" << endl;
        return 1;
    }
    Commit headCommit = getHeadCommit();
    Blob b(cwd/filename);
    if(filesystem::exists(remove_path / b.hash())){
        filesystem::remove(remove_path / b.hash());
    }else{
        if(headCommit.hasBlob(b)){
            filesystem::path tmp = add_path / b.hash();
            if(filesystem::exists(tmp)){
                filesystem::remove(tmp);
            }
        }else{
            b.save(add_path / b.hash());
        }
    }
    return 0;
}

int rm(const string& filename){
    filesystem::path blobFile = cwd / filename;
    for(const auto& entry: filesystem::directory_iterator(add_path)){
        optional<Blob> b = findBlob(entry.path());
        if(b.has_value()){
            if(b.value().getName() == filename){
                filesystem::remove(entry.path());
                return 0;
            }
        }else{
            cout << "Internal Error" << endl;
            return 2;
        }
    }
    if(getHeadCommit().hasReference(filename)){
        string hash = getHeadCommit().findHash(filename);
        Blob b(blob_path / hash);
        b.save(remove_path / hash);
        if(filesystem::exists(blobFile)){
            filesystem::remove(blobFile);
        }
        return 0;
    }
        cout << "No Reason to Remove this File" << endl;
    return 1;
}

int status(){
    cout << "=== Branches ===" << endl;
    string h = readContent(head);
    for(const auto& entry: filesystem::directory_iterator(branches_path)){
        string name = entry.path().filename().string();
        if(name == h) {
            cout << "*";
        }
        cout << name << endl;
    }
    cout << "=== Staged Files ===" << endl;
    for(const auto& entry: filesystem::directory_iterator(add_path)){
        optional<Blob> b = findBlob(entry.path());
        cout << b.value().getName() << endl;
    }
    cout << "=== Removed Files ===" << endl;
    for(const auto& entry: filesystem::directory_iterator(remove_path)){
        optional<Blob> b = findBlob(entry.path());
        cout << b.value().getName() << endl;
    }
    return 0;
}

void printLog(Commit& c){
    std::cout << "===" << std::endl;
    std::cout << "commit " << c.hash() << std::endl;
    std::cout << "Date: " << c.getTimeStamp();
    std::cout << c.getMessage() << std::endl;
}


std::optional<Blob> findBlob(const filesystem::path& p){
    std::ifstream ifs2(p,std::ios::binary);
    if(!filesystem::exists(p)){
        return {};
    }
    Blob b;
    {
        // create and open an archive for input
        std::ifstream ifs2(p,std::ios::binary);
        boost::archive::binary_iarchive ia(ifs2);
        ia >> b;

        // read class state from archive;
        // archive and stream closed when destructors are called
    }
    return std::optional<Blob>(b);
}

std::optional<Commit> findCommit(std::string_view hashCode){
    std::ifstream ifs2(commit_path / hashCode,std::ios::binary);
    if(!filesystem::exists(commit_path / hashCode)){
        return {};
    }
    Commit c;
    {
        // create and open an archive for input
            std::ifstream ifs2(commit_path / hashCode,std::ios::binary);
            boost::archive::binary_iarchive ia(ifs2);
            ia >> c;

        // read class state from archive;
        // archive and stream closed when destructors are called
    }
    return std::optional<Commit>(c);
}

void log(){
    Commit h = getHeadCommit();
    printLog(h);
    optional<Commit> curr_commit = findCommit(h.getParent());
    while(curr_commit.has_value()){
        printLog(curr_commit.value());
        curr_commit = findCommit(curr_commit.value().getParent());
    }
}

int branch(const string& name){
    filesystem::path newBranch = branches_path / name;
    if(filesystem::exists(newBranch)){
        cout << "Branch already exists" << endl;
        return 1;
    }
    ofstream ofs(newBranch);
    string headBranch = readContent(head);
    string head_hash = readContent(branches_path / headBranch);
    ofs << head_hash;
    return 0;
}

int commit(const std::string& message, const std::string& secondParent){
    int total_files = 0;
    Commit headCommit = getHeadCommit();
    Commit newCommit(message, make_daytime_string(), headCommit.hash(), secondParent);
    newCommit.setReference(headCommit.getReference());
    for(const auto& entry: filesystem::directory_iterator(add_path)){
        total_files++;
        optional<Blob> b = findBlob(entry.path());
        if(!b.has_value()){
            cout << "Internal Error" << endl;
            return 2;
        }
        cout << blob_path / b.value().hash() << endl;
        b.value().save(blob_path / b.value().hash());
        newCommit.addReference(b.value().getName(), b.value().hash());
        filesystem::remove(entry.path());
    }
    for(const auto& entry: filesystem::directory_iterator(remove_path)){
        total_files++;
        optional<Blob> b = findBlob(entry.path());
        if(!b.has_value()){
            cout << "Internal Error" << endl;
            return 2;
        }
        newCommit.rmReference(b.value().getName());
        filesystem::remove(entry.path());
    }
    if(total_files == 0){
        cout << "No changes added or removed" << endl;
        return 1;
    }
    newCommit.save();
    filesystem::path headBranch = branches_path / readContent(head);
    ofstream ofs(headBranch);
    if(ofs.is_open()) {
        ofs << newCommit.hash();
        ofs.close();
    }
    return 0;
}

int checkoutBranch(const std::string& branchName){
    filesystem::path branchFile = branches_path / branchName;
    if(readContent(head) == branchName){
        cout << "Already At Branch" << endl;
        return 1;
    }
    if(!filesystem::exists(branchFile)){
        cout << "Branch Does Not Exist" << endl;
        return 1;
    }
    std::optional<Commit> branchCommit = findCommit(readContent(branches_path / branchName));
    checkoutCommit(branchCommit.value().hash());
    ofstream ofs(head);
    if(ofs.is_open()) {
        ofs << branchName;
        ofs.close();
    }
    return 0;
}

int checkoutCommit(const std::string& commitHash){
    std::optional<Commit> targetCommit = findCommit(commitHash);
    if(!targetCommit.has_value()){
        cout << "Commit Does not Exist" << endl;
        return 1;
    }
    Commit headCommit = getHeadCommit();
    vector<filesystem::path> rmList;
    for(const auto& entry: filesystem::directory_iterator(cwd)){
        string filename = entry.path().filename().string();
        if(filename != ".gitlet" && filename != "gitlet" && filename != ".DS_Store"){
            Blob b(entry.path());
            if(!headCommit.hasBlob(filename, b.hash()) && targetCommit.value().hasReference(filename)){
                cout << "There is an untracked file in the way; delete it, or add and commit it first." << endl;
                return 1;
            }
            if(!targetCommit.value().hasBlob(filename, b.hash()) && headCommit.hasBlob(filename, b.hash())){
                rmList.push_back(entry.path());
            }
        }
    }
    for(auto& p:rmList){
        filesystem::remove(p);
    }
    for(const auto& [k,v] : targetCommit.value().getReference()){
        string a = k;
        string a2 = v;
        optional<Blob> b = findBlob(blob_path / v);
        if(b.has_value()){
            ofstream ofs(k);
            ofs << b.value().getContent();
        }
    }
    for(const auto& entry: filesystem::directory_iterator(add_path)){
        filesystem::remove(entry);
    }
    for(const auto& entry: filesystem::directory_iterator(remove_path)){
        filesystem::remove(entry);
    }
    return 0;
}