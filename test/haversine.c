// Online C compiler to run C program online
#include <math.h>
#include <stdio.h>
double deg2rad(double deg);

int main() {
    // Write C code here

    // Haversine formula
    long double radius = 6371;

    long double lat1 = -37.8136;
    long double lon1 = 144.9631;
    long double lat2 = -33.8688;
    long double lon2 = 151.2093;
    
    lat1 = deg2rad(lat1);
    lon1 = deg2rad(lon1);
    lat2 = deg2rad(lat2);
    lon2 = deg2rad(lon2);
    
    long double diff_lat = lat2 - lat1;
    long double diff_lon = lon2 - lon1;
    long double dist = 0;
    dist = pow(sin(diff_lat / 2), 2) +
           (cos(lat1) * cos(lat2) * pow(sin(diff_lon / 2), 2));
           printf("dist: %Lf", dist);
    dist = 2 * asin(sqrt(dist)); 
    dist = radius * dist;
    printf("dist: %Lf", dist);
    return 0;
}

double deg2rad(double deg) {
  return (deg * M_PI / 180);
}
