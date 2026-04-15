#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <string>
#include <future>
#include <mutex>
#include <iomanip>
#include <atomic>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#else
#include <sys/resource.h>
#include <unistd.h>
#endif

using namespace std;
using namespace std::chrono;

mutex consola_mtx;
mutex rezultate_mtx;
atomic<bool> stop_flag(false);

struct RezultatBenchmark {
    string numeAlgoritm;
    long long nanosecunde;
    double milisecunde;
    double secunde;
    double ramMB;
    bool aFostAbortat;
};

vector<RezultatBenchmark> listaRezultate;

size_t getRamUsage() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
        return pmc.PrivateUsage;
    return 0;
#else
    long rss = 0L;
    ifstream fp("/proc/self/status");
    string line;
    while (getline(fp, line)) {
        if (line.compare(0, 6, "VmRSS:") == 0) {
            size_t i = line.find_first_of("0123456789");
            rss = stol(line.substr(i));
            break;
        }
    }
    return (size_t)rss * 1024L;
#endif
}

vector<int> citesteDinFisier(string numeFisier) {
    vector<int> v;
    ifstream fin(numeFisier);
    if (!fin.is_open()) return v;
    int numar;
    while (fin >> numar) v.push_back(numar);
    return v;
}

vector<int> genereazaDate(int n, int optiune) {
    vector<int> v(n);
    random_device rd;
    mt19937 gen(rd());
    if (optiune == 1) {
        uniform_int_distribution<> dis(1, 1000000);
        for (int& x : v) x = dis(gen);
    } else if (optiune == 2) {
        for (int i = 0; i < n; i++) v[i] = i;
    } else if (optiune == 3) {
        for (int i = 0; i < n; i++) v[i] = n - i;
    } else if (optiune == 4) {
        for (int i = 0; i < n; i++) v[i] = i;
        uniform_int_distribution<> dis(0, n - 1);
        for (int i = 0; i < n / 20; i++) swap(v[dis(gen)], v[dis(gen)]);
    }
    return v;
}

void bubbleSort(vector<int>& v) {
    int n = v.size();
    for (int i = 0; i < n - 1; i++) {
        if (stop_flag) return;
        bool swp = false;
        for (int j = 0; j < n - i - 1; j++)
            if (v[j] > v[j + 1]) { swap(v[j], v[j + 1]); swp = true; }
        if (!swp) break;
    }
}

void selectionSort(vector<int>& v) {
    int n = v.size();
    for (int i = 0; i < n - 1; i++) {
        if (stop_flag) return;
        int m = i;
        for (int j = i + 1; j < n; j++) if (v[j] < v[m]) m = j;
        swap(v[i], v[m]);
    }
}

void insertionSort(vector<int>& v) {
    int n = v.size();
    for (int i = 1; i < n; i++) {
        if (stop_flag && i % 100 == 0) return;
        int k = v[i], j = i - 1;
        while (j >= 0 && v[j] > k) { v[j + 1] = v[j]; j--; }
        v[j + 1] = k;
    }
}

void cycleSort(vector<int>& v) {
    int n = v.size();
    for (int cs = 0; cs <= n - 2; cs++) {
        if (stop_flag) return;
        int item = v[cs], pos = cs;
        for (int i = cs + 1; i < n; i++) if (v[i] < item) pos++;
        if (pos == cs) continue;
        while (item == v[pos]) pos++;
        if (pos != cs) swap(item, v[pos]);
        while (pos != cs) {
            pos = cs;
            for (int i = cs + 1; i < n; i++) if (v[i] < item) pos++;
            while (item == v[pos]) pos++;
            if (item != v[pos]) swap(item, v[pos]);
        }
    }
}

void merge(vector<int>& v, int l, int m, int r) {
    int n1 = m - l + 1, n2 = r - m;
    vector<int> L(n1), R(n2);
    for (int i = 0; i < n1; i++) L[i] = v[l + i];
    for (int j = 0; j < n2; j++) R[j] = v[m + 1 + j];
    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) v[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];
    while (i < n1) v[k++] = L[i++];
    while (j < n2) v[k++] = R[j++];
}

void mergeSortRecursive(vector<int>& v, int l, int r) {
    if (stop_flag || l >= r) return;
    int m = l + (r - l) / 2;
    mergeSortRecursive(v, l, m);
    mergeSortRecursive(v, m + 1, r);
    merge(v, l, m, r);
}

void heapify(vector<int>& v, int n, int i) {
    int lr = i, l = 2 * i + 1, r = 2 * i + 2;
    if (l < n && v[l] > v[lr]) lr = l;
    if (r < n && v[r] > v[lr]) lr = r;
    if (lr != i) { swap(v[i], v[lr]); heapify(v, n, lr); }
}

void heapSort(vector<int>& v) {
    int n = v.size();
    for (int i = n / 2 - 1; i >= 0; i--) heapify(v, n, i);
    for (int i = n - 1; i > 0; i--) {
        if (stop_flag && i % 100 == 0) return;
        swap(v[0], v[i]); heapify(v, i, 0);
    }
}

void radixSort(vector<int>& v) {
    if (v.empty()) return;
    int mx = *max_element(v.begin(), v.end());
    for (int exp = 1; mx / exp > 0; exp *= 10) {
        if (stop_flag) return;
        vector<int> out(v.size());
        int count[10] = {0};
        for (int i = 0; i < (int)v.size(); i++) count[(v[i] / exp) % 10]++;
        for (int i = 1; i < 10; i++) count[i] += count[i - 1];
        for (int i = (int)v.size() - 1; i >= 0; i--) {
            out[count[(v[i] / exp) % 10] - 1] = v[i];
            count[(v[i] / exp) % 10]--;
        }
        v = out;
    }
}

void masoaraSortare(string nume, void (*functie)(vector<int>&), vector<int> date) {
    { lock_guard<mutex> lock(consola_mtx); cout << "[START] " << left << setw(18) << nume << "..." << endl; }

    size_t memStart = getRamUsage();
    auto t1 = steady_clock::now();
    functie(date);
    auto t2 = steady_clock::now();
    size_t memEnd = getRamUsage();

    auto d_ns = duration_cast<nanoseconds>(t2 - t1);
    double ramUsed = (memEnd > memStart) ? (double)(memEnd - memStart) / (1024.0 * 1024.0) : 0.01;

    RezultatBenchmark r;
    r.numeAlgoritm = nume;
    r.nanosecunde = d_ns.count();
    r.milisecunde = (double)d_ns.count() / 1000000.0;
    r.secunde = (double)d_ns.count() / 1000000000.0;
    r.ramMB = ramUsed;
    r.aFostAbortat = stop_flag;

    { lock_guard<mutex> lock(rezultate_mtx); listaRezultate.push_back(r); }
}

int main() {
    int choice = 0, n = 0;
    string config = "Not Set";
    vector<int> date;

    stop_flag = false;
    listaRezultate.clear();

    cout << "======================================\n";
    cout << "      Sorting Master Benchmark        \n";
    cout << "======================================\n";
    cout << "1. Random List\n";
    cout << "2. Ascending List\n";
    cout << "3. Descending List\n";
    cout << "4. Almost Sorted List\n";
    cout << "5. From Text File\n";
    cout << "Select distribution type: ";
    cin >> choice;

    if (choice == 1) config = "Random";
    else if (choice == 2) config = "Ascending";
    else if (choice == 3) config = "Descending";
    else if (choice == 4) config = "Almost Sorted";
    else if (choice == 5) config = "File Input";
    else { cout << "Invalid choice. Exiting.\n"; return 1; }

    if (choice == 5) {
        string fn; cout << "Filename: "; cin >> fn;
        date = citesteDinFisier(fn);
        n = (int)date.size();
        config = "File (" + fn + ")";
    } else {
        cout << "Enter number of elements (N): "; cin >> n;
        date = genereazaDate(n, choice);
    }

    if (date.empty()) { cout << "No data loaded.\n"; return 1; }

    cout << "\nSorting " << n << " elements (" << config << ")\n";
    cout << "----------------------------------------------------------\n";

    vector<pair<string, void(*)(vector<int>&)>> algos = {
        {"Bubble Sort", bubbleSort}, {"Selection Sort", selectionSort},
        {"Insertion Sort", insertionSort}, {"Cycle Sort", cycleSort},
        {"Merge Sort", [](vector<int>& v){ if(!v.empty()) mergeSortRecursive(v,0,v.size()-1); }},
        {"Heap Sort", heapSort}, {"Radix Sort", radixSort}
    };

    vector<future<void>> tks;
    for (auto& a : algos) tks.push_back(async(launch::async, masoaraSortare, a.first, a.second, date));

    auto s_wait = steady_clock::now();
    while (duration_cast<seconds>(steady_clock::now() - s_wait).count() < 300) {
        size_t g = 0;
        for (auto& f : tks) if (f.wait_for(milliseconds(10)) == future_status::ready) g++;
        if (g == tks.size()) break;
    }

    stop_flag = true;
    for (auto& f : tks) f.get();

    sort(listaRezultate.begin(), listaRezultate.end(), [](const RezultatBenchmark& a, const RezultatBenchmark& b) {
        return a.nanosecunde < b.nanosecunde;
    });

    ofstream ex("benchmark_results.csv");
    if (ex.is_open()) {
        ex << "Algoritm,Elements,Config,Nanosecunde,Milisecunde,Secunde,RAM(MB),Status\n";
        for (const auto& r : listaRezultate) {
            ex << r.numeAlgoritm << ","
               << n << ","
               << config << ","
               << r.nanosecunde << ","
               << fixed << setprecision(4) << r.milisecunde << ","
               << fixed << setprecision(6) << r.secunde << ","
               << fixed << setprecision(2) << r.ramMB << ","
               << (r.aFostAbortat ? "TIMEOUT" : "OK") << "\n";
        }
        ex.close();
        cout << "\nBenchmark Finished. Results saved to 'benchmark_results.csv'.\n";
    }

    return 0;
}
