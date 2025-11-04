#include <iostream>
#include <iomanip>
#include <vector>
#include <ctime>
#include <pthread.h>
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

struct ThreadArgs {
    Matrix& A;
    Matrix& B;
    Matrix& C;
    int bi, bk, bj, k;
    pthread_mutex_t& mtx;

    ThreadArgs(Matrix& A_, Matrix& B_, Matrix& C_, int bi_, int bk_, int bj_, int k_, pthread_mutex_t& mtx_)
      : A(A_), B(B_), C(C_), bi(bi_), bk(bk_), bj(bj_), k(k_), mtx(mtx_) {}
};

void* mul_block_add(void* arg) {
    ThreadArgs args = *static_cast<ThreadArgs*>(arg);
    Matrix& A = args.A;
    Matrix& B = args.B;
    Matrix& C = args.C;
    int bi = args.bi;
    int bk = args.bk;
    int bj = args.bj;
    int k = args.k;

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

    pthread_mutex_lock(&args.mtx);
    for (int ii = 0; ii < k; ++ii) {
        int i = ai0 + ii;
        for (int jj = 0; jj < k; ++jj) {
            int j = bj0 + jj;
            C[i][j] += temp[ii][jj];
        }
    }
    pthread_mutex_unlock(&args.mtx);

    return nullptr;
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

        vector<pthread_t> threads(totalTasks);
        vector<ThreadArgs> args;
        args.reserve(totalTasks);

        pthread_mutex_t mtx;
        pthread_mutex_init(&mtx, nullptr);

        size_t idx = 0;
        auto t0 = chrono::high_resolution_clock::now();

        for (int bi = 0; bi < blocks; ++bi) {
            for (int bj = 0; bj < blocks; ++bj) {
                for (int bk = 0; bk < blocks; ++bk) {
                    args.emplace_back(A, B, C, bi, bk, bj, k, mtx);
                    int rc = pthread_create(&threads[idx], nullptr, mul_block_add, &args.back());
                    ++idx;
                }
            }
        }

        for (size_t i = 0; i < totalTasks; ++i) {
            pthread_join(threads[i], nullptr);
        }

        auto t1 = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> elapsed = t1 - t0;

        pthread_mutex_destroy(&mtx);

        cout << setw(4) << k << " | " << setw(5) << totalTasks << " | "
             << fixed << setprecision(3) << elapsed.count() << '\n';
    }

    cout << "Готово.\n";
    return 0;
}
