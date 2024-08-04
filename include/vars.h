//
// Created by Yile Hu on 7/25/24.
//

#ifndef GITLET_VARS_H
#define GITLET_VARS_H
#include "common.h"
using namespace std;
filesystem::path cwd = filesystem::current_path();
filesystem::path main_path = cwd / ".gitlet";
filesystem::path staging_path = main_path / "staging";
filesystem::path add_path = staging_path / "add";
filesystem::path remove_path = staging_path / "remove";
filesystem::path commit_path = main_path / "commit";
filesystem::path head = main_path / "HEAD";
filesystem::path branches_path = main_path / "branch";
filesystem::path blob_path = main_path / "blob";
#endif //GITLET_VARS_H
