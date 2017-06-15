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

#ifndef AVLC_IIR_H
#define AVLC_IIR_H


#include <string>
#include <vector>
#include <typeinfo>
#include <typeindex>
#include "iir_types.h"
#include "Token.h"
#include <variant>
#include <optional>


#define getVariantValue(var, member) std::visit([](auto bareElement) {return bareElement->member;}, var)
#define setVariantValue(var, member, value) std::visit([value](auto bareElement) {return bareElement->member = value;}, var)
#define matchesType(variable, class) std::type_index(typeid(variable)) == std::type_index(typeid(class))

namespace IIR {


    const int Base_id = 0;
    const int Error_id = 1;
    const int Design_File_id = 2;
    const int Design_Unit_id = 3;
    const int Library_Clause_id = 4;
    const int Use_Clause_id = 5;
    const int Context_Reference_id = 6;
    const int Integer_Literal_id = 7;
    const int Floating_Point_Literal_id = 8;
    const int Null_Literal_id = 9;
    const int String_id = 10;
    const int Physical_Int_Literal_id = 11;
    const int Physical_Fp_Literal_id = 12;
    const int Simple_Aggregate_id = 13;
    const int Overflow_Literal_id = 14;
    const int Unaffected_Waveform_id = 15;
    const int Waveform_Element_id = 16;
    const int Conditional_Waveform_id = 17;
    const int Conditional_Expression_id = 18;
    const int Association_Element_By_Expression_id = 19;
    const int Association_Element_By_Individual_id = 20;
    const int Association_Element_Open_id = 21;
    const int Association_Element_Package_id = 22;
    const int Association_Element_Type_id = 23;
    const int Association_Element_Subprogram_id = 24;
    const int Choice_By_Range_id = 25;
    const int Choice_By_Expression_id = 26;
    const int Choice_By_Others_id = 27;
    const int Choice_By_None_id = 28;
    const int Choice_By_Name_id = 29;
    const int Entity_Aspect_Entity_id = 30;
    const int Entity_Aspect_Configuration_id = 31;
    const int Entity_Aspect_Open_id = 32;
    const int Block_Configuration_id = 33;
    const int Block_Header_id = 34;
    const int Component_Configuration_id = 35;
    const int Binding_Indication_id = 36;
    const int Entity_Class_id = 37;
    const int Attribute_Value_id = 38;
    const int Signature_id = 39;
    const int Aggregate_Info_id = 40;
    const int Procedure_Call_id = 41;
    const int Record_Element_Constraint_id = 42;
    const int Array_Element_Resolution_id = 43;
    const int Record_Element_Resolution_id = 44;
    const int Record_Resolution_id = 45;
    const int Attribute_Specification_id = 46;
    const int Disconnection_Specification_id = 47;
    const int Configuration_Specification_id = 48;
    const int Access_Type_Definition_id = 49;
    const int Incomplete_Type_Definition_id = 50;
    const int Interface_Type_Definition_id = 51;
    const int File_Type_Definition_id = 52;
    const int Protected_Type_Declaration_id = 53;
    const int Record_Type_Definition_id = 54;
    const int Array_Type_Definition_id = 55;
    const int Array_Subtype_Definition_id = 56;
    const int Record_Subtype_Definition_id = 57;
    const int Access_Subtype_Definition_id = 58;
    const int Physical_Subtype_Definition_id = 59;
    const int Floating_Type_Definition_id = 60;
    const int Floating_Subtype_Definition_id = 61;
    const int Integer_Type_Definition_id = 62;
    const int Integer_Subtype_Definition_id = 63;
    const int Enumeration_Subtype_Definition_id = 64;
    const int Enumeration_Type_Definition_id = 65;
    const int Physical_Type_Definition_id = 66;
    const int Range_Expression_id = 67;
    const int Protected_Type_Body_id = 68;
    const int Wildcard_Type_Definition_id = 69;
    const int Subtype_Definition_id = 70;
    const int Scalar_Nature_Definition_id = 71;
    const int Overload_List_id = 72;
    const int Type_Declaration_id = 73;
    const int Anonymous_Type_Declaration_id = 74;
    const int Subtype_Declaration_id = 75;
    const int Nature_Declaration_id = 76;
    const int Subnature_Declaration_id = 77;
    const int Package_Declaration_id = 78;
    const int Package_Instantiation_Declaration_id = 79;
    const int Package_Body_id = 80;
    const int Configuration_Declaration_id = 81;
    const int Entity_Declaration_id = 82;
    const int Architecture_Body_id = 83;
    const int Context_Declaration_id = 84;
    const int Package_Header_id = 85;
    const int Unit_Declaration_id = 86;
    const int Library_Declaration_id = 87;
    const int Component_Declaration_id = 88;
    const int Attribute_Declaration_id = 89;
    const int Group_Template_Declaration_id = 90;
    const int Group_Declaration_id = 91;
    const int Element_Declaration_id = 92;
    const int Non_Object_Alias_Declaration_id = 93;
    const int Psl_Declaration_id = 94;
    const int Psl_Endpoint_Declaration_id = 95;
    const int Terminal_Declaration_id = 96;
    const int Free_Quantity_Declaration_id = 97;
    const int Across_Quantity_Declaration_id = 98;
    const int Through_Quantity_Declaration_id = 99;
    const int Enumeration_Literal_id = 100;
    const int Function_Declaration_id = 101;
    const int Procedure_Declaration_id = 102;
    const int Function_Body_id = 103;
    const int Procedure_Body_id = 104;
    const int Object_Alias_Declaration_id = 105;
    const int File_Declaration_id = 106;
    const int Guard_Signal_Declaration_id = 107;
    const int Signal_Declaration_id = 108;
    const int Variable_Declaration_id = 109;
    const int Constant_Declaration_id = 110;
    const int Iterator_Declaration_id = 111;
    const int Interface_Signal_Declaration_Extras_id = 112;
    const int Interface_Object_Declaration_id = 113;
    const int Interface_Type_Declaration_id = 114;
    const int Interface_Package_Declaration_id = 115;
    const int Interface_Function_Declaration_id = 116;
    const int Interface_Procedure_Declaration_id = 117;
    const int Signal_Attribute_Declaration_id = 118;
    const int Unary_Operator_id = 119;
    const int Binary_Operator_id = 120;
    const int Function_Call_id = 121;
    const int Aggregate_id = 122;
    const int Parenthesis_Expression_id = 123;
    const int Qualified_Expression_id = 124;
    const int Type_Conversion_id = 125;
    const int Allocator_By_Expression_id = 126;
    const int Allocator_By_Subtype_id = 127;
    const int Selected_Element_id = 128;
    const int Dereference_id = 129;
    const int Implicit_Dereference_id = 130;
    const int Slice_Name_id = 131;
    const int Indexed_Name_id = 132;
    const int Psl_Expression_id = 133;
    const int Sensitized_Process_Statement_id = 134;
    const int Process_Statement_id = 135;
    const int Simple_Signal_Assignment_Statement_id = 136;
    const int Conditional_Signal_Assignment_Statement_id = 137;
    const int Selected_Waveform_Assignment_Statement_id = 138;
    const int Concurrent_Simple_Signal_Assignment_id = 139;
    const int Concurrent_Conditional_Signal_Assignment_id = 140;
    const int Concurrent_Selected_Signal_Assignment_id = 141;
    const int Concurrent_Assertion_Statement_id = 142;
    const int Concurrent_Procedure_Call_Statement_id = 143;
    const int Psl_Assert_Statement_id = 144;
    const int Psl_Cover_Statement_id = 145;
    const int Block_Statement_id = 146;
    const int If_Generate_Statement_id = 147;
    const int Case_Generate_Statement_id = 148;
    const int For_Generate_Statement_id = 149;
    const int Component_Instantiation_Statement_id = 150;
    const int Psl_Default_Clock_id = 151;
    const int Simple_Simultaneous_Statement_id = 152;
    const int Generate_Statement_Body_id = 153;
    const int If_Generate_Else_Clause_id = 154;
    const int Null_Statement_id = 155;
    const int Assertion_Statement_id = 156;
    const int Report_Statement_id = 157;
    const int Wait_Statement_id = 158;
    const int Variable_Assignment_Statement_id = 159;
    const int Conditional_Variable_Assignment_Statement_id = 160;
    const int Return_Statement_id = 161;
    const int For_Loop_Statement_id = 162;
    const int While_Loop_Statement_id = 163;
    const int Next_Statement_id = 164;
    const int Exit_Statement_id = 165;
    const int Case_Statement_id = 166;
    const int Procedure_Call_Statement_id = 167;
    const int If_Statement_id = 168;
    const int Elsif_id = 169;
    const int Character_Literal_id = 170;
    const int Simple_Name_id = 171;
    const int Selected_Name_id = 172;
    const int Operator_Symbol_id = 173;
    const int Reference_Name_id = 174;
    const int Selected_By_All_Name_id = 175;
    const int Parenthesis_Name_id = 176;
    const int External_Name_id = 177;
    const int Package_Pathname_id = 178;
    const int Absolute_Pathname_id = 179;
    const int Relative_Pathname_id = 180;
    const int Pathname_Element_id = 181;
    const int Base_Attribute_id = 182;
    const int Subtype_Attribute_id = 183;
    const int Element_Attribute_id = 184;
    const int Left_Type_Attribute_id = 185;
    const int Right_Type_Attribute_id = 186;
    const int High_Type_Attribute_id = 187;
    const int Low_Type_Attribute_id = 188;
    const int Ascending_Type_Attribute_id = 189;
    const int Image_Attribute_id = 190;
    const int Value_Attribute_id = 191;
    const int Pos_Attribute_id = 192;
    const int Val_Attribute_id = 193;
    const int Succ_Attribute_id = 194;
    const int Pred_Attribute_id = 195;
    const int Leftof_Attribute_id = 196;
    const int Rightof_Attribute_id = 197;
    const int Delayed_Attribute_id = 198;
    const int Stable_Attribute_id = 199;
    const int Quiet_Attribute_id = 200;
    const int Transaction_Attribute_id = 201;
    const int Attribute_id = 202;
    const int Behavior_Attribute_id = 203;
    const int Structure_Attribute_id = 204;
    const int Simple_Name_Attribute_id = 205;
    const int Instance_Name_Attribute_id = 206;
    const int Path_Name_Attribute_id = 207;
    const int Array_Attribute_id = 208;
    const int Attribute_Name_id = 209;

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

    enum class Unary_Operator_Type {
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
        Reduction_Xnor
    };

    enum class Binary_Operator_Type {
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

    enum class External_Name_Type {
        Constant,
        Signal,
        Variable
    };

    enum class Attribute_Type {
        Event,
        Active,
        Last_Event,
        Last_Active,
        Last_Value,
        Driving,
        Driving_Value
    };

    struct Base;
    struct Error;
    struct Design_File;
    struct Design_Unit;
    struct Library_Clause;
    struct Use_Clause;
    struct Context_Reference;
    struct Integer_Literal;
    struct Floating_Point_Literal;
    struct Null_Literal;
    struct String;
    struct Physical_Int_Literal;
    struct Physical_Fp_Literal;
    struct Simple_Aggregate;
    struct Overflow_Literal;
    struct Unaffected_Waveform;
    struct Waveform_Element;
    struct Conditional_Waveform;
    struct Conditional_Expression;
    struct Association_Element_By_Expression;
    struct Association_Element_By_Individual;
    struct Association_Element_Open;
    struct Association_Element_Package;
    struct Association_Element_Type;
    struct Association_Element_Subprogram;
    struct Choice_By_Range;
    struct Choice_By_Expression;
    struct Choice_By_Others;
    struct Choice_By_None;
    struct Choice_By_Name;
    struct Entity_Aspect_Entity;
    struct Entity_Aspect_Configuration;
    struct Entity_Aspect_Open;
    struct Block_Configuration;
    struct Block_Header;
    struct Component_Configuration;
    struct Binding_Indication;
    struct Entity_Class;
    struct Attribute_Value;
    struct Signature;
    struct Aggregate_Info;
    struct Procedure_Call;
    struct Record_Element_Constraint;
    struct Array_Element_Resolution;
    struct Record_Element_Resolution;
    struct Record_Resolution;
    struct Attribute_Specification;
    struct Disconnection_Specification;
    struct Configuration_Specification;
    struct Access_Type_Definition;
    struct Incomplete_Type_Definition;
    struct Interface_Type_Definition;
    struct File_Type_Definition;
    struct Protected_Type_Declaration;
    struct Record_Type_Definition;
    struct Array_Type_Definition;
    struct Array_Subtype_Definition;
    struct Record_Subtype_Definition;
    struct Access_Subtype_Definition;
    struct Physical_Subtype_Definition;
    struct Floating_Type_Definition;
    struct Floating_Subtype_Definition;
    struct Integer_Type_Definition;
    struct Integer_Subtype_Definition;
    struct Enumeration_Subtype_Definition;
    struct Enumeration_Type_Definition;
    struct Physical_Type_Definition;
    struct Range_Expression;
    struct Protected_Type_Body;
    struct Wildcard_Type_Definition;
    struct Subtype_Definition;
    struct Scalar_Nature_Definition;
    struct Overload_List;
    struct Type_Declaration;
    struct Anonymous_Type_Declaration;
    struct Subtype_Declaration;
    struct Nature_Declaration;
    struct Subnature_Declaration;
    struct Package_Declaration;
    struct Package_Instantiation_Declaration;
    struct Package_Body;
    struct Configuration_Declaration;
    struct Entity_Declaration;
    struct Architecture_Body;
    struct Context_Declaration;
    struct Package_Header;
    struct Unit_Declaration;
    struct Library_Declaration;
    struct Component_Declaration;
    struct Attribute_Declaration;
    struct Group_Template_Declaration;
    struct Group_Declaration;
    struct Element_Declaration;
    struct Non_Object_Alias_Declaration;
    struct Psl_Declaration;
    struct Psl_Endpoint_Declaration;
    struct Terminal_Declaration;
    struct Free_Quantity_Declaration;
    struct Across_Quantity_Declaration;
    struct Through_Quantity_Declaration;
    struct Enumeration_Literal;
    struct Function_Declaration;
    struct Procedure_Declaration;
    struct Function_Body;
    struct Procedure_Body;
    struct Object_Alias_Declaration;
    struct File_Declaration;
    struct Guard_Signal_Declaration;
    struct Signal_Declaration;
    struct Variable_Declaration;
    struct Constant_Declaration;
    struct Iterator_Declaration;
    struct Interface_Signal_Declaration_Extras;
    struct Interface_Object_Declaration;
    struct Interface_Type_Declaration;
    struct Interface_Package_Declaration;
    struct Interface_Function_Declaration;
    struct Interface_Procedure_Declaration;
    struct Signal_Attribute_Declaration;
    struct Unary_Operator;
    struct Binary_Operator;
    struct Function_Call;
    struct Aggregate;
    struct Parenthesis_Expression;
    struct Qualified_Expression;
    struct Type_Conversion;
    struct Allocator_By_Expression;
    struct Allocator_By_Subtype;
    struct Selected_Element;
    struct Dereference;
    struct Implicit_Dereference;
    struct Slice_Name;
    struct Indexed_Name;
    struct Psl_Expression;
    struct Sensitized_Process_Statement;
    struct Process_Statement;
    struct Simple_Signal_Assignment_Statement;
    struct Conditional_Signal_Assignment_Statement;
    struct Selected_Waveform_Assignment_Statement;
    struct Concurrent_Simple_Signal_Assignment;
    struct Concurrent_Conditional_Signal_Assignment;
    struct Concurrent_Selected_Signal_Assignment;
    struct Concurrent_Assertion_Statement;
    struct Concurrent_Procedure_Call_Statement;
    struct Psl_Assert_Statement;
    struct Psl_Cover_Statement;
    struct Block_Statement;
    struct If_Generate_Statement;
    struct Case_Generate_Statement;
    struct For_Generate_Statement;
    struct Component_Instantiation_Statement;
    struct Psl_Default_Clock;
    struct Simple_Simultaneous_Statement;
    struct Generate_Statement_Body;
    struct If_Generate_Else_Clause;
    struct Null_Statement;
    struct Assertion_Statement;
    struct Report_Statement;
    struct Wait_Statement;
    struct Variable_Assignment_Statement;
    struct Conditional_Variable_Assignment_Statement;
    struct Return_Statement;
    struct For_Loop_Statement;
    struct While_Loop_Statement;
    struct Next_Statement;
    struct Exit_Statement;
    struct Case_Statement;
    struct Procedure_Call_Statement;
    struct If_Statement;
    struct Elsif;
    struct Character_Literal;
    struct Simple_Name;
    struct Selected_Name;
    struct Operator_Symbol;
    struct Reference_Name;
    struct Selected_By_All_Name;
    struct Parenthesis_Name;
    struct External_Name;
    struct Package_Pathname;
    struct Absolute_Pathname;
    struct Relative_Pathname;
    struct Pathname_Element;
    struct Base_Attribute;
    struct Subtype_Attribute;
    struct Element_Attribute;
    struct Left_Type_Attribute;
    struct Right_Type_Attribute;
    struct High_Type_Attribute;
    struct Low_Type_Attribute;
    struct Ascending_Type_Attribute;
    struct Image_Attribute;
    struct Value_Attribute;
    struct Pos_Attribute;
    struct Val_Attribute;
    struct Succ_Attribute;
    struct Pred_Attribute;
    struct Leftof_Attribute;
    struct Rightof_Attribute;
    struct Delayed_Attribute;
    struct Stable_Attribute;
    struct Quiet_Attribute;
    struct Transaction_Attribute;
    struct Attribute;
    struct Behavior_Attribute;
    struct Structure_Attribute;
    struct Simple_Name_Attribute;
    struct Instance_Name_Attribute;
    struct Path_Name_Attribute;
    struct Array_Attribute;
    struct Attribute_Name;

    using Design_Unit_Collection = std::variant<Entity_Declaration*, Architecture_Body*, Package_Declaration*, Package_Instantiation_Declaration*, Package_Body*, Configuration_Declaration*, Context_Declaration*>;

    using Arch_Name = std::variant<Simple_Name*, Architecture_Body*>;

    using Type_Definitions = std::variant<Error*, Access_Type_Definition*, Incomplete_Type_Definition*, Interface_Type_Definition*, File_Type_Definition*, Record_Type_Definition*, Array_Type_Definition*, Array_Subtype_Definition*, Record_Subtype_Definition*, Access_Subtype_Definition*, Physical_Subtype_Definition*, Floating_Type_Definition*, Floating_Subtype_Definition*, Integer_Type_Definition*, Integer_Subtype_Definition*, Enumeration_Subtype_Definition*, Enumeration_Type_Definition*, Physical_Type_Definition*, Wildcard_Type_Definition*>;

    using Type_Declarations = std::variant<Subtype_Declaration*, Interface_Object_Declaration*, Interface_Type_Declaration*>;

    using Waveform = std::vector<Waveform_Element*>;

    using Conditional_Waveforms = std::vector<Conditional_Waveform*>;

    struct Base {
        Location_Type Location;
        int structId;
        virtual ~Base() {};
    };

    struct Error
            : public Base {
        Base* Error_Origin;
        Base* Type;
        bool Resolved_Flag;
        Base* Base_Type;
        bool Signal_Type_Flag;
        Base* Type_Declarator;
        bool Has_Signal_Flag;
        Iir_Staticness Expr_Staticness;
    };

    struct Design_File
            : public Base {
        std::vector<Design_Unit*> Design_Units;
        Time_Stamp_Id Analysis_Time_Stamp;
        File_Checksum_Id File_Checksum;
        Library_Declaration* Library;
        std::vector<Base*> File_Dependence_List;
        std::filesystem::path Design_File_Filename;
        bool Elab_Flag;
    };

    struct Design_Unit
            : public Base {
        Location_Type Source_Pos;
        Location_Type End_Location;
        bool Configuration_Mark_Flag;
        Design_File* design_File;
        Design_Unit_Collection Library_Unit;
        Date_State_Type Date_State;
        std::vector<Base*> Analysis_Checks_List;
        std::vector<Base*> Dependence_List;
        bool Configuration_Done_Flag;
        Date_Type Date;
        std::vector<Base*> Context_Items;
        std::string Identifier;
        bool Elab_Flag;
    };

    struct Library_Clause
            : public Base {
        Base* Library_Declaration;
        Base* Chain;
        std::string Identifier;
        Base* Parent;
        bool Has_Identifier_List;
    };

    struct Use_Clause
            : public Base {
        Base* Use_Clause_Chain;
        Base* Chain;
        Base* Selected_Name;
        Base* Parent;
    };

    struct Context_Reference
            : public Base {
        Base* Context_Reference_Chain;
        Base* Chain;
        Base* Selected_Name;
        Base* Parent;
    };

    struct Integer_Literal
            : public Base {
        int64_t Value;
        Base* Literal_Origin;
        Base* Type;
        Iir_Staticness Expr_Staticness;
    };

    struct Floating_Point_Literal
            : public Base {
        double Fp_Value;
        Base* Literal_Origin;
        Base* Type;
        Iir_Staticness Expr_Staticness;
    };

    struct Null_Literal
            : public Base {
        Base* Type;
        Iir_Staticness Expr_Staticness;
    };

    struct String
            : public Base {
        std::string str;
        bool Has_Sign;
        Number_Base_Type Bit_String_Base;
        bool Has_Signed;
        Base* Literal_Origin;
        Base* Literal_Subtype;
        Base* Type;
        Iir_Staticness Expr_Staticness;
    };

    struct Physical_Int_Literal
            : public Base {
        int64_t Value;
        Base* Literal_Origin;
        Base* Type;
        Iir_Staticness Expr_Staticness;
        Base* Unit_Name;
        Unit_Declaration* Physical_Unit;
    };

    struct Physical_Fp_Literal
            : public Base {
        double Fp_Value;
        Base* Literal_Origin;
        Base* Type;
        Iir_Staticness Expr_Staticness;
        Base* Unit_Name;
        Unit_Declaration* Physical_Unit;
    };

    struct Simple_Aggregate
            : public Base {
        std::vector<Base*> Simple_Aggregate_List;
        Base* Literal_Origin;
        Base* Literal_Subtype;
        Base* Type;
        Iir_Staticness Expr_Staticness;
    };

    struct Overflow_Literal
            : public Base {
        Base* Literal_Origin;
        Base* Type;
        Iir_Staticness Expr_Staticness;
    };

    struct Unaffected_Waveform
            : public Base {
    };

    struct Waveform_Element
            : public Base {
        Base* We_Value;
        Base* Time;
    };

    struct Conditional_Waveform
            : public Base {
        Waveform waveform;
        Base* Chain;
        Base* Condition;
    };

    struct Conditional_Expression
            : public Base {
        Base* Chain;
        Base* Expression;
        Base* Condition;
    };

    struct Association_Element_By_Expression
            : public Base {
        Base* In_Conversion;
        Base* Out_Conversion;
        bool Whole_Association_Flag;
        bool Collapse_Signal_Flag;
        Base* Formal;
        Base* Actual;
        Base* Chain;
    };

    struct Association_Element_By_Individual
            : public Base {
        Base* Individual_Association_Chain;
        Base* Actual_Type_Definition;
        bool Whole_Association_Flag;
        bool Collapse_Signal_Flag;
        Base* Formal;
        Base* Chain;
        Base* Actual_Type;
        Iir_Staticness Choice_Staticness;
    };

    struct Association_Element_Open
            : public Base {
        bool Artificial_Flag;
        bool Whole_Association_Flag;
        bool Collapse_Signal_Flag;
        Base* Formal;
        Base* Chain;
    };

    struct Association_Element_Package
            : public Base {
        bool Whole_Association_Flag;
        bool Collapse_Signal_Flag;
        Base* Formal;
        Base* Actual;
        Base* Chain;
    };

    struct Association_Element_Type
            : public Base {
        Base* Subprogram_Association_Chain;
        bool Whole_Association_Flag;
        bool Collapse_Signal_Flag;
        Base* Formal;
        Base* Actual;
        Base* Chain;
        Base* Actual_Type;
    };

    struct Association_Element_Subprogram
            : public Base {
        bool Whole_Association_Flag;
        bool Collapse_Signal_Flag;
        Base* Formal;
        Base* Actual;
        Base* Chain;
    };

    struct Choice_By_Range
            : public Base {
        Base* Choice_Range;
        bool Same_Alternative_Flag;
        Base* Associated_Expr;
        Base* Associated_Chain;
        Base* Chain;
        Base* Parent;
        Iir_Staticness Choice_Staticness;
    };

    struct Choice_By_Expression
            : public Base {
        Base* Choice_Expression;
        bool Same_Alternative_Flag;
        Base* Associated_Expr;
        Base* Associated_Chain;
        Base* Chain;
        Base* Parent;
        Iir_Staticness Choice_Staticness;
    };

    struct Choice_By_Others
            : public Base {
        bool Same_Alternative_Flag;
        Base* Associated_Expr;
        Base* Associated_Chain;
        Base* Chain;
        Base* Parent;
    };

    struct Choice_By_None
            : public Base {
        bool Same_Alternative_Flag;
        Base* Associated_Expr;
        Base* Associated_Chain;
        Base* Chain;
        Base* Parent;
    };

    struct Choice_By_Name
            : public Base {
        Base* Choice_Name;
        bool Same_Alternative_Flag;
        Base* Associated_Expr;
        Base* Associated_Chain;
        Base* Chain;
        Base* Parent;
    };

    struct Entity_Aspect_Entity
            : public Base {
        Base* Architecture;
        Base* Entity_Name;
    };

    struct Entity_Aspect_Configuration
            : public Base {
        Base* Configuration_Name;
    };

    struct Entity_Aspect_Open
            : public Base {
    };

    struct Block_Configuration
            : public Base {
        Base* Prev_Block_Configuration;
        Base* Block_Specification;
        Base* Configuration_Item_Chain;
        Base* Chain;
        std::vector<Base*> Declaration_Chain;
        Base* Parent;
    };

    struct Block_Header
            : public Base {
        Base* Port_Chain;
        Base* Generic_Chain;
        Base* Generic_Map_Aspect_Chain;
        Base* Port_Map_Aspect_Chain;
    };

    struct Component_Configuration
            : public Base {
        Base* Block_Configuration;
        Base* Chain;
        Base* Parent;
        std::vector<Base*> Instantiation_List;
        Binding_Indication* binding_Indication;
        Base* Component_Name;
        bool Is_Ref;
    };

    struct Binding_Indication
            : public Base {
        Base* Entity_Aspect;
        Base* Default_Entity_Aspect;
        Base* Generic_Map_Aspect_Chain;
        Base* Port_Map_Aspect_Chain;
    };

    struct Entity_Class
            : public Base {
        Token::Token Entity_Class;
        Base* Chain;
    };

    struct Attribute_Value
            : public Base {
        Base* Designated_Entity;
        Base* Value_Chain;
        Base* Attribute_Specification;
        Base* Spec_Chain;
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
    };

    struct Signature
            : public Base {
        Base* Signature_Prefix;
        std::vector<Base*> Type_Marks_List;
        Base* Return_Type_Mark;
    };

    struct Aggregate_Info
            : public Base {
        bool Aggr_Named_Flag;
        Base* Aggr_Low_Limit;
        bool Aggr_Others_Flag;
        Base* Sub_Aggregate_Info;
        Base* Aggr_High_Limit;
        bool Aggr_Dynamic_Flag;
        int Aggr_Min_Length;
    };

    struct Procedure_Call
            : public Base {
        Base* Prefix;
        Base* Implementation;
        Base* Parameter_Association_Chain;
        Base* Method_Object;
    };

    struct Record_Element_Constraint
            : public Base {
        Base* Element_Declaration;
        Base* Type;
        std::string Identifier;
        bool Visible_Flag;
        Base* Parent;
    };

    struct Array_Element_Resolution
            : public Base {
        Base* Resolution_Indication;
        Base* Element_Subtype_Indication;
    };

    struct Record_Element_Resolution
            : public Base {
        Base* Chain;
        std::string Identifier;
        Base* Resolution_Indication;
    };

    struct Record_Resolution
            : public Base {
        std::vector<Record_Element_Resolution*> chain;
    };

    struct Attribute_Specification
            : public Base {
        Base* Attribute_Value_Spec_Chain;
        Base* Attribute_Specification_Chain;
        std::vector<Base*> Entity_Name_List;
        Base* Attribute_Designator;
        Token::Token Entity_Class;
        Base* Chain;
        Base* Expression;
        Base* Parent;
    };

    struct Disconnection_Specification
            : public Base {
        std::vector<Base*> Signal_List;
        Base* Chain;
        Base* Expression;
        Base* Parent;
        Base* Type_Mark;
        bool Is_Ref;
    };

    struct Configuration_Specification
            : public Base {
        Base* Chain;
        Base* Parent;
        std::vector<Base*> Instantiation_List;
        Binding_Indication* binding_Indication;
        Base* Component_Name;
        bool Is_Ref;
    };

    struct Access_Type_Definition
            : public Base {
        bool Resolved_Flag;
        Base* Base_Type;
        bool Signal_Type_Flag;
        Base* Type_Declarator;
        Base* Incomplete_Type_Ref_Chain;
        Iir_Staticness Type_Staticness;
        Base* Designated_Type;
        Base* Designated_Subtype_Indication;
    };

    struct Incomplete_Type_Definition
            : public Base {
        Base* Complete_Type_Definition;
        bool Resolved_Flag;
        Base* Base_Type;
        bool Signal_Type_Flag;
        Base* Type_Declarator;
        Base* Incomplete_Type_Ref_Chain;
        Iir_Staticness Type_Staticness;
        bool Has_Signal_Flag;
    };

    struct Interface_Type_Definition
            : public Base {
        Base* Associated_Type;
        bool Resolved_Flag;
        Base* Base_Type;
        bool Signal_Type_Flag;
        Base* Type_Declarator;
        Iir_Staticness Type_Staticness;
        bool Has_Signal_Flag;
    };

    struct File_Type_Definition
            : public Base {
        Base* File_Type_Mark;
        bool Text_File_Flag;
        bool Resolved_Flag;
        Base* Base_Type;
        bool Signal_Type_Flag;
        Base* Type_Declarator;
        Iir_Staticness Type_Staticness;
    };

    struct Protected_Type_Declaration
            : public Base {
        Base* Protected_Type_Body;
        std::vector<Base*> Declaration_Chain;
        bool Resolved_Flag;
        Base* Base_Type;
        bool Signal_Type_Flag;
        Base* Type_Declarator;
        Iir_Staticness Type_Staticness;
        bool End_Has_Reserved_Id;
        bool End_Has_Identifier;
    };

    struct Record_Type_Definition
            : public Base {
        bool Resolved_Flag;
        Base* Base_Type;
        bool Signal_Type_Flag;
        Base* Type_Declarator;
        Iir_Staticness Type_Staticness;
        Iir_Constraint Constraint_State;
        std::vector<Base*> Elements_Declaration_List;
        bool Has_Signal_Flag;
        bool End_Has_Reserved_Id;
        bool End_Has_Identifier;
    };

    struct Array_Type_Definition
            : public Base {
        std::vector<Base*> Index_Subtype_Definition_List;
        bool Resolved_Flag;
        Base* Base_Type;
        bool Signal_Type_Flag;
        Base* Type_Declarator;
        Iir_Staticness Type_Staticness;
        Iir_Constraint Constraint_State;
        std::vector<Base*> Index_Subtype_List;
        Type_Definitions Element_Subtype;
        bool Index_Constraint_Flag;
        Base* Element_Subtype_Indication;
        bool Has_Signal_Flag;
    };

    struct Array_Subtype_Definition
            : public Base {
        std::vector<Base*> Index_Constraint_List;
        Base* Array_Element_Constraint;
        bool Resolved_Flag;
        Base* Base_Type;
        bool Signal_Type_Flag;
        Base* Type_Declarator;
        Base* Resolution_Indication;
        Base* Tolerance;
        Iir_Staticness Type_Staticness;
        Iir_Constraint Constraint_State;
        std::vector<Base*> Index_Subtype_List;
        Base* Element_Subtype;
        bool Index_Constraint_Flag;
        bool Has_Signal_Flag;
        Base* Subtype_Type_Mark;
    };

    struct Record_Subtype_Definition
            : public Base {
        bool Resolved_Flag;
        Base* Base_Type;
        bool Signal_Type_Flag;
        Base* Type_Declarator;
        Base* Resolution_Indication;
        Base* Tolerance;
        Iir_Staticness Type_Staticness;
        Iir_Constraint Constraint_State;
        std::vector<Base*> Elements_Declaration_List;
        bool Has_Signal_Flag;
        Base* Subtype_Type_Mark;
    };

    struct Access_Subtype_Definition
            : public Base {
        bool Resolved_Flag;
        Base* Base_Type;
        bool Signal_Type_Flag;
        Base* Type_Declarator;
        Iir_Staticness Type_Staticness;
        Base* Designated_Type;
        Base* Designated_Subtype_Indication;
        Base* Subtype_Type_Mark;
    };

    struct Physical_Subtype_Definition
            : public Base {
        bool Resolved_Flag;
        Base* Base_Type;
        bool Signal_Type_Flag;
        Base* Type_Declarator;
        Base* Range_Constraint;
        Base* Resolution_Indication;
        Iir_Staticness Type_Staticness;
        bool Has_Signal_Flag;
        Base* Subtype_Type_Mark;
        bool Is_Ref;
    };

    struct Floating_Type_Definition
            : public Base {
        bool Resolved_Flag;
        Base* Base_Type;
        bool Signal_Type_Flag;
        Base* Type_Declarator;
        Base* Range_Constraint;
        Iir_Staticness Type_Staticness;
        bool Has_Signal_Flag;
        bool Is_Ref;
    };

    struct Floating_Subtype_Definition
            : public Base {
        bool Resolved_Flag;
        Base* Base_Type;
        bool Signal_Type_Flag;
        Base* Type_Declarator;
        Base* Range_Constraint;
        Iir_Staticness Type_Staticness;
        bool Has_Signal_Flag;
        bool Is_Ref;
        Base* Resolution_Indication;
        Base* Tolerance;
        Base* Subtype_Type_Mark;
    };

    struct Integer_Type_Definition
            : public Base {
        bool Resolved_Flag;
        Base* Base_Type;
        bool Signal_Type_Flag;
        Base* Type_Declarator;
        Base* Range_Constraint;
        Iir_Staticness Type_Staticness;
        bool Has_Signal_Flag;
        bool Is_Ref;
    };

    struct Integer_Subtype_Definition
            : public Base {
        bool Resolved_Flag;
        Base* Base_Type;
        bool Signal_Type_Flag;
        Base* Type_Declarator;
        Base* Range_Constraint;
        Iir_Staticness Type_Staticness;
        bool Has_Signal_Flag;
        bool Is_Ref;
        Base* Resolution_Indication;
        Base* Subtype_Type_Mark;
    };

    struct Enumeration_Subtype_Definition
            : public Base {
        bool Resolved_Flag;
        Base* Base_Type;
        bool Signal_Type_Flag;
        Base* Type_Declarator;
        Base* Range_Constraint;
        Base* Resolution_Indication;
        Iir_Staticness Type_Staticness;
        bool Has_Signal_Flag;
        Base* Subtype_Type_Mark;
        bool Is_Ref;
    };

    struct Enumeration_Type_Definition
            : public Base {
        std::vector<Enumeration_Literal*> Enumeration_Literal_List;
        bool Only_Characters_Flag;
        bool Resolved_Flag;
        Base* Base_Type;
        bool Signal_Type_Flag;
        Base* Type_Declarator;
        Base* Range_Constraint;
        Iir_Staticness Type_Staticness;
        bool Has_Signal_Flag;
        bool Is_Ref;
    };

    struct Physical_Type_Definition
            : public Base {
        std::vector<Base*> Unit_Chain;
        bool Resolved_Flag;
        Base* Base_Type;
        bool Signal_Type_Flag;
        Base* Type_Declarator;
        Base* Range_Constraint;
        Iir_Staticness Type_Staticness;
        bool Has_Signal_Flag;
        bool End_Has_Reserved_Id;
        bool End_Has_Identifier;
        bool Is_Ref;
    };

    struct Range_Expression
            : public Base {
        Iir_Direction Direction;
        Base* Left_Limit;
        Base* Right_Limit_Expr;
        Base* Range_Origin;
        Base* Left_Limit_Expr;
        Base* Right_Limit;
        Base* Type;
        Iir_Staticness Expr_Staticness;
    };

    struct Protected_Type_Body
            : public Base {
        Base* Protected_Type_Declaration;
        Base* Chain;
        std::vector<Base*> Declaration_Chain;
        std::string Identifier;
        Base* Parent;
        bool End_Has_Reserved_Id;
        bool End_Has_Identifier;
    };

    struct Wildcard_Type_Definition
            : public Base {
        bool Resolved_Flag;
        Base* Base_Type;
        bool Signal_Type_Flag;
        Base* Type_Declarator;
        Iir_Staticness Type_Staticness;
    };

    struct Subtype_Definition
            : public Base {
        Base* Range_Constraint;
        Base* Resolution_Indication;
        Base* Tolerance;
        Base* Subtype_Type_Mark;
        bool Is_Ref;
    };

    struct Scalar_Nature_Definition
            : public Base {
        Base* Nature_Declarator;
        Base* Through_Type;
        Base* Across_Type;
        Base* Reference;
    };

    struct Overload_List
            : public Base {
        std::vector<Base*> Overload_List;
    };

    struct Type_Declaration
            : public Base {
        Base* Chain;
        Base* Incomplete_Type_Declaration;
        Type_Definitions Type;
        std::string Identifier;
        bool Visible_Flag;
        Base* Parent;
        bool Use_Flag;
    };

    struct Anonymous_Type_Declaration
            : public Base {
        Base* Subtype_Definition;
        Base* Chain;
        Base* Incomplete_Type_Declaration;
        Base* Type_Definition;
        std::string Identifier;
        Base* Parent;
    };

    struct Subtype_Declaration
            : public Base {
        Base* Chain;
        Type_Definitions Type;
        Base* Subtype_Indication;
        std::string Identifier;
        bool Visible_Flag;
        Base* Parent;
        bool Use_Flag;
    };

    struct Nature_Declaration
            : public Base {
        Base* Chain;
        Base* Nature;
        std::string Identifier;
        bool Visible_Flag;
        Base* Parent;
        bool Use_Flag;
    };

    struct Subnature_Declaration
            : public Base {
        Base* Chain;
        Base* Nature;
        std::string Identifier;
        bool Visible_Flag;
        Base* Parent;
        bool Use_Flag;
    };

    struct Package_Declaration
            : public Base {
        bool Macro_Expanded_Flag;
        Base* Package_Header;
        bool Need_Instance_Bodies;
        bool Need_Body;
        Base* Package_Origin;
        Base* Attribute_Value_Chain;
        Base* Package_Body;
        Base* Chain;
        std::vector<Base*> Declaration_Chain;
        std::string Identifier;
        bool Visible_Flag;
        Design_Unit* Parent_Design_Unit;
        bool End_Has_Reserved_Id;
        bool End_Has_Identifier;
    };

    struct Package_Instantiation_Declaration
            : public Base {
        Base* Attribute_Value_Chain;
        Base* Package_Body;
        Base* Chain;
        Base* Generic_Chain;
        std::vector<Base*> Declaration_Chain;
        std::string Identifier;
        bool Visible_Flag;
        Base* Generic_Map_Aspect_Chain;
        Base* Uninstantiated_Package_Name;
        Base* Uninstantiated_Package_Decl;
        Design_Unit* Parent_Design_Unit;
        bool End_Has_Reserved_Id;
        bool End_Has_Identifier;
    };

    struct Package_Body
            : public Base {
        Base* Package;
        Base* Attribute_Value_Chain;
        Base* Chain;
        std::vector<Base*> Declaration_Chain;
        std::string Identifier;
        Design_Unit* Parent_Design_Unit;
        bool End_Has_Reserved_Id;
        bool End_Has_Identifier;
    };

    struct Configuration_Declaration
            : public Base {
        Base* Attribute_Value_Chain;
        Base* Entity_Name;
        Base* Block_Configuration;
        std::vector<Base*> Declaration_Chain;
        std::string Identifier;
        bool Visible_Flag;
        Design_Unit* Parent_Design_Unit;
        bool End_Has_Reserved_Id;
        bool End_Has_Identifier;
    };

    struct Entity_Declaration
            : public Base {
        Base* Attribute_Value_Chain;
        Base* Concurrent_Statement_Chain;
        Base* Port_Chain;
        Base* Generic_Chain;
        std::vector<Base*> Declaration_Chain;
        std::string Identifier;
        bool Visible_Flag;
        Design_Unit* Parent_Design_Unit;
        bool Is_Within_Flag;
        bool End_Has_Reserved_Id;
        bool End_Has_Identifier;
        bool Has_Begin;
    };

    struct Architecture_Body
            : public Base {
        Base* Default_Configuration_Declaration;
        Base* Attribute_Value_Chain;
        Simple_Name* Entity_Name;
        Base* Concurrent_Statement_Chain;
        std::vector<Base*> Declaration_Chain;
        std::string Identifier;
        bool Visible_Flag;
        bool Foreign_Flag;
        Design_Unit* Parent_Design_Unit;
        bool Is_Within_Flag;
        bool End_Has_Reserved_Id;
        bool End_Has_Identifier;
    };

    struct Context_Declaration
            : public Base {
        std::vector<Base*> Context_Items;
        std::string Identifier;
        bool Visible_Flag;
        Design_Unit* Parent_Design_Unit;
        bool End_Has_Reserved_Id;
        bool End_Has_Identifier;
    };

    struct Package_Header
            : public Base {
        Base* Generic_Chain;
        Base* Generic_Map_Aspect_Chain;
    };

    struct Unit_Declaration
            : public Base {
        Physical_Int_Literal* physical_Literal;
        Base* Chain;
        Base* Type;
        std::string Identifier;
        bool Visible_Flag;
        Base* Parent;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
    };

    struct Library_Declaration
            : public Base {
        std::vector<Design_File*> Design_Files;
        std::filesystem::path Library_Directory;
        Date_Type Date;
        Base* Chain;
        std::string Identifier;
        bool Visible_Flag;
        bool Elab_Flag;
    };

    struct Component_Declaration
            : public Base {
        Base* Chain;
        Base* Port_Chain;
        Base* Generic_Chain;
        std::string Identifier;
        bool Visible_Flag;
        Base* Parent;
        bool Use_Flag;
        bool End_Has_Reserved_Id;
        bool End_Has_Identifier;
        bool Has_Is;
    };

    struct Attribute_Declaration
            : public Base {
        Base* Chain;
        Base* Type;
        std::string Identifier;
        bool Visible_Flag;
        Base* Parent;
        Base* Type_Mark;
        bool Use_Flag;
    };

    struct Group_Template_Declaration
            : public Base {
        Base* Entity_Class_Entry_Chain;
        Base* Chain;
        std::string Identifier;
        bool Visible_Flag;
        Base* Parent;
        bool Use_Flag;
    };

    struct Group_Declaration
            : public Base {
        std::vector<Base*> Group_Constituent_List;
        Base* Group_Template_Name;
        Base* Chain;
        std::string Identifier;
        bool Visible_Flag;
        Base* Parent;
        bool Use_Flag;
    };

    struct Element_Declaration
            : public Base {
        Base* Type;
        Base* Subtype_Indication;
        std::string Identifier;
        bool Visible_Flag;
        Base* Parent;
        bool Has_Identifier_List;
    };

    struct Non_Object_Alias_Declaration
            : public Base {
        Base* Alias_Signature;
        bool Implicit_Alias_Flag;
        Base* Chain;
        std::string Identifier;
        bool Visible_Flag;
        Base* Parent;
        Base* Name;
        bool Use_Flag;
    };

    struct Psl_Declaration
            : public Base {
        Base* Chain;
        std::string Identifier;
        bool Visible_Flag;
        Base* Parent;
        bool Use_Flag;
        PSL_Node Psl_Declaration;
        PSL_NFA PSL_Nfa;
        PSL_Node PSL_Clock;
    };

    struct Psl_Endpoint_Declaration
            : public Base {
        Base* Chain;
        Base* Type;
        std::string Identifier;
        bool Visible_Flag;
        Base* Parent;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        bool Use_Flag;
        PSL_Node Psl_Declaration;
        PSL_NFA PSL_Nfa;
        PSL_Node PSL_Clock;
        bool PSL_EOS_Flag;
        int PSL_Nbr_States;
        std::vector<Base*> PSL_Clock_Sensitivity;
    };

    struct Terminal_Declaration
            : public Base {
        Base* Chain;
        Base* Nature;
        std::string Identifier;
        bool Visible_Flag;
        Base* Parent;
        bool Use_Flag;
    };

    struct Free_Quantity_Declaration
            : public Base {
        Base* Chain;
        Base* Type;
        Base* Default_Value;
        std::string Identifier;
        bool Visible_Flag;
        Base* Parent;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        bool Use_Flag;
    };

    struct Across_Quantity_Declaration
            : public Base {
        Base* Chain;
        Base* Type;
        Base* Default_Value;
        std::string Identifier;
        bool Visible_Flag;
        Base* Tolerance;
        Base* Minus_Terminal;
        Base* Plus_Terminal;
        Base* Parent;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        bool Use_Flag;
    };

    struct Through_Quantity_Declaration
            : public Base {
        Base* Chain;
        Base* Type;
        Base* Default_Value;
        std::string Identifier;
        bool Visible_Flag;
        Base* Tolerance;
        Base* Minus_Terminal;
        Base* Plus_Terminal;
        Base* Parent;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        bool Use_Flag;
    };

    struct Enumeration_Literal
            : public Base {
        int Enum_Pos;
        Base* Literal_Origin;
        Base* Type;
        int Subprogram_Hash;
        std::string Identifier;
        bool Visible_Flag;
        bool Seen_Flag;
        Base* Parent;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        bool Is_Within_Flag;
    };

    struct Function_Declaration
            : public Base {
        Base* Chain;
        Base* Generic_Chain;
        int Subprogram_Depth;
        Base* Interface_Declaration_Chain;
        Iir_All_Sensitized All_Sensitized_State;
        bool Has_Parameter;
        Iir_Predefined_Functions Implicit_Definition;
        Base* Subprogram_Body;
        bool Has_Body;
        int Overload_Number;
        bool Hide_Implicit_Flag;
        int Subprogram_Hash;
        bool Has_Pure;
        Base* Return_Type;
        bool Pure_Flag;
        bool Resolution_Function_Flag;
        std::string Identifier;
        bool Visible_Flag;
        Tri_State_Type Wait_State;
        bool Seen_Flag;
        bool Foreign_Flag;
        Base* Parent;
        Base* Return_Type_Mark;
        bool Is_Within_Flag;
        bool Use_Flag;
    };

    struct Procedure_Declaration
            : public Base {
        Iir_Pure_State Purity_State;
        Base* Generic_Chain;
        int Subprogram_Depth;
        Base* Interface_Declaration_Chain;
        Iir_All_Sensitized All_Sensitized_State;
        bool Has_Parameter;
        Iir_Predefined_Functions Implicit_Definition;
        Base* Subprogram_Body;
        bool Has_Body;
        int Overload_Number;
        bool Hide_Implicit_Flag;
        int Subprogram_Hash;
        std::string Identifier;
        bool Visible_Flag;
        bool Passive_Flag;
        Tri_State_Type Wait_State;
        bool Seen_Flag;
        bool Foreign_Flag;
        Base* Parent;
        Base* Return_Type_Mark;
        bool Is_Within_Flag;
        bool Use_Flag;
        bool Suspend_Flag;
    };

    struct Function_Body
            : public Base {
        Base* Attribute_Value_Chain;
        Base* Chain;
        Base* Subprogram_Specification;
        int Impure_Depth;
        Base* Sequential_Statement_Chain;
        std::vector<Base*> Declaration_Chain;
        std::vector<Base*> Callees_List;
        Base* Parent;
        bool End_Has_Reserved_Id;
        bool End_Has_Identifier;
    };

    struct Procedure_Body
            : public Base {
        Base* Attribute_Value_Chain;
        Base* Chain;
        Base* Subprogram_Specification;
        int Impure_Depth;
        Base* Sequential_Statement_Chain;
        std::vector<Base*> Declaration_Chain;
        std::vector<Base*> Callees_List;
        Base* Parent;
        bool End_Has_Reserved_Id;
        bool End_Has_Identifier;
        bool Suspend_Flag;
    };

    struct Object_Alias_Declaration
            : public Base {
        bool After_Drivers_Flag;
        Base* Chain;
        Base* Type;
        Base* Subtype_Indication;
        std::string Identifier;
        bool Visible_Flag;
        Base* Parent;
        Iir_Staticness Expr_Staticness;
        Base* Name;
        Iir_Staticness Name_Staticness;
        bool Use_Flag;
    };

    struct File_Declaration
            : public Base {
        Base* File_Open_Kind;
        Base* File_Logical_Name;
        Base* Chain;
        Base* Type;
        Base* Subtype_Indication;
        std::optional<Iir_Mode> Mode;
        std::string Identifier;
        bool Visible_Flag;
        Base* Parent;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        bool Use_Flag;
        bool Has_Identifier_List;
    };

    struct Guard_Signal_Declaration
            : public Base {
        Base* Block_Statement;
        std::vector<Base*> Guard_Sensitivity_List;
        Base* Guard_Expression;
        Base* Type;
        bool Guarded_Signal_Flag;
        Iir_Signal_Kind Signal_Kind;
        std::string Identifier;
        bool Visible_Flag;
        Base* Parent;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        bool Has_Active_Flag;
        bool Use_Flag;
    };

    struct Signal_Declaration
            : public Base {
        bool After_Drivers_Flag;
        Base* Chain;
        Base* Type;
        Base* Subtype_Indication;
        bool Guarded_Signal_Flag;
        Iir_Signal_Kind Signal_Kind;
        Base* Default_Value;
        std::string Identifier;
        bool Visible_Flag;
        Base* Parent;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        bool Has_Disconnect_Flag;
        bool Has_Active_Flag;
        bool Use_Flag;
        bool Has_Identifier_List;
        bool Is_Ref;
    };

    struct Variable_Declaration
            : public Base {
        Base* Chain;
        Base* Type;
        Base* Subtype_Indication;
        Base* Default_Value;
        bool Shared_Flag;
        std::string Identifier;
        bool Visible_Flag;
        Base* Parent;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        bool Use_Flag;
        bool Has_Identifier_List;
        bool Is_Ref;
    };

    struct Constant_Declaration
            : public Base {
        bool Deferred_Declaration_Flag;
        Base* Deferred_Declaration;
        Base* Chain;
        Base* Type;
        Base* Subtype_Indication;
        Base* Default_Value;
        std::string Identifier;
        bool Visible_Flag;
        Base* Parent;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        bool Use_Flag;
        bool Has_Identifier_List;
        bool Is_Ref;
    };

    struct Iterator_Declaration
            : public Base {
        Base* Discrete_Range;
        Base* Chain;
        Base* Type;
        Base* Subtype_Indication;
        std::string Identifier;
        bool Visible_Flag;
        Base* Parent;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        bool Use_Flag;
        bool Has_Identifier_List;
    };

    struct Interface_Signal_Declaration_Extras
            : public Base {
        bool Open_Flag;
        bool Guarded_Signal_Flag;
        Iir_Signal_Kind Signal_Kind;
        bool Has_Disconnect_Flag;
        bool Has_Active_Flag;
    };

    struct Interface_Object_Declaration
            : public Base {
        enum class type {constant, variable, signal, file} Interface_Object_Type;
        std::optional<Interface_Signal_Declaration_Extras*> extras;
        std::string Identifier;
        bool Visible_Flag;
        Base* Parent;
        Iir_Staticness Expr_Staticness;
        bool After_Drivers_Flag;
        Interface_Object_Declaration* Chain;
        Base* Type;
        std::optional<Iir_Mode> Mode;
        Base* Default_Value;
        Iir_Staticness Name_Staticness;
        bool Use_Flag;
        Base* Subtype_Indication;
        bool Has_Identifier_List;
        bool Has_Class;
        bool Is_Ref;
    };

    struct Interface_Type_Declaration
            : public Base {
        Base* Interface_Type_Subprograms;
        Base* Chain;
        Base* Type;
        std::string Identifier;
        bool Visible_Flag;
        Base* Parent;
        Iir_Staticness Name_Staticness;
        bool Use_Flag;
        bool Has_Identifier_List;
        bool Is_Ref;
    };

    struct Interface_Package_Declaration
            : public Base {
        Base* Attribute_Value_Chain;
        Base* Chain;
        Base* Generic_Chain;
        std::vector<Base*> Declaration_Chain;
        std::string Identifier;
        bool Visible_Flag;
        Base* Generic_Map_Aspect_Chain;
        Base* Uninstantiated_Package_Name;
        Base* Uninstantiated_Package_Decl;
        Design_Unit* Parent_Design_Unit;
        bool Is_Within_Flag;
    };

    struct Interface_Function_Declaration
            : public Base {
        Base* Chain;
        int Subprogram_Depth;
        Base* Interface_Declaration_Chain;
        Iir_All_Sensitized All_Sensitized_State;
        bool Has_Parameter;
        int Subprogram_Hash;
        bool Has_Pure;
        Base* Return_Type;
        bool Pure_Flag;
        bool Resolution_Function_Flag;
        std::string Identifier;
        bool Visible_Flag;
        bool Seen_Flag;
        bool Foreign_Flag;
        Base* Parent;
        Base* Return_Type_Mark;
        bool Use_Flag;
    };

    struct Interface_Procedure_Declaration
            : public Base {
        Base* Chain;
        int Subprogram_Depth;
        Base* Interface_Declaration_Chain;
        Iir_All_Sensitized All_Sensitized_State;
        bool Has_Parameter;
        int Subprogram_Hash;
        std::string Identifier;
        bool Visible_Flag;
        bool Seen_Flag;
        bool Foreign_Flag;
        Base* Parent;
        Base* Return_Type_Mark;
        bool Use_Flag;
    };

    struct Signal_Attribute_Declaration
            : public Base {
        Base* Signal_Attribute_Chain;
        Base* Chain;
        Base* Parent;
    };

    struct Unary_Operator
            : public Base {
        Unary_Operator_Type Operator_Type;
        Base* Type;
        Iir_Staticness Expr_Staticness;
        Base* Operand;
        Base* Implementation;
    };

    struct Binary_Operator
            : public Base {
        Base* Left;
        Base* Right;
        Binary_Operator_Type Operator_Type;
        Base* Type;
        Iir_Staticness Expr_Staticness;
        Base* Implementation;
    };

    struct Function_Call
            : public Base {
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
        Base* Implementation;
        Base* Parameter_Association_Chain;
        Base* Method_Object;
    };

    struct Aggregate
            : public Base {
        Base* Association_Choices_Chain;
        bool Aggregate_Expand_Flag;
        Base* Aggregate_Info;
        Base* Literal_Origin;
        Base* Literal_Subtype;
        Base* Type;
        Iir_Staticness Expr_Staticness;
    };

    struct Parenthesis_Expression
            : public Base {
        Base* Type;
        Base* Expression;
        Iir_Staticness Expr_Staticness;
    };

    struct Qualified_Expression
            : public Base {
        Base* Type;
        Base* Expression;
        Iir_Staticness Expr_Staticness;
        Base* Type_Mark;
    };

    struct Type_Conversion
            : public Base {
        Base* Type_Conversion_Subtype;
        Base* Type;
        Base* Expression;
        Iir_Staticness Expr_Staticness;
        Base* Type_Mark;
    };

    struct Allocator_By_Expression
            : public Base {
        Base* Type;
        Base* Expression;
        Base* Allocator_Designated_Type;
        Iir_Staticness Expr_Staticness;
    };

    struct Allocator_By_Subtype
            : public Base {
        Base* Allocator_Subtype;
        Base* Type;
        Base* Subtype_Indication;
        Base* Allocator_Designated_Type;
        Iir_Staticness Expr_Staticness;
    };

    struct Selected_Element
            : public Base {
        Base* Selected_Element;
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
    };

    struct Dereference
            : public Base {
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
    };

    struct Implicit_Dereference
            : public Base {
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
    };

    struct Slice_Name
            : public Base {
        Base* Suffix;
        Base* Slice_Subtype;
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
    };

    struct Indexed_Name
            : public Base {
        std::vector<Base*> Index_List;
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
    };

    struct Psl_Expression
            : public Base {
        PSL_Node Psl_Expression;
        Base* Type;
    };

    struct Sensitized_Process_Statement
            : public Base {
        Base* Attribute_Value_Chain;
        Base* Chain;
        Base* Sequential_Statement_Chain;
        std::vector<Base*> Declaration_Chain;
        std::string Label;
        bool Visible_Flag;
        std::vector<Base*> Sensitivity_List;
        bool End_Has_Postponed;
        Base* Process_Origin;
        bool Postponed_Flag;
        std::vector<Base*> Callees_List;
        bool Passive_Flag;
        Tri_State_Type Wait_State;
        bool Seen_Flag;
        Base* Parent;
        bool Is_Within_Flag;
        bool End_Has_Reserved_Id;
        bool End_Has_Identifier;
        bool Has_Label;
        bool Has_Is;
        bool Is_Ref;
    };

    struct Process_Statement
            : public Base {
        Base* Attribute_Value_Chain;
        Base* Chain;
        Base* Sequential_Statement_Chain;
        std::vector<Base*> Declaration_Chain;
        std::string Label;
        bool Visible_Flag;
        bool End_Has_Postponed;
        Base* Process_Origin;
        bool Postponed_Flag;
        std::vector<Base*> Callees_List;
        bool Passive_Flag;
        Tri_State_Type Wait_State;
        bool Seen_Flag;
        Base* Parent;
        bool Is_Within_Flag;
        bool End_Has_Reserved_Id;
        bool End_Has_Identifier;
        bool Has_Label;
        bool Has_Is;
        bool Suspend_Flag;
    };

    struct Simple_Signal_Assignment_Statement
            : public Base {
        Waveform waveform;
        Iir_Delay_Mechanism Delay_Mechanism;
        Tri_State_Type Guarded_Target_State;
        Base* Reject_Time_Expression;
        Base* Chain;
        std::string Label;
        bool Visible_Flag;
        Base* Target;
        Base* Parent;
    };

    struct Conditional_Signal_Assignment_Statement
            : public Base {
        Conditional_Waveforms conditional_waveforms;
        Iir_Delay_Mechanism Delay_Mechanism;
        Tri_State_Type Guarded_Target_State;
        Base* Reject_Time_Expression;
        Base* Chain;
        std::string Label;
        bool Visible_Flag;
        Base* Target;
        Base* Parent;
    };

    struct Selected_Waveform_Assignment_Statement
            : public Base {
        Iir_Delay_Mechanism Delay_Mechanism;
        Tri_State_Type Guarded_Target_State;
        Base* Reject_Time_Expression;
        Base* Chain;
        std::string Label;
        bool Visible_Flag;
        Base* Target;
        Base* Expression;
        Base* Selected_Waveform_Chain;
        Base* Parent;
    };

    struct Concurrent_Simple_Signal_Assignment
            : public Base {
        Waveform waveform;
        Iir_Delay_Mechanism Delay_Mechanism;
        Tri_State_Type Guarded_Target_State;
        Base* Reject_Time_Expression;
        Base* Chain;
        std::string Label;
        bool Visible_Flag;
        Base* Target;
        Base* Parent;
        Base* Guard;
        bool Postponed_Flag;
    };

    struct Concurrent_Conditional_Signal_Assignment
            : public Base {
        Conditional_Waveforms conditional_waveforms;
        Iir_Delay_Mechanism Delay_Mechanism;
        Tri_State_Type Guarded_Target_State;
        Base* Reject_Time_Expression;
        Base* Chain;
        std::string Label;
        bool Visible_Flag;
        Base* Target;
        Base* Parent;
        Base* Guard;
        bool Postponed_Flag;
    };

    struct Concurrent_Selected_Signal_Assignment
            : public Base {
        Iir_Delay_Mechanism Delay_Mechanism;
        Tri_State_Type Guarded_Target_State;
        Base* Reject_Time_Expression;
        Base* Chain;
        std::string Label;
        bool Visible_Flag;
        Base* Target;
        Base* Expression;
        Base* Selected_Waveform_Chain;
        Base* Parent;
        Base* Guard;
        bool Postponed_Flag;
    };

    struct Concurrent_Assertion_Statement
            : public Base {
        Base* Chain;
        std::string Label;
        bool Visible_Flag;
        Base* Assertion_Condition;
        Base* Report_Expression;
        Base* Severity_Expression;
        Base* Parent;
        bool Postponed_Flag;
    };

    struct Concurrent_Procedure_Call_Statement
            : public Base {
        Base* Chain;
        std::string Label;
        bool Visible_Flag;
        Base* Parent;
        Base* Procedure_Call;
        bool Suspend_Flag;
        bool Postponed_Flag;
    };

    struct Psl_Assert_Statement
            : public Base {
        PSL_Node Psl_Property;
        Base* Chain;
        std::string Label;
        bool Visible_Flag;
        bool Postponed_Flag;
        Base* Report_Expression;
        Base* Severity_Expression;
        Base* Parent;
        PSL_NFA PSL_Nfa;
        PSL_Node PSL_Clock;
        bool PSL_EOS_Flag;
        int PSL_Nbr_States;
        std::vector<Base*> PSL_Clock_Sensitivity;
    };

    struct Psl_Cover_Statement
            : public Base {
        PSL_Node Psl_Sequence;
        Base* Chain;
        std::string Label;
        bool Visible_Flag;
        bool Postponed_Flag;
        Base* Report_Expression;
        Base* Severity_Expression;
        Base* Parent;
        PSL_NFA PSL_Nfa;
        PSL_Node PSL_Clock;
        bool PSL_EOS_Flag;
        int PSL_Nbr_States;
        std::vector<Base*> PSL_Clock_Sensitivity;
    };

    struct Block_Statement
            : public Base {
        Base* Guard_Decl;
        Base* Block_Block_Configuration;
        Base* Block_Header;
        Base* Attribute_Value_Chain;
        Base* Concurrent_Statement_Chain;
        Base* Chain;
        std::vector<Base*> Declaration_Chain;
        std::string Label;
        bool Visible_Flag;
        Base* Parent;
        bool Is_Within_Flag;
        bool End_Has_Reserved_Id;
        bool End_Has_Identifier;
    };

    struct If_Generate_Statement
            : public Base {
        Base* Chain;
        std::string Label;
        bool Visible_Flag;
        Base* Generate_Statement_Body;
        Base* Generate_Else_Clause;
        Base* Condition;
        Base* Parent;
        bool Is_Within_Flag;
        bool End_Has_Reserved_Id;
        bool End_Has_Identifier;
    };

    struct Case_Generate_Statement
            : public Base {
        Base* Chain;
        std::string Label;
        bool Visible_Flag;
        Base* Expression;
        Base* Parent;
        std::vector<Base*> Case_Statement_Alternative;
        bool Is_Within_Flag;
        bool End_Has_Reserved_Id;
        bool End_Has_Identifier;
    };

    struct For_Generate_Statement
            : public Base {
        Base* Chain;
        std::string Label;
        bool Visible_Flag;
        Base* Generate_Statement_Body;
        Base* Parameter_Specification;
        Base* Parent;
        bool Is_Within_Flag;
        bool End_Has_Reserved_Id;
        bool End_Has_Identifier;
    };

    struct Component_Instantiation_Statement
            : public Base {
        Base* Instantiated_Unit;
        Base* Default_Binding_Indication;
        Base* Component_Configuration;
        Base* Configuration_Specification;
        Base* Chain;
        std::string Label;
        bool Visible_Flag;
        Base* Generic_Map_Aspect_Chain;
        Base* Port_Map_Aspect_Chain;
        Base* Parent;
    };

    struct Psl_Default_Clock
            : public Base {
        PSL_Node Psl_Boolean;
        Base* Chain;
        std::string Label;
        Base* Parent;
    };

    struct Simple_Simultaneous_Statement
            : public Base {
        Base* Simultaneous_Left;
        Base* Simultaneous_Right;
        Base* Chain;
        std::string Label;
        bool Visible_Flag;
        Base* Tolerance;
        Base* Parent;
    };

    struct Generate_Statement_Body
            : public Base {
        Base* Generate_Block_Configuration;
        std::string Alternative_Label;
        bool Has_End;
        Base* Attribute_Value_Chain;
        Base* Concurrent_Statement_Chain;
        std::vector<Base*> Declaration_Chain;
        Base* Parent;
        bool Is_Within_Flag;
        bool End_Has_Identifier;
        bool Has_Label;
        bool Has_Begin;
    };

    struct If_Generate_Else_Clause
            : public Base {
        bool Visible_Flag;
        Base* Generate_Statement_Body;
        Base* Generate_Else_Clause;
        Base* Condition;
        Base* Parent;
    };

    struct Null_Statement
            : public Base {
        Base* Chain;
        std::string Label;
        bool Visible_Flag;
        Base* Parent;
    };

    struct Assertion_Statement
            : public Base {
        Base* Chain;
        std::string Label;
        bool Visible_Flag;
        Base* Assertion_Condition;
        Base* Report_Expression;
        Base* Severity_Expression;
        Base* Parent;
    };

    struct Report_Statement
            : public Base {
        Base* Chain;
        std::string Label;
        bool Visible_Flag;
        Base* Report_Expression;
        Base* Severity_Expression;
        Base* Parent;
    };

    struct Wait_Statement
            : public Base {
        Base* Condition_Clause;
        Base* Timeout_Clause;
        Base* Chain;
        std::string Label;
        bool Visible_Flag;
        std::vector<Base*> Sensitivity_List;
        Base* Parent;
        bool Is_Ref;
    };

    struct Variable_Assignment_Statement
            : public Base {
        Base* Chain;
        std::string Label;
        bool Visible_Flag;
        Base* Target;
        Base* Expression;
        Base* Parent;
    };

    struct Conditional_Variable_Assignment_Statement
            : public Base {
        Base* Conditional_Expression;
        Base* Chain;
        std::string Label;
        bool Visible_Flag;
        Base* Target;
        Base* Parent;
    };

    struct Return_Statement
            : public Base {
        Base* Chain;
        Base* Type;
        std::string Label;
        bool Visible_Flag;
        Base* Expression;
        Base* Parent;
    };

    struct For_Loop_Statement
            : public Base {
        Base* Chain;
        Base* Sequential_Statement_Chain;
        std::string Label;
        bool Visible_Flag;
        Base* Parameter_Specification;
        Base* Parent;
        bool Is_Within_Flag;
        bool End_Has_Identifier;
        bool Suspend_Flag;
    };

    struct While_Loop_Statement
            : public Base {
        Base* Chain;
        Base* Sequential_Statement_Chain;
        std::string Label;
        bool Visible_Flag;
        Base* Condition;
        Base* Parent;
        bool End_Has_Identifier;
        bool Suspend_Flag;
    };

    struct Next_Statement
            : public Base {
        Base* Chain;
        std::string Label;
        bool Visible_Flag;
        Base* Condition;
        Base* Parent;
        Base* Loop_Label;
    };

    struct Exit_Statement
            : public Base {
        Base* Chain;
        std::string Label;
        bool Visible_Flag;
        Base* Condition;
        Base* Parent;
        Base* Loop_Label;
    };

    struct Case_Statement
            : public Base {
        Base* Chain;
        std::string Label;
        bool Visible_Flag;
        Base* Expression;
        Base* Parent;
        std::vector<Base*> Case_Statement_Alternative;
        bool End_Has_Identifier;
        bool Suspend_Flag;
    };

    struct Procedure_Call_Statement
            : public Base {
        Base* Chain;
        std::string Label;
        bool Visible_Flag;
        Base* Parent;
        Base* Procedure_Call;
        bool Suspend_Flag;
    };

    struct If_Statement
            : public Base {
        Base* Chain;
        Base* Sequential_Statement_Chain;
        std::string Label;
        bool Visible_Flag;
        Base* Condition;
        Base* Else_Clause;
        Base* Parent;
        bool End_Has_Identifier;
        bool Suspend_Flag;
    };

    struct Elsif
            : public Base {
        Base* Sequential_Statement_Chain;
        Base* Condition;
        Base* Else_Clause;
        Base* Parent;
        bool End_Has_Identifier;
    };

    struct Character_Literal
            : public Base {
        Base* Type;
        Base* Base_Name;
        std::string Identifier;
        Base* Named_Entity;
        bool Is_Forward_Ref;
        Base* Alias_Declaration;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
    };

    struct Simple_Name
            : public Base {
        Type_Definitions Type;
        Base* Base_Name;
        std::string Identifier;
        Base* Named_Entity;
        bool Is_Forward_Ref;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
    };

    struct Selected_Name
            : public Base {
        Base* Type;
        Base* Base_Name;
        std::string Identifier;
        Base* Named_Entity;
        bool Is_Forward_Ref;
        Base* Alias_Declaration;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
    };

    struct Operator_Symbol
            : public Base {
        Base* Type;
        Base* Base_Name;
        std::string Identifier;
        Base* Named_Entity;
        bool Is_Forward_Ref;
        Base* Alias_Declaration;
    };

    struct Reference_Name
            : public Base {
        Base* Referenced_Name;
        Base* Named_Entity;
        bool Is_Forward_Ref;
    };

    struct Selected_By_All_Name
            : public Base {
        Base* Type;
        Base* Base_Name;
        Base* Named_Entity;
        bool Is_Forward_Ref;
        Iir_Staticness Expr_Staticness;
        Base* Prefix;
    };

    struct Parenthesis_Name
            : public Base {
        Base* Association_Chain;
        Base* Type;
        Base* Named_Entity;
        bool Is_Forward_Ref;
        Base* Prefix;
    };

    struct External_Name
            : public Base {
        External_Name_Type Name_Type;
        Base* Chain;
        Base* Type;
        Base* Subtype_Indication;
        bool Shared_Flag;
        Base* Parent;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* External_Pathname;
    };

    struct Package_Pathname
            : public Base {
        std::string Identifier;
        Base* Named_Entity;
        bool Is_Forward_Ref;
        Base* Pathname_Suffix;
    };

    struct Absolute_Pathname
            : public Base {
        Base* Pathname_Suffix;
    };

    struct Relative_Pathname
            : public Base {
        Base* Pathname_Suffix;
    };

    struct Pathname_Element
            : public Base {
        Base* Pathname_Expression;
        std::string Identifier;
        Base* Named_Entity;
        bool Is_Forward_Ref;
        Base* Pathname_Suffix;
    };

    struct Base_Attribute
            : public Base {
        Base* Type;
        Base* Prefix;
    };

    struct Subtype_Attribute
            : public Base {
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Type_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
    };

    struct Element_Attribute
            : public Base {
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Type_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
    };

    struct Left_Type_Attribute
            : public Base {
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
    };

    struct Right_Type_Attribute
            : public Base {
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
    };

    struct High_Type_Attribute
            : public Base {
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
    };

    struct Low_Type_Attribute
            : public Base {
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
    };

    struct Ascending_Type_Attribute
            : public Base {
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
    };

    struct Image_Attribute
            : public Base {
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
        Base* Parameter;
    };

    struct Value_Attribute
            : public Base {
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
        Base* Parameter;
    };

    struct Pos_Attribute
            : public Base {
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
        Base* Parameter;
    };

    struct Val_Attribute
            : public Base {
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
        Base* Parameter;
    };

    struct Succ_Attribute
            : public Base {
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
        Base* Parameter;
    };

    struct Pred_Attribute
            : public Base {
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
        Base* Parameter;
    };

    struct Leftof_Attribute
            : public Base {
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
        Base* Parameter;
    };

    struct Rightof_Attribute
            : public Base {
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
        Base* Parameter;
    };

    struct Delayed_Attribute
            : public Base {
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
        Base* Parameter;
        Base* Signal_Attribute_Declaration;
        Base* Attr_Chain;
        bool Has_Active_Flag;
    };

    struct Stable_Attribute
            : public Base {
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
        Base* Parameter;
        Base* Signal_Attribute_Declaration;
        Base* Attr_Chain;
        bool Has_Active_Flag;
    };

    struct Quiet_Attribute
            : public Base {
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
        Base* Parameter;
        Base* Signal_Attribute_Declaration;
        Base* Attr_Chain;
        bool Has_Active_Flag;
    };

    struct Transaction_Attribute
            : public Base {
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
        Base* Parameter;
        Base* Signal_Attribute_Declaration;
        Base* Attr_Chain;
        bool Has_Active_Flag;
    };

    struct Attribute
            : public Base {
        Attribute_Type attribute_Type;
        Base* Type;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
    };

    struct Behavior_Attribute
            : public Base {
    };

    struct Structure_Attribute
            : public Base {
    };

    struct Simple_Name_Attribute
            : public Base {
        std::string Simple_Name_Identifier;
        Base* Simple_Name_Subtype;
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
    };

    struct Instance_Name_Attribute
            : public Base {
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
    };

    struct Path_Name_Attribute
            : public Base {
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
    };

    struct Array_Attribute
            : public Base {
        Array_Attribute_Type Attribute_Type;
        Base* Type;
        Base* Base_Name;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
        Base* Index_Subtype;
        Base* Parameter;
    };

    struct Attribute_Name
            : public Base {
        Base* Attribute_Signature;
        Base* Type;
        Base* Base_Name;
        std::string Identifier;
        Base* Named_Entity;
        bool Is_Forward_Ref;
        Iir_Staticness Expr_Staticness;
        Iir_Staticness Name_Staticness;
        Base* Prefix;
    };

}
#endif // AVLC_IIR_H
