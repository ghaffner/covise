/* This file is part of COVISE.

   You can use it under the terms of the GNU Lesser General Public License
   version 2.1 or later, see lgpl-2.1.txt.

 * License: LGPL 2+ */

#include <stdio.h>
#include <iostream>
using namespace std;

int main(int argc, char **argv)
{
    int numTilesX = 0;
    int numTilesY = 0;
    int resX = 0;
    int resY = 0;
    int sizeX = 0;
    int sizeY = 0;
    int gapX = 0;
    int gapY = 0;
    int numSlaves;
    char hostnameBase[1000];
    char hostnameSuffix[100];
    // check arguments
    if ((argc != 11) && (argc != 1))
    {
        cerr << "Wrong argument number" << endl;
        cerr << "Usage: tiledDisplayConfig" << endl;
        cerr << "or: tiledDisplayConfig numTilesX numTilesY xres yres xsize ysize xgap ygap hostnameBase hostnameSuffix\n" << endl;
        return 1;
    }
    if (argc == 11) // command line mode
    {
        sscanf(argv[1], "%d", &numTilesX);
        sscanf(argv[2], "%d", &numTilesY);
        sscanf(argv[3], "%d", &resX);
        sscanf(argv[4], "%d", &resY);
        sscanf(argv[5], "%d", &sizeX);
        sscanf(argv[6], "%d", &sizeY);
        sscanf(argv[7], "%d", &gapX);
        sscanf(argv[8], "%d", &gapY);
        sscanf(argv[9], "%s", hostnameBase);
        sscanf(argv[10], "%s", hostnameSuffix);
    }
    if (argc == 1) // interactive mode
    {
        fprintf(stderr, "numTiles X: ");
        scanf("%d", &numTilesX);
        fprintf(stderr, "numTiles Y: ");
        scanf("%d", &numTilesY);
        fprintf(stderr, "X resolution in pixels: ");
        scanf("%d", &resX);
        fprintf(stderr, "Y resolution in pixels: ");
        scanf("%d", &resY);
        fprintf(stderr, "X size of one panel in mm: ");
        scanf("%d", &sizeX);
        fprintf(stderr, "Y size of one panel in mm: ");
        scanf("%d", &sizeY);
        fprintf(stderr, "X gap in mm: ");
        scanf("%d", &gapX);
        fprintf(stderr, "Y gap in mm: ");
        scanf("%d", &gapY);
        fprintf(stderr, "hostname with %%d where the number should be: ");
        scanf("%s", hostnameBase);
        fprintf(stderr, "hostname suffix: ");
        scanf("%s", hostnameSuffix);
    }
    numSlaves = ((numTilesX * numTilesY) + 1) / 2; // each host drives 2 tiles
    printf("<YAC>\n");
    printf("<!-- config generated by tiledDisplayConfig -->\n");
    printf("<LOCAL host=\"");
    int i;
    printf("%s,", hostnameBase);
    for (i = 0; i < numSlaves; i++)
    {
        if (i != 0)
            printf(",");
        printf("%s%d%s", hostnameBase, i + 1, hostnameSuffix);
    }
    printf("\">\n\n");

    // MultiPC:
    printf("  <MultiPC>\n");
    printf("    <SyncMode value=\"TCP\" />\n");
    printf("    <NumSlaves value=\"%d\" />\n", numSlaves);
    for (i = 0; i < numSlaves; ++i)
    {
        printf("    <Command value=\"ssh ${USER}@%s%d%s if \\( \\{ test -d `pwd` \\} \\) cd `pwd` \\;cover \\>\\&amp; /tmp/error.log\" name=\"%d\" />\n", hostnameBase, i + 1, hostnameSuffix, i);
    }
    for (i = 0; i < numSlaves; ++i)
    {
        printf("    <Startup value=\"ssh ${USER}@%s%d%s if \\( \\{ test -d `pwd` \\} \\) cd `pwd` \\;opencover \\>\\&amp; /tmp/errors.log\" name=\"%d\" />\n", hostnameBase, i + 1, hostnameSuffix, i);
    }
    printf("  </MultiPC>\n\n");

    // window and channel config
    printf("  <COVERConfig>\n");
    printf("    <NumPipes value=\"1\" />\n");
    printf("    <NumWindows value=\"1\" />\n");
    printf("    <NumScreens value=\"2\" />\n");
    printf("  </COVERConfig>\n");

    printf("\n");
    printf("<!-- one window, two screens -->\n");
    printf("  <WindowConfig>\n");
    printf("    <Window window=\"0\" value=\"MAIN 0 0 0 %d %d\" name=\"0\" />\n", resX * 2, resY);
    printf("  </WindowConfig>\n");
    printf("\n");
    printf("<!-- two channels in Window 0 one left and one right -->\n");
    printf("  <ChannelConfig>\n");
    printf("    <Channel channel=\"0\" value=\"C_A 0 0 0 %d %d\" stereoMode=\"MONO\" name=\"0\" />\n", resX, resY);
    printf("    <Channel channel=\"1\" value=\"C_B 0 %d 0 %d %d\" stereoMode=\"MONO\" name=\"1\" />\n", resX, resX, resY);
    printf("  </ChannelConfig>\n");
    printf("\n");
    printf("</LOCAL>\n\n");

    // Create ScreenConfig entries:
    // First an entry for master node:
    printf("<LOCAL host=\"%s\">\n", hostnameBase);
    printf("  <ScreenConfig>\n");
    printf("    <Screen value=\"S_A %d %d 0 0 0 0.0 0.0 0.0\" name=\"0\" screen=\"0\" />\n", sizeX, sizeY);
    printf("    <Screen value=\"S_B %d %d 0 0 0 0.0 0.0 0.0\" name=\"1\" screen=\"1\" />\n", sizeX, sizeY);
    printf("  </ScreenConfig>\n");
    printf("</LOCAL>\n\n");
    i = 0;
    int x, y;
    int w, h; // width and height of entire wall [mm]
    int xPos;
    int yPos;
    w = numTilesX * (gapX + sizeX) - gapX;
    h = numTilesY * (gapY + sizeY) - gapY;
    for (x = 0; x < numTilesX; x += 2)
    {
        for (y = 0; y < numTilesY; y++)
        {
            printf("<LOCAL host=\"");
            printf("%s%d%s", hostnameBase, i + 1, hostnameSuffix);
            printf("\">\n");
            printf("  <ScreenConfig>\n");
            xPos = (x * (sizeX + gapX)) - (w / 2);
            yPos = -((y * (sizeY + gapY)) - (h / 2));
            printf("    <Screen value=\"S_A %d %d %d 0 %d 0.0 0.0 0.0\" name=\"0\" screen=\"0\" />\n", sizeX, sizeY, xPos, yPos);
            xPos += gapX + sizeX;
            printf("    <Screen value=\"S_B %d %d %d 0 %d 0.0 0.0 0.0\" name=\"1\" screen=\"1\" />\n", sizeX, sizeY, xPos, yPos);
            printf("  </ScreenConfig>\n");
            printf("</LOCAL>\n\n");
            i++;
        }
    }
    printf("</YAC>\n");
}
