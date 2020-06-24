
/**
 * Copyright (C) 2016 ADVACAM
 * @author    Daniel Turecek <daniel.turecek@advacam.com>
 */
#include "pxcapi.h"
#include <cstdlib>
#include <cstring>
#include <algorithm>

#define SINGLE_CHIP_PIXSIZE 65536
#define ERRMSG_BUFF_SIZE    512

int printError(const char* errorPrefix)
{
    char errorMsg[ERRMSG_BUFF_SIZE];
    pxcGetLastError(errorMsg, ERRMSG_BUFF_SIZE);
    printf("%s: %s\n", errorPrefix, errorMsg);
    return -1;
}

int singleMeasurementTest(unsigned deviceIndex, double frameTime, const char* filePath)
{
    // Measure one frame and get its data
    unsigned short frameData[SINGLE_CHIP_PIXSIZE];
    unsigned frameSize = SINGLE_CHIP_PIXSIZE;
    int rc = pxcMeasureSingleFrame(deviceIndex, frameTime, frameData, &frameSize, PXC_TRG_NO);
    if (rc)
        return printError("Could not measure frame");

    unsigned width = 0, height = 0;
    pxcGetDeviceDimensions(0, &width, &height);
    printf("Dimensions: %u, %u\n", width, height);

    char buff[1024];
    memset(buff, 0, 1024);
    unsigned size = 1024;
    pxcGetMetaDataValue(0, 0, "Start time", buff, &size);
    printf("Value: %s\n", buff);

    pxcSaveMeasuredFrame(0, 0, filePath);
    return 0;
}

int main (int argc, char const* argv[])
{
    double threshold = 1.0;
    double frameTime = 0.1;
    char filePath[512] = "test.pfm";

    sscanf(argv[1],"%lf", &threshold);
    printf("threshold = %.3f\n", threshold);

    sscanf(argv[2],"%lf", &frameTime);
    printf("frameTime = %.3f\n", frameTime);

    strcpy(filePath, argv[3]); 
    printf("filePath = %s\n", filePath);
    
    // Initialize Pixet
    int rc = pxcInitialize();
    if (rc)
        return printError("Could not initialize Pixet");


    int connectedDevicesCount = pxcGetDevicesCount();
    printf("Connected devices: %d\n", connectedDevicesCount);

    if (connectedDevicesCount == 0)
        return pxcExit();

    char name[1000];
    pxcGetDeviceName(0, name, 1000);
    printf("Connected Device: %s\n", name);
    
    rc = pxcSetThreshold(0, 0, threshold);
    if (rc)
        return printError("Could not set treshold");
    
    singleMeasurementTest(0, frameTime, filePath);

    // Exit Pixet
    return pxcExit();
}
