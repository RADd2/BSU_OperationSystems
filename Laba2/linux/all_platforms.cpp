#include <iostream>
#include <iomanip>
#include <vector>
#include <ctime>
#include <thread>
#include <functional>
#include <mutex>
#include <algorithm>
#include <chrono>

using namespace std;
using Matrix = vector<vector<int>>;

Matrix random_matrix(int n) {
    Matrix m(n, vector<int>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            m[i][j] = rand() % 5 + 1;
    return m;
}

void print_matrix(const Matrix& m, const string& name) {
    cout << name << " (" << m.size() << "x" << m.size() << "):\n";
    for (const auto& row : m) {
        for (int x : row)
            cout << setw(4) << x << ' ';
        cout << '\n';
    }
    cout << '\n';
}

void mul_block_add(const Matrix& A, const Matrix& B, Matrix& C,
                   int bi, int bk, int bj, int k, mutex& mtx) {
    int ai0 = bi * k;
    int ak0 = bk * k;
    int bk0 = bk * k;
    int bj0 = bj * k;

    Matrix temp(k, vector<int>(k, 0));

    for (int ii = 0; ii < k; ++ii) {
        int i = ai0 + ii;
        for (int jj = 0; jj < k; ++jj) {
            int j = bj0 + jj;
            int sum = 0;
            for (int ss = 0; ss < k; ++ss) {
                int sA = ak0 + ss;
                int sB = bk0 + ss;
                sum += A[i][sA] * B[sB][j];
            }
            temp[ii][jj] += sum;
        }
    }

    {
        lock_guard<mutex> lock(mtx);
        for (int ii = 0; ii < k; ++ii) {
            int i = ai0 + ii;
            for (int jj = 0; jj < k; ++jj) {
                int j = bj0 + jj;
                C[i][j] += temp[ii][jj];
            }
        }
    }
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    int n;
    cout << "Введите размер матрицы n: ";
    cin >> n;

    Matrix A = random_matrix(n);
    Matrix B = random_matrix(n);

    // print_matrix(A, "A");
    // print_matrix(B, "B");

    vector<int> divisors;
    for (int d = 1; d * d <= n; ++d) {
        if (n % d == 0) {
            divisors.push_back(d);
            if (d != n / d) divisors.push_back(n / d);
        }
    }
    sort(divisors.begin(), divisors.end());

    cout << "Измерение времени для всех делителей k (которые делят " << n << "):\n";
    cout << " k   | tasks | time (ms)\n";
    cout << "--------------------------------\n";

    for (int k : divisors) {
        int blocks = n / k;
        size_t totalTasks;
        if (blocks > 25) {
            cout << setw(4) << k << " | " << setw(5) << "TOO" << " | "
                 << "skipped (blocks too large)\n";
            continue;
        } else {
            totalTasks = static_cast<size_t>(blocks) * blocks * blocks;
        }

        Matrix C(n, vector<int>(n, 0));
        vector<thread> threads;
        threads.reserve(totalTasks);
        mutex mtx;

        auto t0 = chrono::high_resolution_clock::now();

        for (int bi = 0; bi < blocks; ++bi) {
            for (int bj = 0; bj < blocks; ++bj) {
                for (int bk = 0; bk < blocks; ++bk) {
                    threads.emplace_back(mul_block_add,
                                         cref(A), cref(B), ref(C),
                                         bi, bk, bj, k, ref(mtx));
                }
            }
        }

        for (size_t i = 0; i < threads.size(); ++i) {
            threads[i].join();
        }

        auto t1 = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> elapsed = t1 - t0;

        cout << setw(4) << k << " | " << setw(5) << totalTasks << " | "
             << fixed << setprecision(3) << elapsed.count() << '\n';
    }

    cout << "Готово.\n";
    return 0;
}
