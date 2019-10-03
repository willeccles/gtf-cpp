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

#include <cstdlib>
#include <fstream>
#include <map>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

#define NO_SCORE std::numeric_limits<double>::infinity

struct GTFSequence {
    std::string seqname;    // sequence name
    std::string source;     // source
    std::size_t start;      // start
    std::size_t end;        // end
    double      score;      // either a score or NO_SCORE if a .
    char        strand;     // + or -
    short       frame;      // frame (0, 1, 2)

    std::map<std::string, std::string> attributes;  // any attributes to use

    bool has_attribute(const std::string& attr) const noexcept {
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

        // Gets the next sequence and returns true or returns false if none
        // are left to be gotten.
        bool next_sequence(GTFSequence& out_seq) const {
            std::string line;
            std::stringstream ss;
            while (std::getline(infile, line)) {
                sanitize_line(line);
                if (!valid_line(line)) {
                    continue;
                } else {
                    ss = std::stringstream(line);
                    std::string tmpscore;
                    ss >> out_seq.seqname
                        >> out_seq.source
                        >> out_seq.start
                        >> out_seq.end
                        >> tmpscore // check this afterwards
                        >> out_seq.strand
                        >> out_seq.frame;
                    if (tmpscore == ".") {
                        out_seq.score = NO_SCORE;
                    } else {
                        out_seq.score = std::atof(tmpscore);
                    }

                    out_seq.attributes.clear();
                    // get the attributes if any
                    std::string tmpattr, tmpkey, tmpval;
                    while (std::getline(ss, tmpattr, ';')) {
                        std::getline(tmpattr, tmpkey, '\t');
                        std::getline(tmpattr, tmpval);
                        out_seq.attributes.insert(tmpattr, tmpval);
                    }

                    return true;
                }
            }

            return false;
        }

    private:
        std::ifstream infile;

        const static std::basic_regex valid_line_regex("^\\S+\\t\\S+\\t\\S+\\t\\d+\\t\\d+\\t\\S+\\t\\S+\\t\\d(\\t\\S+\\t\\S+)*$");
        
        static inline std::string& sanitize_line(std::string& line) {
            // get rid of comments
            // TODO maybe make this not find '#' characters that are in strings
            if (line.find_first_of('#') != line.npos()) {
                line.erase(line.find_first_of('#'), line.length() - 1);
            }
            
            // trim the right side
            line.erase(line.find_last_not_of(" \t") + 1);

            // trim the left side
            line.erase(0, line.find_first_not_of(" \t"));

            // we sanitized the line in place, but return a reference anyway
            return line;
        }

        static inline bool valid_line(const std::string& line) {
            return std::regex_search(line, valid_line_regex);
        }
};

#endif /* _GTF_H */
