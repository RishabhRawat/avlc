//
// Created by scalpel on 12/3/17.
//

#ifndef AVLC_IIR_TYPES_H
#define AVLC_IIR_TYPES_H

#include <experimental/filesystem>

#define filesystem experimental::filesystem

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
}

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

enum Number_Base_Type {
    Base_None,
    Base_2,
    Base_4,
    Base_8,
    Base_16,
    Base_10
};

enum class Token_Type {
    Invalid,
    Next_Event_E
};

enum class Iir_Constraint {
    Unconstrained, Partially_Constrained, Fully_Constrained
};

enum class Iir_All_Sensitized {
    Unknown, No_Signal, Read_Signal, Invalid_Signal
};

enum class Tri_State_Type {
    Unknown, False, True
};

enum class Iir_Pure_State {
    Unknown, Pure, Maybe_Impure, Impure
};

enum class Interface_Type {
    Generic,
    Port,
    Procedure_Parameter,
    Function_Parameter
};

enum class Iir_Staticness { Unknown, None, Globally, Locally };

using Time_Stamp_Id = std::tm;
using File_Checksum_Id = std::string;

//TODO: This type is only for file positions, as opposed to Iir position, ghdl uses same for both
struct Location_Type {
    std::filesystem::path filePath;
    uint32_t Line = 0;
    uint32_t Line_Pos = 0;
    uint32_t Pos = 0;
    bool operator== (const Location_Type& right) {
        return filePath == right.filePath && Line == right.Line && Line_Pos == right.Line_Pos && Pos == right.Line_Pos;
    }
};

inline long LocationToPosition (const Location_Type& l) {
    return l.Line_Pos + l.Pos;
}

enum Iir_Predefined_Functions {
    Iir_Predefined_Error,

//  Predefined operators for BOOLEAN type

//  LRM08 9.2.2 Logical Operators
    Iir_Predefined_Boolean_And,
    Iir_Predefined_Boolean_Or,
    Iir_Predefined_Boolean_Nand,
    Iir_Predefined_Boolean_Nor,
    Iir_Predefined_Boolean_Xor,
    Iir_Predefined_Boolean_Xnor,
    Iir_Predefined_Boolean_Not,

//  LRM08 5.2.6 Predefined operations on scalar types.
    Iir_Predefined_Boolean_Rising_Edge,
    Iir_Predefined_Boolean_Falling_Edge,

//  Predefined operators for any enumeration type.

//  LRM08 9.2.3 Relational Operators
    Iir_Predefined_Enum_Equality,
    Iir_Predefined_Enum_Inequality,
    Iir_Predefined_Enum_Less,
    Iir_Predefined_Enum_Less_Equal,
    Iir_Predefined_Enum_Greater,
    Iir_Predefined_Enum_Greater_Equal,

//  LRM08 5.2.6 Predefined operations on scalar types.
    Iir_Predefined_Enum_Minimum,
    Iir_Predefined_Enum_Maximum,
    Iir_Predefined_Enum_To_String,

//  Predefined operators for BIT type.

//  LRM08 9.2.2 Logical Operators
    Iir_Predefined_Bit_And,
    Iir_Predefined_Bit_Or,
    Iir_Predefined_Bit_Nand,
    Iir_Predefined_Bit_Nor,
    Iir_Predefined_Bit_Xor,
    Iir_Predefined_Bit_Xnor,
    Iir_Predefined_Bit_Not,

//  LRM08 9.2.3 Relational Operators
    Iir_Predefined_Bit_Match_Equality,
    Iir_Predefined_Bit_Match_Inequality,
    Iir_Predefined_Bit_Match_Less,
    Iir_Predefined_Bit_Match_Less_Equal,
    Iir_Predefined_Bit_Match_Greater,
    Iir_Predefined_Bit_Match_Greater_Equal,

//  LRM08 9.2.9 Condition operator
    Iir_Predefined_Bit_Condition,

//  LRM08 5.2.6 Predefined operations on scalar types.
    Iir_Predefined_Bit_Rising_Edge,
    Iir_Predefined_Bit_Falling_Edge,

//  Predefined operators for any integer type.

//  LRM08 9.2.3 Relational Operators
    Iir_Predefined_Integer_Equality,
    Iir_Predefined_Integer_Inequality,
    Iir_Predefined_Integer_Less,
    Iir_Predefined_Integer_Less_Equal,
    Iir_Predefined_Integer_Greater,
    Iir_Predefined_Integer_Greater_Equal,

//  LRM08 9.2.6 Sign operators
    Iir_Predefined_Integer_Identity,
    Iir_Predefined_Integer_Negation,

//  LRM08 9.2.8 Miscellaneous operators
    Iir_Predefined_Integer_Absolute,

//  LRM08 9.2.5 Adding operators
    Iir_Predefined_Integer_Plus,
    Iir_Predefined_Integer_Minus,

//  LRM08 9.2.7 Multiplying operators
    Iir_Predefined_Integer_Mul,
    Iir_Predefined_Integer_Div,
    Iir_Predefined_Integer_Mod,
    Iir_Predefined_Integer_Rem,

//  LRM08 9.2.8 Miscellaneous operators
    Iir_Predefined_Integer_Exp,

//  LRM08 5.2.6 Predefined operations on scalar types.
    Iir_Predefined_Integer_Minimum,
    Iir_Predefined_Integer_Maximum,
    Iir_Predefined_Integer_To_String,

//  Predefined operators for any floating type.

//  LRM08 9.2.3 Relational Operators
    Iir_Predefined_Floating_Equality,
    Iir_Predefined_Floating_Inequality,
    Iir_Predefined_Floating_Less,
    Iir_Predefined_Floating_Less_Equal,
    Iir_Predefined_Floating_Greater,
    Iir_Predefined_Floating_Greater_Equal,

//  LRM08 9.2.6 Sign operators
    Iir_Predefined_Floating_Identity,
    Iir_Predefined_Floating_Negation,

//  LRM08 9.2.8 Miscellaneous operators
    Iir_Predefined_Floating_Absolute,

//  LRM08 9.2.5 Adding operators
    Iir_Predefined_Floating_Plus,
    Iir_Predefined_Floating_Minus,

//  LRM08 9.2.7 Multiplying operators
    Iir_Predefined_Floating_Mul,
    Iir_Predefined_Floating_Div,

//  LRM08 9.2.8 Miscellaneous operators
    Iir_Predefined_Floating_Exp,

//  LRM08 5.2.6 Predefined operations on scalar types.
    Iir_Predefined_Floating_Minimum,
    Iir_Predefined_Floating_Maximum,
    Iir_Predefined_Floating_To_String,
    Iir_Predefined_Real_To_String_Digits,
    Iir_Predefined_Real_To_String_Format,

//  Predefined operator for universal types.

//  LRM08 9.2.7 Multiplying operators
    Iir_Predefined_Universal_R_I_Mul,
    Iir_Predefined_Universal_I_R_Mul,
    Iir_Predefined_Universal_R_I_Div,

//  Predefined operators for physical types.

//  LRM08 9.2.3 Relational Operators
    Iir_Predefined_Physical_Equality,
    Iir_Predefined_Physical_Inequality,
    Iir_Predefined_Physical_Less,
    Iir_Predefined_Physical_Less_Equal,
    Iir_Predefined_Physical_Greater,
    Iir_Predefined_Physical_Greater_Equal,

//  LRM08 9.2.6 Sign operators
    Iir_Predefined_Physical_Identity,
    Iir_Predefined_Physical_Negation,

//  LRM08 9.2.8 Miscellaneous operators
    Iir_Predefined_Physical_Absolute,

//  LRM08 9.2.5 Adding operators
    Iir_Predefined_Physical_Plus,
    Iir_Predefined_Physical_Minus,

//  LRM08 9.2.7 Multiplying operators
    Iir_Predefined_Physical_Integer_Mul,
    Iir_Predefined_Physical_Real_Mul,
    Iir_Predefined_Integer_Physical_Mul,
    Iir_Predefined_Real_Physical_Mul,
    Iir_Predefined_Physical_Integer_Div,
    Iir_Predefined_Physical_Real_Div,
    Iir_Predefined_Physical_Physical_Div,

//  LRM08 5.2.6 Predefined operations on scalar types.
    Iir_Predefined_Physical_Minimum,
    Iir_Predefined_Physical_Maximum,
    Iir_Predefined_Physical_To_String,
    Iir_Predefined_Time_To_String_Unit,

//  Predefined operators for access.

//  LRM08 9.2.3 Relational Operators
    Iir_Predefined_Access_Equality,
    Iir_Predefined_Access_Inequality,

//  Predefined operators for record.

//  LRM08 9.2.3 Relational Operators
    Iir_Predefined_Record_Equality,
    Iir_Predefined_Record_Inequality,

//  Predefined operators for array.

//  LRM08 9.2.3 Relational Operators
    Iir_Predefined_Array_Equality,
    Iir_Predefined_Array_Inequality,
    Iir_Predefined_Array_Less,
    Iir_Predefined_Array_Less_Equal,
    Iir_Predefined_Array_Greater,
    Iir_Predefined_Array_Greater_Equal,

//  LRM08 9.2.5 Adding operators
    Iir_Predefined_Array_Array_Concat,
    Iir_Predefined_Array_Element_Concat,
    Iir_Predefined_Element_Array_Concat,
    Iir_Predefined_Element_Element_Concat,

//  LRM08 5.3.2.4 Predefined operations on array types
    Iir_Predefined_Array_Minimum,
    Iir_Predefined_Array_Maximum,
    Iir_Predefined_Vector_Minimum,
    Iir_Predefined_Vector_Maximum,

//  LRM08 9.2.4 Shift operators
    Iir_Predefined_Array_Sll,
    Iir_Predefined_Array_Srl,
    Iir_Predefined_Array_Sla,
    Iir_Predefined_Array_Sra,
    Iir_Predefined_Array_Rol,
    Iir_Predefined_Array_Ror,

//  LRM08 9.2.2 Logical operators
//  Predefined operators for one dimensional array.
//  For bit and boolean type, the operations are the same.  To be
//  neutral, we use TF (for True/False) instead of Bit, Boolean or
//  Logic.
    Iir_Predefined_TF_Array_And,
    Iir_Predefined_TF_Array_Or,
    Iir_Predefined_TF_Array_Nand,
    Iir_Predefined_TF_Array_Nor,
    Iir_Predefined_TF_Array_Xor,
    Iir_Predefined_TF_Array_Xnor,
    Iir_Predefined_TF_Array_Not,

//  LRM08 9.2.2 Logical operators
    Iir_Predefined_TF_Reduction_And,
    Iir_Predefined_TF_Reduction_Or,
    Iir_Predefined_TF_Reduction_Nand,
    Iir_Predefined_TF_Reduction_Nor,
    Iir_Predefined_TF_Reduction_Xor,
    Iir_Predefined_TF_Reduction_Xnor,
    Iir_Predefined_TF_Reduction_Not,

//  LRM08 9.2.2 Logical operators
    Iir_Predefined_TF_Array_Element_And,
    Iir_Predefined_TF_Element_Array_And,
    Iir_Predefined_TF_Array_Element_Or,
    Iir_Predefined_TF_Element_Array_Or,
    Iir_Predefined_TF_Array_Element_Nand,
    Iir_Predefined_TF_Element_Array_Nand,
    Iir_Predefined_TF_Array_Element_Nor,
    Iir_Predefined_TF_Element_Array_Nor,
    Iir_Predefined_TF_Array_Element_Xor,
    Iir_Predefined_TF_Element_Array_Xor,
    Iir_Predefined_TF_Array_Element_Xnor,
    Iir_Predefined_TF_Element_Array_Xnor,

//  LRM08 9.2.3 Relational Operators
    Iir_Predefined_Bit_Array_Match_Equality,
    Iir_Predefined_Bit_Array_Match_Inequality,

//  LRM08 5.3.2.4 Predefined operations on array types
    Iir_Predefined_Array_Char_To_String,
    Iir_Predefined_Bit_Vector_To_Ostring,
    Iir_Predefined_Bit_Vector_To_Hstring,

//  LRM08 9.2.3 Relational Operators
//  IEEE.Std_Logic_1164.Std_Ulogic
    Iir_Predefined_Std_Ulogic_Match_Equality,
    Iir_Predefined_Std_Ulogic_Match_Inequality,
    Iir_Predefined_Std_Ulogic_Match_Less,
    Iir_Predefined_Std_Ulogic_Match_Less_Equal,
    Iir_Predefined_Std_Ulogic_Match_Greater,
    Iir_Predefined_Std_Ulogic_Match_Greater_Equal,

//  LRM08 9.2.3 Relational Operators
    Iir_Predefined_Std_Ulogic_Array_Match_Equality,
    Iir_Predefined_Std_Ulogic_Array_Match_Inequality,

//  --  Predefined attribute functions.
//  Iir_Predefined_Attribute_Image,
//  Iir_Predefined_Attribute_Value,
//  Iir_Predefined_Attribute_Pos,
//  Iir_Predefined_Attribute_Val,
//  Iir_Predefined_Attribute_Succ,
//  Iir_Predefined_Attribute_Pred,
//  Iir_Predefined_Attribute_Leftof,
//  Iir_Predefined_Attribute_Rightof,
//  Iir_Predefined_Attribute_Left,
//  Iir_Predefined_Attribute_Right,
//  Iir_Predefined_Attribute_Event,
//  Iir_Predefined_Attribute_Active,
//  Iir_Predefined_Attribute_Last_Event,
//  Iir_Predefined_Attribute_Last_Active,
//  Iir_Predefined_Attribute_Last_Value,
//  Iir_Predefined_Attribute_Driving,
//  Iir_Predefined_Attribute_Driving_Value,

//  Impure subprograms.

//  LRM08 5.4.3 Allocation and deallocation of objects
    Iir_Predefined_Deallocate,

//  LRM08 5.5.2 File operations
    Iir_Predefined_File_Open,
    Iir_Predefined_File_Open_Status,
    Iir_Predefined_File_Close,
    Iir_Predefined_Read,
    Iir_Predefined_Read_Length,
    Iir_Predefined_Flush,
    Iir_Predefined_Write,
    Iir_Predefined_Endfile,

//  Misc impure functions.
    Iir_Predefined_Now_Function,

//  A not predefined and not known function.  User function.
    Iir_Predefined_None,

//  Defined in package ieee.std_logic_1164

//  Std_Ulogic operations.
    Iir_Predefined_Ieee_1164_Scalar_And,
    Iir_Predefined_Ieee_1164_Scalar_Nand,
    Iir_Predefined_Ieee_1164_Scalar_Or,
    Iir_Predefined_Ieee_1164_Scalar_Nor,
    Iir_Predefined_Ieee_1164_Scalar_Xor,
    Iir_Predefined_Ieee_1164_Scalar_Xnor,
    Iir_Predefined_Ieee_1164_Scalar_Not,

//  Std_Logic_Vector or Std_Ulogic_Vector operations.
//  Length of the result is the length of the left operand.
    Iir_Predefined_Ieee_1164_Vector_And,
    Iir_Predefined_Ieee_1164_Vector_Nand,
    Iir_Predefined_Ieee_1164_Vector_Or,
    Iir_Predefined_Ieee_1164_Vector_Nor,
    Iir_Predefined_Ieee_1164_Vector_Xor,
    Iir_Predefined_Ieee_1164_Vector_Xnor,
    Iir_Predefined_Ieee_1164_Vector_Not,

//  Numeric_Std.
//  Abbreviations:
//  Uns: Unsigned, Sgn: Signed, Nat: Natural, Int: Integer.
    Iir_Predefined_Ieee_Numeric_Std_Add_Uns_Uns,
    Iir_Predefined_Ieee_Numeric_Std_Add_Uns_Nat,
    Iir_Predefined_Ieee_Numeric_Std_Add_Nat_Uns,
    Iir_Predefined_Ieee_Numeric_Std_Add_Sgn_Sgn,
    Iir_Predefined_Ieee_Numeric_Std_Add_Sgn_Int,
    Iir_Predefined_Ieee_Numeric_Std_Add_Int_Sgn,

    Iir_Predefined_Ieee_Numeric_Std_Eq_Uns_Uns,
    Iir_Predefined_Ieee_Numeric_Std_Eq_Uns_Nat,
    Iir_Predefined_Ieee_Numeric_Std_Eq_Nat_Uns,
    Iir_Predefined_Ieee_Numeric_Std_Eq_Sgn_Sgn,
    Iir_Predefined_Ieee_Numeric_Std_Eq_Sgn_Int,
    Iir_Predefined_Ieee_Numeric_Std_Eq_Int_Sgn
};


#endif //AVLC_IIR_TYPES_H
