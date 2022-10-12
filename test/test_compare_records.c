#include "../sensor.h"
#include <stdlib.h>

int main() {
    Record record1 = {2022, 10, 12, 21, 58, 10, -3.1554, 120.2245, 4.509, -5.03, 1};
    printf("record1: ");
    PrintRecord(&record1);
    Record record2 = {2022, 10, 12, 21, 58, 10, -4.1554, 122.2245, 5.69, -3.03, 2};
    printf("record2: ");
    PrintRecord(&record2);

    float abs_distance, delta_mag, delta_dep;
    int rank = 1;
    int res;
    res = CompareRecords(&record1, &record2, &rank, &abs_distance, &delta_mag, &delta_dep);

    return EXIT_SUCCESS;
}