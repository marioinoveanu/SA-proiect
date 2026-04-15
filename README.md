# Studiu experimental asupra metodelor de sortare

Acest proiect realizează o analiză comparativă a performanței pentru șapte algoritmi de sortare clasici, implementați în C++. Studiul urmărește validarea proprietăților teoretice ale algoritmilor (complexitate temporală și spațială) prin teste pe seturi de date de dimensiuni variate și cu distribuții diferite.

### 📌 Conținut

* **SortingMaster.cpp** — Implementarea algoritmilor, a sistemului de benchmark multithreaded și a logicii de monitorizare RAM;
* **benchmark_results.csv** — Rezultate brute exportate (Algoritm, N, Distribuție, Timp, RAM, Status);
* **Analiza_Sortare.xlsx** — Prelucrarea datelor sub formă de tabele pivot și grafice comparative;
* **Raport_Sortare.pdf** — Documentația științifică care interpretează discrepanțele dintre teoria $O(f(n))$ și realitatea hardware.

### 🧪 Descriere experiment

Testarea a fost realizată pe vectori cu dimensiuni cuprinse între **N=10** și **N=10^7** elemente, utilizând patru scenarii de distribuție:
1.  **Random:** Elemente generate aleatoriu (uniform);
2.  **Ascending:** Liste deja sortate (pentru a verifica Best Case);
3.  **Descending:** Liste sortate invers (pentru a verifica Worst Case);
4.  **Almost Sorted:** Liste cvasi-ordonate (aproximativ 5% din elemente sunt amestecate).

**Metodologie de măsurare:**
* **Timpul de execuție:** Măsurat cu precizie de nanosecunde folosind `std::chrono::steady_clock`.
* **Memoria RAM:** Monitorizare în timp real a setului de lucru (Working Set) folosind API-uri specifice (`GetProcessMemoryInfo` pe Windows, `/proc/self/status` pe Linux).
* **Controlul execuției:** Utilizarea `std::async` pentru rularea paralelă și a unui `stop_flag` pentru a opri algoritmii care depășesc pragul de **TIMEOUT** (300 secunde).

### 📈 Algoritmi Testați

| Categorie | Algoritmi | Complexitate Teoretică |
| :--- | :--- | :--- |
| **Elementari** | Bubble Sort, Selection Sort, Insertion Sort | $O(n^2)$ |
| **Eficienți** | Merge Sort, Heap Sort | $O(n \log n)$ |
| **Specializați** | Radix Sort, Cycle Sort | $O(nk)$ / $O(n^2)$ |

### 📊 Rezultate și Concluzii (Preview)

* **Eficiență:** Algoritmii $O(n \log n)$ devin obligatorii pentru seturi mari de date, în timp ce algoritmii de tip $O(n^2)$ sunt practic inutilizabili peste pragul de 100.000 de elemente.
* **Adaptabilitate:** **Insertion Sort** confirmă superioritatea pe listele "Almost Sorted" și pe dimensiuni foarte mici (N < 50), depășind deseori Merge Sort datorită overhead-ului redus.
* **Limitări practice:** Cycle Sort, deși teoretic optim pentru numărul de scrieri, prezintă un timp de execuție ridicat în acest context experimental.

### 📚 Bibliografie

* **Thomas H. Cormen** - *Introduction to Algorithms* (CLRS);
* **Donald Knuth** - *The Art of Computer Programming, Vol. 3: Sorting and Searching*;
* **C++ Reference** - Documentația pentru modelele de memorie și execuție asincronă.

### 📎 Licență

Proiect realizat ca parte a disciplinei **Algoritmi și Structuri de Date** — Semestrul II, 2026.
