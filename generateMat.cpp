#include "publicHead.hpp"

int main(int argc, char const *argv[])
{
    // float alpha_mat[COLUMN_NUM] = {0};
    // for(int i = 0; i < COLUMN_NUM; ++i)
    // {
    //     float alpha = 0;
    //     if(i >= LAYER_1_FIRST_INDEX && i < LAYER_2_FIRST_INDEX)
    //         alpha = LAYER_1_ALPHA;
    //     if(i >= LAYER_2_FIRST_INDEX && i < LAYER_3_FIRST_INDEX)
    //         alpha = LAYER_2_ALPHA;
    //     if(i >= LAYER_3_FIRST_INDEX && i < LAYER_4_FIRST_INDEX)
    //         alpha = LAYER_3_ALPHA;
    //     if(i >= LAYER_4_FIRST_INDEX)
    //         alpha = LAYER_4_ALPHA;
    //     alpha_mat[i] = alpha;
    // }

    // std::cout << "A" << std::endl;
    int rest_rows = INTR_ROWS;
    for(int i = 0; i < CUTTED_FILE_NUM; ++i)
    {
        char filename[40] = {0};
        // std::cout << "A" << std::endl;
        sprintf(filename, "cutted/tempData-%03d.bin", i);
        std::cout << filename << std::endl;
        std::ifstream fin;
        // std::cout << "A" << std::endl;
        fin.open(filename, std::ios::in|std::ios::binary);
        // float mat[ROWS_PRE_FILE][COLUMN_NUM] = {0};
        float** mat = new float*[ROWS_PRE_FILE];
        for(int i = 0; i < ROWS_PRE_FILE; ++i)
        {
            mat[i] = new float[COLUMN_NUM];
            memset(mat[i], 0, sizeof(float) * COLUMN_NUM);
        }
        // std::cout << "A" << std::endl;
        for(int j = 0; j < (rest_rows < ROWS_PRE_FILE ? rest_rows : ROWS_PRE_FILE); ++j)
        {
            char floatChar[4] = {0};
            fin.read(floatChar, 4);
            float floatTemp = 0;
            memcpy(&floatTemp, floatChar, 4);
            mat[j][0] = 75.0;
            mat[j][COLUMN_NUM - 1] = floatTemp;
            for(int k = 1; k < COLUMN_NUM - 1; ++k)
            {
                mat[j][k] = 37.0;
            }
        }
        fin.close();

        char output_filename[40] = {0};
        sprintf(output_filename, "mats/tempMat-%03d.bin", i);
        std::ofstream fout;
        fout.open(output_filename, std::ios::binary|std::ios::out);
        for(int j = 0; j < (rest_rows < ROWS_PRE_FILE ? rest_rows : ROWS_PRE_FILE); ++j)
        {
            for(int k = 0; k < COLUMN_NUM; ++k)
            {
                char floatChar[4] = {0};
                memcpy(floatChar, &mat[j][k], 4);
                fout.write(floatChar, 4);
            }
        }
        fout.close();
        for(int i = 0; i < ROWS_PRE_FILE; ++i)
        {
            // mat[i] = new float[COLUMN_NUM];
            // memset(mat[i], 0, sizeof(float) * COLUMN_NUM);
            delete[] mat[i];
        }
        delete[] mat;
        rest_rows -= ROWS_PRE_FILE;
    }
    return 0;
}
