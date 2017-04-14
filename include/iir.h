/*
 * schematiK - An automagic schematic generation tool
 *
 * Copyright (c) 2016, Rishabh Rawat <rishabhrawat1994@gmail.com>
 * Permission to use, copy, modify, and/or distribute this software for any purpose with or without
 * fee is hereby granted, provided that the above copyright notice and this permission notice appear
 * in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 */

#ifndef AVLC_IIR_H
#define AVLC_IIR_H

#include <string>
#include <vector>
#include "iir_types.h"
#include "types.h"
//#include "Scanner.h"
#include <variant>
#include <optional>

struct Iir {
    Location_Type Location;
};

struct Iir_Unused: public virtual Iir {
};

struct Iir_Type_Abs: public virtual Iir {
    Iir* Type;
};

struct Iir_Design_Unit;

struct Iir_Parent_Design_Unit_Abs : public virtual Iir {
    Iir_Design_Unit* Parent_Design_Unit;
};

struct Iir_Type_Declarator_Abs: public virtual Iir {
    bool Resolved_Flag;
    Iir* Base_Type;
    bool Signal_Type_Flag;
    Iir* Type_Declarator;
};

using Iir_Base_Type_Abs = Iir_Type_Declarator_Abs;

using Iir_Resolved_Flag_Abs = Iir_Type_Declarator_Abs;

using Iir_Signal_Type_Flag_Abs = Iir_Type_Declarator_Abs;

struct Iir_Has_Signal_Flag_Abs: public virtual Iir {
    bool Has_Signal_Flag;
};

struct Iir_Expr_Staticness_Abs: public virtual Iir {
    Iir_Staticness Expr_Staticness;
};

struct Iir_Error
        : public Iir_Type_Abs,
          public Iir_Type_Declarator_Abs,
          public Iir_Has_Signal_Flag_Abs,
          public Iir_Expr_Staticness_Abs {
    Iir* Error_Origin;
};

struct Iir_Chain_Abs: public virtual Iir {
    Iir* Chain;
};

struct Iir_Elab_Flag_Abs: public virtual Iir {
    bool Elab_Flag;
};

struct Iir_Library_Declaration;

struct Iir_Design_File: public Iir_Elab_Flag_Abs {
    std::vector<Iir_Design_Unit*> Design_Units;
    Time_Stamp_Id Analysis_Time_Stamp;
    File_Checksum_Id File_Checksum;
    Iir_Library_Declaration* Library;
    std::vector<Iir*> File_Dependence_List;
    std::filesystem::path Design_File_Filename;
};

struct Iir_Entity_Declaration;
struct Iir_Architecture_Body;
struct Iir_Package_Declaration;
struct Iir_Package_Instantiation_Declaration;
struct Iir_Package_Body;
struct Iir_Configuration_Declaration;
struct Iir_Context_Declaration;

using Iir_Design_Unit_n = std::variant<Iir_Entity_Declaration*,
                                       Iir_Architecture_Body*,
                                       Iir_Package_Declaration*,
                                       Iir_Package_Instantiation_Declaration*,
                                       Iir_Package_Body*,
                                       Iir_Configuration_Declaration*,
                                       Iir_Context_Declaration*>;

struct Iir_Date_Abs: public virtual Iir {
    Date_Type Date;
};

struct Iir_Context_Items_Abs: public virtual Iir {
    std::vector<Iir*> Context_Items;
};

struct Iir_Identifier_Abs: public virtual Iir {
    std::string Identifier;
};

struct  Iir_Design_Unit
        : public Iir_Date_Abs,
          public Iir_Context_Items_Abs,
          // public Iir_Chain_Abs, NOTE: use Iir_File's vector
          public Iir_Identifier_Abs,
          public Iir_Elab_Flag_Abs {
    Location_Type Source_Pos;
    Location_Type End_Location;
    bool Configuration_Mark_Flag;
    Iir_Design_File* Design_File;
    Iir_Design_Unit_n Library_Unit;
    Date_State_Type Date_State;
    std::vector<Iir*> Analysis_Checks_List;
//    Iir* Hash_Chain;
    std::vector<Iir*> Dependence_List;
    bool Configuration_Done_Flag;
};

struct Iir_Parent_Abs: public virtual Iir {
    Iir* Parent;
};

struct Iir_Has_Identifier_List_Abs: public virtual Iir {
    bool Has_Identifier_List;
};

struct Iir_Library_Clause
        : public Iir_Chain_Abs, public Iir_Identifier_Abs, public Iir_Parent_Abs, public Iir_Has_Identifier_List_Abs {
    Iir* Library_Declaration;
};

struct Iir_Selected_Name_Abs: public virtual Iir {
    Iir* Selected_Name;
};

struct Iir_Use_Clause: public Iir_Chain_Abs, public Iir_Selected_Name_Abs, public Iir_Parent_Abs {
    Iir* Use_Clause_Chain;
};

struct Iir_Context_Reference: public Iir_Chain_Abs, public Iir_Selected_Name_Abs, public Iir_Parent_Abs {
    Iir* Context_Reference_Chain;
};

struct Iir_Value_Abs: public virtual Iir {
    int64_t Value;
};

struct Iir_Literal_Origin_Abs: public virtual Iir {
    Iir* Literal_Origin;
};

struct Iir_Integer_Literal
        : public Iir_Value_Abs, public Iir_Literal_Origin_Abs, public Iir_Type_Abs, public Iir_Expr_Staticness_Abs {
};

struct Iir_Fp_Value_Abs: public virtual Iir {
    double Fp_Value;
};

struct Iir_Floating_Point_Literal
        : public Iir_Fp_Value_Abs, public Iir_Literal_Origin_Abs, public Iir_Type_Abs, public Iir_Expr_Staticness_Abs {
};

struct Iir_Null_Literal: public Iir_Type_Abs, public Iir_Expr_Staticness_Abs {
};

struct Iir_Literal_Subtype_Abs: public virtual Iir {
    Iir* Literal_Subtype;
};

struct Iir_String
        : public Iir_Literal_Origin_Abs,
          public Iir_Literal_Subtype_Abs,
          public Iir_Type_Abs,
          public Iir_Expr_Staticness_Abs {
    std::string str;
    bool Has_Sign;
    Number_Base_Type Bit_String_Base;
    bool Has_Signed;
};

struct Iir_Unit_Declaration;

struct Iir_Physical_Unit_Abs: public virtual Iir {
    Iir* Unit_Name;
    Iir_Unit_Declaration* Physical_Unit;
};

using Iir_Unit_Name_Abs = Iir_Physical_Unit_Abs;

struct Iir_Physical_Int_Literal
        : public Iir_Value_Abs,
          public Iir_Literal_Origin_Abs,
          public Iir_Type_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Physical_Unit_Abs {
};

struct Iir_Physical_Fp_Literal
        : public Iir_Fp_Value_Abs,
          public Iir_Literal_Origin_Abs,
          public Iir_Type_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Physical_Unit_Abs {
};

struct Iir_Simple_Aggregate
        : public Iir_Literal_Origin_Abs,
          public Iir_Literal_Subtype_Abs,
          public Iir_Type_Abs,
          public Iir_Expr_Staticness_Abs {
    std::vector<Iir*> Simple_Aggregate_List;
};

struct Iir_Overflow_Literal: public Iir_Literal_Origin_Abs, public Iir_Type_Abs, public Iir_Expr_Staticness_Abs {
};


struct Iir_Unaffected_Waveform: public virtual Iir {
};

struct Iir_Waveform_Element: public virtual Iir {
    Iir* We_Value;
    Iir* Time;
};

using Iir_Waveform = std::vector<Iir_Waveform_Element*>;

struct Iir_Condition_Abs: public virtual Iir {
    Iir* Condition;
};

struct Iir_Conditional_Waveform: public Iir_Chain_Abs, public Iir_Condition_Abs {
    Iir_Waveform waveform;
};

struct Iir_Expression_Abs: public virtual Iir {
    Iir* Expression;
};

struct Iir_Conditional_Expression: public Iir_Chain_Abs, public Iir_Expression_Abs, public Iir_Condition_Abs {
};

struct Iir_Formal_Abs: public virtual Iir {
    bool Whole_Association_Flag;
    bool Collapse_Signal_Flag;
    Iir* Formal;
};

struct Iir_Actual_Abs: public virtual Iir {
    Iir* Actual;
};

using Iir_Whole_Association_Flag_Abs = Iir_Formal_Abs;

using Iir_Collapse_Signal_Flag_Abs = Iir_Formal_Abs;

struct Iir_Association_Element_By_Expression
        : public Iir_Formal_Abs,
          public Iir_Actual_Abs,
          public Iir_Chain_Abs {
    Iir* In_Conversion;
    Iir* Out_Conversion;
};

struct Iir_Actual_Type_Abs: public virtual Iir {
    Iir* Actual_Type;
};

struct Iir_Choice_Staticness_Abs: public virtual Iir {
    Iir_Staticness Choice_Staticness;
};

struct Iir_Association_Element_By_Individual
        : public Iir_Formal_Abs,
          public Iir_Chain_Abs,
          public Iir_Actual_Type_Abs,
          public Iir_Choice_Staticness_Abs {
    Iir* Individual_Association_Chain;
    Iir* Actual_Type_Definition;
};

struct Iir_Association_Element_Open
        : public Iir_Formal_Abs,
          public Iir_Chain_Abs {
    bool Artificial_Flag;
};

struct Iir_Association_Element_Package
        : public Iir_Formal_Abs,
          public Iir_Actual_Abs,
          public Iir_Chain_Abs {
};

struct Iir_Association_Element_Type
        : public Iir_Formal_Abs,
          public Iir_Actual_Abs,
          public Iir_Chain_Abs,
          public Iir_Actual_Type_Abs {
    Iir* Subprogram_Association_Chain;
};

struct Iir_Association_Element_Subprogram
        : public Iir_Formal_Abs,
          public Iir_Actual_Abs,
          public Iir_Chain_Abs {
};


// Iir_Kind_Choice_By_Others (Short)
// Iir_Kind_Choice_By_None (Short)
// Iir_Kind_Choice_By_Range (Short)
// Iir_Kind_Choice_By_Name (Short)
// Iir_Kind_Choice_By_Expression (Short)
//  (Iir_Kinds_Choice)
//
//   Get/Set_Parent (Field0)
//
//  For a list of choices, only the first one is associated, the following
//  associations have the same_alternative_flag set.
//   Get/Set_Chain (Field2)
//
//  These are elements of an choice chain, which is used for
//  case_statement, concurrent_select_signal_assignment, aggregates.
//
//  Get/Set what is associated with the choice.  There are two different
//  nodes, one for simple association and the other for chain association.
//  They don't have the same properties (normal vs chain), so the right
//  field must be selected according to the property to have working
//  walkers. Both fields are never used at the same time.
//
//  For:
//  * an expression for an aggregate
//  * an individual association
//  * a generate_statement_body chain for a case_generate_statement
//   Get/Set_Associated_Expr (Field3)
//   Get/Set_Associated_Block (Alias Field3)
//
//  For
//  * a waveform_chain for a concurrent_select_signal_assignment,
//  * a sequential statement chain for a case_statement.
//   Get/Set_Associated_Chain (Field4)
//
//  Should be a simple_name.
// Only for Iir_Kind_Choice_By_Name:
//   Get/Set_Choice_Name (Field5)
//
// Only for Iir_Kind_Choice_By_Expression:
//   Get/Set_Choice_Expression (Field5)
//
// Only for Iir_Kind_Choice_By_Range:
//   Get/Set_Choice_Range (Field5)
//
//   Get/Set_Same_Alternative_Flag (Flag1)
//
// Only for Iir_Kind_Choice_By_Range:
// Only for Iir_Kind_Choice_By_Expression:
//   Get/Set_Choice_Staticness (State1)




struct Iir_Choice_Abs : public Iir_Chain_Abs,
                        public Iir_Parent_Abs {
    bool Same_Alternative_Flag;
    Iir* Associated_Expr;
    Iir* Associated_Chain;
};

struct Iir_Choice_By_Range
        : public Iir_Choice_Abs,
          public Iir_Choice_Staticness_Abs {
    Iir* Choice_Range;
};

struct Iir_Choice_By_Expression
        : public Iir_Choice_Abs,
          public Iir_Choice_Staticness_Abs {
    Iir* Choice_Expression;
};

struct Iir_Kind_Choice_By_Others
        : public Iir_Choice_Abs {
};

struct Iir_Choice_By_None
        : public Iir_Choice_Abs {
};

struct Iir_Choice_By_Name
        : public Iir_Choice_Abs {
    Iir* Choice_Name;
};

struct Iir_Entity_Name_Abs: public virtual Iir {
    Iir* Entity_Name;
};

struct Iir_Entity_Aspect_Entity: public Iir_Entity_Name_Abs {
    Iir* Architecture;
};

struct Iir_Entity_Aspect_Configuration: public virtual Iir {
    Iir* Configuration_Name;
};

struct Iir_Entity_Aspect_Open: public virtual Iir {
};

struct Iir_Declaration_Chain_Abs: public virtual Iir {
    std::vector<Iir*> Declaration_Chain;
};

struct Iir_Block_Configuration: public Iir_Chain_Abs, public Iir_Declaration_Chain_Abs, public Iir_Parent_Abs {
    Iir* Prev_Block_Configuration;
    Iir* Block_Specification;
    Iir* Configuration_Item_Chain;
};

struct Iir_Port_Chain_Abs: public virtual Iir {
    Iir* Port_Chain;
};

struct Iir_Generic_Chain_Abs: public virtual Iir {
    Iir* Generic_Chain;
};

struct Iir_Generic_Map_Aspect_Chain_Abs: public virtual Iir {
    Iir* Generic_Map_Aspect_Chain;
};

struct Iir_Port_Map_Aspect_Chain_Abs: public virtual Iir {
    Iir* Port_Map_Aspect_Chain;
};

struct Iir_Block_Header
        : public Iir_Port_Chain_Abs,
          public Iir_Generic_Chain_Abs,
          public Iir_Generic_Map_Aspect_Chain_Abs,
          public Iir_Port_Map_Aspect_Chain_Abs {
};

struct Iir_Block_Configuration_Abs: public virtual Iir {
    Iir* Block_Configuration;
};

struct Iir_Component_Name_Abs: public virtual Iir {
    std::vector<Iir*> Instantiation_List;
    Iir* Binding_Indication;
    Iir* Component_Name;
};

using Iir_Instantiation_List_Abs = Iir_Component_Name_Abs;

using Iir_Binding_Indication_Abs = Iir_Component_Name_Abs;

struct Iir_Is_Ref_Abs: public virtual Iir {
    bool Is_Ref;
};

struct Iir_Component_Configuration
        : public Iir_Block_Configuration_Abs,
          public Iir_Chain_Abs,
          public Iir_Parent_Abs,
          public Iir_Component_Name_Abs,
          public Iir_Is_Ref_Abs {
};

struct Iir_Binding_Indication: public Iir_Generic_Map_Aspect_Chain_Abs, public Iir_Port_Map_Aspect_Chain_Abs {
    Iir* Entity_Aspect;
    Iir* Default_Entity_Aspect;
};

struct Iir_Entity_Class_Abs: public virtual Iir {
    Token_Type Entity_Class;
};

struct Iir_Entity_Class: public Iir_Entity_Class_Abs, public Iir_Chain_Abs {
};

struct Iir_Base_Name_Abs: public virtual Iir {
    Iir* Base_Name;
};

struct Iir_Name_Staticness_Abs: public virtual Iir {
    Iir_Staticness Name_Staticness;
};

struct Iir_Attribute_Value
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs {
    Iir* Designated_Entity;
    Iir* Value_Chain;
    Iir* Attribute_Specification;
    Iir* Spec_Chain;
};

struct Iir_Return_Type_Mark_Abs: public virtual Iir {
    Iir* Return_Type_Mark;
};

struct Iir_Signature: public Iir_Return_Type_Mark_Abs {
    Iir* Signature_Prefix;
    std::vector<Iir*> Type_Marks_List;
};

struct Iir_Aggregate_Info: public virtual Iir {
    bool Aggr_Named_Flag;
    Iir* Aggr_Low_Limit;
    bool Aggr_Others_Flag;
    Iir* Sub_Aggregate_Info;
    Iir* Aggr_High_Limit;
    bool Aggr_Dynamic_Flag;
    int Aggr_Min_Length;
};

struct Iir_Prefix_Abs: public virtual Iir {
    Iir* Prefix;
};

struct Iir_Implementation_Abs: public virtual Iir {
    Iir* Implementation;
};

struct Iir_Parameter_Association_Chain_Abs: public virtual Iir {
    Iir* Parameter_Association_Chain;
    Iir* Method_Object;
};

using Iir_Method_Object_Abs = Iir_Parameter_Association_Chain_Abs;

struct Iir_Procedure_Call
        : public Iir_Prefix_Abs,
          public Iir_Implementation_Abs,
          public Iir_Parameter_Association_Chain_Abs {
};
//
//struct Iir_Element_Position_Abs: public virtual Iir {
//    Iir_Index32 Element_Position;
//    Iir* Base_Element_Declaration;
//};
//
//using Iir_Base_Element_Declaration_Abs = Iir_Element_Position_Abs;

struct Iir_Visible_Flag_Abs: public virtual Iir {
    bool Visible_Flag;
};

struct Iir_Record_Element_Constraint
        : public Iir_Type_Abs,
//          public Iir_Element_Position_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Abs {
    Iir* Element_Declaration;
};

struct Iir_Resolution_Indication_Abs: public virtual Iir {
    Iir* Resolution_Indication;
};

struct Iir_Element_Subtype_Indication_Abs: public virtual Iir {
    Iir* Element_Subtype_Indication;
};

struct Iir_Array_Element_Resolution: public Iir_Resolution_Indication_Abs, public Iir_Element_Subtype_Indication_Abs {
};

struct Iir_Record_Element_Resolution
        : public Iir_Chain_Abs, public Iir_Identifier_Abs, public Iir_Resolution_Indication_Abs {
};

struct Iir_Record_Resolution: public virtual Iir {
    std::vector<Iir_Record_Element_Resolution*> chain;
};

struct Iir_Attribute_Specification
        : public Iir_Entity_Class_Abs, public Iir_Chain_Abs, public Iir_Expression_Abs, public Iir_Parent_Abs {
    Iir* Attribute_Value_Spec_Chain;
    Iir* Attribute_Specification_Chain;
    std::vector<Iir*> Entity_Name_List;
    Iir* Attribute_Designator;
};

struct Iir_Type_Mark_Abs: public virtual Iir {
    Iir* Type_Mark;
};

struct Iir_Disconnection_Specification
        : public Iir_Chain_Abs,
          public Iir_Expression_Abs,
          public Iir_Parent_Abs,
          public Iir_Type_Mark_Abs,
          public Iir_Is_Ref_Abs {
    std::vector<Iir*> Signal_List;
};

struct Iir_Configuration_Specification
        : public Iir_Chain_Abs,
          public Iir_Parent_Abs,
          public Iir_Component_Name_Abs,
          public Iir_Is_Ref_Abs {
};

struct Iir_Incomplete_Type_Ref_Chain_Abs: public virtual Iir {
    Iir* Incomplete_Type_Ref_Chain;
};

struct Iir_Type_Staticness_Abs: public virtual Iir {
    Iir_Staticness Type_Staticness;
};

struct Iir_Designated_Type_Abs: public virtual Iir {
    Iir* Designated_Type;
    Iir* Designated_Subtype_Indication;
};

using Iir_Designated_Subtype_Indication_Abs = Iir_Designated_Type_Abs;

struct Iir_Access_Type_Definition
        : public Iir_Type_Declarator_Abs,
          public Iir_Incomplete_Type_Ref_Chain_Abs,
          public Iir_Type_Staticness_Abs,
          public Iir_Designated_Type_Abs {
};

struct Iir_Incomplete_Type_Definition
        : public Iir_Type_Declarator_Abs,
          public Iir_Incomplete_Type_Ref_Chain_Abs,
          public Iir_Type_Staticness_Abs,
          public Iir_Has_Signal_Flag_Abs {
    Iir* Complete_Type_Definition;
};

struct Iir_Interface_Type_Definition
        : public Iir_Type_Declarator_Abs,
          public Iir_Type_Staticness_Abs,
          public Iir_Has_Signal_Flag_Abs {
    Iir* Associated_Type;
};

struct Iir_File_Type_Definition
        : public Iir_Type_Declarator_Abs,
          public Iir_Type_Staticness_Abs {
    Iir* File_Type_Mark;
    bool Text_File_Flag;
};

struct Iir_End_Has_Reserved_Id_Abs: public virtual Iir {
    bool End_Has_Reserved_Id;
};

struct Iir_End_Has_Identifier_Abs: public virtual Iir {
    bool End_Has_Identifier;
};

struct Iir_Protected_Type_Declaration
        : public Iir_Declaration_Chain_Abs,
          public Iir_Type_Declarator_Abs,
          public Iir_Type_Staticness_Abs,
          public Iir_End_Has_Reserved_Id_Abs,
          public Iir_End_Has_Identifier_Abs {
    Iir* Protected_Type_Body;
};

struct Iir_Constraint_State_Abs: public virtual Iir {
    Iir_Constraint Constraint_State;
};

struct Iir_Elements_Declaration_List_Abs: public virtual Iir {
    std::vector<Iir*> Elements_Declaration_List;
};

struct Iir_Record_Type_Definition
        : public Iir_Type_Declarator_Abs,
          public Iir_Type_Staticness_Abs,
          public Iir_Constraint_State_Abs,
          public Iir_Elements_Declaration_List_Abs,
          public Iir_Has_Signal_Flag_Abs,
          public Iir_End_Has_Reserved_Id_Abs,
          public Iir_End_Has_Identifier_Abs {
};

struct Iir_Index_Subtype_List_Abs: public virtual Iir {
    std::vector<Iir*> Index_Subtype_List;
    Iir* Element_Subtype;
    bool Index_Constraint_Flag;
};

using Iir_Element_Subtype_Abs = Iir_Index_Subtype_List_Abs;

using Iir_Index_Constraint_Flag_Abs = Iir_Index_Subtype_List_Abs;

struct Iir_Array_Type_Definition
        : public Iir_Type_Declarator_Abs,
          public Iir_Type_Staticness_Abs,
          public Iir_Constraint_State_Abs,
          public Iir_Index_Subtype_List_Abs,
          public Iir_Element_Subtype_Indication_Abs,
          public Iir_Has_Signal_Flag_Abs {
    std::vector<Iir*> Index_Subtype_Definition_List;
};

struct Iir_Tolerance_Abs: public virtual Iir {
    Iir* Tolerance;
};

struct Iir_Subtype_Type_Mark_Abs: public virtual Iir {
    Iir* Subtype_Type_Mark;
};

struct Iir_Array_Subtype_Definition
        : public Iir_Type_Declarator_Abs,
          public Iir_Resolution_Indication_Abs,
          public Iir_Tolerance_Abs,
          public Iir_Type_Staticness_Abs,
          public Iir_Constraint_State_Abs,
          public Iir_Index_Subtype_List_Abs,
          public Iir_Has_Signal_Flag_Abs,
          public Iir_Subtype_Type_Mark_Abs {
    std::vector<Iir*> Index_Constraint_List;
    Iir* Array_Element_Constraint;
};

struct Iir_Record_Subtype_Definition
        : public Iir_Type_Declarator_Abs,
          public Iir_Resolution_Indication_Abs,
          public Iir_Tolerance_Abs,
          public Iir_Type_Staticness_Abs,
          public Iir_Constraint_State_Abs,
          public Iir_Elements_Declaration_List_Abs,
          public Iir_Has_Signal_Flag_Abs,
          public Iir_Subtype_Type_Mark_Abs {
};

struct Iir_Access_Subtype_Definition
        : public Iir_Type_Declarator_Abs,
          public Iir_Type_Staticness_Abs,
          public Iir_Designated_Type_Abs,
          public Iir_Subtype_Type_Mark_Abs {
};

struct Iir_Range_Constraint_Abs: public virtual Iir {
    Iir* Range_Constraint;
};

struct Iir_Physical_Subtype_Definition
        : public Iir_Type_Declarator_Abs,
          public Iir_Range_Constraint_Abs,
          public Iir_Resolution_Indication_Abs,
          public Iir_Type_Staticness_Abs,
          public Iir_Has_Signal_Flag_Abs,
          public Iir_Subtype_Type_Mark_Abs,
          public Iir_Is_Ref_Abs {
};

struct Iir_Floating_Subtype_Definition
        : public Iir_Type_Declarator_Abs,
          public Iir_Range_Constraint_Abs,
          public Iir_Resolution_Indication_Abs,
          public Iir_Tolerance_Abs,
          public Iir_Type_Staticness_Abs,
          public Iir_Has_Signal_Flag_Abs,
          public Iir_Subtype_Type_Mark_Abs,
          public Iir_Is_Ref_Abs {
};

struct Iir_Integer_Subtype_Definition
        : public Iir_Type_Declarator_Abs,
          public Iir_Range_Constraint_Abs,
          public Iir_Resolution_Indication_Abs,
          public Iir_Type_Staticness_Abs,
          public Iir_Has_Signal_Flag_Abs,
          public Iir_Subtype_Type_Mark_Abs,
          public Iir_Is_Ref_Abs {
};

struct Iir_Enumeration_Subtype_Definition
        : public Iir_Type_Declarator_Abs,
          public Iir_Range_Constraint_Abs,
          public Iir_Resolution_Indication_Abs,
          public Iir_Type_Staticness_Abs,
          public Iir_Has_Signal_Flag_Abs,
          public Iir_Subtype_Type_Mark_Abs,
          public Iir_Is_Ref_Abs {
};

struct Iir_Enumeration_Literal;

struct Iir_Enumeration_Type_Definition
        : public Iir_Type_Declarator_Abs,
          public Iir_Range_Constraint_Abs,
          public Iir_Type_Staticness_Abs,
          public Iir_Has_Signal_Flag_Abs,
          public Iir_Is_Ref_Abs {
    std::vector<Iir_Enumeration_Literal*> Enumeration_Literal_List;
    bool Only_Characters_Flag;
};

struct Iir_Integer_Type_Definition
        : public Iir_Type_Declarator_Abs,
          public Iir_Range_Constraint_Abs,
          public Iir_Type_Staticness_Abs,
          public Iir_Has_Signal_Flag_Abs,
          public Iir_Is_Ref_Abs {
};

struct Iir_Floating_Type_Definition
        : public Iir_Type_Declarator_Abs,
          public Iir_Range_Constraint_Abs,
          public Iir_Type_Staticness_Abs,
          public Iir_Has_Signal_Flag_Abs,
          public Iir_Is_Ref_Abs {
};

struct Iir_Physical_Type_Definition
        : public Iir_Type_Declarator_Abs,
          public Iir_Range_Constraint_Abs,
          public Iir_Type_Staticness_Abs,
          public Iir_Has_Signal_Flag_Abs,
          public Iir_End_Has_Reserved_Id_Abs,
          public Iir_End_Has_Identifier_Abs,
          public Iir_Is_Ref_Abs {
    std::vector<Iir*> Unit_Chain;
};

struct Iir_Range_Expression: public Iir_Type_Abs, public Iir_Expr_Staticness_Abs {
    Iir_Direction Direction;
    Iir* Left_Limit;
    Iir* Right_Limit_Expr;
    Iir* Range_Origin;
    Iir* Left_Limit_Expr;
    Iir* Right_Limit;
};

struct Iir_Protected_Type_Body
        : public Iir_Chain_Abs,
          public Iir_Declaration_Chain_Abs,
          public Iir_Identifier_Abs,
          public Iir_Parent_Abs,
          public Iir_End_Has_Reserved_Id_Abs,
          public Iir_End_Has_Identifier_Abs {
    Iir* Protected_Type_Declaration;
};

struct Iir_Wildcard_Type_Definition
        : public Iir_Type_Declarator_Abs,
          public Iir_Type_Staticness_Abs {
};

struct Iir_Subtype_Definition
        : public Iir_Range_Constraint_Abs,
          public Iir_Resolution_Indication_Abs,
          public Iir_Tolerance_Abs,
          public Iir_Subtype_Type_Mark_Abs,
          public Iir_Is_Ref_Abs {
};

struct Iir_Scalar_Nature_Definition: public virtual Iir {
    Iir* Nature_Declarator;
    Iir* Through_Type;
    Iir* Across_Type;
    Iir* Reference;
};

struct Iir_Overload_List: public virtual Iir {
    std::vector<Iir*> Overload_List;
};

struct Iir_Type_Definition_Abs: public virtual Iir {
    Iir* Incomplete_Type_Declaration;
    Iir* Type_Definition;
};

using Iir_Incomplete_Type_Declaration_Abs = Iir_Type_Definition_Abs;

struct Iir_Use_Flag_Abs: public virtual Iir {
    bool Use_Flag;
};

struct Iir_Type_Declaration
        : public Iir_Chain_Abs,
          public Iir_Type_Definition_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Use_Flag_Abs {
};

struct Iir_Anonymous_Type_Declaration
        : public Iir_Chain_Abs,
          public Iir_Type_Definition_Abs,
          public Iir_Identifier_Abs,
          public Iir_Parent_Abs {
    Iir* Subtype_Definition;
};

struct Iir_Subtype_Indication_Abs: public virtual Iir {
    Iir* Subtype_Indication;
};

struct Iir_Subtype_Declaration
        : public Iir_Chain_Abs,
          public Iir_Type_Abs,
          public Iir_Subtype_Indication_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Use_Flag_Abs {
};

struct Iir_Nature_Abs: public virtual Iir {
    Iir* Nature;
};

struct Iir_Nature_Declaration
        : public Iir_Chain_Abs,
          public Iir_Nature_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Use_Flag_Abs {
};

struct Iir_Subnature_Declaration
        : public Iir_Chain_Abs,
          public Iir_Nature_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Use_Flag_Abs {
};

struct Iir_Attribute_Value_Chain_Abs: public virtual Iir {
    Iir* Attribute_Value_Chain;
};

struct Iir_Package_Body_Abs: public virtual Iir {
    Iir* Package_Body;
};

struct Iir_Package_Declaration
        : public Iir_Attribute_Value_Chain_Abs,
          public Iir_Package_Body_Abs,
          public Iir_Chain_Abs,
          public Iir_Declaration_Chain_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Design_Unit_Abs,
          public Iir_End_Has_Reserved_Id_Abs,
          public Iir_End_Has_Identifier_Abs {
    bool Macro_Expanded_Flag;
    Iir* Package_Header;
    bool Need_Instance_Bodies;
    bool Need_Body;
    Iir* Package_Origin;
};

struct Iir_Uninstantiated_Package_Name_Abs: public virtual Iir {
    Iir* Uninstantiated_Package_Name;
    Iir* Uninstantiated_Package_Decl;
};

using Iir_Uninstantiated_Package_Decl_Abs = Iir_Uninstantiated_Package_Name_Abs;

struct Iir_Package_Instantiation_Declaration
        : public Iir_Attribute_Value_Chain_Abs,
          public Iir_Package_Body_Abs,
          public Iir_Chain_Abs,
          public Iir_Generic_Chain_Abs,
          public Iir_Declaration_Chain_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Generic_Map_Aspect_Chain_Abs,
          public Iir_Uninstantiated_Package_Name_Abs,
          public Iir_Parent_Design_Unit_Abs,
          public Iir_End_Has_Reserved_Id_Abs,
          public Iir_End_Has_Identifier_Abs {
};

struct Iir_Package_Body
        : public Iir_Attribute_Value_Chain_Abs,
          public Iir_Chain_Abs,
          public Iir_Declaration_Chain_Abs,
          public Iir_Identifier_Abs,
          public Iir_Parent_Design_Unit_Abs,
          public Iir_End_Has_Reserved_Id_Abs,
          public Iir_End_Has_Identifier_Abs {
    Iir* Package;
};

struct Iir_Configuration_Declaration
        : public Iir_Attribute_Value_Chain_Abs,
          public Iir_Entity_Name_Abs,
          public Iir_Block_Configuration_Abs,
          public Iir_Declaration_Chain_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Design_Unit_Abs,
          public Iir_End_Has_Reserved_Id_Abs,
          public Iir_End_Has_Identifier_Abs {
};

struct Iir_Concurrent_Statement_Chain_Abs: public virtual Iir {
    Iir* Concurrent_Statement_Chain;
};

struct Iir_Is_Within_Flag_Abs: public virtual Iir {
    bool Is_Within_Flag;
};

struct Iir_Has_Begin_Abs: public virtual Iir {
    bool Has_Begin;
};

struct Iir_Entity_Declaration
        : public Iir_Attribute_Value_Chain_Abs,
          public Iir_Concurrent_Statement_Chain_Abs,
          public Iir_Port_Chain_Abs,
          public Iir_Generic_Chain_Abs,
          public Iir_Declaration_Chain_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Design_Unit_Abs,
          public Iir_Is_Within_Flag_Abs,
          public Iir_End_Has_Reserved_Id_Abs,
          public Iir_End_Has_Identifier_Abs,
          public Iir_Has_Begin_Abs {
};

struct Iir_Foreign_Flag_Abs: public virtual Iir {
    bool Foreign_Flag;
};

struct Iir_Architecture_Body
        : public Iir_Attribute_Value_Chain_Abs,
          public Iir_Entity_Name_Abs,
          public Iir_Concurrent_Statement_Chain_Abs,
          public Iir_Declaration_Chain_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Foreign_Flag_Abs,
          public Iir_Parent_Design_Unit_Abs,
          public Iir_Is_Within_Flag_Abs,
          public Iir_End_Has_Reserved_Id_Abs,
          public Iir_End_Has_Identifier_Abs {
    Iir* Default_Configuration_Declaration;
};

struct Iir_Context_Declaration
        : public Iir_Context_Items_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Design_Unit_Abs,
          public Iir_End_Has_Reserved_Id_Abs,
          public Iir_End_Has_Identifier_Abs {
};

struct Iir_Package_Header: public Iir_Generic_Chain_Abs, public Iir_Generic_Map_Aspect_Chain_Abs {
};

struct Iir_Unit_Declaration
        : public Iir_Chain_Abs,
          public Iir_Type_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs {
    Iir* Physical_Literal;
};

struct Iir_Library_Declaration
        : public Iir_Date_Abs,
          public Iir_Chain_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Elab_Flag_Abs {
    std::vector<Iir_Design_File*> Design_Files;
    std::filesystem::path Library_Directory;
};

struct Iir_Has_Is_Abs: public virtual Iir {
    bool Has_Is;
};

struct Iir_Component_Declaration
        : public Iir_Chain_Abs,
          public Iir_Port_Chain_Abs,
          public Iir_Generic_Chain_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Use_Flag_Abs,
          public Iir_End_Has_Reserved_Id_Abs,
          public Iir_End_Has_Identifier_Abs,
          public Iir_Has_Is_Abs {
};

struct Iir_Attribute_Declaration
        : public Iir_Chain_Abs,
          public Iir_Type_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Type_Mark_Abs,
          public Iir_Use_Flag_Abs {
};

struct Iir_Group_Template_Declaration
        : public Iir_Chain_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Use_Flag_Abs {
    Iir* Entity_Class_Entry_Chain;
};

struct Iir_Group_Declaration
        : public Iir_Chain_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Use_Flag_Abs {
    std::vector<Iir*> Group_Constituent_List;
    Iir* Group_Template_Name;
};

struct Iir_Element_Declaration
        : public Iir_Type_Abs,
          public Iir_Subtype_Indication_Abs,
//          public Iir_Element_Position_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Has_Identifier_List_Abs {
};

struct Iir_Name_Abs: public virtual Iir {
    Iir* Name;
};

struct Iir_Non_Object_Alias_Declaration
        : public Iir_Chain_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Name_Abs,
          public Iir_Use_Flag_Abs {
    Iir* Alias_Signature;
    bool Implicit_Alias_Flag;
};

struct Iir_Psl_Declaration_Abs: public virtual Iir {
    PSL_Node Psl_Declaration;
};

struct Iir_PSL_Clock_Abs: public virtual Iir {
    PSL_NFA PSL_Nfa;
    PSL_Node PSL_Clock;
};

using Iir_PSL_NFA_Abs = Iir_PSL_Clock_Abs;

struct Iir_Psl_Declaration
        : public Iir_Chain_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Use_Flag_Abs,
          public Iir_Psl_Declaration_Abs,
          public Iir_PSL_Clock_Abs {
};

struct Iir_PSL_Nbr_States_Abs: public virtual Iir {
    bool PSL_EOS_Flag;
    int PSL_Nbr_States;
    std::vector<Iir*> PSL_Clock_Sensitivity;
};

using Iir_PSL_Clock_Sensitivity_Abs = Iir_PSL_Nbr_States_Abs;

using Iir_PSL_EOS_Flag_Abs = Iir_PSL_Nbr_States_Abs;

struct Iir_Psl_Endpoint_Declaration
        : public Iir_Chain_Abs,
          public Iir_Type_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Use_Flag_Abs,
          public Iir_Psl_Declaration_Abs,
          public Iir_PSL_Clock_Abs,
          public Iir_PSL_Nbr_States_Abs {
};

struct Iir_Terminal_Declaration
        : public Iir_Chain_Abs,
          public Iir_Nature_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Use_Flag_Abs {
};

struct Iir_Default_Value_Abs: public virtual Iir {
    Iir* Default_Value;
};

struct Iir_Free_Quantity_Declaration
        : public Iir_Chain_Abs,
          public Iir_Type_Abs,
          public Iir_Default_Value_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Use_Flag_Abs {
};

struct Iir_Plus_Terminal_Abs: public virtual Iir {
    Iir* Minus_Terminal;
    Iir* Plus_Terminal;
};

using Iir_Minus_Terminal_Abs = Iir_Plus_Terminal_Abs;

struct Iir_Across_Quantity_Declaration
        : public Iir_Chain_Abs,
          public Iir_Type_Abs,
          public Iir_Default_Value_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Tolerance_Abs,
          public Iir_Plus_Terminal_Abs,
          public Iir_Parent_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Use_Flag_Abs {
};

struct Iir_Through_Quantity_Declaration
        : public Iir_Chain_Abs,
          public Iir_Type_Abs,
          public Iir_Default_Value_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Tolerance_Abs,
          public Iir_Plus_Terminal_Abs,
          public Iir_Parent_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Use_Flag_Abs {
};

struct Iir_Subprogram_Hash_Abs: public virtual Iir {
    int Subprogram_Hash;
};

struct Iir_Seen_Flag_Abs: public virtual Iir {
    bool Seen_Flag;
};

struct Iir_Enumeration_Literal
        : public Iir_Literal_Origin_Abs,
          public Iir_Type_Abs,
          public Iir_Subprogram_Hash_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Seen_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Is_Within_Flag_Abs {
    int Enum_Pos;
};

struct Iir_Interface_Declaration_Chain_Abs: public virtual Iir {
    int Subprogram_Depth;
    Iir* Interface_Declaration_Chain;
    Iir_All_Sensitized All_Sensitized_State;
    bool Has_Parameter;
};

struct Iir_Subprogram_Body_Abs: public virtual Iir {
    Iir_Predefined_Functions Implicit_Definition;
    Iir* Subprogram_Body;
    bool Has_Body;
    int Overload_Number;
    bool Hide_Implicit_Flag;
};

using Iir_Overload_Number_Abs = Iir_Subprogram_Body_Abs;

using Iir_Subprogram_Depth_Abs = Iir_Interface_Declaration_Chain_Abs;

struct Iir_Return_Type_Abs: public virtual Iir {
    bool Has_Pure;
    Iir* Return_Type;
    bool Pure_Flag;
    bool Resolution_Function_Flag;
};

using Iir_Implicit_Definition_Abs = Iir_Subprogram_Body_Abs;

using Iir_Resolution_Function_Flag_Abs = Iir_Return_Type_Abs;

struct Iir_Wait_State_Abs: public virtual Iir {
    Tri_State_Type Wait_State;
};

using Iir_All_Sensitized_State_Abs = Iir_Interface_Declaration_Chain_Abs;

using Iir_Pure_Flag_Abs = Iir_Return_Type_Abs;

using Iir_Hide_Implicit_Flag_Abs = Iir_Subprogram_Body_Abs;

using Iir_Has_Pure_Abs = Iir_Return_Type_Abs;

using Iir_Has_Body_Abs = Iir_Subprogram_Body_Abs;

using Iir_Has_Parameter_Abs = Iir_Interface_Declaration_Chain_Abs;

struct Iir_Function_Declaration
        : public Iir_Chain_Abs,
          public Iir_Generic_Chain_Abs,
          public Iir_Interface_Declaration_Chain_Abs,
          public Iir_Subprogram_Body_Abs,
          public Iir_Subprogram_Hash_Abs,
          public Iir_Return_Type_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Wait_State_Abs,
          public Iir_Seen_Flag_Abs,
          public Iir_Foreign_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Return_Type_Mark_Abs,
          public Iir_Is_Within_Flag_Abs,
          public Iir_Use_Flag_Abs {
};

struct Iir_Passive_Flag_Abs: public virtual Iir {
    bool Passive_Flag;
};

struct Iir_Suspend_Flag_Abs: public virtual Iir {
    bool Suspend_Flag;
};

struct Iir_Procedure_Declaration
        : public Iir_Generic_Chain_Abs,
          public Iir_Interface_Declaration_Chain_Abs,
          public Iir_Subprogram_Body_Abs,
          public Iir_Subprogram_Hash_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Passive_Flag_Abs,
          public Iir_Wait_State_Abs,
          public Iir_Seen_Flag_Abs,
          public Iir_Foreign_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Return_Type_Mark_Abs,
          public Iir_Is_Within_Flag_Abs,
          public Iir_Use_Flag_Abs,
          public Iir_Suspend_Flag_Abs {
    Iir_Pure_State Purity_State;
};

struct Iir_Subprogram_Specification_Abs: public virtual Iir {
    Iir* Subprogram_Specification;
    int Impure_Depth;
};

struct Iir_Sequential_Statement_Chain_Abs: public virtual Iir {
    Iir* Sequential_Statement_Chain;
};

using Iir_Impure_Depth_Abs = Iir_Subprogram_Specification_Abs;

struct Iir_Callees_List_Abs: public virtual Iir {
    std::vector<Iir*> Callees_List;
};

struct Iir_Function_Body
        : public Iir_Attribute_Value_Chain_Abs,
          public Iir_Chain_Abs,
          public Iir_Subprogram_Specification_Abs,
          public Iir_Sequential_Statement_Chain_Abs,
          public Iir_Declaration_Chain_Abs,
          public Iir_Callees_List_Abs,
          public Iir_Parent_Abs,
          public Iir_End_Has_Reserved_Id_Abs,
          public Iir_End_Has_Identifier_Abs {
};

struct Iir_Procedure_Body
        : public Iir_Attribute_Value_Chain_Abs,
          public Iir_Chain_Abs,
          public Iir_Subprogram_Specification_Abs,
          public Iir_Sequential_Statement_Chain_Abs,
          public Iir_Declaration_Chain_Abs,
          public Iir_Callees_List_Abs,
          public Iir_Parent_Abs,
          public Iir_End_Has_Reserved_Id_Abs,
          public Iir_End_Has_Identifier_Abs,
          public Iir_Suspend_Flag_Abs {
};

struct Iir_After_Drivers_Flag_Abs: public virtual Iir {
    bool After_Drivers_Flag;
};

struct Iir_Object_Alias_Declaration
        : public Iir_After_Drivers_Flag_Abs,
          public Iir_Chain_Abs,
          public Iir_Type_Abs,
          public Iir_Subtype_Indication_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Use_Flag_Abs {
};

struct Iir_Mode_Abs: public virtual Iir {
    std::optional<Iir_Mode> Mode;
};

struct Iir_File_Declaration
        : public Iir_Chain_Abs,
          public Iir_Type_Abs,
          public Iir_Subtype_Indication_Abs,
          public Iir_Mode_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Use_Flag_Abs,
          public Iir_Has_Identifier_List_Abs {
    Iir* File_Open_Kind;
    Iir* File_Logical_Name;
};

struct Iir_Guarded_Signal_Flag_Abs: public virtual Iir {
    bool Guarded_Signal_Flag;
    Iir_Signal_Kind Signal_Kind;
};

using Iir_Signal_Kind_Abs = Iir_Guarded_Signal_Flag_Abs;

struct Iir_Has_Active_Flag_Abs: public virtual Iir {
    bool Has_Active_Flag;
};

struct Iir_Guard_Signal_Declaration
        : public Iir_Type_Abs,
          public Iir_Guarded_Signal_Flag_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Has_Active_Flag_Abs,
          public Iir_Use_Flag_Abs {
    Iir* Block_Statement;
    std::vector<Iir*> Guard_Sensitivity_List;
    Iir* Guard_Expression;
};

struct Iir_Has_Disconnect_Flag_Abs: public virtual Iir {
    bool Has_Disconnect_Flag;
};

struct Iir_Signal_Declaration
        : public Iir_After_Drivers_Flag_Abs,
          public Iir_Chain_Abs,
          public Iir_Type_Abs,
          public Iir_Subtype_Indication_Abs,
          public Iir_Guarded_Signal_Flag_Abs,
          public Iir_Default_Value_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Has_Disconnect_Flag_Abs,
          public Iir_Has_Active_Flag_Abs,
          public Iir_Use_Flag_Abs,
          public Iir_Has_Identifier_List_Abs,
          public Iir_Is_Ref_Abs {
};

struct Iir_Shared_Flag_Abs: public virtual Iir {
    bool Shared_Flag;
};

struct Iir_Variable_Declaration
        : public Iir_Chain_Abs,
          public Iir_Type_Abs,
          public Iir_Subtype_Indication_Abs,
          public Iir_Default_Value_Abs,
          public Iir_Shared_Flag_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Use_Flag_Abs,
          public Iir_Has_Identifier_List_Abs,
          public Iir_Is_Ref_Abs {
};

struct Iir_Constant_Declaration
        : public Iir_Chain_Abs,
          public Iir_Type_Abs,
          public Iir_Subtype_Indication_Abs,
          public Iir_Default_Value_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Use_Flag_Abs,
          public Iir_Has_Identifier_List_Abs,
          public Iir_Is_Ref_Abs {
    bool Deferred_Declaration_Flag;
    Iir* Deferred_Declaration;
};

struct Iir_Iterator_Declaration
        : public Iir_Chain_Abs,
          public Iir_Type_Abs,
          public Iir_Subtype_Indication_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Use_Flag_Abs,
          public Iir_Has_Identifier_List_Abs {
    Iir* Discrete_Range;
};

struct Iir_Has_Class_Abs: public virtual Iir {
    bool Has_Class;
};


struct Iir_Interface_Signal_Declaration_Extras
        : public Iir_Guarded_Signal_Flag_Abs,
          public Iir_Has_Disconnect_Flag_Abs,
          public Iir_Has_Active_Flag_Abs {
    bool Open_Flag;
};

struct Iir_Interface_Object_Declaration :
        public Iir_Identifier_Abs,
        public Iir_Visible_Flag_Abs,
        public Iir_Parent_Abs,
        public Iir_Expr_Staticness_Abs,
        public Iir_After_Drivers_Flag_Abs,
        public Iir_Chain_Abs,
        public Iir_Type_Abs,
        public Iir_Mode_Abs,
        public Iir_Default_Value_Abs,
        public Iir_Name_Staticness_Abs,
        public Iir_Use_Flag_Abs,
        public Iir_Subtype_Indication_Abs,
        public Iir_Has_Identifier_List_Abs,
        public Iir_Has_Class_Abs,
        public Iir_Is_Ref_Abs {
    enum class type { constant, variable, signal, file } Interface_Object_Type;
    std::optional<Iir_Interface_Signal_Declaration_Extras> extras;
};


struct Iir_Interface_Type_Declaration
        : public Iir_Chain_Abs,
          public Iir_Type_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Use_Flag_Abs,
          public Iir_Has_Identifier_List_Abs,
          public Iir_Is_Ref_Abs {
    Iir* Interface_Type_Subprograms;
};

struct Iir_Interface_Package_Declaration
        : public Iir_Attribute_Value_Chain_Abs,
          public Iir_Chain_Abs,
          public Iir_Generic_Chain_Abs,
          public Iir_Declaration_Chain_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Generic_Map_Aspect_Chain_Abs,
          public Iir_Uninstantiated_Package_Name_Abs,
          public Iir_Parent_Design_Unit_Abs,
          public Iir_Is_Within_Flag_Abs {
};

struct Iir_Interface_Function_Declaration
        : public Iir_Chain_Abs,
          public Iir_Interface_Declaration_Chain_Abs,
          public Iir_Subprogram_Hash_Abs,
          public Iir_Return_Type_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Seen_Flag_Abs,
          public Iir_Foreign_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Return_Type_Mark_Abs,
          public Iir_Use_Flag_Abs {
};

struct Iir_Interface_Procedure_Declaration
        : public Iir_Chain_Abs,
          public Iir_Interface_Declaration_Chain_Abs,
          public Iir_Subprogram_Hash_Abs,
          public Iir_Identifier_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Seen_Flag_Abs,
          public Iir_Foreign_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Return_Type_Mark_Abs,
          public Iir_Use_Flag_Abs {
};

struct Iir_Signal_Attribute_Declaration: public Iir_Chain_Abs, public Iir_Parent_Abs {
    Iir* Signal_Attribute_Chain;
};

struct Iir_Operand_Abs: public virtual Iir {
    Iir* Operand;
};

enum class Iir_Unary_Operator_Type {
    Identity,
    Negation,
    Absolute,
    Not,
    Condition,
    Reduction_And,
    Reduction_Or,
    Reduction_Nand,
    Reduction_Nor,
    Reduction_Xor,
    Reduction_Xnor,

};

struct Iir_Unary_Operator
        : public Iir_Type_Abs, public Iir_Expr_Staticness_Abs, public Iir_Operand_Abs, public Iir_Implementation_Abs {
    Iir_Unary_Operator_Type Operator_Type;
};

enum class Iir_Binary_Operator_Type {
    And,
    Or,
    Nand,
    Nor,
    Xor,
    Xnor,
    Equality,
    Inequality,
    Less_Than,
    Less_Than_Or_Equal,
    Greater_Than,
    Greater_Than_Or_Equal,
    Match_Equality,
    Match_Inequality,
    Match_Less_Than,
    Match_Less_Than_Or_Equal,
    Match_Greater_Than,
    Match_Greater_Than_Or_Equal,
    Sll,
    Sla,
    Srl,
    Sra,
    Rol,
    Ror,
    Addition,
    Subtraction,
    Concatenation,
    Multiplication,
    Division,
    Modulus,
    Remainder,
    Exponentiation
};

struct Iir_Binary_Operator
        : public Iir_Type_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Implementation_Abs {
    Iir* Left;
    Iir* Right;
    Iir_Binary_Operator_Type Operator_Type;
};


struct Iir_Function_Call
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs,
          public Iir_Implementation_Abs,
          public Iir_Parameter_Association_Chain_Abs {
};

struct Iir_Aggregate
        : public Iir_Literal_Origin_Abs,
          public Iir_Literal_Subtype_Abs,
          public Iir_Type_Abs,
          public Iir_Expr_Staticness_Abs {
    Iir* Association_Choices_Chain;
    bool Aggregate_Expand_Flag;
    Iir* Aggregate_Info;
};

struct Iir_Parenthesis_Expression: public Iir_Type_Abs, public Iir_Expression_Abs, public Iir_Expr_Staticness_Abs {
};

struct Iir_Qualified_Expression
        : public Iir_Type_Abs, public Iir_Expression_Abs, public Iir_Expr_Staticness_Abs, public Iir_Type_Mark_Abs {
};

struct Iir_Type_Conversion
        : public Iir_Type_Abs, public Iir_Expression_Abs, public Iir_Expr_Staticness_Abs, public Iir_Type_Mark_Abs {
    Iir* Type_Conversion_Subtype;
};

struct Iir_Allocator_Designated_Type_Abs: public virtual Iir {
    Iir* Allocator_Designated_Type;
};

struct Iir_Allocator_By_Expression
        : public Iir_Type_Abs,
          public Iir_Expression_Abs,
          public Iir_Allocator_Designated_Type_Abs,
          public Iir_Expr_Staticness_Abs {
};

struct Iir_Allocator_By_Subtype
        : public Iir_Type_Abs,
          public Iir_Subtype_Indication_Abs,
          public Iir_Allocator_Designated_Type_Abs,
          public Iir_Expr_Staticness_Abs {
    Iir* Allocator_Subtype;
};

struct Iir_Selected_Element
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs {
    Iir* Selected_Element;
};

struct Iir_Dereference
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs {
};

struct Iir_Implicit_Dereference
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs {
};

struct Iir_Slice_Name
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs {
    Iir* Suffix;
    Iir* Slice_Subtype;
};

struct Iir_Indexed_Name
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs {
    std::vector<Iir*> Index_List;
};

struct Iir_Psl_Expression: public Iir_Type_Abs {
    PSL_Node Psl_Expression;
};

struct Iir_Label_Abs: public virtual Iir {
    std::string Label;
};

struct Iir_Sensitivity_List_Abs: public virtual Iir {
    std::vector<Iir*> Sensitivity_List;
};

struct Iir_Process_Origin_Abs: public virtual Iir {
    bool End_Has_Postponed;
    Iir* Process_Origin;
};

struct Iir_Postponed_Flag_Abs: public virtual Iir {
    bool Postponed_Flag;
};

using Iir_End_Has_Postponed_Abs = Iir_Process_Origin_Abs;

struct Iir_Has_Label_Abs: public virtual Iir {
    bool Has_Label;
};

struct Iir_Sensitized_Process_Statement
        : public Iir_Attribute_Value_Chain_Abs,
          public Iir_Chain_Abs,
          public Iir_Sequential_Statement_Chain_Abs,
          public Iir_Declaration_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Sensitivity_List_Abs,
          public Iir_Process_Origin_Abs,
          public Iir_Postponed_Flag_Abs,
          public Iir_Callees_List_Abs,
          public Iir_Passive_Flag_Abs,
          public Iir_Wait_State_Abs,
          public Iir_Seen_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Is_Within_Flag_Abs,
          public Iir_End_Has_Reserved_Id_Abs,
          public Iir_End_Has_Identifier_Abs,
          public Iir_Has_Label_Abs,
          public Iir_Has_Is_Abs,
          public Iir_Is_Ref_Abs {
};

struct Iir_Process_Statement
        : public Iir_Attribute_Value_Chain_Abs,
          public Iir_Chain_Abs,
          public Iir_Sequential_Statement_Chain_Abs,
          public Iir_Declaration_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Process_Origin_Abs,
          public Iir_Postponed_Flag_Abs,
          public Iir_Callees_List_Abs,
          public Iir_Passive_Flag_Abs,
          public Iir_Wait_State_Abs,
          public Iir_Seen_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Is_Within_Flag_Abs,
          public Iir_End_Has_Reserved_Id_Abs,
          public Iir_End_Has_Identifier_Abs,
          public Iir_Has_Label_Abs,
          public Iir_Has_Is_Abs,
          public Iir_Suspend_Flag_Abs {
};

struct Iir_Guarded_Target_State_Abs: public virtual Iir {
    Iir_Delay_Mechanism Delay_Mechanism;
    Tri_State_Type Guarded_Target_State;
    Iir* Reject_Time_Expression;
};

struct Iir_Target_Abs: public virtual Iir {
    Iir* Target;
};

struct Iir_Guard_Abs: public virtual Iir {
    Iir* Guard;
};

using Iir_Delay_Mechanism_Abs = Iir_Guarded_Target_State_Abs;

using Iir_Reject_Time_Expression_Abs = Iir_Guarded_Target_State_Abs;

struct Iir_Concurrent_Simple_Signal_Assignment
        : public Iir_Guarded_Target_State_Abs,
          public Iir_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Target_Abs,
          public Iir_Guard_Abs,
          public Iir_Postponed_Flag_Abs,
          public Iir_Parent_Abs {
    Iir_Waveform waveform;
};

using Iir_Conditional_Waveforms = std::vector<Iir_Conditional_Waveform*>;

struct Iir_Concurrent_Conditional_Signal_Assignment
        : public Iir_Guarded_Target_State_Abs,
          public Iir_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Target_Abs,
          public Iir_Guard_Abs,
          public Iir_Postponed_Flag_Abs,
          public Iir_Parent_Abs {
    Iir_Conditional_Waveforms conditional_waveforms;
};

struct Iir_Selected_Waveform_Chain_Abs: public virtual Iir {
    Iir* Selected_Waveform_Chain;
};

struct Iir_Concurrent_Selected_Signal_Assignment
        : public Iir_Guarded_Target_State_Abs,
          public Iir_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Target_Abs,
          public Iir_Guard_Abs,
          public Iir_Postponed_Flag_Abs,
          public Iir_Expression_Abs,
          public Iir_Selected_Waveform_Chain_Abs,
          public Iir_Parent_Abs {
};

struct Iir_Assertion_Condition_Abs: public virtual Iir {
    Iir* Assertion_Condition;
};

struct Iir_Report_Expression_Abs: public virtual Iir {
    Iir* Report_Expression;
    Iir* Severity_Expression;
};

using Iir_Severity_Expression_Abs = Iir_Report_Expression_Abs;

struct Iir_Concurrent_Assertion_Statement
        : public Iir_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Postponed_Flag_Abs,
          public Iir_Assertion_Condition_Abs,
          public Iir_Report_Expression_Abs,
          public Iir_Parent_Abs {
};

struct Iir_Procedure_Call_Abs: public virtual Iir {
    Iir* Procedure_Call;
};

struct Iir_Concurrent_Procedure_Call_Statement
        : public Iir_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Postponed_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Procedure_Call_Abs,
          public Iir_Suspend_Flag_Abs {
};

struct Iir_Psl_Assert_Statement
        : public Iir_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Postponed_Flag_Abs,
          public Iir_Report_Expression_Abs,
          public Iir_Parent_Abs,
          public Iir_PSL_Clock_Abs,
          public Iir_PSL_Nbr_States_Abs {
    PSL_Node Psl_Property;
};

struct Iir_Psl_Cover_Statement
        : public Iir_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Postponed_Flag_Abs,
          public Iir_Report_Expression_Abs,
          public Iir_Parent_Abs,
          public Iir_PSL_Clock_Abs,
          public Iir_PSL_Nbr_States_Abs {
    PSL_Node Psl_Sequence;
};

struct Iir_Block_Statement
        : public Iir_Attribute_Value_Chain_Abs,
          public Iir_Concurrent_Statement_Chain_Abs,
          public Iir_Chain_Abs,
          public Iir_Declaration_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Is_Within_Flag_Abs,
          public Iir_End_Has_Reserved_Id_Abs,
          public Iir_End_Has_Identifier_Abs {
    Iir* Guard_Decl;
    Iir* Block_Block_Configuration;
    Iir* Block_Header;
};

struct Iir_Generate_Statement_Body_Abs: public virtual Iir {
    Iir* Generate_Statement_Body;
};

struct Iir_Generate_Else_Clause_Abs: public virtual Iir {
    Iir* Generate_Else_Clause;
};

struct Iir_If_Generate_Statement
        : public Iir_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Generate_Statement_Body_Abs,
          public Iir_Generate_Else_Clause_Abs,
          public Iir_Condition_Abs,
          public Iir_Parent_Abs,
          public Iir_Is_Within_Flag_Abs,
          public Iir_End_Has_Reserved_Id_Abs,
          public Iir_End_Has_Identifier_Abs {
};

struct Iir_Case_Statement_Alternative_Chain_Abs: public virtual Iir {
    Iir* Case_Statement_Alternative_Chain;
};

struct Iir_Case_Generate_Statement
        : public Iir_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Expression_Abs,
          public Iir_Parent_Abs,
          public Iir_Case_Statement_Alternative_Chain_Abs,
          public Iir_Is_Within_Flag_Abs,
          public Iir_End_Has_Reserved_Id_Abs,
          public Iir_End_Has_Identifier_Abs {
};

struct Iir_Parameter_Specification_Abs: public virtual Iir {
    Iir* Parameter_Specification;
};

struct Iir_For_Generate_Statement
        : public Iir_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Generate_Statement_Body_Abs,
          public Iir_Parameter_Specification_Abs,
          public Iir_Parent_Abs,
          public Iir_Is_Within_Flag_Abs,
          public Iir_End_Has_Reserved_Id_Abs,
          public Iir_End_Has_Identifier_Abs {
};

struct Iir_Component_Instantiation_Statement
        : public Iir_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Generic_Map_Aspect_Chain_Abs,
          public Iir_Port_Map_Aspect_Chain_Abs,
          public Iir_Parent_Abs {
    Iir* Instantiated_Unit;
    Iir* Default_Binding_Indication;
    Iir* Component_Configuration;
    Iir* Configuration_Specification;
};

struct Iir_Psl_Default_Clock: public Iir_Chain_Abs, public Iir_Label_Abs, public Iir_Parent_Abs {
    PSL_Node Psl_Boolean;
};

struct Iir_Simple_Simultaneous_Statement
        : public Iir_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Tolerance_Abs,
          public Iir_Parent_Abs {
    Iir* Simultaneous_Left;
    Iir* Simultaneous_Right;
};

struct Iir_Generate_Statement_Body
        : public Iir_Attribute_Value_Chain_Abs,
          public Iir_Concurrent_Statement_Chain_Abs,
          public Iir_Declaration_Chain_Abs,
          public Iir_Parent_Abs,
          public Iir_Is_Within_Flag_Abs,
          public Iir_End_Has_Identifier_Abs,
          public Iir_Has_Label_Abs,
          public Iir_Has_Begin_Abs {
    Iir* Generate_Block_Configuration;
    std::string Alternative_Label;
    bool Has_End;
};

struct Iir_If_Generate_Else_Clause
        : public Iir_Visible_Flag_Abs,
          public Iir_Generate_Statement_Body_Abs,
          public Iir_Generate_Else_Clause_Abs,
          public Iir_Condition_Abs,
          public Iir_Parent_Abs {
};

struct Iir_Simple_Signal_Assignment_Statement
        : public Iir_Guarded_Target_State_Abs,
          public Iir_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Target_Abs,
          public Iir_Parent_Abs {
    Iir_Waveform waveform;
};

struct Iir_Conditional_Signal_Assignment_Statement
        : public Iir_Guarded_Target_State_Abs,
          public Iir_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Target_Abs,
          public Iir_Parent_Abs {
    Iir_Conditional_Waveforms conditional_waveforms;
};

struct Iir_Selected_Waveform_Assignment_Statement
        : public Iir_Guarded_Target_State_Abs,
          public Iir_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Target_Abs,
          public Iir_Expression_Abs,
          public Iir_Selected_Waveform_Chain_Abs,
          public Iir_Parent_Abs {
};

struct Iir_Null_Statement
        : public Iir_Chain_Abs, public Iir_Label_Abs, public Iir_Visible_Flag_Abs, public Iir_Parent_Abs {
};

struct Iir_Assertion_Statement
        : public Iir_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Assertion_Condition_Abs,
          public Iir_Report_Expression_Abs,
          public Iir_Parent_Abs {
};

struct Iir_Report_Statement
        : public Iir_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Report_Expression_Abs,
          public Iir_Parent_Abs {
};

struct Iir_Wait_Statement
        : public Iir_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Sensitivity_List_Abs,
          public Iir_Parent_Abs,
          public Iir_Is_Ref_Abs {
    Iir* Condition_Clause;
    Iir* Timeout_Clause;
};

struct Iir_Variable_Assignment_Statement
        : public Iir_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Target_Abs,
          public Iir_Expression_Abs,
          public Iir_Parent_Abs {
};

struct Iir_Conditional_Variable_Assignment_Statement
        : public Iir_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Target_Abs,
          public Iir_Parent_Abs {
    Iir* Conditional_Expression;
};

struct Iir_Return_Statement
        : public Iir_Chain_Abs,
          public Iir_Type_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Expression_Abs,
          public Iir_Parent_Abs {
};

struct Iir_For_Loop_Statement
        : public Iir_Chain_Abs,
          public Iir_Sequential_Statement_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parameter_Specification_Abs,
          public Iir_Parent_Abs,
          public Iir_Is_Within_Flag_Abs,
          public Iir_End_Has_Identifier_Abs,
          public Iir_Suspend_Flag_Abs {
};

struct Iir_While_Loop_Statement
        : public Iir_Chain_Abs,
          public Iir_Sequential_Statement_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Condition_Abs,
          public Iir_Parent_Abs,
          public Iir_End_Has_Identifier_Abs,
          public Iir_Suspend_Flag_Abs {
};

struct Iir_Loop_Label_Abs: public virtual Iir {
    Iir* Loop_Label;
};

struct Iir_Next_Statement
        : public Iir_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Condition_Abs,
          public Iir_Parent_Abs,
          public Iir_Loop_Label_Abs {
};

struct Iir_Exit_Statement
        : public Iir_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Condition_Abs,
          public Iir_Parent_Abs,
          public Iir_Loop_Label_Abs {
};

struct Iir_Case_Statement
        : public Iir_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Expression_Abs,
          public Iir_Parent_Abs,
          public Iir_Case_Statement_Alternative_Chain_Abs,
          public Iir_End_Has_Identifier_Abs,
          public Iir_Suspend_Flag_Abs {
};

struct Iir_Procedure_Call_Statement
        : public Iir_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Procedure_Call_Abs,
          public Iir_Suspend_Flag_Abs {
};

struct Iir_Else_Clause_Abs: public virtual Iir {
    Iir* Else_Clause;
};

struct Iir_If_Statement
        : public Iir_Chain_Abs,
          public Iir_Sequential_Statement_Chain_Abs,
          public Iir_Label_Abs,
          public Iir_Visible_Flag_Abs,
          public Iir_Condition_Abs,
          public Iir_Else_Clause_Abs,
          public Iir_Parent_Abs,
          public Iir_End_Has_Identifier_Abs,
          public Iir_Suspend_Flag_Abs {
};

struct Iir_Elsif
        : public Iir_Sequential_Statement_Chain_Abs,
          public Iir_Condition_Abs,
          public Iir_Else_Clause_Abs,
          public Iir_Parent_Abs,
          public Iir_End_Has_Identifier_Abs {
};

struct Iir_Named_Entity_Abs: public virtual Iir {
    Iir* Named_Entity;
    bool Is_Forward_Ref;
};

struct Iir_Alias_Declaration_Abs: public virtual Iir {
    Iir* Alias_Declaration;
};

using Iir_Is_Forward_Ref_Abs = Iir_Named_Entity_Abs;

struct Iir_Character_Literal
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Identifier_Abs,
          public Iir_Named_Entity_Abs,
          public Iir_Alias_Declaration_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs {
};

struct Iir_Simple_Name
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Identifier_Abs,
          public Iir_Named_Entity_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs {
};

struct Iir_Selected_Name
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Identifier_Abs,
          public Iir_Named_Entity_Abs,
          public Iir_Alias_Declaration_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs {
};

struct Iir_Operator_Symbol
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Identifier_Abs,
          public Iir_Named_Entity_Abs,
          public Iir_Alias_Declaration_Abs {
};

struct Iir_Reference_Name: public Iir_Named_Entity_Abs {
    Iir* Referenced_Name;
};

struct Iir_Selected_By_All_Name
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Named_Entity_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Prefix_Abs {
};

struct Iir_Parenthesis_Name
        : public Iir_Type_Abs, public Iir_Named_Entity_Abs, public Iir_Prefix_Abs {
    Iir* Association_Chain;
};

struct Iir_External_Pathname_Abs: public virtual Iir {
    Iir* External_Pathname;
};

enum class External_Name_Type {
    Constant,
    Signal,
    Variable
};

struct Iir_External_Name
        : public Iir_Chain_Abs,
          public Iir_Type_Abs,
          public Iir_Subtype_Indication_Abs,
          public Iir_Shared_Flag_Abs,
          public Iir_Parent_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_External_Pathname_Abs {
    External_Name_Type Name_Type;
};

struct Iir_Pathname_Suffix_Abs: public virtual Iir {
    Iir* Pathname_Suffix;
};

struct Iir_Package_Pathname
        : public Iir_Identifier_Abs,
          public Iir_Named_Entity_Abs,
          public Iir_Pathname_Suffix_Abs {
};

struct Iir_Absolute_Pathname: public Iir_Pathname_Suffix_Abs {
};

struct Iir_Relative_Pathname: public Iir_Pathname_Suffix_Abs {
};

struct Iir_Pathname_Element
        : public Iir_Identifier_Abs,
          public Iir_Named_Entity_Abs,
          public Iir_Pathname_Suffix_Abs {
    Iir* Pathname_Expression;
};

struct Iir_Base_Attribute: public Iir_Type_Abs, public Iir_Prefix_Abs {
};

struct Iir_Subtype_Attribute
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Type_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs {
};

struct Iir_Element_Attribute
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Type_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs {
};

struct Iir_Left_Type_Attribute
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs {
};

struct Iir_Right_Type_Attribute
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs {
};

struct Iir_High_Type_Attribute
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs {
};

struct Iir_Low_Type_Attribute
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs {
};

struct Iir_Ascending_Type_Attribute
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs {
};

struct Iir_Parameter_Abs: public virtual Iir {
    Iir* Parameter;
};

struct Iir_Image_Attribute
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs,
          public Iir_Parameter_Abs {
};

struct Iir_Value_Attribute
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs,
          public Iir_Parameter_Abs {
};

struct Iir_Pos_Attribute
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs,
          public Iir_Parameter_Abs {
};

struct Iir_Val_Attribute
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs,
          public Iir_Parameter_Abs {
};

struct Iir_Succ_Attribute
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs,
          public Iir_Parameter_Abs {
};

struct Iir_Pred_Attribute
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs,
          public Iir_Parameter_Abs {
};

struct Iir_Leftof_Attribute
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs,
          public Iir_Parameter_Abs {
};

struct Iir_Rightof_Attribute
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs,
          public Iir_Parameter_Abs {
};

struct Iir_Attr_Chain_Abs: public virtual Iir {
    Iir* Signal_Attribute_Declaration;
    Iir* Attr_Chain;
};

using Iir_Signal_Attribute_Declaration_Abs = Iir_Attr_Chain_Abs;

struct Iir_Delayed_Attribute
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs,
          public Iir_Parameter_Abs,
          public Iir_Attr_Chain_Abs,
          public Iir_Has_Active_Flag_Abs {
};

struct Iir_Stable_Attribute
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs,
          public Iir_Parameter_Abs,
          public Iir_Attr_Chain_Abs,
          public Iir_Has_Active_Flag_Abs {
};

struct Iir_Quiet_Attribute
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs,
          public Iir_Parameter_Abs,
          public Iir_Attr_Chain_Abs,
          public Iir_Has_Active_Flag_Abs {
};

struct Iir_Transaction_Attribute
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs,
          public Iir_Parameter_Abs,
          public Iir_Attr_Chain_Abs,
          public Iir_Has_Active_Flag_Abs {
};

enum class Iir_Attribute_Type {
    Event,
    Active,
    Last_Event,
    Last_Active,
    Last_Value,
    Driving,
    Driving_Value
};

struct Iir_Attribute
        : public Iir_Type_Abs, public Iir_Expr_Staticness_Abs, public Iir_Name_Staticness_Abs, public Iir_Prefix_Abs {
    Iir_Attribute_Type Attribute_Type;
};

struct Iir_Behavior_Attribute: public virtual Iir {
};

struct Iir_Structure_Attribute: public virtual Iir {
};

struct Iir_Simple_Name_Attribute
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs {
    std::string Simple_Name_Identifier;
    Iir* Simple_Name_Subtype;
};

struct Iir_Instance_Name_Attribute
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs {
};

struct Iir_Path_Name_Attribute
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs {
};

struct Iir_Index_Subtype_Abs: public virtual Iir {
    Iir* Index_Subtype;
};

enum class Array_Attribute_Type {
    Left,
    Right,
    High,
    Low,
    Length,
    Ascending,
    Range,
    Reverse_Range
};

struct Iir_Array_Attribute
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs,
          public Iir_Index_Subtype_Abs,
          public Iir_Parameter_Abs {
    Array_Attribute_Type Attribute_Type;
};

struct Iir_Attribute_Name
        : public Iir_Type_Abs,
          public Iir_Base_Name_Abs,
          public Iir_Identifier_Abs,
          public Iir_Named_Entity_Abs,
          public Iir_Expr_Staticness_Abs,
          public Iir_Name_Staticness_Abs,
          public Iir_Prefix_Abs {
    Iir* Attribute_Signature;
};
#endif //AVLC_IIR_H
