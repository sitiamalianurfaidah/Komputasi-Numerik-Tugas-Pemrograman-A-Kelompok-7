#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROWS 1000
#define MAX_COLS 2

// menyimpan data
typedef struct {
    double dataA[MAX_ROWS];
    double dataB[MAX_ROWS];
    int n;    // jumlah data
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
        // parsing CSV
        if (sscanf(line, "%lf,%lf", &table->dataA[row], &table->dataB[row]) == 2) {
            row++;
        } 
        else {    // menangani data yang kosong
            table->dataA[row] = -9999; // placeholder untuk nilai kosong
            table->dataB[row] = -9999;
            row++;
        }
    }
    table->n = row;

    fclose(file);
}

// menampilkan data
void print_data(const DataTable* table) {
    printf("Data A\tData B\n");
    for (int i = 0; i < table->n; i++) {
        printf("%.2f\t%.2f\n", table->dataA[i], table->dataB[i]);
    }
}

// metode (?)
void method_name(const DataTable* table) {
    // metode apa ?
}

// visualisasi data      (ini grafik sederhana (bgt) gitu..., ubah aja entar kalo mau yg lebih bagus)
void plot_data(const DataTable* table) {
    printf("\n[Visualisasi Data]\n");
    for (int i = 0; i < table->n; i++) {
        printf("%2d | ", i);
        int stars = (int)(table->dataA[i] / 2);
        for (int j = 0; j < stars; j++) {
            printf("*");
        }
        printf("\n");
    }
}

int main() {
    DataTable table;
    
    // membaca file CSV
    const char* filename = "data.csv";   // entar ganti yeah nama file nya
    read_csv(filename, &table);

    // menampilkan data awal
    print_data(&table);

    // melakukan metode
    // method_name(&table);

    // visualisasi hasil
    plot_data(&table);

    return 0;
}