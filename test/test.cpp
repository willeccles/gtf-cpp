#include "../gtf.h"
#include <iostream>

int main(void) {
    try {
        GTFFile testgtf("badfile.gtf");
    } catch (const GTFError& error) {
        std::cout << "Error with file badfile.gtf: " << error.what() << '\n';
    }

    GTFFile testgtf("test.gtf");

    GTFSequence tempseq;
    for (int i = 1; testgtf.next_sequence(tempseq); i++) {
        std::cout << "Sequence " << i << ":\n";
        std::cout << tempseq.seqname << ' ';    // sequence name
        std::cout << tempseq.source << ' ';     // source
        std::cout << tempseq.feature << ' ';    // feature
        std::cout << tempseq.start << ' ';      // start
        std::cout << tempseq.end << ' ';        // end
        std::cout << tempseq.score << ' ';      // either a score or NO_SCORE if a .
        std::cout << tempseq.strand << ' ';     // + or -
        std::cout << tempseq.frame << '\n';     // frame (0, 1, 2)

        for (auto& p : tempseq.attributes) {
            std::cout << p.first << ": " << p.second << '\n';
        }

        std::cout << '\n';
    }
    

    return 0;
}
