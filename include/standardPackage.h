/*
 * avlc: another vhdl language compiler
 * Copyright (C) 2016 Rishabh Rawat
 *
 * avlc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef AVLC_STANDARDPACKAGE_H
#define AVLC_STANDARDPACKAGE_H

#include "iir.h"

namespace standardPackage {
//  This is a special node, not really declared in the STANDARD package,
//  used to mark a node as erroneous.
//  Its kind is Iir_Kind_Error.
    const IIR::Error* Error_Mark(0);

// Some well know values declared in the STANDARD package.
// These values (except time_base) *must* not be modified, and are set by
// create_std_standard_package.
// Time_base is the base unit of time.  It is set during the creation of
// all these nodes, and can be modified only *immediately* after.

    IIR::Unit_Declaration* Time_Base = nullptr;

    IIR::Design_File* Std_Standard_File = nullptr;
    IIR::Design_Unit* Std_Standard_Unit = nullptr;
    IIR::Package_Declaration* Standard_Package = nullptr;

// Boolean values.
    IIR::Type_Declaration* Boolean_Type_Declaration = nullptr;
    IIR::Enumeration_Type_Definition* Boolean_Type_Definition = nullptr;
    IIR::Enumeration_Literal* Boolean_False = nullptr;
    IIR::Enumeration_Literal* Boolean_True = nullptr;

// Bit values.
    IIR::Type_Declaration* Bit_Type_Declaration = nullptr;
    IIR::Enumeration_Type_Definition* Bit_Type_Definition = nullptr;
    IIR::Enumeration_Literal* Bit_0 = nullptr;
    IIR::Enumeration_Literal* Bit_1 = nullptr;

// Predefined character.
    IIR::Type_Declaration* Character_Type_Declaration = nullptr;
    IIR::Enumeration_Type_Definition* Character_Type_Definition = nullptr;

// severity level.
    IIR::Type_Declaration* Severity_Level_Type_Declaration = nullptr;
    IIR::Enumeration_Type_Definition* Severity_Level_Type_Definition = nullptr;
    IIR::Enumeration_Literal* Severity_Level_Note = nullptr;
    IIR::Enumeration_Literal* Severity_Level_Warning = nullptr;
    IIR::Enumeration_Literal* Severity_Level_Error = nullptr;
    IIR::Enumeration_Literal* Severity_Level_Failure = nullptr;

// Universal types.
    IIR::Anonymous_Type_Declaration* Universal_Integer_Type_Declaration;
    IIR::Integer_Type_Definition* Universal_Integer_Type_Definition;
    IIR::Subtype_Declaration* Universal_Integer_Subtype_Declaration;
    IIR::Integer_Subtype_Definition* Universal_Integer_Subtype_Definition;

    IIR::Integer_Literal* Universal_Integer_One;

    IIR::Anonymous_Type_Declaration* Universal_Real_Type_Declaration;
    IIR::Floating_Type_Definition* Universal_Real_Type_Definition;
    IIR::Subtype_Declaration* Universal_Real_Subtype_Declaration;
    IIR::Floating_Subtype_Definition* Universal_Real_Subtype_Definition;

// Predefined integer type.
    IIR::Anonymous_Type_Declaration* Integer_Type_Declaration;
    IIR::Integer_Type_Definition* Integer_Type_Definition;
    IIR::Subtype_Declaration* Integer_Subtype_Declaration;
    IIR::Integer_Subtype_Definition* Integer_Subtype_Definition;

//  Type used when the type of an expression is incorrect.
    IIR::Error* Error_Type;

// Predefined real type.
    IIR::Anonymous_Type_Declaration* Real_Type_Declaration;
    IIR::Floating_Type_Definition* Real_Type_Definition;
    IIR::Subtype_Declaration* Real_Subtype_Declaration;
    IIR::Floating_Subtype_Definition* Real_Subtype_Definition;

// Predefined natural subtype.
    IIR::Subtype_Declaration* Natural_Subtype_Declaration;
    IIR::Integer_Subtype_Definition* Natural_Subtype_Definition;

// Predefined positive subtype.
    IIR::Subtype_Declaration* Positive_Subtype_Declaration;
    IIR::Integer_Subtype_Definition* Positive_Subtype_Definition;

// Predefined positive subtype.
    IIR::Type_Declaration* String_Type_Declaration;
    IIR::Array_Type_Definition* String_Type_Definition;

// Predefined positive subtype.
    IIR::Type_Declaration* Bit_Vector_Type_Declaration;
    IIR::Array_Type_Definition* Bit_Vector_Type_Definition;

// predefined time subtype
    IIR::Anonymous_Type_Declaration* Time_Type_Declaration;
    IIR::Physical_Type_Definition* Time_Type_Definition;
    IIR::Physical_Subtype_Definition* Time_Subtype_Definition;
    IIR::Subtype_Declaration* Time_Subtype_Declaration;

//  For VHDL-93
    IIR::Physical_Subtype_Definition* Delay_Length_Subtype_Definition;
    IIR::Subtype_Declaration* Delay_Length_Subtype_Declaration;

//  For VHDL-93:
//  type File_Open_Kind
    IIR::Type_Declaration* File_Open_Kind_Type_Declaration;
    IIR::Enumeration_Type_Definition* File_Open_Kind_Type_Definition;
    IIR::Enumeration_Literal* File_Open_Kind_Read_Mode;
    IIR::Enumeration_Literal* File_Open_Kind_Write_Mode;
    IIR::Enumeration_Literal* File_Open_Kind_Append_Mode;

//  For VHDL-93:
//  type File_Open_Status
    IIR::Type_Declaration* File_Open_Status_Type_Declaration;
    IIR::Enumeration_Type_Definition* File_Open_Status_Type_Definition;
    IIR::Enumeration_Literal* File_Open_Status_Open_Ok;
    IIR::Enumeration_Literal* File_Open_Status_Status_Error;
    IIR::Enumeration_Literal* File_Open_Status_Name_Error;
    IIR::Enumeration_Literal* File_Open_Status_Mode_Error;

//  For VHDL-93:
   //FIXME: string //    atribute foreign ;
            IIR::Attribute_Declaration* Foreign_Attribute;

//  For VHDL-08
    IIR::Array_Type_Definition* Boolean_Vector_Type_Definition;
    IIR::Type_Declaration* Boolean_Vector_Type_Declaration;

    IIR::Array_Type_Definition* Integer_Vector_Type_Definition;
    IIR::Type_Declaration* Integer_Vector_Type_Declaration;

    IIR::Array_Type_Definition* Real_Vector_Type_Definition;
    IIR::Type_Declaration* Real_Vector_Type_Declaration;

    IIR::Array_Type_Definition* Time_Vector_Type_Definition;
    IIR::Type_Declaration* Time_Vector_Type_Declaration;

//  Internal use only.
//  These types should be considered like universal types, but
//  furthermore, they can be converted to any integer/real types while
//  universal cannot.

    IIR::Integer_Type_Definition* Convertible_Integer_Type_Definition;
    IIR::Floating_Type_Definition* Convertible_Real_Type_Definition;
    IIR::Anonymous_Type_Declaration* Convertible_Integer_Type_Declaration;
    IIR::Anonymous_Type_Declaration* Convertible_Real_Type_Declaration;

    IIR::Integer_Subtype_Definition* Convertible_Integer_Subtype_Definition;
    IIR::Subtype_Declaration* Convertible_Integer_Subtype_Declaration;

//  Wilcard types.
//  Err, we break privacy for iir numbers, but this allow use of them in
//  case statements.
//    const Iir Wildcard_Any_Type = 7;
//    const Iir Wildcard_Any_Aggregate_Type = 8;
//    const Iir Wildcard_Any_String_Type = 9;
//    const Iir Wildcard_Any_Access_Type = 10;

//  Subtype for all wildcard types, so that missing choice can be detected
//  at compilation time.
/*
    subtype Iir_Wildcard_Types
    is Iir
    range 7 .. 10;
*/
//  Chain of wildcard declarations, to own the nodes.
    std::vector<IIR::Base*> Wildcard_Type_Declaration_Chain;

//  Create the first well-known nodes.
    void Create_First_Nodes();

//  Create the node for the standard package.
    void Create_Std_Standard_Package(IIR::Library_Declaration* Parent);

};

#endif //AVLC_STANDARDPACKAGE_H
