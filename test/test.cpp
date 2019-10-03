#include "../gtf.h"
#include <iostream>

int main(void) {
    try {
        GTFFile testgtf("badfile.gtf");
    } catch (const GTFError& error) {
        std::cout << "Error with file badfile.gtf: " << error.what() << '\n';
    }

    GTFFile testgtf("test.gtf");
    testgtf.load();

    for (auto& gtfseq : testgtf) {
        std::cout << gtfseq.seqname << ' ';    // sequence name
        std::cout << gtfseq.source << ' ';     // source
        std::cout << gtfseq.feature << ' ';    // feature
        std::cout << gtfseq.start << ' ';      // start
        std::cout << gtfseq.end << ' ';        // end
        std::cout << gtfseq.score << ' ';      // either a score or NO_SCORE if a .
        std::cout << gtfseq.strand << ' ';     // + or -
        std::cout << gtfseq.frame << '\n';     // frame (0, 1, 2)

        for (auto& [key, val] : gtfseq.attributes) {
            std::cout << key << ": " << val << '\n';
        }

        std::cout << '\n';
    }
    

    return 0;
}
