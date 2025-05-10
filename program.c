#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_ROWS 1000

// Struktur buat nyimpen data
typedef struct {
    int year[MAX_ROWS];
    double percent[MAX_ROWS];     
    double population[MAX_ROWS];  
    int n;                       
    int missingYear[10];          
    int missingCount;
} DataTable;

// Baca file CSV
void read_csv(const char* filename, DataTable* table) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Gagal buka file %s. Pastikan file-nya ada ya!\n", filename);
        exit(1);
    }

    char line[200];
    int row = 0;
    fgets(line, sizeof(line), file); // skip header

    while (fgets(line, sizeof(line), file)) {
        int year;
        double percent, population;
        if (sscanf(line, "%d,%lf,%lf", &year, &percent, &population) == 3) {
            table->year[row] = year;
            table->percent[row] = percent;
            table->population[row] = population;
            row++;
        }
    }

    table->n = row;
    fclose(file);
}

// Cari tahun-tahun yang datanya hilang
void find_missing_years(DataTable* table, const int* knownMissing, int knownCount) {
    table->missingCount = 0;
    for (int i = 0; i < knownCount; i++) {
        int found = 0;
        for (int j = 0; j < table->n; j++) {
            if (table->year[j] == knownMissing[i]) {
                found = 1;
                break;
            }
        }
        if (!found) {
            table->missingYear[table->missingCount++] = knownMissing[i];
        }
    }
}

// Regresi polinomial
void polynomial_regression(const double* x, const double* y, int n, int degree, double* coeffs) {
    double X[20][20] = {0};
    double Y[20] = {0};

    for (int i = 0; i <= degree; i++) {
        for (int j = 0; j <= degree; j++) {
            for (int k = 0; k < n; k++)
                X[i][j] += pow(x[k], i + j);
        }
        for (int k = 0; k < n; k++)
            Y[i] += y[k] * pow(x[k], i);
    }

    for (int i = 0; i <= degree; i++) {
        for (int j = i + 1; j <= degree; j++) {
            double ratio = X[j][i] / X[i][i];
            for (int k = 0; k <= degree; k++)
                X[j][k] -= ratio * X[i][k];
            Y[j] -= ratio * Y[i];
        }
    }

    for (int i = degree; i >= 0; i--) {
        coeffs[i] = Y[i];
        for (int j = i + 1; j <= degree; j++)
            coeffs[i] -= X[i][j] * coeffs[j];
        coeffs[i] /= X[i][i];
    }
}

double evaluate_polynomial(double* coeffs, int degree, double x) {
    double result = 0;
    for (int i = 0; i <= degree; i++) {
        result += coeffs[i] * pow(x, i);
    }
    return result;
}

double clamp(double val, double min, double max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

void print_polynomial(double* coeffs, int degree) {
    printf("y = ");
    for (int i = degree; i >= 0; i--) {
        printf("%+.4lfx^%d ", coeffs[i], i);
    }
    printf("\n");
}

int main() {
    DataTable table;
    const char* filename = "Data_Tugas_Pemrograman_A.csv";

    read_csv(filename, &table);

    int knownMissing[] = {2005, 2006, 2015, 2016};
    find_missing_years(&table, knownMissing, 4);

    double x[MAX_ROWS], y_percent[MAX_ROWS], y_pop[MAX_ROWS];
    int count = 0;
    for (int i = 0; i < table.n; i++) {
        x[count] = table.year[i];
        y_percent[count] = table.percent[i] / 100.0; // Normalisasi ke 0–1
        y_pop[count] = table.population[i];
        count++;
    }

    double coeff_percent[10], coeff_pop[10];

    polynomial_regression(x, y_percent, count, 3, coeff_percent);
    polynomial_regression(x, y_pop, count, 2, coeff_pop);

    printf("\n[ANALISIS DATA]\n");
    if (table.missingCount == 0) {
        printf("Tidak ada tahun yang hilang terdeteksi.\n");
    } else {
        for (int i = 0; i < table.missingCount; i++) {
            int year = table.missingYear[i];
            double estPercent = clamp(evaluate_polynomial(coeff_percent, 3, year), 0, 1) * 100.0;
            double estPop = evaluate_polynomial(coeff_pop, 2, year);
            printf("Estimasi percent tahun %d: %.4f\n", year, estPercent);
            printf("Estimasi populasi tahun %d: %.4f\n", year, estPop);
        }
    }

    printf("\n[Regresi polinomial percent derajat 3]\n");
    print_polynomial(coeff_percent, 3);
    double est2035 = clamp(evaluate_polynomial(coeff_percent, 3, 2035), 0, 1) * 100.0;
    printf("Estimasi percent tahun 2035: %.4f\n", est2035);

    printf("\n[Regresi polinomial populasi derajat 2]\n");
    print_polynomial(coeff_pop, 2);
    printf("Estimasi populasi tahun 2030: %.4f\n", evaluate_polynomial(coeff_pop, 2, 2030));

    return 0;
}