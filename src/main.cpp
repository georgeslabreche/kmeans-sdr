
#include <stdint.h>
#include <dirent.h>
#include <stdlib.h>     /* exit, EXIT_FAILURE */

#include <string>
#include <iostream>
#include <vector>

#include <dkm.hpp>
#include <dkm_utils.hpp>


#define BUFFER_SIZE_8BIT        419000
#define BUFFER_SIZE_32BIT       BUFFER_SIZE_8BIT / sizeof(uint32_t)

uint32_t byteswap_uint32(uint32_t val)
{
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF ); 
    return (val << 16) | (val >> 16);
}


int write_centroids(std::tuple<std::vector<std::array<float, BUFFER_SIZE_32BIT>>, std::vector<uint32_t>> *cluster_data, const char* csv_filepath)
{
    try
    {
        /* Create a new CSV file and write centroid rows to it */
        std::ofstream csv_file(csv_filepath);

        /* For each means vector */
        for (const auto& means : std::get<0>(*cluster_data))
        {
            /* Initialize the CSV data row */
            std::string csv_row("");

            /* Write all mean values in a CSV row */
            for(float m : means)
            {
                csv_row.append(std::to_string(m));
                csv_row.append(",");
            }

            /* Write row to the CSV file */
            csv_row.append("\n");
            csv_file << csv_row;
        }

        /* Close CSV file */
        csv_file.close();
    }
    catch(...)
    {
        std::cout << "Error: an unknown error occured while writing the CSV output file for the cluster centroids: " << csv_filepath << std::endl;
        return -1;
    }

    return 0;
}

// --------------------------------------------------------------------------
// the main function
// the data is a 2-channel timeseries
// in 32 bit interleaved float format [4-byte I value][4-byte Q value][I][Q][I][Q].. and so forth 

int main(int argc, char **argv)
{
    /* the return code */
    int rc = 0;

    /* the k value */
    int k = 2;

    /* training data directory path */
    char training_data_dir[] = "data/training/k2/";

    /* test data directory path */
    char test_data_dir[] = "data/test/k2/";
    
    /* the buffer that will contain the data */
    uint32_t buffer_32bit[BUFFER_SIZE_32BIT];

    /* the cf32 file pointer */
    FILE *fp;

    /* the file path */
    std::string filepath;

    /* size of the cf32 file that is read */
    size_t size_read = 0;


    /*******************************************************
     * collect the training data
     *******************************************************/

    /* declare the vector that will contain the training data */
    std::vector<std::array<float, BUFFER_SIZE_32BIT>> training_data_vector;

    DIR *dir;
    struct dirent *ent;
    if((dir = opendir(training_data_dir)) != NULL)
    {
        /* loop through the training data files within directory */
        while((ent = readdir (dir)) != NULL) 
        {
            if(strstr(ent->d_name, ".cf32"))
            {
                /* build the filepath string and push it into the fector */
                filepath = std::string(training_data_dir) + std::string(ent->d_name);
                //printf("%s\n", filepath.c_str());

                /* open the file pointer */
                fp = fopen(filepath.c_str(), "r");

                /* read cf32 data into the buffer */
                size_read = fread(buffer_32bit, sizeof(uint32_t), BUFFER_SIZE_32BIT, fp);

                /* check for errors */
                if(size_read != BUFFER_SIZE_32BIT)
                {
                    /* close the file pointer */
                    fclose(fp);

                    /* exit program with failure */
                    printf("error reading training data file %zu != %zu\n", size_read, BUFFER_SIZE_32BIT);
                    exit(EXIT_FAILURE);
                }

                /* the array that will contain cf32 data */
                std::array<float, BUFFER_SIZE_32BIT> cf32_data_array;

                /* build the cf32 data array */
                for(size_t j = 0; j < BUFFER_SIZE_32BIT; j++)
                {
                    /* populate cf32 data array with normalized data */
                    cf32_data_array.at(j) = ((int)byteswap_uint32(buffer_32bit[j])) / 4294967295.0;

#if 0
                    printf("%08x ", cf32_data_array.at(j));

                    if(j == 5)
                    {
                        printf("\n");
                        break;
                    }
#endif
                }

                /* push the cf32 data array into the training data vector */
                training_data_vector.push_back(cf32_data_array);

                /* close the file pointer */
                fclose(fp);
            }
        }
        closedir (dir);
    } 
    else
    {
        printf("error opening training data directory\n");
        exit(EXIT_FAILURE);
    }


    /*******************************************************
     * train the cluster data and write the centroids file
     *******************************************************/

    /* this will contain the cluster data */
    std::tuple<std::vector<std::array<float, BUFFER_SIZE_32BIT>>, std::vector<uint32_t>> cluster_data;

    /* use K-Means Lloyd algorithm to build clusters */ 
    cluster_data = dkm::kmeans_lloyd<float, BUFFER_SIZE_32BIT>(training_data_vector, k);

    /* write centroids to csv file */
    rc = write_centroids(&cluster_data, "data/centroids_k2.csv");



    /*******************************************************
     * predict against the test data set
     *******************************************************/

    /* get the centroids vector */
    std::vector<std::array<float, BUFFER_SIZE_32BIT>> centroids_vector = std::get<0>(cluster_data);

    /* Read the cluster centroids CSV file */
    //std::vector<std::array<float, KMEANS_IMAGE_SIZE>> clusterCentroidsVector;
    //clusterCentroidsVector = dkm::load_csv<float, BUFFER_SIZE_32BIT>("data/centroids_k2.csv");

    DIR *dir_test;
    struct dirent *ent_test;
    if((dir_test = opendir(test_data_dir)) != NULL)
    {
        /* loop through the test data files within directory */
        while((ent_test = readdir(dir_test)) != NULL) 
        {
            if(strstr(ent_test->d_name, ".cf32"))
            {
                /* build the filepath string */
                filepath = std::string(test_data_dir) + std::string(ent_test->d_name);
                //printf("%s\n", filepath.c_str());

                /* open the file pointer */
                fp = fopen(filepath.c_str(), "r");

                /* read cf32 data into the buffer */
                size_read = fread(buffer_32bit, sizeof(uint32_t), BUFFER_SIZE_32BIT, fp);

                /* check for errors */
                if(size_read != BUFFER_SIZE_32BIT)
                {
                    /* close the file pointer */
                    fclose(fp);

                    /* exit program with failure */
                    printf("error reading test data file %zu != %zu\n", size_read, BUFFER_SIZE_32BIT);
                    exit(EXIT_FAILURE);
                }

                /* the array that will contain the cf32 test data */
                std::array<float, BUFFER_SIZE_32BIT> cf32_test_data_array;

                /* build the cf32 data array */
                for(size_t j = 0; j < BUFFER_SIZE_32BIT; j++)
                {
                    /* populate cf32 test data array with normalized data */
                    cf32_test_data_array.at(j) = ((int)byteswap_uint32(buffer_32bit[j])) / 4294967295.0;
                }

                /* predict the label (i.e. if there is beacon or not) */
                size_t predict_label = dkm::predict(centroids_vector, cf32_test_data_array);

                /* print the prediction result */
                printf("%s,%zu\n", ent_test->d_name, predict_label);

                /* close the file pointer */
                fclose(fp);

            }
        }
    }



    /* end of program */
    return rc;
}