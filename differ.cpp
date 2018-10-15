#include "publicHead.hpp"

void threadDiffer(float** mat, int row, int startIndex, int length, float* alpha_mat)
{
    for(int i = startIndex; i < startIndex + length; ++i)
    {
        mat[row + 1][i] = alpha_mat[i]*(mat[row][i + 1] - 2*mat[row][i] + mat[row][i - 1])*RDTHICKNESS*RDTHICKNESS + mat[row][i];
    }
}

int main(int argc, char const *argv[])
{
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
    int rest_rows = INTR_ROWS;
    for(int i = 0; i < CUTTED_FILE_NUM; ++i)
    {
        char filename[40] = {0};
        sprintf(filename, "mats/tempMat-%03d.bin", i);
        std::ifstream fin;
        fin.open(filename, std::ios::binary | std::ios::in);
        float** mat = new float*[ROWS_PRE_FILE + 1];
        for(int j = 0; j < ROWS_PRE_FILE + 1; ++j)
        {
            mat[j] = new float[COLUMN_NUM];
            memset(mat[j], 0, sizeof(float) * COLUMN_NUM);
        }
        for(int j = 0; j < (rest_rows < ROWS_PRE_FILE ? rest_rows : ROWS_PRE_FILE); ++j)
        {
            for(int k = 0; k < COLUMN_NUM; ++k)
            {
                float floatTemp = 0;
                unsigned char floatChar[4] = {0};
                fin.read((char*)floatChar, 4);
                memcpy(&floatTemp, floatChar, 4);
                mat[j][i] = floatTemp;
            }
        }
        fin.close();
        if(i)
        {
            std::ifstream f2in;
            f2in.open("gap.bin", std::ios::binary | std::ios::in);
            for(int j = 0; j < COLUMN_NUM; ++j)
            {
                char floatChar[4] = {0};
                f2in.read(floatChar, 4);
                float floatTemp = 0;
                memcpy(&floatTemp, floatChar, 4);
                mat[0][j] = floatTemp;
            }
            f2in.close();
        }
        std::cout << filename << " mat loaded. " << std::endl;
        std::cout << "Ready to calculate. " << std::endl;
        
        for(int j = 0; j < (rest_rows < ROWS_PRE_FILE ? rest_rows : ROWS_PRE_FILE); ++j)
        {
            std::vector<std::thread> ts;
            ts.push_back(std::thread(threadDiffer, mat, j, THREAD_0_START_INDEX, THREAD_0_LENGTH, alpha_mat));
            ts.push_back(std::thread(threadDiffer, mat, j, THREAD_1_START_INDEX, THREAD_1_LENGTH, alpha_mat));
            ts.push_back(std::thread(threadDiffer, mat, j, THREAD_2_START_INDEX, THREAD_2_LENGTH, alpha_mat));
            ts.push_back(std::thread(threadDiffer, mat, j, THREAD_3_START_INDEX, THREAD_3_LENGTH, alpha_mat));
            ts.push_back(std::thread(threadDiffer, mat, j, THREAD_4_START_INDEX, THREAD_4_LENGTH, alpha_mat));
            ts.push_back(std::thread(threadDiffer, mat, j, THREAD_5_START_INDEX, THREAD_5_LENGTH, alpha_mat));
            for(auto& i : ts)
            {
                i.join();
            }
        }
        float gap[COLUMN_NUM] = {0};
        for(int j = 0; j < COLUMN_NUM; ++j)
        {
            gap[j] = mat[rest_rows < ROWS_PRE_FILE ? rest_rows : ROWS_PRE_FILE][j];
        }
        std::ofstream fout;
        fout.open("gap.bin", std::ios::binary | std::ios::out);
        for(int j = 0; j < COLUMN_NUM; ++j)
        {
            char floatChar[4] = {0};
            memcpy(floatChar, &gap[j], 4);
            fout.write(floatChar, 4);
        }
        fout.close();
        std::ofstream f2out;
        char f2out_filename[40] = {0};
        sprintf(f2out_filename, "ans/sepMat-%03d.bin", i);
        f2out.open(f2out_filename, std::ios::binary | std::ios::out);
        for(int j = 0; j < ROWS_PRE_FILE; j += INSERT_PTS)
        {
            for(int k = 0; k < COLUMN_NUM; ++k)
            {
                char floatChar[4] = {0};
                memcpy(floatChar, &mat[j][k], 4);
                f2out.write(floatChar, 4);
            }
        } 
        f2out.close();
        std::cout << "Export data " << f2out_filename << " done." << std::endl;
        std::cout << "Releasing memory..." << std::endl;
        for(int j = 0; j < ROWS_PRE_FILE + 1; ++j)
        {
            // mat[j] = new float[COLUMN_NUM];
            // memset(mat[j], 0, sizeof(float) * COLUMN_NUM);
            delete[] mat[j];
        }
        delete[] mat;
        rest_rows -= ROWS_PRE_FILE;
    }
    return 0;
}