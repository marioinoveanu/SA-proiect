#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <string>
#include <future>
#include <mutex>

using namespace std;
using namespace std::chrono;

mutex consola_mtx;

vector<int> genereazaVector(int n)
{
    vector<int> v(n);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 100000);
    for (int i = 0; i < n; i++)
        v[i] = dis(gen);
    return v;
}

// --- Algoritmi O(n^2) ---

void insertionSort(vector<int>& v)
{
    int n = v.size();
    for (int i = 1; i < n; i++)
    {
        int key = v[i];
        int j = i - 1;
        while (j >= 0 && v[j] > key)
        {
            v[j + 1] = v[j];
            j = j - 1;
        }
        v[j + 1] = key;
    }
}

void selectionSort(vector<int>& v)
{
    int n = v.size();
    for (int i = 0; i < n - 1; i++)
    {
        int min_idx = i;
        for (int j = i + 1; j < n; j++)
            if (v[j] < v[min_idx])
                min_idx = j;
        swap(v[i], v[min_idx]);
    }
}

void cycleSort(vector<int>& v)
{
    int n = v.size();
    for (int cycle_start = 0; cycle_start <= n - 2; cycle_start++)
        {
        int item = v[cycle_start];
        int pos = cycle_start;
        for (int i = cycle_start + 1; i < n; i++)
            if (v[i] < item)
                pos++;
        if (pos == cycle_start)
            continue;
        while (item == v[pos])
            pos += 1;
        if (pos != cycle_start)
            swap(item, v[pos]);
        while (pos != cycle_start)
        {
            pos = cycle_start;
            for (int i = cycle_start + 1; i < n; i++)
                if (v[i] < item)
                    pos += 1;
            while (item == v[pos])
                pos += 1;
            if (item != v[pos])
                swap(item, v[pos]);
        }
    }
}

// --- Algoritmi O(n log n) ---

void merge(vector<int>& v, int l, int m, int r)
{
    int n1 = m - l + 1;
    int n2 = r - m;
    vector<int> L(n1), R(n2);
    for (int i = 0; i < n1; i++)
        L[i] = v[l + i];
    for (int j = 0; j < n2; j++)
        R[j] = v[m + 1 + j];
    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
            v[k++] = L[i++];
        else
            v[k++] = R[j++];
    }
    while (i < n1)
        v[k++] = L[i++];
    while (j < n2)
        v[k++] = R[j++];
}

void mergeSortRecursive(vector<int>& v, int l, int r)
{
    if (l < r)
    {
        int m = l + (r - l) / 2;
        mergeSortRecursive(v, l, m);
        mergeSortRecursive(v, m + 1, r);
        merge(v, l, m, r);
    }
}

void callMergeSort(vector<int>& v)
{
    mergeSortRecursive(v, 0, v.size() - 1);
}

void heapify(vector<int>& v, int n, int i)
{
    int largest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    if (l < n && v[l] > v[largest])
        largest = l;
    if (r < n && v[r] > v[largest])
        largest = r;
    if (largest != i)
    {
        swap(v[i], v[largest]);
        heapify(v, n, largest);
    }
}

void heapSort(vector<int>& v)
{
    int n = v.size();
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(v, n, i);
    for (int i = n - 1; i > 0; i--)
    {
        swap(v[0], v[i]);
        heapify(v, i, 0);
    }
}

long long masoaraSortare(string nume, void (*functie)(vector<int>&), vector<int> date)
{
    {
        lock_guard<mutex> lock(consola_mtx);
        cout << "[START] " << nume << " a pornit..." << endl;
    }
    auto t1 = high_resolution_clock::now();
    functie(date);

    auto t2 = high_resolution_clock::now();

    auto durata = duration_cast<milliseconds>(t2 - t1).count();

    {
        lock_guard<mutex> lock(consola_mtx);
        cout << "[GATA]  " << nume << " a terminat in: " << durata << " ms" << endl;
    }
    return durata;
}

int main()
{
    int n;
    cout << "Introduceti numarul de elemente pentru test: ";
    cin >> n;

    if (n > 25000)
    {
        cout << "Atentie: Pentru n > 25.000, algoritmii O(n^2) vor dura considerabil!\n";
    }

    vector<int> dateOriginale = genereazaVector(n);

    cout << "\nLansare algoritmi in paralel \n";
    cout << "----------------------------------------------------------\n";

    auto f1 = async(launch::async, masoaraSortare, "Selection Sort", selectionSort, dateOriginale);
    auto f2 = async(launch::async, masoaraSortare, "Insertion Sort", insertionSort, dateOriginale);
    auto f3 = async(launch::async, masoaraSortare, "Merge Sort    ", callMergeSort, dateOriginale);
    auto f4 = async(launch::async, masoaraSortare, "Heap Sort     ", heapSort, dateOriginale);
    auto f5 = async(launch::async, masoaraSortare, "Cycle Sort    ", cycleSort, dateOriginale);

    f1.get();
    f2.get();
    f3.get();
    f4.get();
    f5.get();

    cout << "----------------------------------------------------------\n";
    cout << "Toate testele au fost finalizate." << endl;

    return 0;
}
