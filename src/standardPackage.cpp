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
#include <iir.h>
#include <iir_utils.h>
#include "standardPackage.h"
#include "Keywords.h"

using namespace standardPackage;

//  For speed reasons, some often used nodes are hard-coded.
const Iir Error_Mark = 2;
Iir_Staticness Time_Staticness;
Options options;

template<typename T>
inline T* Create_Std_Decl() {
    T* Res = new T;
    Res->Parent = Standard_Package;
    return Res;
}

inline Iir_Type_Declaration* Create_Std_Type(Iir_Type_Declarator_Abs* Def, std::string Name) {
    auto Decl = Create_Std_Decl<Iir_Type_Declaration>();
    Decl->Identifier = Name;
    Decl->Visible_Flag = true;
    Decl->Type_Definition = Def;
    Standard_Package->Declaration_Chain.push_back(Decl);
    Def->Type_Declarator = Decl;
    return Decl;
}

template<typename T>
Iir_Anonymous_Type_Declaration* Create_Integer_Type(T* Type_Definition, std::string Type_Name) {
    Type_Definition = new T;
    Type_Definition->Base_Type = Type_Definition;

    Type_Definition->Type_Staticness = Iir_Staticness::Locally;
    Type_Definition->Signal_Type_Flag = true;
    Type_Definition->Has_Signal_Flag = !options.Flag_Whole_Analyze;

    auto Type_Decl = Create_Std_Decl<Iir_Anonymous_Type_Declaration>();
    Type_Decl->Identifier = Type_Name;
    Type_Decl->Type_Definition = Type_Definition;
    Type_Definition->Type_Declarator = Type_Decl;
    return Type_Decl;
}

Iir_Enumeration_Literal* Create_Std_Literal(std::string Name, Iir_Enumeration_Type_Definition* Sub_Type) {

    auto result = Create_Std_Decl<Iir_Enumeration_Literal>();
    result->Identifier = Name;
    result->Visible_Flag = true;
    result->Type = Sub_Type;
    result->Expr_Staticness = Iir_Staticness::Locally;
    result->Name_Staticness = Iir_Staticness::Locally;
    result->Enum_Pos = Sub_Type->Enumeration_Literal_List.size() - 1;

    //TODO: Sem.Compute_Subprogram_Hash (Res);
    Sub_Type->Enumeration_Literal_List.push_back(result);
    return result;
}

//  Create:
//  function NAME (signal S : I inter_type) return BOOLEAN;
inline void Create_Edge_Function(std::string Name, Iir_Predefined_Functions Func, Iir* Inter_Type) {
//FIXME: Inter : Iir_Interface_Constant_Declaration;
    auto Decl = Create_Std_Decl<Iir_Function_Declaration>();
    Decl->Identifier = Name;
    Decl->Visible_Flag = true;
    Decl->Return_Type = Boolean_Type_Definition;
    Decl->Pure_Flag = true;
    //FIXME: nexxt line
    Decl->Implicit_Definition = Func;

    auto Inter = new Iir_Interface_Signal_Declaration;
    Inter->Identifier = "S";
    Inter->Type = Inter_Type;
    Inter->Mode = Iir_In_Mode;
    Decl->Interface_Declaration_Chain = Inter;

    //TODO: Sem.Compute_Subprogram_Hash (Decl);
    Standard_Package->Declaration_Chain.push_back(Decl);
}

std::pair<Iir_Integer_Subtype_Definition*, Iir_Subtype_Declaration>
Create_Integer_Subtype(Iir* Type_Definition, Iir* Type_Decl, bool Is_64) {
    auto Subtype_Definition = new Iir_Integer_Subtype_Definition;
    Subtype_Definition->Base_Type = Type_Definition;
    auto Constraint = Create_Std_Range_Expr(Create_Std_Integer(Low_Bound(Is_64), Universal_Integer_Type_Definition),
            Create_Std_Integer(High_Bound(Is_64), Universal_Integer_Type_Definition),
            Universal_Integer_Type_Definition);
    Subtype_Definition->Range_Constraint = Constraint;
    Subtype_Definition->Type_Staticness = Iir_Staticness::Locally;
    Subtype_Definition->Signal_Type_Flag = true;
    Subtype_Definition->Has_Signal_Flag = not options.Flag_Whole_Analyze;

//  subtype is
    auto Subtype_Decl = Create_Std_Decl<Iir_Subtype_Declaration>();
    Subtype_Decl->Identifier = Type_Decl->Identifier;
    Subtype_Decl->Visible_Flag = true;
    Subtype_Decl->Type = Subtype_Definition;
    Subtype_Decl->Subtype_Indication = Subtype_Definition;
    Subtype_Definition->Type_Declarator = Subtype_Decl;
    Type_Decl->Subtype_Definition = Subtype_Definition;
    return std::make_pair(Subtype_Definition, Subtype_Decl);
}

inline Iir_Integer_Literal* Create_Std_Integer(Iir_Int64 Val, Iir* Lit_Type) {
    auto Res = new Iir_Integer_Literal;
    Res->Value = Val;
    Res->Type = Lit_Type;
    Res->Expr_Staticness = Iir_Staticness::Locally;
    return Res;
}

Iir_Physical_Int_Literal* Create_Std_Phys_Lit(int64_t Value, Iir_Unit_Declaration* Unit) {
    auto Lit = new Iir_Physical_Int_Literal;
    Lit->Value = Value;
    Lit->Physical_Unit = Unit;
    Lit->Type = Time_Type_Definition;
    Lit->Expr_Staticness = Time_Staticness;
    return Lit;
}

Iir_Unit_Declaration* Create_Unit(int64_t Multiplier_Value, Iir_Unit_Declaration* Multiplier, const std::string& Name) {
    auto Unit = Create_Std_Decl<Iir_Unit_Declaration>();
    Unit->Identifier = Name;
    Unit->Visible_Flag = true;
    Unit->Type = Time_Type_Definition;

    auto Lit1 = Create_Std_Phys_Lit(Multiplier_Value, Multiplier);
    auto Lit = Create_Std_Phys_Lit(Multiplier_Value * Multiplier->Physical_Literal->Value,
            Multiplier->Physical_Literal->Physical_unit);
    Lit->Literal_Origin = Lit1;
    Unit->Physical_Literal = Lit;

    Unit->Expr_Staticness = Time_Staticness;
    Unit->Name_Staticness = Iir_Staticness::Locally;
    Time_Type_Definition->Unit_Chain.push_back(Unit);
};


void Create_Wildcard_Type (Iir* Def, std::string Name ) {
    auto Decl = Create_Std_Decl<Iir_Type_Declaration>();
    Decl->Identifier = Name;
    Def->Base_Type = Def;
    Def->Type_Staticness = Iir_Staticness::None;
    Decl->Type_Definition = Def;
    Def->Type_Declarator = Decl;

    Wildcard_Type_Declaration_Chain.push_back(Decl);
    Decl->Chain = Wildcard_Type_Declaration_Chain.back();
}

//  Create:
//  function TO_STRING (VALUE: inter_type) return STRING;
void Create_To_String(Iir* Inter_Type, Iir_Predefined_Functions Imp, std::string Name = Keywords::Name_To_String,
        std::string Inter2_Id = "", Iir* Inter2_Type = nullptr) {
    auto Decl = Create_Std_Decl<Iir_Function_Declaration>();
    Decl->Identifier = Name;
    Decl->Visible_Flag = true;
    Decl->Return_Type = String_Type_Definition;
    Decl->Pure_Flag = true;
    Decl->Implicit_Definition = Imp;

    auto Inter = new Iir_Interface_Constant_Declaration;
    Inter->Identifier = Keywords::Name_Value;
    Inter->Type = Inter_Type;
    Inter->Mode = Iir_In_Mode;
    Decl->Interface_Declaration_Chain = Inter;

    if (Inter2_Id.empty()) {
        auto Inter2 = new Iir_Interface_Constant_Declaration;
        Inter2->Identifier = Inter2_Id;
        Inter2->Type = Inter2_Type;
        Inter2->Mode = Iir_In_Mode;
        Inter->Chain = Inter2;
    }

//FIXME: Sem.Compute_Subprogram_Hash (Decl);
    Standard_Package->Declaration_Chain.push_back(Decl);
}

Iir_Simple_Name* Create_Std_Type_Mark (Iir_Type_Abs* Ref) {
    auto Res = Build_Simple_Name(Ref, {0, 0});
    Res->Type = Ref->Type;
    return Res;
}

//  Create an array of EL_TYPE, indexed by Natural.
std::pair<Iir_Array_Type_Definition*, Iir_Type_Declaration*>
Create_Array_Type(Iir* El_Decl, std::string Name) {
    auto Element = Create_Std_Type_Mark(El_Decl);
    auto Index = Create_Std_Type_Mark(Natural_Subtype_Declaration);

    auto Def = new Iir_Array_Type_Definition;
    Def->Base_Type = Def;

    Def->Index_Subtype_Definition_List.push_back(Index);
    Def->Index_Subtype_List.push_back(Index);

    Def->Element_Subtype_Indication = Element;
    Def->Element_Subtype = El_Decl->Type;
    Def->Type_Staticness = Iir_Staticness::None;
    Def->Signal_Type_Flag = true;
    Def->Has_Signal_Flag = !options.Flag_Whole_Analyze;

    auto Decl = Create_Std_Type(Def, Name);

    //FIXME: Add_Implicit_Operations(Decl);

    return std::make_pair(Def, Decl);
}


Iir_Range_Expression* Create_Std_Range_Expr (Iir* Left,Iir* Right, Iir* Rtype) {
    auto Res = new Iir_Range_Expression;
    Res->Left_Limit = Left;
    Res->Left_Limit_Expr = Left;
    Res->Direction = Iir_To;
    Res->Right_Limit = Right;
    Res->Right_Limit_Expr = Right;
    Res->Expr_Staticness = Iir_Staticness::Locally;
    Res->Type = Rtype;
    return Res;
}

void create_options(Iir_Library_Declaration* Parent) {
/*
type Bound_Array is array (Boolean) of Iir_Int64;
Low_Bound : constant Bound_Array = (False => -(2 ** 31),
        true => -(2 ** 63));
High_Bound : constant Bound_Array = (False => (2 ** 31) - 1,
        true => (2 ** 63) - 1);

Std_Location: Location_Type = Location_Nil;
Std_Filename : std::string = Null_Identifier;

procedure Create_First_Nodes
is
        procedure Create_Known_Iir (Kind : Iir_Kind; Val : Iir) is
        begin
if Create_Std_Iir (Kind) /= Val then
raise Internal_Error;
end if;
end Create_Known_Iir;
begin
        Std_Filename = Name_Table.Get_Identifier ("*std_standard*");
Std_Location = Files_Map.Source_File_To_Location
        (Files_Map.Create_Virtual_Source_File (Std_Filename));

if Create_Iir_Error /= Error_Mark then
raise Internal_Error;
end if;
Set_Location (Error_Mark, Std_Location);

Create_Known_Iir (Iir_Integer_Type_Definition,
        Universal_Integer_Type_Definition);
Create_Known_Iir (Iir_Floating_Type_Definition,
        Universal_Real_Type_Definition);

Create_Known_Iir (Iir_Integer_Type_Definition,
        Convertible_Integer_Type_Definition);
Create_Known_Iir (Iir_Floating_Type_Definition,
        Convertible_Real_Type_Definition);

Create_Known_Iir (Iir_Wildcard_Type_Definition,
        Wildcard_Any_Type);
Create_Known_Iir (Iir_Wildcard_Type_Definition,
        Wildcard_Any_Aggregate_Type);
Create_Known_Iir (Iir_Wildcard_Type_Definition,
        Wildcard_Any_String_Type);
Create_Known_Iir (Iir_Wildcard_Type_Definition,
        Wildcard_Any_Access_Type);
end Create_First_Nodes;

procedure Create_Std_Standard_Package (Parent : Iir_Library_Declaration)
is
        function Get_Std_Character (Char: Character) return std::string
        renames Name_Table.Get_Identifier;


function Create_Std_Fp (Val : Iir_Fp64; Lit_Type : Iir)
return Iir_Floating_Point_Literal
        is
Res : Iir_Floating_Point_Literal;
begin
        Res = Create_Std_Iir (Iir_Floating_Point_Literal);
Set_Fp_Value (Res, Val);
Set_Type (Res, Lit_Type);
Set_Expr_Staticness (Res, Locally);
return Res;
end Create_Std_Fp;

procedure Add_Implicit_Operations (Decl : Iir)
is
        Nxt : Iir;
begin
        Sem_Decls.Create_Implicit_Operations (Decl, true);

//  Update Last_Decl
loop
        Nxt = Get_Chain (Last_Decl);
exit when Nxt = nullptr;
Last_Decl = Nxt;
end loop;
end Add_Implicit_Operations;

//  Find implicit declaration of "**" for type declaration TYPE_DECL
//  and append it at the current end of std_package.
procedure Relocate_Exp_At_End (Type_Decl : Iir)
is
        Prev_El, El : Iir;
begin
        pragma Assert
(Get_Kind (Type_Decl) = Iir_Anonymous_Type_Declaration);
El = Type_Decl;
loop
        Prev_El = El;
El = Get_Chain (El);
pragma Assert (Get_Kind (El) = Iir_Function_Declaration);
exit when
Get_Implicit_Definition (El) = Iir_Predefined_Integer_Exp;
exit when
Get_Implicit_Definition (El) = Iir_Predefined_Floating_Exp;
end loop;

//  EL must not be the last element, otherwise Add_Decl will break
//  the chain.
pragma Assert (Is_Valid (Get_Chain (El)));

//  Remove from the chain.
Set_Chain (Prev_El, Get_Chain (El));
Set_Chain (El, nullptr);

//  Append.
Standard_Package->Declaration_Chain.push_back(El);
end Relocate_Exp_At_End;

begin
 */
//  Create design file.
    auto Std_Standard_File = new Iir_Design_File;
    Std_Standard_File->Library = Parent;
    Std_Standard_File->Design_File_Filename = "*std_standard*";
    //NOTE: Std_Time_Stamp : constant Time_Stamp_String = "20020601000000.000";
    Std_Standard_File->Analysis_Time_Stamp = "beggining of time";

//  Create design unit.
    Iir_Design_Unit* Std_Standard_Unit = new Iir_Design_Unit;
    Std_Standard_Unit->Identifier = Keywords::Name_Standard;
    Std_Standard_File->Design_Units.push_back(Std_Standard_Unit);
    Std_Standard_Unit->Design_File = Std_Standard_File;
    Std_Standard_Unit->Date_State = Date_Analyze;
    Std_Standard_Unit->Date = 10;

// Adding "package STANDARD is"
    auto Standard_Package = new Iir_Package_Declaration;
    Standard_Package->Identifier = Keywords::Name_Standard;
    Standard_Package->Visible_Flag = true;
    Standard_Package->Need_Body = false;

    Std_Standard_Unit->Library_Unit = Standard_Package;
    Standard_Package->Parent_Design_Unit = Std_Standard_Unit;

// boolean
    {
// (false, true)
        Boolean_Type_Definition = new Iir_Enumeration_Type_Definition;
        Boolean_Type_Definition->Base_Type = Boolean_Type_Definition;

        Boolean_False = Create_Std_Literal(Keywords::Name_False, Boolean_Type_Definition);
        Boolean_True = Create_Std_Literal(Keywords::Name_True, Boolean_Type_Definition);
        Boolean_Type_Definition->Type_Staticness = Iir_Staticness::Locally;
        Boolean_Type_Definition->Signal_Type_Flag = true;
        Boolean_Type_Definition->Has_Signal_Flag = !options.Flag_Whole_Analyze;

// type boolean is
        Boolean_Type_Declaration = Create_Std_Type(Boolean_Type_Definition, Keywords::Name_Boolean);

        Create_Range_Constraint_For_Enumeration_Type(*Boolean_Type_Definition);
        //TODO:        Add_Implicit_Operations(Boolean_Type_Declaration);
    }

    if (options.standard >= Vhdl_Std::Vhdl_08) {
//  Rising_Edge and Falling_Edge
        Create_Edge_Function(Keywords::Name_Rising_Edge, Iir_Predefined_Boolean_Rising_Edge, Boolean_Type_Definition);
        Create_Edge_Function(Keywords::Name_Falling_Edge, Iir_Predefined_Boolean_Falling_Edge, Boolean_Type_Definition);
    }

// bit.
    {
// ('0', '1')
        Bit_Type_Definition = new Iir_Enumeration_Type_Definition;
        Bit_Type_Definition->Base_Type = Bit_Type_Definition;
        Bit_0 = Create_Std_Literal("0", Bit_Type_Definition);
        Bit_1 = Create_Std_Literal("1", Bit_Type_Definition);
        Bit_Type_Definition->Type_Staticness = Iir_Staticness::Locally;
        Bit_Type_Definition->Signal_Type_Flag = true;
        Bit_Type_Definition->Has_Signal_Flag = !options.Flag_Whole_Analyze;
        Bit_Type_Definition->Only_Characters_Flag = true;

// type bit is
        Bit_Type_Declaration = Create_Std_Type(Bit_Type_Definition, Keywords::Name_Bit);

        Create_Range_Constraint_For_Enumeration_Type(*Bit_Type_Definition);
        //TODO:        Add_Implicit_Operations(Bit_Type_Declaration);
    }

    if (options.standard >= Vhdl_Std::Vhdl_08) {
//  Rising_Edge and Falling_Edge
        Create_Edge_Function(Keywords::Name_Rising_Edge, Iir_Predefined_Bit_Rising_Edge, Bit_Type_Definition);
        Create_Edge_Function(Keywords::Name_Falling_Edge, Iir_Predefined_Bit_Falling_Edge, Bit_Type_Definition);
    }

// characters.
    {
        Character_Type_Definition = new Iir_Enumeration_Type_Definition;
        Character_Type_Definition->Base_Type = Character_Type_Definition;

        for (auto i = Keywords::KeywordTable.find(Keywords::Name_Nul);
                i <= Keywords::KeywordTable.find(Keywords::Name_Usp); ++i) {
            auto element = Create_Std_Literal(i->first, Character_Type_Declaration);
        }

        for (auto i = ' '; i <= '~'; ++i) {
            auto element = Create_Std_Literal(std::string(1, i), Character_Type_Declaration);
        }

        auto element = Create_Std_Literal(Keywords::Name_Del, Character_Type_Definition);

        if (options.standard != Vhdl_Std::Vhdl_87) {
            for (auto i = Keywords::KeywordTable.find(Keywords::Name_C128);
                    i <= Keywords::KeywordTable.find(Keywords::Name_C159); ++i) {
                auto element = Create_Std_Literal(i->first, Character_Type_Declaration);
            }
            // Because char will not fit
            for (unsigned int i = 160; i <= 255; ++i) {
                auto element = Create_Std_Literal(std::string(1, char(i)), Character_Type_Declaration);
            }
        }

        Character_Type_Definition->Type_Staticness = Iir_Staticness::Locally;
        Character_Type_Definition->Signal_Type_Flag = true;
        Character_Type_Definition->Has_Signal_Flag = !options.Flag_Whole_Analyze;

// type character is
        Character_Type_Declaration = Create_Std_Type(Character_Type_Definition, Keywords::Name_Character);

        Create_Range_Constraint_For_Enumeration_Type(*Character_Type_Definition);
        //TODO: Add_Implicit_Operations(Character_Type_Declaration);

    }
// severity level.
    {
// (note, warning, error, failure)
        Severity_Level_Type_Definition = new Iir_Enumeration_Type_Definition;
        Severity_Level_Type_Definition->Base_Type = Severity_Level_Type_Definition;

        Severity_Level_Note = Create_Std_Literal(Keywords::Name_Note, Severity_Level_Type_Definition);
        Severity_Level_Warning = Create_Std_Literal(Keywords::Name_Warning, Severity_Level_Type_Definition);
        Severity_Level_Error = Create_Std_Literal(Keywords::Name_Error, Severity_Level_Type_Definition);
        Severity_Level_Failure = Create_Std_Literal(Keywords::Name_Failure, Severity_Level_Type_Definition);

        Severity_Level_Type_Definition->Type_Staticness = Iir_Staticness::Locally;
        Severity_Level_Type_Definition->Signal_Type_Flag = true;
        Severity_Level_Type_Definition->Has_Signal_Flag = !options.Flag_Whole_Analyze;

// type severity_level is
        Severity_Level_Type_Declaration = Create_Std_Type(Severity_Level_Type_Definition,
                Keywords::Name_Severity_Level);

        Create_Range_Constraint_For_Enumeration_Type(*Severity_Level_Type_Definition);
        //TODO:        Add_Implicit_Operations(Severity_Level_Type_Declaration);
    }

// universal integer
    {
        Universal_Integer_Type_Declaration = Create_Integer_Type<Iir_Integer_Type_Definition>(
                Universal_Integer_Type_Definition, Keywords::Name_Universal_Integer);
        Standard_Package->Declaration_Chain.push_back(Universal_Integer_Type_Declaration);

        [Universal_Integer_Subtype_Definition, Universal_Integer_Subtype_Declaration] = Create_Integer_Subtype(
                Universal_Integer_Type_Definition,
                Universal_Integer_Type_Declaration,
                options.Flag_Time_64 || options.Flag_Integer_64);

        Standard_Package->Declaration_Chain.push_back(Universal_Integer_Subtype_Declaration);
        Universal_Integer_Type_Declaration->Subtype_Definition = Universal_Integer_Subtype_Definition;
//  Do not create implicit operations yet, since "**" needs integer
//  type.
    }

//  Universal integer constant 1.
    Universal_Integer_Type_Definition = new Iir_Integer_Type_Definition;
    Universal_Integer_One = Create_Std_Integer(1, Universal_Integer_Type_Definition);

// Universal real.
    {
//        Constraint : Iir_Range_Expression;

        Universal_Real_Type_Definition = new Iir_Floating_Type_Definition;

        Universal_Real_Type_Definition->Base_Type = Universal_Real_Type_Definition;
        Universal_Real_Type_Definition->Type_Staticness = Iir_Staticness::Locally;
        Universal_Real_Type_Definition->Signal_Type_Flag = true;
        Universal_Real_Type_Definition->Has_Signal_Flag = false;

//  type universal_real is ...
        Universal_Real_Type_Declaration = Create_Std_Decl<Iir_Anonymous_Type_Declaration>();
        Universal_Real_Type_Declaration->Identifier = Keywords::Name_Universal_Real;
        Universal_Real_Type_Declaration->Type_Definition = Universal_Real_Type_Definition;
        Universal_Real_Type_Definition->Type_Declarator = Universal_Real_Type_Declaration;
        Standard_Package->Declaration_Chain.push_back(Universal_Real_Type_Declaration);

        Universal_Real_Subtype_Definition = new Iir_Floating_Subtype_Definition;
        Universal_Real_Subtype_Definition->Base_Type = Universal_Real_Subtype_Definition;

 auto Constraint = Create_Std_Range_Expr
        (Create_Std_Fp (Iir_Fp64'First, Universal_Real_Type_Definition),
Create_Std_Fp (Iir_Fp64'Last, Universal_Real_Type_Definition),
Universal_Real_Type_Definition);

        Universal_Real_Subtype_Definition->Range_Constraint = Constraint;
        Universal_Real_Subtype_Definition->Type_Staticness = Iir_Staticness::Locally;
        Universal_Real_Subtype_Definition->Signal_Type_Flag = true;
        Universal_Real_Subtype_Definition->Has_Signal_Flag = false;

//  subtype universal_real is ...
        Universal_Real_Subtype_Declaration = Create_Std_Decl<Iir_Subtype_Declaration>();
        Universal_Real_Subtype_Declaration->Identifier = Keywords::Name_Universal_Real;
        Universal_Real_Subtype_Declaration->Type = Universal_Real_Subtype_Definition;
        Universal_Real_Subtype_Declaration->Subtype_Indication = Universal_Real_Subtype_Definition;
        Universal_Real_Subtype_Definition->Type_Declarator = Universal_Real_Subtype_Declaration;
        Universal_Real_Type_Declaration->Subtype_Definition = Universal_Real_Subtype_Definition;

        Standard_Package->Declaration_Chain.push_back(Universal_Real_Subtype_Declaration);

//  Do not create implicit operations yet, since "**" needs integer
//  type.
    }

// Convertible type.
    {
        Convertible_Integer_Type_Declaration = Create_Integer_Type(Convertible_Integer_Type_Definition,
                Keywords::Name_Convertible_Integer);
        [Convertible_Integer_Subtype_Definition, Convertible_Integer_Subtype_Declaration] =
                Create_Integer_Subtype(Convertible_Integer_Type_Definition, Convertible_Integer_Type_Declaration,
                        options.Flag_Time_64 or options.Flag_Integer_64);

//  Not added in std.standard.
    }

    {
        Convertible_Real_Type_Definition = new Iir_Floating_Type_Definition;
        Convertible_Real_Type_Definition->Base_Type = Convertible_Real_Type_Definition;
        Convertible_Real_Type_Definition->Type_Staticness = Iir_Staticness::Locally;
        Convertible_Real_Type_Definition->Signal_Type_Flag = true;
        Convertible_Real_Type_Definition->Has_Signal_Flag = false;

        Convertible_Real_Type_Declaration = Create_Std_Decl<Iir_Anonymous_Type_Declaration>();
        Convertible_Real_Type_Declaration->Identifier = Keywords::Name_Convertible_Real;
        Convertible_Real_Type_Declaration->Type_Definition = Convertible_Real_Type_Definition;
        Convertible_Real_Type_Definition->Type_Declarator = Convertible_Real_Type_Declaration;
    }

// integer type.
    {
        Integer_Type_Declaration = Create_Integer_Type(Integer_Type_Definition, Keywords::Name_Integer);
        Standard_Package->Declaration_Chain.push_back(Integer_Type_Declaration);

//  Now that Integer is declared, create operations for universal
//  types.  They will be inserted just after the type declaration,
//  but cannot be done before as "**" relies on Integer.
        //TODO: Add_Implicit_Operations(Universal_Integer_Type_Declaration);
        //TODO: Add_Implicit_Operations(Universal_Real_Type_Declaration);

//  Don't define "**" for universal types before the declaration of
//  Integer, so move them.
        //FIXME: Relocate_Exp_At_End(Universal_Integer_Type_Declaration);
        //FIXME: Relocate_Exp_At_End(Universal_Real_Type_Declaration);

        //TODO: Add_Implicit_Operations(Integer_Type_Declaration);

        [Integer_Subtype_Definition,
                Integer_Subtype_Declaration] = Create_Integer_Subtype(Integer_Type_Definition,
                Integer_Type_Declaration,
                options.Flag_Integer_64);
        Standard_Package->Declaration_Chain.push_back(Integer_Subtype_Declaration);
    }

// Real type.
    {
        Real_Type_Definition = new Iir_Floating_Type_Definition;
        Real_Type_Definition->Base_Type = Real_Type_Definition;
        Real_Type_Definition->Type_Staticness = Iir_Staticness::Locally;
        Real_Type_Definition->Signal_Type_Flag = true;
        Real_Type_Definition->Has_Signal_Flag = !options.Flag_Whole_Analyze;

        Real_Type_Declaration = Create_Std_Decl<Iir_Anonymous_Type_Declaration>();
        Real_Type_Declaration->Identifier = Keywords::Name_Real;
        Real_Type_Declaration->Type_Definition = Real_Type_Definition;
        Real_Type_Definition->Type_Declarator = Real_Type_Declaration;
        Standard_Package->Declaration_Chain.push_back(Real_Type_Declaration);

//TODO: Add_Implicit_Operations (Real_Type_Declaration);

        Real_Subtype_Definition = new Iir_Floating_Subtype_Definition;
        Real_Subtype_Definition->Base_Type = Real_Type_Definition;
        auto Constraint = Create_Std_Range_Expr(Create_Std_Fp(Iir_Fp64
        'First, Universal_Real_Type_Definition),
        Create_Std_Fp(Iir_Fp64
        'Last, Universal_Real_Type_Definition), Universal_Real_Type_Definition);
        Real_Subtype_Definition->Range_Constraint = Constraint;
        Real_Subtype_Definition->Type_Staticness = Iir_Staticness::Locally;
        Real_Subtype_Definition->Signal_Type_Flag = true;
        Real_Subtype_Definition->Has_Signal_Flag = not options.Flag_Whole_Analyze;

        Real_Subtype_Declaration = Create_Std_Decl<Iir_Subtype_Declaration>();
        Real_Subtype_Declaration->Identifier = Keywords::Name_Real;
        Real_Subtype_Declaration->Visible_Flag = true;
        Real_Subtype_Declaration->Type = Real_Subtype_Definition;
        Real_Subtype_Declaration->Subtype_Indication = Real_Subtype_Definition;
        Real_Subtype_Definition->Type_Declarator = Real_Subtype_Declaration;
        Standard_Package->Declaration_Chain.push_back(Real_Subtype_Declaration);

        Real_Type_Declaration->Subtype_Definition = Real_Subtype_Definition;
    }

// time definition
    {
        /*
    declare

    Last_Unit : Iir_Unit_Declaration;
    use Iir_Chains.Unit_Chain_Handling;



    Constraint : Iir_Range_Expression;
    begin
         */

        Iir_Unit_Declaration* Time_Fs_Unit;
        Iir_Unit_Declaration* Time_Ps_Unit;
        Iir_Unit_Declaration* Time_Ns_Unit;
        Iir_Unit_Declaration* Time_Us_Unit;
        Iir_Unit_Declaration* Time_Ms_Unit;
        Iir_Unit_Declaration* Time_Sec_Unit;
        Iir_Unit_Declaration* Time_Min_Unit;
        Iir_Unit_Declaration* Time_Hr_Unit;

        if (options.standard >= Vhdl_Std::Vhdl_93c)
            Time_Staticness = Iir_Staticness::Globally;
        else
            Time_Staticness = Iir_Staticness::Locally;

        Time_Type_Definition = new Iir_Physical_Type_Definition;
        Time_Type_Definition->Base_Type = Time_Type_Definition;
        Time_Type_Definition->Type_Staticness = Iir_Staticness::Locally; //NOTE: Time_Staticness
        Time_Type_Definition->Signal_Type_Flag = true;
        Time_Type_Definition->Has_Signal_Flag = not options.Flag_Whole_Analyze;
        Time_Type_Definition->End_Has_Reserved_Id = true;

        Time_Fs_Unit = Create_Std_Decl<Iir_Unit_Declaration>();
        Time_Fs_Unit->Identifier = Keywords::Name_Fs;
        Time_Fs_Unit->Visible_Flag = true;
        Time_Fs_Unit->Type = Time_Type_Definition;
        Time_Fs_Unit->Expr_Staticness = Time_Staticness;
        Time_Fs_Unit->Name_Staticness = Iir_Staticness::Locally;
        Time_Fs_Unit->Physical_Literal = Create_Std_Phys_Lit(1, Time_Fs_Unit);
        Time_Type_Definition->Unit_Chain.push_back(Time_Fs_Unit);

        Time_Ps_Unit = Create_Unit(1000, Time_Fs_Unit, Keywords::Name_Ps);
        Time_Ns_Unit = Create_Unit(1000, Time_Ps_Unit, Keywords::Name_Ns);
        Time_Us_Unit = Create_Unit(1000, Time_Ns_Unit, Keywords::Name_Us);
        Time_Ms_Unit = Create_Unit(1000, Time_Us_Unit, Keywords::Name_Ms);
        Time_Sec_Unit = Create_Unit(1000, Time_Ms_Unit, Keywords::Name_Sec);
        Time_Min_Unit = Create_Unit(60, Time_Sec_Unit, Keywords::Name_Min);
        Time_Hr_Unit = Create_Unit(60, Time_Min_Unit, Keywords::Name_Hr);

//  type is
        Time_Type_Declaration = Create_Std_Decl<Iir_Anonymous_Type_Declaration>();
        Time_Type_Declaration->Identifier = Keywords::Name_Time;
        Time_Type_Declaration->Type_Definition = Time_Type_Definition;
        Time_Type_Definition->Type_Declarator = Time_Type_Declaration;
        Standard_Package->Declaration_Chain.push_back(Time_Type_Declaration);

//TODO: Add_Implicit_Operations (Time_Type_Declaration);

        Time_Subtype_Definition = new Iir_Physical_Subtype_Definition;
        auto Constraint = Create_Std_Range_Expr(Create_Std_Phys_Lit(Low_Bound(options.Flag_Time_64), Time_Fs_Unit),
                Create_Std_Phys_Lit(High_Bound(options.Flag_Time_64), Time_Fs_Unit), Time_Type_Definition);
        Time_Subtype_Definition->Range_Constraint = Constraint;
        Time_Subtype_Definition->Base_Type = Time_Type_Definition;
//Set_Subtype_Type_Mark (Time_Subtype_Definition,
//                       Time_Type_Definition);
        Time_Subtype_Definition->Type_Staticness = Time_Staticness;
        Time_Subtype_Definition->Signal_Type_Flag = true;
        Time_Subtype_Definition->Has_Signal_Flag = !options.Flag_Whole_Analyze;

//  subtype time is
        Time_Subtype_Declaration =
                Create_Std_Decl<Iir_Subtype_Declaration>();
        Time_Subtype_Declaration->Identifier = Keywords::Name_Time;
        Time_Subtype_Declaration->Visible_Flag = true;
        Time_Subtype_Declaration->Type = Time_Subtype_Definition;
        Time_Subtype_Declaration->Subtype_Indication = Time_Subtype_Definition;
        Time_Subtype_Definition->Type_Declarator = Time_Subtype_Declaration;
        Standard_Package->Declaration_Chain.push_back(Time_Subtype_Declaration);
        Time_Type_Declaration->Subtype_Definition = Time_Subtype_Definition;

// The default time base.
        switch (options.Time_Resolution) {
        case 'f':
            Time_Base = Time_Fs_Unit;
            break;
        case 'p':
            Time_Base = Time_Ps_Unit;
            break;
        case 'n':
            Time_Base = Time_Ns_Unit;
            break;
        case 'u':
            Time_Base = Time_Us_Unit;
            break;
        case 'm':
            Time_Base = Time_Ms_Unit;
            break;
        case 's':
            Time_Base = Time_Sec_Unit;
            break;
        case 'M':
            Time_Base = Time_Min_Unit;
            break;
        case 'h':
            Time_Base = Time_Hr_Unit;
            break;
        default:
            throw std::logic_error("Invalid time format");
        }

//  VHDL93
//  subtype DELAY_LENGTH is TIME range 0 to TIME'HIGH
        if (options.standard >= Vhdl_Std::Vhdl_93c) {
            Delay_Length_Subtype_Definition = new Iir_Physical_Subtype_Definition;
            Delay_Length_Subtype_Definition->Subtype_Type_Mark = Create_Std_Type_Mark(Time_Subtype_Declaration);
            auto Constraint = Create_Std_Range_Expr(Create_Std_Phys_Lit(0, Time_Fs_Unit),
                    Create_Std_Phys_Lit(High_Bound(options.Flag_Time_64), Time_Fs_Unit), Time_Type_Definition);
            Delay_Length_Subtype_Definition->Range_Constraint = Constraint;
            Delay_Length_Subtype_Definition->Base_Type = Time_Type_Definition;
            Delay_Length_Subtype_Definition->Type_Staticness = Time_Staticness;
            Delay_Length_Subtype_Definition->Signal_Type_Flag = true;
            Delay_Length_Subtype_Definition->Has_Signal_Flag = not options.Flag_Whole_Analyze;

//  subtype delay_length is ...
            Delay_Length_Subtype_Declaration = Create_Std_Decl<Iir_Subtype_Declaration>();
            Delay_Length_Subtype_Declaration->Identifier = Keywords::Name_Delay_Length;
            Delay_Length_Subtype_Declaration->Visible_Flag = true;
            Delay_Length_Subtype_Declaration->Type = Delay_Length_Subtype_Definition;
            Delay_Length_Subtype_Definition->Type_Declarator = Delay_Length_Subtype_Declaration;
            Delay_Length_Subtype_Declaration->Subtype_Indication = Delay_Length_Subtype_Definition;
            Standard_Package->Declaration_Chain.push_back(Delay_Length_Subtype_Declaration);
        }
        else {
            Delay_Length_Subtype_Definition = nullptr;
            Delay_Length_Subtype_Declaration = nullptr;
        }
    }

//  VHDL87:
//  function NOW return TIME
//
//  impure function NOW return DELAY_LENGTH.
    {
        auto Function_Now = Create_Std_Decl<Iir_Function_Declaration>();

        Function_Now->Identifier = Keywords::Name_Now;
        Function_Now->Visible_Flag = true;
        Function_Now->Visible_Flag = true;
        if (options.standard == Vhdl_Std::Vhdl_87)
            Function_Now->Return_Type = Time_Subtype_Definition;
        else
            Function_Now->Return_Type = Delay_Length_Subtype_Definition;

        if (options.standard == Vhdl_Std::Vhdl_02)
            Function_Now->Pure_Flag = true;
        else
            Function_Now->Pure_Flag = false;

        Function_Now->Implicit_Definition = Iir_Predefined_Now_Function;
        //TODO: Sem.Compute_Subprogram_Hash(Function_Now);
        Standard_Package->Declaration_Chain.push_back(Function_Now);
    }

// natural subtype
    {
        Natural_Subtype_Definition = new Iir_Integer_Subtype_Definition;
        Natural_Subtype_Definition->Base_Type = Integer_Type_Definition;
        Natural_Subtype_Definition->Subtype_Type_Mark = Create_Std_Type_Mark(Integer_Subtype_Declaration);
        auto Constraint = Create_Std_Range_Expr
                (Create_Std_Integer(0, Integer_Type_Definition),
                        Create_Std_Integer(High_Bound(options.Flag_Integer_64),
                                Integer_Type_Definition),
                        Integer_Type_Definition);
        Natural_Subtype_Definition->Range_Constraint = Constraint;
        Natural_Subtype_Definition->Type_Staticness = Iir_Staticness::Locally;
        Natural_Subtype_Definition->Signal_Type_Flag = true;
        Natural_Subtype_Definition->Has_Signal_Flag = not options.Flag_Whole_Analyze;

        Natural_Subtype_Declaration = Create_Std_Decl<Iir_Subtype_Declaration>();
        Natural_Subtype_Declaration->Identifier = Keywords::Name_Natural;
        Natural_Subtype_Declaration->Visible_Flag = true;
        Natural_Subtype_Declaration->Type = Natural_Subtype_Definition;
        Natural_Subtype_Declaration->Subtype_Indication = Natural_Subtype_Definition;
        Standard_Package->Declaration_Chain.push_back(Natural_Subtype_Declaration);
        Natural_Subtype_Definition->Type_Declarator = Natural_Subtype_Declaration;
    }

// positive subtype
    {
        Positive_Subtype_Definition = new Iir_Integer_Subtype_Definition;
        Positive_Subtype_Definition->Base_Type = Integer_Type_Definition;
        Positive_Subtype_Definition->Subtype_Type_Mark = Create_Std_Type_Mark(Integer_Subtype_Declaration);
        auto Constraint = Create_Std_Range_Expr(Create_Std_Integer(1, Integer_Type_Definition),
                Create_Std_Integer(High_Bound(options.Flag_Integer_64), Integer_Type_Definition),
                Integer_Type_Definition);
        Positive_Subtype_Definition->Range_Constraint = Constraint;
        Positive_Subtype_Definition->Type_Staticness = Iir_Staticness::Locally;
        Positive_Subtype_Definition->Signal_Type_Flag = true;
        Positive_Subtype_Definition->Has_Signal_Flag = not options.Flag_Whole_Analyze;

        Positive_Subtype_Declaration = Create_Std_Decl<Iir_Subtype_Declaration>();
        Positive_Subtype_Declaration->Identifier = Keywords::Name_Positive;
        Positive_Subtype_Declaration->Visible_Flag = true;
        Positive_Subtype_Declaration->Type = Positive_Subtype_Definition;
        Positive_Subtype_Declaration->Subtype_Indication = Positive_Subtype_Definition;
        Standard_Package->Declaration_Chain.push_back(Positive_Subtype_Declaration);
        Positive_Subtype_Definition->Type_Declarator = Positive_Subtype_Declaration;
    }

// string type.
// type string is array (positive range <>) of character;
    {
        auto Element = Create_Std_Type_Mark(Character_Type_Declaration);

        String_Type_Definition = new Iir_Array_Type_Definition;
        String_Type_Definition->Base_Type = String_Type_Definition;
        std::vector<Iir*> Index_List;
        Index_List.push_back(Create_Std_Type_Mark(Positive_Subtype_Declaration));
        String_Type_Definition->Index_Subtype_Definition_List = Index_List;
        String_Type_Definition->Index_Subtype_List = Index_List;
        String_Type_Definition->Element_Subtype_Indication = Element;
        String_Type_Definition->Element_Subtype = Character_Type_Definition;
        String_Type_Definition->Type_Staticness = Iir_Staticness::None;
        String_Type_Definition->Signal_Type_Flag = true;
        String_Type_Definition->Has_Signal_Flag = not options.Flag_Whole_Analyze;

        String_Type_Declaration = Create_Std_Type(String_Type_Definition, Keywords::Name_String);

//TODO: Add_Implicit_Operations (String_Type_Declaration);
    }

    if (options.standard >= Vhdl_Std::Vhdl_08) {
//  type Boolean_Vector is array (Natural range <>) of Boolean;
        [Boolean_Vector_Type_Definition, Boolean_Vector_Type_Declaration] =
                Create_Array_Type(Boolean_Type_Declaration, Keywords::Name_Boolean_Vector);
    }

// bit_vector type.
// type bit_vector is array (natural range <>) of bit;
    [Bit_Vector_Type_Definition, Bit_Vector_Type_Declaration] =
            Create_Array_Type(Bit_Type_Declaration, Keywords::Name_Bit_Vector);

//  LRM08 5.3.2.4 Predefined operations on array types
//  The following operations are implicitly declared in package
//  STD.STANDARD immediately following the declaration of type
//  BIT_VECTOR:
    if (options.standard >= Vhdl_Std::Vhdl_08) {
        Create_To_String(Bit_Vector_Type_Definition, Iir_Predefined_Bit_Vector_To_Ostring, Keywords::Name_To_Ostring);
        Create_To_String(Bit_Vector_Type_Definition, Iir_Predefined_Bit_Vector_To_Hstring, Keywords::Name_To_Hstring);
    }

//  VHDL 2008
//  Vector types
    if (options.standard >= Vhdl_Std::Vhdl_08) {
// type integer_vector is array (natural range <>) of Integer;
        [Integer_Vector_Type_Definition, Integer_Vector_Type_Declaration] =
                Create_Array_Type(Integer_Subtype_Declaration, Keywords::Name_Integer_Vector);

// type Real_vector is array (natural range <>) of Real;
        [Real_Vector_Type_Definition, Real_Vector_Type_Declaration] =
                Create_Array_Type(Real_Subtype_Declaration, Keywords::Name_Real_Vector);

// type Time_vector is array (natural range <>) of Time;
        [Time_Vector_Type_Definition, Time_Vector_Type_Declaration] =
                Create_Array_Type(Time_Subtype_Declaration, Keywords::Name_Time_Vector);
    }

//  VHDL93:
//  type file_open_kind is (read_mode, write_mode, append_mode);
    if (options.standard >= Vhdl_Std::Vhdl_93c) {
        File_Open_Kind_Type_Definition = new Iir_Enumeration_Type_Definition;
        File_Open_Kind_Type_Definition->Base_Type = File_Open_Kind_Type_Definition;

        File_Open_Kind_Read_Mode = Create_Std_Literal(Keywords::Name_Read_Mode, File_Open_Kind_Type_Definition);
        File_Open_Kind_Write_Mode = Create_Std_Literal(Keywords::Name_Write_Mode, File_Open_Kind_Type_Definition);
        File_Open_Kind_Append_Mode = Create_Std_Literal(Keywords::Name_Append_Mode, File_Open_Kind_Type_Definition);
        File_Open_Kind_Type_Definition->Type_Staticness = Iir_Staticness::Locally;
        File_Open_Kind_Type_Definition->Signal_Type_Flag = true;
        File_Open_Kind_Type_Definition->Has_Signal_Flag = !options.Flag_Whole_Analyze;

//  type file_open_kind is
        File_Open_Kind_Type_Declaration = Create_Std_Type(File_Open_Kind_Type_Definition,
                Keywords::Name_File_Open_Kind);

        Create_Range_Constraint_For_Enumeration_Type(*File_Open_Kind_Type_Definition);
//TODO: Add_Implicit_Operations (File_Open_Kind_Type_Declaration);
    }
    else {
        File_Open_Kind_Type_Declaration = nullptr;
        File_Open_Kind_Type_Definition = nullptr;
        File_Open_Kind_Read_Mode = nullptr;
        File_Open_Kind_Write_Mode = nullptr;
        File_Open_Kind_Append_Mode = nullptr;
    }

//  VHDL93:
//  type file_open_status is
//      (open_ok, status_error, name_error, mode_error);
    if (options.standard >= Vhdl_Std::Vhdl_93c) {
        File_Open_Status_Type_Definition =
                new Iir_Enumeration_Type_Definition;
        File_Open_Status_Type_Definition->Base_Type = File_Open_Status_Type_Definition;

        File_Open_Status_Open_Ok = Create_Std_Literal(Keywords::Name_Open_Ok, File_Open_Status_Type_Definition);
        File_Open_Status_Status_Error = Create_Std_Literal(Keywords::Name_Status_Error,
                File_Open_Status_Type_Definition);
        File_Open_Status_Name_Error = Create_Std_Literal(Keywords::Name_Name_Error, File_Open_Status_Type_Definition);
        File_Open_Status_Mode_Error = Create_Std_Literal(Keywords::Name_Mode_Error, File_Open_Status_Type_Definition);
        File_Open_Status_Type_Definition->Type_Staticness = Iir_Staticness::Locally;
        File_Open_Status_Type_Definition->Signal_Type_Flag = true;
        File_Open_Status_Type_Definition->Has_Signal_Flag = !options.Flag_Whole_Analyze;

//  type file_open_kind is
        File_Open_Status_Type_Declaration = Create_Std_Type(File_Open_Status_Type_Definition,
                Keywords::Name_File_Open_Status);
        Create_Range_Constraint_For_Enumeration_Type(*File_Open_Status_Type_Definition);
//TODO: Add_Implicit_Operations (File_Open_Status_Type_Declaration);
    }
    else {
        File_Open_Status_Type_Declaration = nullptr;
        File_Open_Status_Type_Definition = nullptr;
        File_Open_Status_Open_Ok = nullptr;
        File_Open_Status_Status_Error = nullptr;
        File_Open_Status_Name_Error = nullptr;
        File_Open_Status_Mode_Error = nullptr;
    }

//  VHDL93:
//  attribute FOREIGN: string;
    if (options.standard >= Vhdl_Std::Vhdl_93c) {
        Foreign_Attribute = Create_Std_Decl<Iir_Attribute_Declaration>();
        Foreign_Attribute->Identifier = Keywords::Name_Foreign;
        Foreign_Attribute->Visible_Flag = true;
        Foreign_Attribute->Type_Mark = Create_Std_Type_Mark(String_Type_Declaration);
        Foreign_Attribute->Type = String_Type_Definition;
        Standard_Package->Declaration_Chain.push_back(Foreign_Attribute);
    }
    else
        Foreign_Attribute = nullptr;

    if (options.standard >= Vhdl_Std::Vhdl_08) {
        Create_To_String(Boolean_Type_Definition, Iir_Predefined_Enum_To_String);
        Create_To_String(Bit_Type_Definition, Iir_Predefined_Enum_To_String);
        Create_To_String(Character_Type_Definition, Iir_Predefined_Enum_To_String);
        Create_To_String(Severity_Level_Type_Definition, Iir_Predefined_Enum_To_String);
        Create_To_String(Universal_Integer_Type_Definition, Iir_Predefined_Integer_To_String);
        Create_To_String(Universal_Real_Type_Definition, Iir_Predefined_Floating_To_String);
        Create_To_String(Integer_Type_Definition, Iir_Predefined_Integer_To_String);
        Create_To_String(Real_Type_Definition, Iir_Predefined_Floating_To_String);
        Create_To_String(Time_Type_Definition, Iir_Predefined_Physical_To_String);
        Create_To_String(File_Open_Kind_Type_Definition, Iir_Predefined_Enum_To_String);
        Create_To_String(File_Open_Status_Type_Definition, Iir_Predefined_Enum_To_String);

//  Predefined overload TO_STRING operations
        Create_To_String(Real_Type_Definition, Iir_Predefined_Real_To_String_Digits, Keywords::Name_To_String,
                Keywords::Name_Digits, Natural_Subtype_Definition);
        Create_To_String(Real_Type_Definition, Iir_Predefined_Real_To_String_Format, Keywords::Name_To_String,
                Keywords::Name_Format, String_Type_Definition);
        Create_To_String(Time_Type_Definition, Iir_Predefined_Time_To_String_Unit, Keywords::Name_To_String,
                Keywords::Name_Unit, Time_Subtype_Definition);
    }

//  Wilcard types.
//  Create the declaration and give them meaningful (and invalid) names
//  so that error messages are clear for the user.
    Create_Wildcard_Type(Wildcard_Any_Type, "any type");
    Create_Wildcard_Type(Wildcard_Any_Aggregate_Type, "any aggregate type");
    Create_Wildcard_Type(Wildcard_Any_String_Type, "any string type");
    Create_Wildcard_Type(Wildcard_Any_Access_Type, "any access type");

    Error_Type = Create_Error_Type(Wildcard_Any_Type);
    Error_Type->Error_Origin = nullptr;
    Create_Wildcard_Type(Error_Type, "unknown type");
}