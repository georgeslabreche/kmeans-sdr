#include <iostream>
#include <vector>
#include <stdint.h>

#include <dkm.hpp>
#include <dkm_utils.hpp>


#define BUFFER_SIZE 419000

// --------------------------------------------------------------------------
// the main function

int main(int argc, char **argv)
{
    /* the return code */
    int rc = 0;

    /* the data is a 2-channel timeseries */
    /* in 32 bit interleaved float format [4-byte I value][4-byte Q value][I][Q][I][Q].. and so forth */

    
    /* the buffer that will contain the data */
    uint8_t buffer_8bit[BUFFER_SIZE];

    /* the file pointer */
    FILE *fp;
    fp = fopen("data/beacon/training/1645433807.48_raw_ulf406025000_dlf1544185000_gain65_inputsr1500000_outputsr37500_1.cf32", "rb");


    /* Read in 256 8-bit numbers into the buffer */
    size_t bytes_read = 0;
    bytes_read = fread(buffer_8bit, sizeof(uint8_t), BUFFER_SIZE, fp);

    for(int i = 0; i < BUFFER_SIZE; i++)
    {
        printf("%c ", ((char *)buffer_8bit)[i]);
    }

    fclose(fp);

    /* train with k=2 to see if unsupervised learning differenciates between beacon and no beacon */


    /* train with k=3 to see if unsupervised learning differenciates between beacon, no beacon, and unknown */





    /* end of program */
    return rc;
}