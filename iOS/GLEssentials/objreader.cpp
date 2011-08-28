/*
 *  objreader.cpp
 *  GLSL Demo
 *
 *  Created by Chema on 12/14/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <fstream>

#include "stringaux.h"
#include "objreader.h"


ObjReader::Data ObjReader::read (const char* filename) {

    
    ObjReader::Data data;
	using std::getline;
	using std::ifstream;
	using std::string;
	
	string		str, strFile;
	ifstream	ifs(filename);
	if ( ifs.is_open() )
	{
		while ( getline(ifs, str) )	{
            std::vector<std::string> tokens;
            stringaux::SplitString(str, tokens,' ');
            
            if (tokens.size()>0) {
                if (tokens[0] == "v") {
                    data.vx_.push_back(Real3(atof(tokens[1].c_str()),atof(tokens[2].c_str()),atof(tokens[3].c_str()) ) );
                    //std::cout<<"v "<<data.vx_.size()-1<<data.vx_[data.vx_.size()-1]<<std::endl;
                }	
                if (tokens[0] == "vt") {
                    data.st_.push_back(Real2(atof(tokens[1].c_str()),atof(tokens[2].c_str()) ) );
                    //std::cout<<"vt "<<data.st_.size()-1<<data.st_[data.st_.size()-1]<<std::endl;
                }	
                if (tokens[0] == "vn") {
                    data.vn_.push_back(Real3(atof(tokens[1].c_str()),atof(tokens[2].c_str()),atof(tokens[3].c_str()) ) );
                    //std::cout<<"vn "<<data.vn_.size()-1<<data.vn_[data.vn_.size()-1]<<std::endl;
                }
                if (tokens[0] == "f") {
                    for (unsigned i=0;i<tokens.size()-1;i++) {
                        std::vector<std::string> ftokens;
                        stringaux::SplitString(tokens[i+1], ftokens,'/');
                        if (data.st_.size()) {
                            data.fc_.push_back(Int3(atoi(ftokens[0].c_str()),atoi(ftokens[1].c_str()),atoi(ftokens[2].c_str()) ) );
                        }
                        else {
                            data.fc_.push_back(Int3(atoi(ftokens[0].c_str()),-1,atoi(ftokens[2].c_str()) ) );
                        }
                        //std::cout<<"f "<<data.fc_.size()-1<<data.fc_[data.fc_.size()-1]<<std::endl;
                    }
                }
                if (tokens[0] == "usemtl") {
                    std::vector<std::string> mtokens;
                    stringaux::SplitString(tokens[1], mtokens,'_');
                    if (mtokens.size()==2)
                        data.matfile_ = mtokens[1];
                    //std::cout<<data.matfile_<<std::endl;
                }
            }
        }
    }
	return data;
}
    

