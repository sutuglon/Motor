//
//  IO.h
//  GLSL Demo
//
//  Created by Chema on 3/30/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


// IO
#include <fstream>
#include <vector>

namespace InputOutput {
    template<typename T>
    void write(std::ofstream& fout, const std::vector<T>& vec) {
        int size = vec.size();
        fout.write(reinterpret_cast<const char*>(&size), sizeof(int));
        fout.write(reinterpret_cast<const char*>(&vec[0]), size*sizeof(T));
    }
    template<typename T>
    void read(std::ifstream& fin, std::vector<T>& vec) {
        vec.clear();
        int size = 0;
        fin.read(reinterpret_cast<char*>(&size), sizeof(int));
        vec.resize(size);
        fin.read(reinterpret_cast<char*>(&vec[0]), size*sizeof(T));
    }
}