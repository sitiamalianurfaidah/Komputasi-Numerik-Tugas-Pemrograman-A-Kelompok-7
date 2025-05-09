#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_ROWS 1000
#define MAX_COLS 3

// Struktur data
typedef struct {
    int year[MAX_ROWS];
    double percent[MAX_ROWS];
    double population[MAX_ROWS];
    int n; // jumlah data
    int missingYear[MAX_ROWS];
    int missingCount;
} DataTable;

// Membaca file CSV
void read_csv(const char* filename, DataTable* table) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Tidak dapat membuka file %s\n", filename);
        exit(1);
    }

    char line[100];
    int row = 0;

    while (fgets(line, sizeof(line), file)) {
        if (row == 0 && strstr(line, "Year")) { // skip header
            row++;
            continue;
        }
        if (sscanf(line, "%d,%lf,%lf", &table->year[row], &table->percent[row], &table->population[row]) == 3) {
            row++;
        } else { // data hilang
            table->year[row] = -9999;
            table->percent[row] = -9999;
            table->population[row] = -9999;
            row++;
        }
    }
    table->n = row;
    fclose(file);
}

// Menampilkan data
void print_data(const DataTable* table) {
    printf("\n[Data CSV]\n");
    printf("Year\tPercent\t\tPopulation\n");
    for (int i = 0; i < table->n; i++) {
        printf("%d\t%.4f\t%.4f\n", table->year[i], table->percent[i], table->population[i]);
    }
}

// Cari tahun yang hilang
void find_missing_years(DataTable* table) {
    int minYear = table->year[1];
    int maxYear = table->year[table->n - 1];
    table->missingCount = 0;

    for (int y = minYear; y <= maxYear; y++) {
        int found = 0;
        for (int i = 0; i < table->n; i++) {
            if (table->year[i] == y) {
                found = 1;
                break;
            }
        }
        if (!found) {
            table->missingYear[table->missingCount++] = y;
        }
    }
}

// Fungsi Lagrange Interpolasi
double lagrange_interpolate(int n, double x[], double y[], double X) {
    double sum = 0;
    for (int i = 0; i < n; i++) {
        double term = y[i];
        for (int j = 0; j < n; j++) {
            if (j != i)
                term *= (X - x[j]) / (x[i] - x[j]);
        }
        sum += term;
    }
    return sum;
}

// Fungsi Regresi Polinomial Least Squares (derajat m)
void polynomial_regression(int m, int N, double x[], double y[], double coeff[]) {
    double X[2 * m + 1];
    for (int i = 0; i < 2 * m + 1; i++) {
        X[i] = 0;
        for (int j = 0; j < N; j++)
            X[i] += pow(x[j], i);
    }

    double B[m + 1][m + 2];
    for (int i = 0; i <= m; i++)
        for (int j = 0; j <= m; j++)
            B[i][j] = X[i + j];

    double Y[m + 1];
    for (int i = 0; i <= m; i++) {
        Y[i] = 0;
        for (int j = 0; j < N; j++)
            Y[i] += pow(x[j], i) * y[j];
    }

    for (int i = 0; i <= m; i++)
        B[i][m + 1] = Y[i];

    // Gaussian Elimination
    for (int i = 0; i < m; i++) {
        for (int k = i + 1; k <= m; k++) {
            if (fabs(B[i][i]) < fabs(B[k][i])) {
                for (int j = 0; j <= m + 1; j++) {
                    double temp = B[i][j];
                    B[i][j] = B[k][j];
                    B[k][j] = temp;
                }
            }
        }
    }

    for (int i = 0; i < m; i++) {
        for (int k = i + 1; k <= m; k++) {
            double t = B[k][i] / B[i][i];
            for (int j = 0; j <= m + 1; j++)
                B[k][j] -= t * B[i][j];
        }
    }

    // Back substitution
    for (int i = m; i >= 0; i--) {
        coeff[i] = B[i][m + 1];
        for (int j = 0; j <= m; j++) {
            if (j != i)
                coeff[i] -= B[i][j] * coeff[j];
        }
        coeff[i] /= B[i][i];
    }
}

// Fungsi analisis data: interpolasi + regresi
void analyze_data(DataTable* table) {
    printf("\n[ANALISIS DATA]\n");

    // Estimasi data hilang
    if (table->missingCount == 0) {
        printf("Tidak ada tahun yang hilang terdeteksi.\n");
    } else {
        printf("\n[Estimasi nilai hilang dengan Lagrange]\n");
        for (int idx = 0; idx < table->missingCount; idx++) {
            int missingYear = table->missingYear[idx];
            double x_pts[4], y_pts[4];
            int count = 0;
            for (int i = 0; i < table->n && count < 4; i++) {
                if (table->year[i] != -9999 && table->population[i] != -9999 && abs(table->year[i] - missingYear) <= 5) {
                    x_pts[count] = table->year[i];
                    y_pts[count] = table->population[i];
                    count++;
                }
            }
            if (count == 4) {
                double estPop = lagrange_interpolate(4, x_pts, y_pts, missingYear);
                printf("Populasi estimasi tahun %d: %.4f\n", missingYear, estPop);
            } else {
                printf("Tidak cukup data untuk estimasi populasi tahun %d\n", missingYear);
            }

            double y_percent[4];
            count = 0;
            for (int i = 0; i < table->n && count < 4; i++) {
                if (table->year[i] != -9999 && table->percent[i] != -9999 && abs(table->year[i] - missingYear) <= 5) {
                    x_pts[count] = table->year[i];
                    y_percent[count] = table->percent[i];
                    count++;
                }
            }
            if (count == 4) {
                double estPercent = lagrange_interpolate(4, x_pts, y_percent, missingYear);
                printf("Percent estimasi tahun %d: %.4f\n", missingYear, estPercent);
            } else {
                printf("Tidak cukup data untuk estimasi percent tahun %d\n", missingYear);
            }
        }
    }

    // Regresi polinomial populasi
    printf("\n[Regresi polinomial populasi derajat 2]\n");
    double x_data[MAX_ROWS], y_data[MAX_ROWS];
    int N = 0;
    for (int i = 0; i < table->n; i++) {
        if (table->population[i] != -9999) {
            x_data[N] = table->year[i];
            y_data[N] = table->population[i];
            N++;
        }
    }
    double coeff[3];
    polynomial_regression(2, N, x_data, y_data, coeff);
    printf("Persamaan populasi: y = (%.4f) + (%.4f)x + (%.4f)x^2\n", coeff[0], coeff[1], coeff[2]);

    double predPop = coeff[0] + coeff[1] * 2030 + coeff[2] * pow(2030, 2);
    printf("Estimasi populasi tahun 2030: %.4f\n", predPop);

    // Regresi polinomial percent
    printf("\n[Regresi polinomial percent derajat 3]\n");
    N = 0;
    for (int i = 0; i < table->n; i++) {
        if (table->percent[i] != -9999) {
            x_data[N] = table->year[i];
            y_data[N] = table->percent[i];
            N++;
        }
    }
    double coeff2[4];
    polynomial_regression(3, N, x_data, y_data, coeff2);
    printf("Persamaan percent: y = (%.4f) + (%.4f)x + (%.4f)x^2 + (%.4f)x^3\n", coeff2[0], coeff2[1], coeff2[2], coeff2[3]);

    double predPercent = coeff2[0] + coeff2[1] * 2035 + coeff2[2] * pow(2035, 2) + coeff2[3] * pow(2035, 3);
    printf("Estimasi percent tahun 2035: %.4f\n", predPercent);
}

int main() {
    DataTable table;
    const char* filename = "Data_Tugas_Pemrograman_A.csv";

    read_csv(filename, &table);
    print_data(&table);

    find_missing_years(&table);

    printf("\nTahun yang hilang:\n");
    for (int i = 0; i < table.missingCount; i++) {
        printf("%d\n", table.missingYear[i]);
    }

    analyze_data(&table);

    return 0;
}