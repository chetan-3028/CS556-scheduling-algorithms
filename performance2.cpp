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

    vector<Packet> traffic = {
        {1,0,0,0}, {2,0,0,1}, {3,0,1,0}, {4,0,1,2}, {5,0,2,0},
        {6,1,0,2}, {7,1,2,1},
        {8,2,1,1}, {9,2,2,2},
        {10,3,0,1}, {11,3,1,0}, {12,3,2,1},
        {13,4,0,0}, {14,4,1,2}, {15,4,2,2},
        {16,5,0,2}, {17,5,1,1}, {18,5,2,0}
    };

    // arrivals
    map<int, vector<Packet>> arrivals;
    for (auto &p : traffic) {
        arrivals[p.arr_time].push_back(p);
    }

    // VOQ
    vector<vector<queue<int>>> voq(N, vector<queue<int>>(N));

    int time = 0;
    vector<string> log;

    while (true) {

        // ARRIVALS
        if (arrivals.count(time)) {
            for (auto &p : arrivals[time]) {
                voq[p.in_port][p.out_port].push(p.pid);
            }
        }

        vector<pair<int,int>> best_match;

        vector<int> outputs(N);
        iota(outputs.begin(), outputs.end(), 0);

        // MATCHING (brute force)
        do {
            vector<pair<int,int>> match;
            set<int> used_out;

            for (int i = 0; i < N; i++) {
                int o = outputs[i];

                if (!voq[i][o].empty() && !used_out.count(o)) {
                    match.push_back({i, o});
                    used_out.insert(o);
                }
            }

            if (match.size() > best_match.size()) {
                best_match = match;
            }

        } while (next_permutation(outputs.begin(), outputs.end()));

        // SEND
        vector<tuple<int,int,int>> sent;

        for (auto &[i, o] : best_match) {
            int pid = voq[i][o].front();
            voq[i][o].pop();
            sent.push_back({pid, i, o});
        }

        // LOG SENT
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

        // CHECK EXIT
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

    // PRINT LOG
    for (auto &s : log) {
        cout << s << endl;
    }

    // WRITE TO FILE
    ofstream outFile("voq_log.txt");

    if (!outFile) {
        cout << "Error opening file!" << endl;
        return 1;
    }

    for (auto &s : log) {
        outFile << s << endl;
    }

    outFile.close();

    cout << "Logs written to voq_log.txt" << endl;

    return 0;
}
