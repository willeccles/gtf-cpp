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

#define NO_SCORE std::numeric_limits<double>::infinity()

using GTFError = std::runtime_error;

const std::regex valid_gtf_line_regex{"^\\S+\\t\\S+\\t\\S+\\t\\d+\\t\\d+\\t\\S+\\t\\S+\\t\\d([\\t\\s]\\S+\\s\"?[^\\s\\t\"]+\"?;)*$"};

struct GTFSequence {
    std::string seqname;    // sequence name
    std::string source;     // source
    std::string feature;    // feature
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
                throw GTFError("Error opening GTF file");
            }
        }

        ~GTFFile() {
            infile.close();
        }

        void close() {
            infile.close();
        }

        // Gets the next sequence and returns true or returns false if none
        // are left to be gotten.
        bool next_sequence(GTFSequence& out_seq) {
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
                        >> out_seq.feature
                        >> out_seq.start
                        >> out_seq.end
                        >> tmpscore // check this afterwards
                        >> out_seq.strand
                        >> out_seq.frame;
                    if (tmpscore == ".") {
                        out_seq.score = NO_SCORE;
                    } else {
                        out_seq.score = std::atof(tmpscore.c_str());
                    }

                    out_seq.attributes.clear();
                    // get the attributes if any
                    std::string tmpkey, tmpval;
                    while (ss >> tmpkey) {
                        std::getline(ss, tmpval, ';');
                        out_seq.attributes[tmpkey] = sanitize_attr_value(tmpval);
                    }

                    return true;
                }
            }

            return false;
        }

    private:
        std::ifstream infile;

        static inline std::string& trim(std::string& str) {
            if (str.empty()) return str;

            // trim the right side
            str.erase(str.find_last_not_of(" \t") + 1);

            // trim the left side
            str.erase(0, str.find_first_not_of(" \t"));

            // return reference to str
            return str;
        }
        
        static inline std::string& sanitize_line(std::string& line) {
            if (line.empty()) return line;

            // get rid of comments
            std::size_t hashpos = line.find_first_of('#');
            if (hashpos != line.npos) {
                line.erase(hashpos, line.size() - 1);
            }

            trim(line);

            // we sanitized the line in place, but return a reference anyway
            return line;
        }

        static inline std::string& sanitize_attr_value(std::string& value) {
            trim(value);
            if (value.empty()) return value;

            if (value[0] == '"') {
                value.erase(0, 1);
            }

            if (value[value.size()-1] == '"') {
                value.erase(value.size()-1);
            }

            return value;
        }

        static inline bool valid_line(const std::string& line) {
            return std::regex_search(line, valid_gtf_line_regex);
        }
};

#endif /* _GTF_H */
