#include <stdio.h>

void printArray (int Array[], int size) {
    int i;
    for (i = 0; i < size; i++)
        printf("%d ", Array[i]);
    printf("\n");
}

void merge (int Array[], int low, int middle, int high) {
    int i, j, k;
    int n1 = middle - low + 1;
    int n2 = high - middle;
    int left[n1], right[n2];
    for (i = 0; i < n1; i++)
        left[i] = Array[low + i];
    for (j = 0; j < n2; j++)
        right[j] = Array[middle + 1 + j];
    i = 0;
    j = 0;
    k = low;
    while (i < n1 && j < n2) {
        if (left[i] <= right[j]) {
            Array[k] = left[i];
            i++;
        } else {
            Array[k] = right[j];
            j++;
        }
        k++;
    }
    while (i < n1) {
        Array[k] = left[i];
        i++;
        k++;
    }
    while (j < n2) {
        Array[k] = right[j];
        j++;
        k++;
    }
}

void mergeSort (int Array[], int low, int high) {
    if (low < high) {
        int middle = (low + high) / 2;
        mergeSort(Array, low, middle);
        mergeSort(Array, middle + 1, high);
        merge(Array, low, middle, high);
    }
}

int main(){
    int mark[5] = {19, 10, 8, 17, 9};
    printArray(mark, 5);
    mergeSort(mark, 0, 5);
    printArray(mark, 5);
    return 0;
}