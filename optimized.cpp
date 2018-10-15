#include <iostream>
#include <fstream>
#include <thread>
#include <cstring>
#include <ctime>

#define RAW_DATA_ROWS 5401 
// Source data.
#define ROWS_PRE_FILE 20000000 
// Custom defined.
#define INSERT_PTS 100000 
// Custom defined.
#define INTR_ROWS 540000000 
// (RAW_DATA_ROWS - 1) * INSERT_PTS
#define COLUMN_NUM 154 
// Custom defined. 1 points / 0.1 mm
#define RDTHICKNESS 10000.0
// 1 m / 0.1 mm
#define CUTTED_FILE_NUM 27 
// cutTempData.cpp result
#define LAYER_1_ALPHA 1.9850e-7 
#define LAYER_2_ALPHA 2.0440e-7
#define LAYER_3_ALPHA 3.5137e-7
#define LAYER_4_ALPHA 2.3611e-5
#define LAYER_1_FIRST_INDEX 1
#define LAYER_1_LAST_INDEX 6
#define LAYER_2_FIRST_INDEX 7
#define LAYER_2_LAST_INDEX 66
#define LAYER_3_FIRST_INDEX 67
#define LAYER_3_LAST_INDEX 102
#define LAYER_4_FIRST_INDEX 103
#define LAYER_4_LAST_INDEX 152
#define THREAD_0_START_INDEX 1
#define THREAD_1_START_INDEX 26
#define THREAD_2_START_INDEX 51
#define THREAD_3_START_INDEX 76
#define THREAD_4_START_INDEX 101
#define THREAD_5_START_INDEX 127
#define THREAD_0_LENGTH 25
#define THREAD_1_LENGTH 25
#define THREAD_2_LENGTH 25
#define THREAD_3_LENGTH 25
#define THREAD_4_LENGTH 26
#define THREAD_5_LENGTH 26

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
void threadDiffer(float** mat, int row, int startIndex, int length, float* alpha_mat)
{
    for(int i = startIndex; i < startIndex + length; ++i)
    {
        // mat[row + 1][i] = alpha_mat[i]*(mat[row][i + 1] - 2*mat[row][i] + mat[row][i - 1])*RDTHICKNESS*RDTHICKNESS/INSERT_PTS + mat[row][i];
        mat[row + 1][i] = alpha_mat[i]*(mat[row][i + 1] - 2*mat[row][i] + mat[row][i - 1])*1e4 + mat[row][i];
    }
}
void Display_Time()
{
    std::time_t curr_time = std::time(nullptr);
    std::cout  << std::asctime(std::localtime(&curr_time));
}
int main(int argc, char const *argv[])
{
    float** final_result = new float*[RAW_DATA_ROWS + 1];
    for(int i = 0; i < RAW_DATA_ROWS + 1; ++i)
    {
        final_result[i] = new float[COLUMN_NUM];
    }
    float alpha_mat[COLUMN_NUM] = {0};
    for(int i = 0; i < COLUMN_NUM; ++i)
    {
        float alpha = 0;
        if(i >= LAYER_1_FIRST_INDEX && i < LAYER_2_FIRST_INDEX)
            alpha = LAYER_1_ALPHA;
        if(i >= LAYER_2_FIRST_INDEX && i < LAYER_3_FIRST_INDEX)
            alpha = LAYER_2_ALPHA;
        if(i >= LAYER_3_FIRST_INDEX && i < LAYER_4_FIRST_INDEX)
            alpha = LAYER_3_ALPHA;
        if(i >= LAYER_4_FIRST_INDEX)
            alpha = LAYER_4_ALPHA;
        alpha_mat[i] = alpha;
    }
    float raw_temp_data[RAW_DATA_ROWS] = {0};
    float *gap = new float[COLUMN_NUM];
    std::ifstream fin;
    fin.open("tempData.txt");
    for(int i = 0; i < RAW_DATA_ROWS; ++i)
    {
        fin >> raw_temp_data[i];
    }
    fin.close();
    std::cout << "Loaded raw temp data." << std::endl;
    for(int i = 0; i < RAW_DATA_ROWS - 1; ++i)
    {
        Display_Time();
        std::cout << std::endl << "Current Time: " << i << " Sec." << std::endl;
        std::cout << "Loading edge conditions..." << std::endl;
        float** workbench = new float*[INSERT_PTS + 1];
        for(int j = 0; j < INSERT_PTS + 1; ++j)
        {
            workbench[j] = new float[COLUMN_NUM];
        }
        float* intr_result = Linear_Interpolation(raw_temp_data[i], raw_temp_data[i + 1], INSERT_PTS);
        for(int j = 0; j < INSERT_PTS; ++j)
        {
            workbench[j][COLUMN_NUM - 1] = intr_result[j];
            workbench[j][0] = 75.0;
        }
        delete[] intr_result;
        if(i)
        {
            for(int j = 1; j < COLUMN_NUM - 1; ++j)
            {
                workbench[0][j] = gap[j];
            }
        }
        else
        {
            for(int j = 1; j < COLUMN_NUM - 1; ++j)
            {
                workbench[0][j] = 37.0;
            }
            for(int j = 0; j < COLUMN_NUM; ++j)
            {
                final_result[i][j] = workbench[i][j];
            }
        }
        std::cout << "Edge conditions ready. " << std::endl;
        std::cout << "Start to calculate differential. " << std::endl;
        std::thread ts[6];
        for(int k = 0; k < INSERT_PTS; ++k)
        {
            ts[0] = std::thread(threadDiffer, workbench, k, THREAD_0_START_INDEX, THREAD_0_LENGTH, alpha_mat);
            ts[1] = std::thread(threadDiffer, workbench, k, THREAD_1_START_INDEX, THREAD_1_LENGTH, alpha_mat);
            ts[2] = std::thread(threadDiffer, workbench, k, THREAD_2_START_INDEX, THREAD_2_LENGTH, alpha_mat);
            ts[3] = std::thread(threadDiffer, workbench, k, THREAD_3_START_INDEX, THREAD_3_LENGTH, alpha_mat);
            ts[4] = std::thread(threadDiffer, workbench, k, THREAD_4_START_INDEX, THREAD_4_LENGTH, alpha_mat);
            ts[5] = std::thread(threadDiffer, workbench, k, THREAD_5_START_INDEX, THREAD_5_LENGTH, alpha_mat);
            for(int j = 0; j < 6; ++j)
            {
                ts[j].join();
            }
            // for(int l = 0; l < COLUMN_NUM; ++l)
            // {
            //     printf("%6f ", workbench[k + 1][l]);
            // }
            // std::cout << std::endl;
        }
        for(int j = 0; j < COLUMN_NUM; ++j)
        {
            gap[j] = workbench[INSERT_PTS][j];
            final_result[i + 1][j] = workbench[INSERT_PTS - 1][j];
            std::cout << final_result[i + 1][j] << " ";
        }
        std::cout << std::endl;

        std::cout << "Differential calculation done." << std::endl;
        std::cout << "Releasing memeory..." << std::endl;
        for(int j = 0; j < INSERT_PTS; ++j)
        {
            delete[] workbench[j];
        }
        delete[] workbench;
    }
    delete[] gap;
    Display_Time();
    std::cout << "All calculation done." << std::endl;
    std::cout << "Exporting calculation result..." << std::endl;
    std::ofstream fout;
    fout.open("finalResult.bin", std::ios::binary | std::ios::out);
    for(int i = 0; i < RAW_DATA_ROWS; ++i)
    {
        for(int j = 0; j < COLUMN_NUM; ++j)
        {
            char floatChar[4] = {0};
            memcpy(floatChar, &final_result[i][j], 4);
            fout.write(floatChar, 4);
        }
    }
    fout.close();
    Display_Time();
    std::cout << "All tasks done." << std::endl;
    return 0;
}
