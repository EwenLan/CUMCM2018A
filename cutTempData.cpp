#include "publicHead.hpp"

static float* Linear_Interpolation(float start, float end, int n)
{
    float dd = end - start;
    float cent = dd / n;
    float* result = new float[n];
    result[0] = start;
    for(int i = 1; i < n; ++i)
    {
        result[i] = result[i - 1] + cent;
    }
    return result;
}
int main()
{
    float raw_temp_data[RAW_DATA_ROWS] = {0};
    std::ifstream fin;
    fin.open("tempData.txt");
    for(int i = 0; i < RAW_DATA_ROWS; ++i)
    {
        // std::cout << i << std::endl;
        fin >> raw_temp_data[i];
    }
    fin.close();
    std::cout << "Stage 1: Done." << std::endl;
    float* intr_temp_data = new float[INTR_ROWS];
    for(int i = 0; i < RAW_DATA_ROWS - 1; ++i)
    {
        // std::cout << i << std::endl;
        float* short_result = Linear_Interpolation(raw_temp_data[i], raw_temp_data[i + 1], INSERT_PTS);
        for(int j = 0; j < INSERT_PTS; ++j)
        {
            intr_temp_data[i * INSERT_PTS + j] = short_result[j];
        }
    }

    int file_num = 0;
    if(INTR_ROWS % ROWS_PRE_FILE)
    {
        file_num = (RAW_DATA_ROWS - 1) * INSERT_PTS / ROWS_PRE_FILE + 1;
    }
    else
    {
        file_num = (RAW_DATA_ROWS - 1) * INSERT_PTS / ROWS_PRE_FILE;
    }
    int rest_rows = INTR_ROWS;
    for(int i = 0; i < file_num; ++i)
    {
        char filename[40] = {0};
        sprintf(filename, "cutted/tempData-%03d.bin", i);
        std::ofstream fout;
        fout.open(filename, std::ios::binary|std::ios::out);
        unsigned char float_uchar[4] = {0};
        for(int j = 0; j < (rest_rows < ROWS_PRE_FILE? rest_rows : ROWS_PRE_FILE); ++j)
        {
            memcpy(float_uchar, (unsigned char*)&intr_temp_data[i * ROWS_PRE_FILE + j], 4);
            fout.write((char *)float_uchar, 4);
        }
        fout.close();
        rest_rows -= ROWS_PRE_FILE;
    }
    return 0;
}
