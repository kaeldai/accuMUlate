// ***************************************************************************
// bamtools_pileup_engine.h (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 10 October 2011
// ---------------------------------------------------------------------------
// Provides pileup at position functionality for various tools.
// ***************************************************************************

#ifndef BAMTOOLS_PILEUP_ENGINE_H
#define BAMTOOLS_PILEUP_ENGINE_H

//#include "utils/utils_global.h"

#include <api/BamAlignment.h>
#include <vector>

namespace LocalBamToolsUtils {

// contains auxiliary data about a single BamAlignment
// at current position considered
    struct PileupAlignment {

        // data members
        BamTools::BamAlignment Alignment;
        int32_t PositionInAlignment;
        bool IsCurrentDeletion;
        bool IsNextDeletion;
        bool IsNextInsertion;
        int DeletionLength;
        int InsertionLength;
        bool IsSegmentBegin;
        bool IsSegmentEnd;

        // ctor
        PileupAlignment(const BamTools::BamAlignment &al)
                : Alignment(al), PositionInAlignment(-1), IsCurrentDeletion(false), IsNextDeletion(false),
                  IsNextInsertion(false), DeletionLength(0), InsertionLength(0), IsSegmentBegin(false),
                  IsSegmentEnd(false) { }
    };

// contains all data at a position
    struct PileupPosition {

        // data members
        int RefId;
        int Position;
        std::vector<PileupAlignment> PileupAlignments;

        // ctor
        PileupPosition(const int &refId = 0,
                       const int &position = 0,
                       const std::vector<PileupAlignment> &alignments = std::vector<PileupAlignment>())
                : RefId(refId), Position(position), PileupAlignments(alignments) { }
    };

    class PileupVisitor {

    public:
        PileupVisitor(void) { }

        virtual ~PileupVisitor(void) { }

    public:
        virtual void Visit(const PileupPosition &pileupData) = 0;
    };

    class PileupEngine {

    public:
        PileupEngine(void);

        ~PileupEngine(void);

    public:
        bool AddAlignment(const BamTools::BamAlignment &al);

        void AddVisitor(PileupVisitor *visitor);

        void Flush(void);

    private:
        struct PileupEnginePrivate;
        PileupEnginePrivate *d;
    };

} // LocalBamToolsUtils

#endif // BAMTOOLS_PILEUP_ENGINE_H
