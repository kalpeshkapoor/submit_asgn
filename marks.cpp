#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <pwd.h>
#include <iomanip>  // for setw() and left
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <ctime>
#include <iomanip>

using namespace std;

const string base_dir = "/home/<userid>/data_dir/";

string eval_file  = base_dir + "eval.csv";
string alias_file = base_dir + "alias.csv";
string log_file = base_dir + "log.csv";

int col_width = 8;

string trim(const string &s) {
  size_t start = s.find_first_not_of(" \t\r\n");
  size_t end = s.find_last_not_of(" \t\r\n");

  if (start == string::npos || end == string::npos) {
    return ""; // empty or all spaces
  }
  return s.substr(start, end - start + 1);
}

vector<string> split(const string &s, char delimiter) {  // split a string by a delimiter
  vector<string> tokens;
  string token;
  istringstream tokenStream(s);
  while (getline(tokenStream, token, delimiter)) {
    tokens.push_back(trim(token));
  }
  return tokens;
}

bool load_alias_data(const string &filename, unordered_map<string, string> &userid_to_emailid) {
  ifstream file(filename);
  if (!file.is_open()) {
    cerr << "error: unable to open alias file" << endl;
    return false;
  }

  string line, userid, emailid;
  getline(file, line); // skip header
  while (getline(file, line)) {
    stringstream ss(line);
    getline(ss, userid, ',');
    getline(ss, emailid, ',');

    userid = trim(userid);
    emailid = trim(emailid);

    if (!userid.empty() && !emailid.empty()) {
      userid_to_emailid[userid] = emailid;
    }
  }
  file.close();
  return true;
}

void print_record(const string &filename, const string &userid, const string &emailid) { // search by emailid
  ifstream file(filename);

  if (!file.is_open()) {
    cerr << "error: could not open the user database file" << endl;
    return;
  }

  string line;
  vector<string> headers;

  if (getline(file, line)) { // read the header line
    headers = split(line, ',');
  } else {
    cerr << "error: empty or corrupt database file" << endl;
    return;
  }

  bool found = false;

  while (getline(file, line) && !found) {
    vector<string> values = split(line, ',');
    if (!values.empty() && values[1] == emailid) {
      if (userid != emailid) {
        cout << left << setw(col_width) <<  "User id" <<  ": " << userid << endl;
      }
      for (size_t i = 0; i < headers.size(); ++i) {
        cout << left << setw(col_width) << headers[i] << ": ";
        if (i < values.size()) {
          cout << values[i] << endl;
        } else {
          cout << "N/A" << endl;
        }
      }

      found = true;
    }
  }

  if (!found) {
    cout << "error: emailid " << emailid << " not found in the database." << endl;
  }

  file.close();
}

void log_record(const string &filename, const string &userid, const string &emailid) {
  ofstream file(filename, ios::app);

  if (!file.is_open()) {
    return;
  }

  std::time_t currentTime = std::time(nullptr);
  std::tm* localTime = std::localtime(&currentTime);

  char formattedTime[80];
  std::strftime(formattedTime, 80, "%d %b %Y %H:%M:%S", localTime);
  file << formattedTime << " " << userid << " " << emailid << endl;
  file.close();
}

int main() {
  struct passwd *pw = getpwuid(getuid());
  if (! pw) {
    cerr << "error: cannot find userid" << endl;
    return 1;
  }

  string userid = pw->pw_name;
  if (userid.empty()) {
    cerr << "error: could not determine logged-in user" << endl;
    return 2;
  }

  unordered_map<string, string> userid_to_emailid;
  if (!load_alias_data(alias_file, userid_to_emailid)) return 3;

  string emailid = userid;
  if (userid_to_emailid.find(userid) != userid_to_emailid.end()) {
    emailid = userid_to_emailid.at(userid);
  }

  transform(emailid.begin(), emailid.end(), emailid.begin(), [](unsigned char c){ return tolower(c); });  // convert to lower case

  print_record(eval_file, userid, emailid);
  log_record(log_file, userid, emailid);

  return 0;
}
