#include <iostream>
#include <bits/stdc++.h>
#include <fstream>
using namespace std;

struct Packet {
    int pid, arr_time, in_port, out_port;

    Packet(int p, int a_t, int i_p, int o_p) {
        pid = p;
        arr_time = a_t;
        in_port = i_p;
        out_port = o_p;
    }
};

int N = 3;

int main() {

    vector<Packet> traffic;

    vector<vector<int>> data = {
        {1,0,0,0}, {2,0,0,1}, {3,0,1,0}, {4,0,1,2}, {5,0,2,0},
        {6,1,0,2}, {7,1,2,1},
        {8,2,1,1}, {9,2,2,2},
        {10,3,0,1}, {11,3,1,0}, {12,3,2,1},
        {13,4,0,0}, {14,4,1,2}, {15,4,2,2},
        {16,5,0,2}, {17,5,1,1}, {18,5,2,0}
    };

    for (auto &p : data) {
        traffic.emplace_back(p[0], p[1], p[2], p[3]);
    }

    map<int, vector<Packet>> arrivals;
    for (auto &p : traffic) {
        arrivals[p.arr_time].push_back(p);
    }

    vector<queue<Packet>> input_queue(N);
    int time = 0;
    vector<string> log;

    while (true) {

        // ARRIVALS
        if (arrivals.count(time)) {
            for (auto &p : arrivals[time]) {
                input_queue[p.in_port].push(p);
            }
        }

        set<int> used_output_ports;
        vector<Packet> sent;

        // PROCESS INPUTS
        for (int i = 0; i < N; i++) {
            if (!input_queue[i].empty()) {
                Packet pkt = input_queue[i].front();

                if (used_output_ports.find(pkt.out_port) == used_output_ports.end()) {
                    used_output_ports.insert(pkt.out_port);
                    input_queue[i].pop();
                    sent.push_back(pkt);
                } else {
                    stringstream ss;
                    ss << "t=" << time << ": HoL BLOCK at input "
                       << i << " for pid " << pkt.pid;
                    log.push_back(ss.str());
                }
            }
        }

        // LOG SENT PACKETS
        if (!sent.empty()) {
            stringstream ss;
            ss << "t=" << time << ": Sent ";

            for (int i = 0; i < sent.size(); i++) {
                auto &p = sent[i];
                ss << "(" << p.pid << "," << p.in_port << "," << p.out_port << ")";
                if (i != sent.size() - 1) ss << " ";
            }

            log.push_back(ss.str());
        }

        // 🔥 BACKLOG CALCULATION
        int backlog = 0;
        for (auto &q : input_queue) {
            backlog += q.size();
        }

        stringstream bss;
        bss << "t=" << time << ": Backlog = " << backlog;
        log.push_back(bss.str());

        // EXIT CONDITION
        if (all_of(input_queue.begin(), input_queue.end(), [](auto &q) {
                return q.empty();
            }) && time > arrivals.rbegin()->first) {
            break;
        }

        time++;
    }

    // PRINT LOG
    for (auto &s : log) {
        cout << s << endl;
    }

    // WRITE TO FILE
    ofstream outFile("fifo_log.txt");

    if (!outFile) {
        cout << "Error opening file!" << endl;
        return 1;
    }

    for (auto &s : log) {
        outFile << s << endl;
    }

    outFile.close();

    cout << "Logs written to fifo_log.txt" << endl;

    return 0;
}
