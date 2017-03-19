//
// Created by scalpel on 12/3/17.
//

#ifndef AVLC_IIR_TYPES_H
#define AVLC_IIR_TYPES_H

// Date State.
// This indicates the origin of the data information.
// This also indicates the state of the unit (loaded or not).
enum Date_State_Type {
// The unit is not yet in the library.
            Date_Extern = 0,

// The unit is not loaded (still on the disk). All the informations come from the library file.
            Date_Disk = 1,

// The unit has been parsed, but not analyzed. Only the date information come from the library.
            Date_Parse = 2,

// The unit has been analyzed.
            Date_Analyze = 3
};

// A date is used for analysis order. All design units from a library are ordered according to the date.
/*enum Date_Type {
    // The unit is obsoleted (ie replaced) by a more recently analyzed design unit.another design unit.
    // If another design unit depends (directly or not) on an obseleted design unit, it is also obsolete,
    // and cannot be defined.
            Date_Obsolete = 0,
    // The unit was not analyzed.
            Date_Not_Analyzed = 1,
    // The unit has been analyzed but it has bad dependences.
            Date_Bad_Analyze = 2,
    // The unit has been parsed but not analyzed.
            Date_Parsed = 4,
    // The unit is being analyzed.
            Date_Analyzing = 5,
    // This unit has just been analyzed and should be marked at the last analyzed unit.
            Date_Analyzed = 6,
    // Used only for default configuration. Such units are always up-to-date.
            Date_Uptodate = 7
    // All Dates type above 10 are valid
}*/
using Date_Type = unsigned int;
enum Iir_Mode {
    Iir_Unknown_Mode,
    Iir_Linkage_Mode,
    Iir_Buffer_Mode,
    Iir_Out_Mode,
    Iir_Inout_Mode,
    Iir_In_Mode
};


bool isDateValid(Date_Type date_type) {
    //FIXME: wtf is this
    return date_type > 10;
};

enum Iir_Signal_Kind {
        Iir_Register_Kind,
        Iir_Bus_Kind
};

enum Iir_Delay_Mechanism {
    Iir_Inertial_Delay,
    Iir_Transport_Delay
};

enum Iir_Direction {
    Iir_To,
    Iir_Downto
};

enum class Iir_Staticness { Unknown, None, Globally, Locally };

#endif //AVLC_IIR_TYPES_H
