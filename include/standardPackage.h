/*
 * avlc: another vhdl language compiler
 * Copyright (C) 2016 Rishabh Rawat
 *
 * avlc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef AVLC_STANDARDPACKAGE_H
#define AVLC_STANDARDPACKAGE_H

namespace standardPackage {
//  This is a special node, not really declared in the STANDARD package,
//  used to mark a node as erroneous.
//  Its kind is Iir_Kind_Error.
    const Iir Error_Mark;

// Some well know values declared in the STANDARD package.
// These values (except time_base) *must* not be modified, and are set by
// create_std_standard_package.
// Time_base is the base unit of time.  It is set during the creation of
// all these nodes, and can be modified only *immediatly* after.

    Iir_Unit_Declaration* Time_Base = nullptr;

    Iir_Design_File* Std_Standard_File = nullptr;
    Iir_Design_Unit* Std_Standard_Unit = nullptr;
    Iir_Package_Declaration* Standard_Package = nullptr;

// Boolean values.
    Iir_Type_Declaration* Boolean_Type_Declaration = nullptr;
    Iir_Enumeration_Type_Definition* Boolean_Type_Definition = nullptr;
    Iir_Enumeration_Literal* Boolean_False = nullptr;
    Iir_Enumeration_Literal* Boolean_True = nullptr;

// Bit values.
    Iir_Type_Declaration* Bit_Type_Declaration = nullptr;
    Iir_Enumeration_Type_Definition* Bit_Type_Definition = nullptr;
    Iir_Enumeration_Literal* Bit_0 = nullptr;
    Iir_Enumeration_Literal* Bit_1 = nullptr;

// Predefined character.
    Iir_Type_Declaration* Character_Type_Declaration = nullptr;
    Iir_Enumeration_Type_Definition* Character_Type_Definition = nullptr;

// severity level.
    Iir_Type_Declaration* Severity_Level_Type_Declaration = nullptr;
    Iir_Enumeration_Type_Definition* Severity_Level_Type_Definition = nullptr;
    Iir_Enumeration_Literal* Severity_Level_Note = nullptr;
    Iir_Enumeration_Literal* Severity_Level_Warning = nullptr;
    Iir_Enumeration_Literal* Severity_Level_Error = nullptr;
    Iir_Enumeration_Literal* Severity_Level_Failure = nullptr;

// Universal types.
    Iir_Anonymous_Type_Declaration* Universal_Integer_Type_Declaration;
    Iir_Integer_Type_Definition* Universal_Integer_Type_Definition;
    Iir_Subtype_Declaration* Universal_Integer_Subtype_Declaration;
    Iir_Integer_Subtype_Definition* Universal_Integer_Subtype_Definition;

    Iir_Integer_Type_Definition* Universal_Integer_Type_Definition;
    Iir_Floating_Type_Definition* Universal_Real_Type_Definition;

    Iir_Integer_Type_Definition* Convertible_Integer_Type_Definition;
    Iir_Floating_Type_Definition* Convertible_Real_Type_Definition;

    Iir_Integer_Literal* Universal_Integer_One;

    Iir_Anonymous_Type_Declaration* Universal_Real_Type_Declaration;
    Iir_Floating_Type_Definition* Universal_Real_Type_Definition;
    Iir_Subtype_Declaration* Universal_Real_Subtype_Declaration;
    Iir_Floating_Subtype_Definition* Universal_Real_Subtype_Definition;

// Predefined integer type.
    Iir_Anonymous_Type_Declaration* Integer_Type_Declaration;
    Iir_Integer_Type_Definition* Integer_Type_Definition;
    Iir_Subtype_Declaration* Integer_Subtype_Declaration;
    Iir_Integer_Subtype_Definition* Integer_Subtype_Definition;

//  Type used when the type of an expression is incorrect.
    Iir_Error* Error_Type;

// Predefined real type.
    Iir_Anonymous_Type_Declaration* Real_Type_Declaration;
    Iir_Floating_Type_Definition* Real_Type_Definition;
    Iir_Subtype_Declaration* Real_Subtype_Declaration;
    Iir_Floating_Subtype_Definition* Real_Subtype_Definition;

// Predefined natural subtype.
    Iir_Subtype_Declaration* Natural_Subtype_Declaration;
    Iir_Integer_Subtype_Definition* Natural_Subtype_Definition;

// Predefined positive subtype.
    Iir_Subtype_Declaration* Positive_Subtype_Declaration;
    Iir_Integer_Subtype_Definition* Positive_Subtype_Definition;

// Predefined positive subtype.
    Iir_Type_Declaration* String_Type_Declaration;
    Iir_Array_Type_Definition* String_Type_Definition;

// Predefined positive subtype.
    Iir_Type_Declaration* Bit_Vector_Type_Declaration;
    Iir_Array_Type_Definition* Bit_Vector_Type_Definition;

// predefined time subtype
    Iir_Anonymous_Type_Declaration* Time_Type_Declaration;
    Iir_Physical_Type_Definition* Time_Type_Definition;
    Iir_Physical_Subtype_Definition* Time_Subtype_Definition;
    Iir_Subtype_Declaration* Time_Subtype_Declaration;

//  For VHDL-93
    Iir_Physical_Subtype_Definition* Delay_Length_Subtype_Definition;
    Iir_Subtype_Declaration* Delay_Length_Subtype_Declaration;

//  For VHDL-93:
//  type File_Open_Kind
    Iir_Type_Declaration* File_Open_Kind_Type_Declaration;
    Iir_Enumeration_Type_Definition* File_Open_Kind_Type_Definition;
    Iir_Enumeration_Literal* File_Open_Kind_Read_Mode;
    Iir_Enumeration_Literal* File_Open_Kind_Write_Mode;
    Iir_Enumeration_Literal* File_Open_Kind_Append_Mode;

//  For VHDL-93:
//  type File_Open_Status
    Iir_Type_Declaration* File_Open_Status_Type_Declaration;
    Iir_Enumeration_Type_Definition* File_Open_Status_Type_Definition;
    Iir_Enumeration_Literal* File_Open_Status_Open_Ok;
    Iir_Enumeration_Literal* File_Open_Status_Status_Error;
    Iir_Enumeration_Literal* File_Open_Status_Name_Error;
    Iir_Enumeration_Literal* File_Open_Status_Mode_Error;

//  For VHDL-93:
   //FIXME: string //    atribute foreign ;
            Iir_Attribute_Declaration* Foreign_Attribute;

//  For VHDL-08
    Iir_Array_Type_Definition Boolean_Vector_Type_Definition;
    Iir_Type_Declaration Boolean_Vector_Type_Declaration;

    Iir_Array_Type_Definition Integer_Vector_Type_Definition;
    Iir_Type_Declaration Integer_Vector_Type_Declaration;

    Iir_Array_Type_Definition Real_Vector_Type_Definition;
    Iir_Type_Declaration Real_Vector_Type_Declaration;

    Iir_Array_Type_Definition Time_Vector_Type_Definition;
    Iir_Type_Declaration Time_Vector_Type_Declaration;

//  Internal use only.
//  These types should be considered like universal types, but
//  furthermore, they can be converted to any integer/real types while
//  universal cannot.
    Iir_Integer_Type_Definition* Convertible_Integer_Type_Definition;
    Iir_Floating_Type_Definition* Convertible_Real_Type_Definition;
    Iir_Anonymous_Type_Declaration* Convertible_Integer_Type_Declaration;
    Iir_Anonymous_Type_Declaration* Convertible_Real_Type_Declaration;

    Iir_Integer_Subtype_Definition* Convertible_Integer_Subtype_Definition;
    Iir_Subtype_Declaration* Convertible_Integer_Subtype_Declaration;

//  Wilcard types.
//  Err, we break privacy for iir numbers, but this allow use of them in
//  case statements.
    const Iir Wildcard_Any_Type = 7;
    const Iir Wildcard_Any_Aggregate_Type = 8;
    const Iir Wildcard_Any_String_Type = 9;
    const Iir Wildcard_Any_Access_Type = 10;

//  Subtype for all wildcard types, so that missing choice can be detected
//  at compilation time.
/*
    subtype Iir_Wildcard_Types
    is Iir
    range 7 .. 10;
*/
//  Chain of wildcard declarations, to own the nodes.
    std::vector<Iir*> Wildcard_Type_Declaration_Chain;

//  Create the first well-known nodes.
    void Create_First_Nodes();

//  Create the node for the standard package.
    void Create_Std_Standard_Package(Iir_Library_Declaration Parent);

};

#endif //AVLC_STANDARDPACKAGE_H
