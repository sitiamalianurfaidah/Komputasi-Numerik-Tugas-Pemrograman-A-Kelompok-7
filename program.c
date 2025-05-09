#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define MAX_ROWS 1000
#define MAX_COLS 3

// menyimpan data
typedef struct {
    int year[MAX_ROWS];
    double percent [MAX_ROWS];
    double population [MAX_ROWS];
    int n;    // jumlah data
    int missingYear [MAX_ROWS];
    int missingCount;
} DataTable;

// membaca file CSV
void read_csv(const char* filename, DataTable* table) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Tidak dapat membuka file %s\n", filename);
        exit(1);
    }

    char line[100];
    int row = 0;

    while (fgets(line, sizeof(line), file)) {
        // Karna tabel dimulai dari header, abaikan header
        if (row == 0 && strstr(line, "Year")) {
            continue;
        }
        // parsing CSV
        if (sscanf(line, "%d,%lf,%lf", &table->year[row], &table->percent[row], &table->population[row]) == 3) {
            row++;
        } 
        else {    // menangani data yang kosong
            table->year[row] = -9999; // placeholder untuk nilai kosong
            table->percent[row] = -9999;
            table->population[row] = -9999;
            row++;
        }
    }
    table->n = row;

    fclose(file);
}

// menampilkan data
void print_data(const DataTable* table) {
    printf("Year\tPercent\nPopulation\n");
    for (int i = 0; i < table->n; i++) {
        printf("%d\t%.4f\t%.4f\n", table->year[i], table->percent[i], table->population[i]);
    }
}

void find_missing_years(DataTable* table) {
    int minYear = (int)table->year[0];
    int maxYear = (int)table->year[table->n - 1];

    table->missingCount = 0;

    for (int y = minYear; y <= maxYear; y++) {
        int found = 0;
        for (int i = 0; i < table->n; i++) {
            if ((int)table->year[i] == y) {
                found = 1;
                break;
            }
        }
        if (!found) {
            table->missingYear [table->missingCount++] = y;
        }
    }
}

// // metode (?)
// void method_name(const DataTable* table) {
//     // metode apa ?
// }

// visualisasi data      (ini grafik sederhana (bgt) gitu..., ubah aja entar kalo mau yg lebih bagus)
void plot_data(const DataTable* table) {
    printf("\n[Visualisasi Data]\n");
    for (int i = 0; i < table->n; i++) {
        printf("%2d | ", i);
        int stars = (int)(table->year[i] / 2);
        for (int j = 0; j < stars; j++) {
            printf("*");
        }
        printf("\n");
    }
}

int main() {
    DataTable table;
    
    // membaca file CSV
    const char* filename = "Data_Tugas_Pemrograman_A.csv";   // entar ganti yeah nama file nya
    read_csv(filename, &table);

    // menampilkan data awal
    print_data(&table);
    find_missing_years(&table);

    printf("\nTahun yang hilang:\n");
    for (int i = 0; i < table.missingCount; i++) {
        printf("%d\n", table.missingYear[i]);
    }

    // melakukan metode
    // method_name(&table);

    // visualisasi hasil
    plot_data(&table);

    return 0;
}