#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <numeric>
#include <vector>
#include <sstream>
#include <string>
#include <set>

using namespace std;

string exec(const char* cmd) {
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "ERROR";
    char buffer[256];
    string result = "";
    while(!feof(pipe)) {
    	if(fgets(buffer, 256, pipe) != NULL)
    		result += buffer;
    }
    pclose(pipe);
    return result;
}

string toUpper(const string& s) {
    string ret;
    for (const char& c: s) {
        if ('a' <= c && c <= 'z') {
            ret += static_cast<char>('A' + c - 'a');
        }  else {
            ret += c;
        }
    }
    return ret;
}

vector< string > parse(string str) {
    vector<string> ret;
    istringstream iss(str);
    string line;
    while (getline(iss, line)) {
        istringstream ss(line);
        vector<string> tokens;
        string token;
        while (ss >> token) {
            tokens.push_back(token);
        }
        
        if (tokens.size() > 3) { 
            ret.push_back(toUpper(tokens[3]));    
        }
    }   

    return ret;
}

vector<string> readAddresses(const string& fileName) {
    ifstream in(fileName);
    string mac;
    vector<string> ret;
    while (getline(in, mac)) {
        if (mac.size() > 0) 
            ret.push_back(mac);
    }
    in.close();
    return ret;
}

void writeAddresses(const string& fileName,const vector<string>& addresses) {
    ofstream out(fileName);
    for (const string& x : addresses) {
        out << x << "\n";
    }
    out.close();
}

int main(int argc,char* argv[]) {
    unsigned int t = 1;
    if (argc > 2) {
        cout << "Prea multe argumente\n";
        return 0;
    } else 
    if (argc == 2) {
        t = atoi(argv[1]);
    }
   
    while (1) {
        vector<string> addresses = parse(exec("arp -a")); 
        vector<string> stored = readAddresses("mac.txt");
        sort(begin(addresses), end(addresses));
        sort(begin(stored), end(stored));
        unsigned int n = stored.size(), j = 0;
        for (size_t i = 0; i < n; i++) {
            while (j < addresses.size() && stored[i] > addresses[j]) {
               stored.push_back(addresses[j++]);
            }

            if (j >= addresses.size()) {
                cout << stored[i] << "\n";
            } else
            if (stored[i] == addresses[j]) {
                j++;
            } else {
                cout << stored[i] << "\n";
            }
        }
      
        cout << "\n";
        while (j < addresses.size()) {
            stored.push_back(addresses[j++]);
        }

        writeAddresses("mac.txt", stored);
        this_thread::sleep_for(chrono::seconds(t));
    }

    return 0;
}   
