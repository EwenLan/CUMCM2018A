#include <iostream>
#include <fstream>
#include <cstring>

int main(int argc, char const *argv[])
{
    std::ifstream fin;
    fin.open("finalResult.bin", std::ios::binary|std::ios::in);
    float** fr = new float*[5401];
    for(int i = 0; i < 5401; ++i)
    {
        fr[i] = new float[154];
    }
    for(int i = 0; i < 5401; ++i)
    {
        for(int j = 0; j < 154; ++j)
        {
            char floatChar[4] = {0};
            fin.read(floatChar, 4);
            memcpy(&fr[i][j], floatChar, 4);
        }
    }
    fin.close();
    std::ofstream fout;
    fout.open("finalResult.csv", std::ios::out);
    for(int i =0; i < 5401; ++i)
    {
        for(int j = 0; j < 154; ++j)
        {
            fout << fr[i][j] << ", ";
        }
        fout << "\n";
    }
    fout.close();
    return 0;
}
