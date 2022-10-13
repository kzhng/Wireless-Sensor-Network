#include <stdio.h>
#include <stdlib.h>

int main() {
        FILE *fp;
        fp = fopen("report_log", "w");

        fprintf(fp, "---------------------------------------------------------------------------------------------------------\n");
        fprintf(fp, "Iteration: \n");
        fprintf(fp, "Logged time: \n");
        fprintf(fp, "Alert reported time: \n");
        fprintf(fp, "Alert type: \n");
        fprintf(fp, "\nReporting Node          Seismic Coord                        Magnitude                   IPv4\n");
        fprintf(fp, "blah blah\n");
        fprintf(fp, "\nAdjacent Nodes          Seismic Coord     Diff(Coord,km)     Magnitude     Diff(Mag)     IPv4\n");
        for (int i=0; i<4;i++) {
            fprintf(fp, "blah blah blah\n");
        }
        fprintf(fp, "\nBalloon seismic reporting time: \n");
        fprintf(fp, "Balloon seismic reporting Coord: \n");
        fprintf(fp, "Balloon seismic reporting Coord Diff with Reporting Node (km): \n");
        fprintf(fp, "Balloon seismic reporting Magnitude: \n");
        fprintf(fp, "Balloon seismic reporting Magnitude Diff with Reporting Node: \n");
        
        fprintf(fp, "\nCommunication time (seconds): \n");
        fprintf(fp, "Total messages sent between reporting node and base station: \n");
        fprintf(fp, "Number of adjacent matches to reporting node: \n");
        fprintf(fp, "Coordinate difference threshold (km): \n");
        fprintf(fp, "Magnitude difference threshold: \n");
        fprintf(fp, "Earthquake magnitude threshold: \n");
        fprintf(fp, "---------------------------------------------------------------------------------------------------------\n");
        fclose(fp);
    return 0;
}