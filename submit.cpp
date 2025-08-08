#include <iostream>
#include <filesystem>
#include <cstring>
#include <unistd.h>
#include <pwd.h>
#include <vector>
#include <sys/stat.h>

using namespace std;
namespace fs = filesystem;

const string nfs_root = "/nfscommon/common/";
const string sub_dir = "/submissions/";

uid_t stud_uid;
uid_t fac_uid;

void print_usage(char* prog) {
  cout << "Usage: " << prog << " [-l] <file1> <file2> ... <directory>" << endl;
}

bool all_exists(const vector<fs::path>& source_paths) {
  for (const auto& source_path : source_paths) {
    if (!fs::exists(source_path)) {
      cerr << "error: " << source_path.string() << " does not exist." << endl;
      return false;
    }
  }
  return true;
}

bool all_can_be_copied(const vector<fs::path>& source_paths, const fs::path& destination_dir) {
  setuid(fac_uid);
  for (const auto& source_path : source_paths) {
    if (fs::exists(destination_dir / source_path.filename())) {
      cerr << "error: " << source_path.string() << " already exists in the destination." << endl;
      return false;
    }
  }
  setuid(stud_uid);
  return true;
}

void copy_recursive(const fs::path& source, const fs::path& destination) {
  if (fs::is_directory(source)) {
    fs::create_directory(destination);
    chmod(destination.c_str(), 0755);
    for (const auto& entry : fs::directory_iterator(source)) {
      copy_recursive(entry, destination / entry.path().filename());
    }
  } else {
    fs::copy_file(source, destination, fs::copy_options::overwrite_existing);
    chmod(destination.c_str(), 0444);
  }
}

void create_sub_dir(const fs::path& destination_dir) {
  setuid(fac_uid);
  if (!fs::exists(destination_dir)) {
    fs::create_directories(destination_dir);
    chmod(destination_dir.c_str(), 0700);
  }
  setuid(stud_uid);
}

void list_files(const fs::path& destination_dir) {
  setuid(fac_uid);
  for (const auto& entry : fs::recursive_directory_iterator(destination_dir)) { // calculate the relative path to the submission directory
    fs::path relative_path = fs::relative(entry.path(), destination_dir);
    cout << relative_path.string() << endl;
  }
  setuid(stud_uid);
}

fs::path create_dir_in_tmp(string sname) {
  time_t now = time(0);
  stringstream ss;
  ss << now;

  fs::path temp_dir = fs::temp_directory_path() / (sname + "_" + ss.str());
  fs::create_directories(temp_dir);
  chmod(temp_dir.c_str(), 0755);
  return temp_dir;
}

void submit_files(fs::path& source, const fs::path& destination) {
  setuid(fac_uid);
  cout << "saved:";
  for (const auto& entry : fs::directory_iterator(source)) {
    copy_recursive(entry, destination / entry.path().filename());
    cout << " " << entry.path().filename().string();
  }
  cout << endl;
  setuid(stud_uid);
}

int main(int argc, char* argv[]) {
  stud_uid = getuid();
  fac_uid = geteuid();

  if (argc < 2) {
    print_usage(argv[0]);
    return 1;
  }

  string student = getpwuid(stud_uid)->pw_name;
  cout << "user: " << student << endl;
  string faculty = getpwuid(fac_uid)->pw_name; // cout << "faculty: " << faculty << endl; return 0;

  setuid(stud_uid);
  fs::path submission_dir = nfs_root + faculty + sub_dir + student;  // cout << submission_dir.string() << endl;
  create_sub_dir(submission_dir);

  if (argv[1][0] == '-') {
    if (strcmp(argv[1], "-l") == 0) {   // check for the "-l" option
      list_files(submission_dir);
      return 0;
    }

    cerr << "error: invalid option: " << argv[1] << endl;
    print_usage(argv[0]);
    return 1;
  }

  vector<fs::path> source_paths;
  for (int i = 1; i < argc; ++i) {                // collect source paths
    source_paths.push_back(fs::path(argv[i]));
  }

  if (! all_exists(source_paths) || ! all_can_be_copied(source_paths, submission_dir)) {
    return 1;
  }

  fs::path temp_dir = create_dir_in_tmp(student); // cout << temp_dir.string() << endl;

  for (const auto& source_path : source_paths) {
    copy_recursive(source_path, temp_dir / source_path.filename());
  }

  submit_files(temp_dir, submission_dir);
  fs::remove_all(temp_dir);
  return 0;
}
