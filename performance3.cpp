#include <bits/stdc++.h>
#include <fstream>
using namespace std;

struct Packet {
    int pid, arr_time, in_port, out_port;

    Packet(int p, int t, int i, int o) {
        pid = p;
        arr_time = t;
        in_port = i;
        out_port = o;
    }
};

int main() {

    int N = 3;

    vector<Packet> packets = {
        {1,0,0,0}, {2,0,0,1}, {3,0,1,0}, {4,0,1,2}, {5,0,2,0},
        {6,1,0,2}, {7,1,2,1},
        {8,2,1,1}, {9,2,2,2},
        {10,3,0,1}, {11,3,1,0}, {12,3,2,1},
        {13,4,0,0}, {14,4,1,2}, {15,4,2,2},
        {16,5,0,2}, {17,5,1,1}, {18,5,2,0}
    };

    map<int, vector<Packet>> arrivals;
    for (auto &p : packets) {
        arrivals[p.arr_time].push_back(p);
    }

    vector<vector<queue<int>>> voq(N, vector<queue<int>>(N));

    vector<int> input_ptr(N, 0);
    vector<int> output_ptr(N, 0);

    int time = 0;
    vector<string> log;

    while (true) {

        // ARRIVALS
        if (arrivals.count(time)) {
            for (auto &p : arrivals[time]) {
                voq[p.in_port][p.out_port].push(p.pid);
            }
        }

        map<int, int> grants;
        map<int, int> accepts;

        // REQUEST
        vector<vector<int>> requests(N);
        for (int i = 0; i < N; i++) {
            for (int o = 0; o < N; o++) {
                if (!voq[i][o].empty()) {
                    requests[o].push_back(i);
                }
            }
        }

        // GRANT
        for (int o = 0; o < N; o++) {
            if (!requests[o].empty()) {
                int start = output_ptr[o];

                for (int k = 0; k < N; k++) {
                    int i = (start + k) % N;

                    if (find(requests[o].begin(), requests[o].end(), i) != requests[o].end()) {
                        grants[o] = i;
                        break;
                    }
                }
            }
        }

        // ACCEPT
        for (int i = 0; i < N; i++) {
            vector<int> possible;

            for (auto &[o, g] : grants) {
                if (g == i) possible.push_back(o);
            }

            if (!possible.empty()) {
                int start = input_ptr[i];

                for (int k = 0; k < N; k++) {
                    int o = (start + k) % N;

                    if (find(possible.begin(), possible.end(), o) != possible.end()) {
                        accepts[i] = o;
                        break;
                    }
                }
            }
        }

        // SEND
        vector<tuple<int,int,int>> sent;

        for (auto &[i, o] : accepts) {
            int pid = voq[i][o].front();
            voq[i][o].pop();

            sent.push_back({pid, i, o});

            input_ptr[i] = (o + 1) % N;
            output_ptr[o] = (i + 1) % N;
        }

        // LOG SENT (consistent format)
        if (!sent.empty()) {
            stringstream ss;
            ss << "t=" << time << ": Sent [";

            for (int i = 0; i < sent.size(); i++) {
                auto &[pid, in, out] = sent[i];
                ss << "(" << pid << "," << in << "," << out << ")";
                if (i != sent.size() - 1) ss << ", ";
            }

            ss << "]";
            log.push_back(ss.str());
        }

        // 🔥 BACKLOG CALCULATION
        int backlog = 0;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                backlog += voq[i][j].size();
            }
        }

        stringstream bss;
        bss << "t=" << time << ": Backlog = " << backlog;
        log.push_back(bss.str());

        // EXIT
        bool all_empty = true;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (!voq[i][j].empty()) {
                    all_empty = false;
                    break;
                }
            }
        }

        if (all_empty && time > arrivals.rbegin()->first) {
            break;
        }

        time++;
    }

    // PRINT
    for (auto &s : log) {
        cout << s << endl;
    }

    // WRITE FILE
    ofstream outFile("islip_log.txt");

    if (!outFile) {
        cout << "Error opening file!" << endl;
        return 1;
    }

    for (auto &s : log) {
        outFile << s << endl;
    }

    outFile.close();

    cout << "Logs written to islip_log.txt" << endl;

    return 0;
}
