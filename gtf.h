/*
 * gtf.h
 *
 * Author: Will Eccles
 * Date: 2019-10-03
 * Description: Implements a simple header-only GTF file parser.
 * 
 * See also:
 *   https://web.archive.org/web/20191002053706/http://mblab.wustl.edu/GTF22.html
 *
 * Copyright 2019 Will Eccles
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _GTF_H
#define _GTF_H

#include <string>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <map>
#include <cstdlib>

#define NO_SCORE std::numeric_limits<double>::infinity

struct GTFSequence {
    std::string seqname;
    std::string source;
    std::size_t start;
    std::size_t end;
    double      score;
    char        strand;
    int         frame;

    std::map<std::string, std::string> attributes;

    bool has_attribute(const std::string& attr) {
        return (attributes.find(attr) != attributes.end());
    }
};

class GTFFile {
    public:
        GTFFile(const std::string& filename): infile(filename) {
            if (!infile) {
                throw std::runtime_error("Error opening GTF file");
            }
        }

    private:
        std::ifstream infile;
};

#endif /* _GTF_H */
