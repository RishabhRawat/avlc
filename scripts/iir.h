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
#include "iir_types.h"
#include "types.h"
#include <variant>
#include <optional>


namespace IIR {


    const int Base_id = 17;
    const int Iir_Type_Abs_id = 18;
    const int Iir_Parent_Design_Unit_Abs_id = 19;
    const int Iir_Type_Declarator_Abs_id = 20;
    const int Iir_Has_Signal_Flag_Abs_id = 21;
    const int Iir_Expr_Staticness_Abs_id = 22;
    const int Iir_Error_id = 23;
    const int Iir_Chain_Abs_id = 24;
    const int Iir_Elab_Flag_Abs_id = 25;
    const int Iir_Design_File_id = 26;
    const int Iir_Date_Abs_id = 27;
    const int Iir_Context_Items_Abs_id = 28;
    const int Iir_Identifier_Abs_id = 29;
    const int Iir_Design_Unit_id = 30;
    const int Iir_Parent_Abs_id = 31;
    const int Iir_Has_Identifier_List_Abs_id = 32;
    const int Iir_Library_Clause_id = 33;
    const int Iir_Selected_Name_Abs_id = 34;
    const int Iir_Use_Clause_id = 35;
    const int Iir_Context_Reference_id = 36;
    const int Iir_Value_Abs_id = 37;
    const int Iir_Literal_Origin_Abs_id = 38;
    const int Iir_Integer_Literal_id = 39;
    const int Iir_Fp_Value_Abs_id = 40;
    const int Iir_Floating_Point_Literal_id = 41;
    const int Iir_Null_Literal_id = 42;
    const int Iir_Literal_Subtype_Abs_id = 43;
    const int Iir_String_id = 44;
    const int Iir_Physical_Unit_Abs_id = 45;
    const int Iir_Physical_Int_Literal_id = 46;
    const int Iir_Physical_Fp_Literal_id = 47;
    const int Iir_Simple_Aggregate_id = 48;
    const int Iir_Overflow_Literal_id = 49;
    const int Iir_Unaffected_Waveform_id = 50;
    const int Iir_Waveform_Element_id = 51;
    const int Iir_Condition_Abs_id = 52;
    const int Iir_Conditional_Waveform_id = 53;
    const int Iir_Expression_Abs_id = 54;
    const int Iir_Conditional_Expression_id = 55;
    const int Iir_Formal_Abs_id = 56;
    const int Iir_Actual_Abs_id = 57;
    const int Iir_Association_Element_By_Expression_id = 58;
    const int Iir_Actual_Type_Abs_id = 59;
    const int Iir_Choice_Staticness_Abs_id = 60;
    const int Iir_Association_Element_By_Individual_id = 61;
    const int Iir_Association_Element_Open_id = 62;
    const int Iir_Association_Element_Package_id = 63;
    const int Iir_Association_Element_Type_id = 64;
    const int Iir_Association_Element_Subprogram_id = 65;
    const int Iir_Choice_Abs_id = 66;
    const int Iir_Choice_By_Range_id = 67;
    const int Iir_Choice_By_Expression_id = 68;
    const int Iir_Kind_Choice_By_Others_id = 69;
    const int Iir_Choice_By_None_id = 70;
    const int Iir_Choice_By_Name_id = 71;
    const int Iir_Entity_Name_Abs_id = 72;
    const int Iir_Entity_Aspect_Entity_id = 73;
    const int Iir_Entity_Aspect_Configuration_id = 74;
    const int Iir_Entity_Aspect_Open_id = 75;
    const int Iir_Declaration_Chain_Abs_id = 76;
    const int Iir_Block_Configuration_id = 77;
    const int Iir_Port_Chain_Abs_id = 78;
    const int Iir_Generic_Chain_Abs_id = 79;
    const int Iir_Generic_Map_Aspect_Chain_Abs_id = 80;
    const int Iir_Port_Map_Aspect_Chain_Abs_id = 81;
    const int Iir_Block_Header_id = 82;
    const int Iir_Block_Configuration_Abs_id = 83;
    const int Iir_Component_Name_Abs_id = 84;
    const int Iir_Is_Ref_Abs_id = 85;
    const int Iir_Component_Configuration_id = 86;
    const int Iir_Binding_Indication_id = 87;
    const int Iir_Entity_Class_Abs_id = 88;
    const int Iir_Entity_Class_id = 89;
    const int Iir_Base_Name_Abs_id = 90;
    const int Iir_Name_Staticness_Abs_id = 91;
    const int Iir_Attribute_Value_id = 92;
    const int Iir_Return_Type_Mark_Abs_id = 93;
    const int Iir_Signature_id = 94;
    const int Iir_Aggregate_Info_id = 95;
    const int Iir_Prefix_Abs_id = 96;
    const int Iir_Implementation_Abs_id = 97;
    const int Iir_Parameter_Association_Chain_Abs_id = 98;
    const int Iir_Procedure_Call_id = 99;
    const int Iir_Visible_Flag_Abs_id = 100;
    const int Iir_Record_Element_Constraint_id = 101;
    const int Iir_Resolution_Indication_Abs_id = 102;
    const int Iir_Element_Subtype_Indication_Abs_id = 103;
    const int Iir_Array_Element_Resolution_id = 104;
    const int Iir_Record_Element_Resolution_id = 105;
    const int Iir_Record_Resolution_id = 106;
    const int Iir_Attribute_Specification_id = 107;
    const int Iir_Type_Mark_Abs_id = 108;
    const int Iir_Disconnection_Specification_id = 109;
    const int Iir_Configuration_Specification_id = 110;
    const int Iir_Incomplete_Type_Ref_Chain_Abs_id = 111;
    const int Iir_Type_Staticness_Abs_id = 112;
    const int Iir_Designated_Type_Abs_id = 113;
    const int Iir_Access_Type_Definition_id = 114;
    const int Iir_Incomplete_Type_Definition_id = 115;
    const int Iir_Interface_Type_Definition_id = 116;
    const int Iir_File_Type_Definition_id = 117;
    const int Iir_End_Has_Reserved_Id_Abs_id = 118;
    const int Iir_End_Has_Identifier_Abs_id = 119;
    const int Iir_Protected_Type_Declaration_id = 120;
    const int Iir_Constraint_State_Abs_id = 121;
    const int Iir_Elements_Declaration_List_Abs_id = 122;
    const int Iir_Record_Type_Definition_id = 123;
    const int Iir_Index_Subtype_List_Abs_id = 124;
    const int Iir_Array_Type_Definition_id = 125;
    const int Iir_Tolerance_Abs_id = 126;
    const int Iir_Subtype_Type_Mark_Abs_id = 127;
    const int Iir_Array_Subtype_Definition_id = 128;
    const int Iir_Record_Subtype_Definition_id = 129;
    const int Iir_Access_Subtype_Definition_id = 130;
    const int Iir_Range_Constraint_Abs_id = 131;
    const int Iir_Physical_Subtype_Definition_id = 132;
    const int Iir_Floating_Type_Definition_id = 133;
    const int Iir_Floating_Subtype_Definition_id = 134;
    const int Iir_Integer_Type_Definition_id = 135;
    const int Iir_Integer_Subtype_Definition_id = 136;
    const int Iir_Enumeration_Subtype_Definition_id = 137;
    const int Iir_Enumeration_Type_Definition_id = 138;
    const int Iir_Physical_Type_Definition_id = 139;
    const int Iir_Range_Expression_id = 140;
    const int Iir_Protected_Type_Body_id = 141;
    const int Iir_Wildcard_Type_Definition_id = 142;
    const int Iir_Subtype_Definition_id = 143;
    const int Iir_Scalar_Nature_Definition_id = 144;
    const int Iir_Overload_List_id = 145;
    const int Iir_Type_Definition_Abs_id = 146;
    const int Iir_Use_Flag_Abs_id = 147;
    const int Iir_Type_Declaration_id = 148;
    const int Iir_Anonymous_Type_Declaration_id = 149;
    const int Iir_Subtype_Indication_Abs_id = 150;
    const int Iir_Subtype_Declaration_id = 151;
    const int Iir_Nature_Abs_id = 152;
    const int Iir_Nature_Declaration_id = 153;
    const int Iir_Subnature_Declaration_id = 154;
    const int Iir_Attribute_Value_Chain_Abs_id = 155;
    const int Iir_Package_Body_Abs_id = 156;
    const int Iir_Package_Declaration_id = 157;
    const int Iir_Uninstantiated_Package_Name_Abs_id = 158;
    const int Iir_Package_Instantiation_Declaration_id = 159;
    const int Iir_Package_Body_id = 160;
    const int Iir_Configuration_Declaration_id = 161;
    const int Iir_Concurrent_Statement_Chain_Abs_id = 162;
    const int Iir_Is_Within_Flag_Abs_id = 163;
    const int Iir_Has_Begin_Abs_id = 164;
    const int Iir_Entity_Declaration_id = 165;
    const int Iir_Foreign_Flag_Abs_id = 166;
    const int Iir_Architecture_Body_id = 167;
    const int Iir_Context_Declaration_id = 168;
    const int Iir_Package_Header_id = 169;
    const int Iir_Unit_Declaration_id = 170;
    const int Iir_Library_Declaration_id = 171;
    const int Iir_Has_Is_Abs_id = 172;
    const int Iir_Component_Declaration_id = 173;
    const int Iir_Attribute_Declaration_id = 174;
    const int Iir_Group_Template_Declaration_id = 175;
    const int Iir_Group_Declaration_id = 176;
    const int Iir_Element_Declaration_id = 177;
    const int Iir_Name_Abs_id = 178;
    const int Iir_Non_Object_Alias_Declaration_id = 179;
    const int Iir_Psl_Declaration_Abs_id = 180;
    const int Iir_PSL_Clock_Abs_id = 181;
    const int Iir_Psl_Declaration_id = 182;
    const int Iir_PSL_Nbr_States_Abs_id = 183;
    const int Iir_Psl_Endpoint_Declaration_id = 184;
    const int Iir_Terminal_Declaration_id = 185;
    const int Iir_Default_Value_Abs_id = 186;
    const int Iir_Free_Quantity_Declaration_id = 187;
    const int Iir_Plus_Terminal_Abs_id = 188;
    const int Iir_Across_Quantity_Declaration_id = 189;
    const int Iir_Through_Quantity_Declaration_id = 190;
    const int Iir_Subprogram_Hash_Abs_id = 191;
    const int Iir_Seen_Flag_Abs_id = 192;
    const int Iir_Enumeration_Literal_id = 193;
    const int Iir_Interface_Declaration_Chain_Abs_id = 194;
    const int Iir_Subprogram_Body_Abs_id = 195;
    const int Iir_Return_Type_Abs_id = 196;
    const int Iir_Wait_State_Abs_id = 197;
    const int Iir_Function_Declaration_id = 198;
    const int Iir_Passive_Flag_Abs_id = 199;
    const int Iir_Suspend_Flag_Abs_id = 200;
    const int Iir_Procedure_Declaration_id = 201;
    const int Iir_Subprogram_Specification_Abs_id = 202;
    const int Iir_Sequential_Statement_Chain_Abs_id = 203;
    const int Iir_Callees_List_Abs_id = 204;
    const int Iir_Function_Body_id = 205;
    const int Iir_Procedure_Body_id = 206;
    const int Iir_After_Drivers_Flag_Abs_id = 207;
    const int Iir_Object_Alias_Declaration_id = 208;
    const int Iir_Mode_Abs_id = 209;
    const int Iir_File_Declaration_id = 210;
    const int Iir_Guarded_Signal_Flag_Abs_id = 211;
    const int Iir_Has_Active_Flag_Abs_id = 212;
    const int Iir_Guard_Signal_Declaration_id = 213;
    const int Iir_Has_Disconnect_Flag_Abs_id = 214;
    const int Iir_Signal_Declaration_id = 215;
    const int Iir_Shared_Flag_Abs_id = 216;
    const int Iir_Variable_Declaration_id = 217;
    const int Iir_Constant_Declaration_id = 218;
    const int Iir_Iterator_Declaration_id = 219;
    const int Iir_Has_Class_Abs_id = 220;
    const int Iir_Interface_Signal_Declaration_Extras_id = 221;
    const int Iir_Interface_Object_Declaration_id = 222;
    const int Iir_Interface_Type_Declaration_id = 223;
    const int Iir_Interface_Package_Declaration_id = 224;
    const int Iir_Interface_Function_Declaration_id = 225;
    const int Iir_Interface_Procedure_Declaration_id = 226;
    const int Iir_Signal_Attribute_Declaration_id = 227;
    const int Iir_Operand_Abs_id = 228;
    const int Iir_Unary_Operator_id = 229;
    const int Iir_Binary_Operator_id = 230;
    const int Iir_Function_Call_id = 231;
    const int Iir_Aggregate_id = 232;
    const int Iir_Parenthesis_Expression_id = 233;
    const int Iir_Qualified_Expression_id = 234;
    const int Iir_Type_Conversion_id = 235;
    const int Iir_Allocator_Designated_Type_Abs_id = 236;
    const int Iir_Allocator_By_Expression_id = 237;
    const int Iir_Allocator_By_Subtype_id = 238;
    const int Iir_Selected_Element_id = 239;
    const int Iir_Dereference_id = 240;
    const int Iir_Implicit_Dereference_id = 241;
    const int Iir_Slice_Name_id = 242;
    const int Iir_Indexed_Name_id = 243;
    const int Iir_Psl_Expression_id = 244;
    const int Iir_Label_Abs_id = 245;
    const int Iir_Sensitivity_List_Abs_id = 246;
    const int Iir_Process_Origin_Abs_id = 247;
    const int Iir_Postponed_Flag_Abs_id = 248;
    const int Iir_Has_Label_Abs_id = 249;
    const int Iir_Sensitized_Process_Statement_id = 250;
    const int Iir_Process_Statement_id = 251;
    const int Iir_Guarded_Target_State_Abs_id = 252;
    const int Iir_Target_Abs_id = 253;
    const int Iir_Guard_Abs_id = 254;
    const int Iir_Simple_Signal_Assignment_Statement_id = 255;
    const int Iir_Conditional_Signal_Assignment_Statement_id = 256;
    const int Iir_Selected_Waveform_Assignment_Statement_id = 257;
    const int Iir_Concurrent_Simple_Signal_Assignment_id = 258;
    const int Iir_Concurrent_Conditional_Signal_Assignment_id = 259;
    const int Iir_Selected_Waveform_Chain_Abs_id = 260;
    const int Iir_Concurrent_Selected_Signal_Assignment_id = 261;
    const int Iir_Assertion_Condition_Abs_id = 262;
    const int Iir_Report_Expression_Abs_id = 263;
    const int Iir_Concurrent_Assertion_Statement_id = 264;
    const int Iir_Procedure_Call_Abs_id = 265;
    const int Iir_Concurrent_Procedure_Call_Statement_id = 266;
    const int Iir_Psl_Assert_Statement_id = 267;
    const int Iir_Psl_Cover_Statement_id = 268;
    const int Iir_Block_Statement_id = 269;
    const int Iir_Generate_Statement_Body_Abs_id = 270;
    const int Iir_Generate_Else_Clause_Abs_id = 271;
    const int Iir_If_Generate_Statement_id = 272;
    const int Iir_Case_Statement_Alternative_Chain_Abs_id = 273;
    const int Iir_Case_Generate_Statement_id = 274;
    const int Iir_Parameter_Specification_Abs_id = 275;
    const int Iir_For_Generate_Statement_id = 276;
    const int Iir_Component_Instantiation_Statement_id = 277;
    const int Iir_Psl_Default_Clock_id = 278;
    const int Iir_Simple_Simultaneous_Statement_id = 279;
    const int Iir_Generate_Statement_Body_id = 280;
    const int Iir_If_Generate_Else_Clause_id = 281;
    const int Iir_Null_Statement_id = 282;
    const int Iir_Assertion_Statement_id = 283;
    const int Iir_Report_Statement_id = 284;
    const int Iir_Wait_Statement_id = 285;
    const int Iir_Variable_Assignment_Statement_id = 286;
    const int Iir_Conditional_Variable_Assignment_Statement_id = 287;
    const int Iir_Return_Statement_id = 288;
    const int Iir_For_Loop_Statement_id = 289;
    const int Iir_While_Loop_Statement_id = 290;
    const int Iir_Loop_Label_Abs_id = 291;
    const int Iir_Next_Statement_id = 292;
    const int Iir_Exit_Statement_id = 293;
    const int Iir_Case_Statement_id = 294;
    const int Iir_Procedure_Call_Statement_id = 295;
    const int Iir_Else_Clause_Abs_id = 296;
    const int Iir_If_Statement_id = 297;
    const int Iir_Elsif_id = 298;
    const int Iir_Named_Entity_Abs_id = 299;
    const int Iir_Alias_Declaration_Abs_id = 300;
    const int Iir_Character_Literal_id = 301;
    const int Iir_Simple_Name_id = 302;
    const int Iir_Selected_Name_id = 303;
    const int Iir_Operator_Symbol_id = 304;
    const int Iir_Reference_Name_id = 305;
    const int Iir_Selected_By_All_Name_id = 306;
    const int Iir_Parenthesis_Name_id = 307;
    const int Iir_External_Pathname_Abs_id = 308;
    const int Iir_External_Name_id = 309;
    const int Iir_Pathname_Suffix_Abs_id = 310;
    const int Iir_Package_Pathname_id = 311;
    const int Iir_Absolute_Pathname_id = 312;
    const int Iir_Relative_Pathname_id = 313;
    const int Iir_Pathname_Element_id = 314;
    const int Iir_Base_Attribute_id = 315;
    const int Iir_Subtype_Attribute_id = 316;
    const int Iir_Element_Attribute_id = 317;
    const int Iir_Left_Type_Attribute_id = 318;
    const int Iir_Right_Type_Attribute_id = 319;
    const int Iir_High_Type_Attribute_id = 320;
    const int Iir_Low_Type_Attribute_id = 321;
    const int Iir_Ascending_Type_Attribute_id = 322;
    const int Iir_Parameter_Abs_id = 323;
    const int Iir_Image_Attribute_id = 324;
    const int Iir_Value_Attribute_id = 325;
    const int Iir_Pos_Attribute_id = 326;
    const int Iir_Val_Attribute_id = 327;
    const int Iir_Succ_Attribute_id = 328;
    const int Iir_Pred_Attribute_id = 329;
    const int Iir_Leftof_Attribute_id = 330;
    const int Iir_Rightof_Attribute_id = 331;
    const int Iir_Attr_Chain_Abs_id = 332;
    const int Iir_Delayed_Attribute_id = 333;
    const int Iir_Stable_Attribute_id = 334;
    const int Iir_Quiet_Attribute_id = 335;
    const int Iir_Transaction_Attribute_id = 336;
    const int Iir_Attribute_id = 337;
    const int Iir_Behavior_Attribute_id = 338;
    const int Iir_Structure_Attribute_id = 339;
    const int Iir_Simple_Name_Attribute_id = 340;
    const int Iir_Instance_Name_Attribute_id = 341;
    const int Iir_Path_Name_Attribute_id = 342;
    const int Iir_Index_Subtype_Abs_id = 343;
    const int Iir_Array_Attribute_id = 344;
    const int Iir_Attribute_Name_id = 345;

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

    class Design_Unit;

    class Library_Declaration;

    class Entity_Declaration;

    class Architecture_Body;

    class Package_Declaration;

    class Package_Instantiation_Declaration;

    class Package_Body;

    class Configuration_Declaration;

    class Context_Declaration;

    class Unit_Declaration;

    class Binding_Indication;

    class Enumeration_Literal;

    class Base {
        Location_Type Location;
    };

    class Iir_Type_Abs
            : public virtual Iir {
        Iir* Type;
    };

    class Iir_Parent_Design_Unit_Abs
            : public virtual Iir {
        Iir_Design_Unit* Parent_Design_Unit;
    };

    class Iir_Type_Declarator_Abs
            : public virtual Iir {
        bool Resolved_Flag;
        Iir* Base_Type;
        bool Signal_Type_Flag;
        Iir* Type_Declarator;
    };

    class Iir_Has_Signal_Flag_Abs
            : public virtual Iir {
        bool Has_Signal_Flag;
    };

    class Iir_Expr_Staticness_Abs
            : public virtual Iir {
        Iir_Staticness Expr_Staticness;
    };

    class Iir_Error
            : public Iir_Type_Abs,
              public Iir_Type_Declarator_Abs,
              public Iir_Has_Signal_Flag_Abs,
              public Iir_Expr_Staticness_Abs {
        Iir* Error_Origin;
    };

    class Iir_Chain_Abs
            : public virtual Iir {
        Iir* Chain;
    };

    class Iir_Elab_Flag_Abs
            : public virtual Iir {
        bool Elab_Flag;
    };

    class Iir_Design_File
            : public Iir_Elab_Flag_Abs {
        std::vector<Iir_Design_Unit*> Design_Units;
        Time_Stamp_Id Analysis_Time_Stamp;
        File_Checksum_Id File_Checksum;
        Iir_Library_Declaration* Library;
        std::vector<Iir*> File_Dependence_List;
        std::filesystem::path Design_File_Filename;
    };

    class Iir_Date_Abs
            : public virtual Iir {
        Date_Type Date;
    };

    class Iir_Context_Items_Abs
            : public virtual Iir {
        std::vector<Iir*> Context_Items;
    };

    class Iir_Identifier_Abs
            : public virtual Iir {
        std::string Identifier;
    };

    class Iir_Design_Unit
            : public Iir_Date_Abs,
              public Iir_Context_Items_Abs,
              public Iir_Identifier_Abs,
              public Iir_Elab_Flag_Abs {
        Location_Type Source_Pos;
        Location_Type End_Location;
        bool Configuration_Mark_Flag;
        Iir_Design_File* Design_File;
        Iir_Design_Unit_n Library_Unit;
        Date_State_Type Date_State;
        std::vector<Iir*> Analysis_Checks_List;
        std::vector<Iir*> Dependence_List;
        bool Configuration_Done_Flag;
    };

    class Iir_Parent_Abs
            : public virtual Iir {
        Iir* Parent;
    };

    class Iir_Has_Identifier_List_Abs
            : public virtual Iir {
        bool Has_Identifier_List;
    };

    class Iir_Library_Clause
            : public Iir_Chain_Abs,
              public Iir_Identifier_Abs,
              public Iir_Parent_Abs,
              public Iir_Has_Identifier_List_Abs {
        Iir* Library_Declaration;
    };

    class Iir_Selected_Name_Abs
            : public virtual Iir {
        Iir* Selected_Name;
    };

    class Iir_Use_Clause
            : public Iir_Chain_Abs,
              public Iir_Selected_Name_Abs,
              public Iir_Parent_Abs {
        Iir* Use_Clause_Chain;
    };

    class Iir_Context_Reference
            : public Iir_Chain_Abs,
              public Iir_Selected_Name_Abs,
              public Iir_Parent_Abs {
        Iir* Context_Reference_Chain;
    };

    class Iir_Value_Abs
            : public virtual Iir {
        int64_t Value;
    };

    class Iir_Literal_Origin_Abs
            : public virtual Iir {
        Iir* Literal_Origin;
    };

    class Iir_Integer_Literal
            : public Iir_Value_Abs,
              public Iir_Literal_Origin_Abs,
              public Iir_Type_Abs,
              public Iir_Expr_Staticness_Abs {
    };

    class Iir_Fp_Value_Abs
            : public virtual Iir {
        double Fp_Value;
    };

    class Iir_Floating_Point_Literal
            : public Iir_Fp_Value_Abs,
              public Iir_Literal_Origin_Abs,
              public Iir_Type_Abs,
              public Iir_Expr_Staticness_Abs {
    };

    class Iir_Null_Literal
            : public Iir_Type_Abs,
              public Iir_Expr_Staticness_Abs {
    };

    class Iir_Literal_Subtype_Abs
            : public virtual Iir {
        Iir* Literal_Subtype;
    };

    class Iir_String
            : public Iir_Literal_Origin_Abs,
              public Iir_Literal_Subtype_Abs,
              public Iir_Type_Abs,
              public Iir_Expr_Staticness_Abs {
        std::string str;
        bool Has_Sign;
        Number_Base_Type Bit_String_Base;
        bool Has_Signed;
    };

    class Iir_Physical_Unit_Abs
            : public virtual Iir {
        Iir* Unit_Name;
        Iir_Unit_Declaration* Physical_Unit;
    };

    class Iir_Physical_Int_Literal
            : public Iir_Value_Abs,
              public Iir_Literal_Origin_Abs,
              public Iir_Type_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Physical_Unit_Abs {
    };

    class Iir_Physical_Fp_Literal
            : public Iir_Fp_Value_Abs,
              public Iir_Literal_Origin_Abs,
              public Iir_Type_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Physical_Unit_Abs {
    };

    class Iir_Simple_Aggregate
            : public Iir_Literal_Origin_Abs,
              public Iir_Literal_Subtype_Abs,
              public Iir_Type_Abs,
              public Iir_Expr_Staticness_Abs {
        std::vector<Iir*> Simple_Aggregate_List;
    };

    class Iir_Overflow_Literal
            : public Iir_Literal_Origin_Abs,
              public Iir_Type_Abs,
              public Iir_Expr_Staticness_Abs {
    };

    class Iir_Unaffected_Waveform
            : public virtual Iir {
    };

    class Iir_Waveform_Element
            : public virtual Iir {
        Iir* We_Value;
        Iir* Time;
    };

    class Iir_Condition_Abs
            : public virtual Iir {
        Iir* Condition;
    };

    class Iir_Conditional_Waveform
            : public Iir_Chain_Abs,
              public Iir_Condition_Abs {
        Iir_Waveform waveform;
    };

    class Iir_Expression_Abs
            : public virtual Iir {
        Iir* Expression;
    };

    class Iir_Conditional_Expression
            : public Iir_Chain_Abs,
              public Iir_Expression_Abs,
              public Iir_Condition_Abs {
    };

    class Iir_Formal_Abs
            : public virtual Iir {
        bool Whole_Association_Flag;
        bool Collapse_Signal_Flag;
        Iir* Formal;
    };

    class Iir_Actual_Abs
            : public virtual Iir {
        Iir* Actual;
    };

    class Iir_Association_Element_By_Expression
            : public Iir_Formal_Abs,
              public Iir_Actual_Abs,
              public Iir_Chain_Abs {
        Iir* In_Conversion;
        Iir* Out_Conversion;
    };

    class Iir_Actual_Type_Abs
            : public virtual Iir {
        Iir* Actual_Type;
    };

    class Iir_Choice_Staticness_Abs
            : public virtual Iir {
        Iir_Staticness Choice_Staticness;
    };

    class Iir_Association_Element_By_Individual
            : public Iir_Formal_Abs,
              public Iir_Chain_Abs,
              public Iir_Actual_Type_Abs,
              public Iir_Choice_Staticness_Abs {
        Iir* Individual_Association_Chain;
        Iir* Actual_Type_Definition;
    };

    class Iir_Association_Element_Open
            : public Iir_Formal_Abs,
              public Iir_Chain_Abs {
        bool Artificial_Flag;
    };

    class Iir_Association_Element_Package
            : public Iir_Formal_Abs,
              public Iir_Actual_Abs,
              public Iir_Chain_Abs {
    };

    class Iir_Association_Element_Type
            : public Iir_Formal_Abs,
              public Iir_Actual_Abs,
              public Iir_Chain_Abs,
              public Iir_Actual_Type_Abs {
        Iir* Subprogram_Association_Chain;
    };

    class Iir_Association_Element_Subprogram
            : public Iir_Formal_Abs,
              public Iir_Actual_Abs,
              public Iir_Chain_Abs {
    };

    class Iir_Choice_Abs
            : public Iir_Chain_Abs,
              public Iir_Parent_Abs {
        bool Same_Alternative_Flag;
        Iir* Associated_Expr;
        Iir* Associated_Chain;
    };

    class Iir_Choice_By_Range
            : public Iir_Choice_Abs,
              public Iir_Choice_Staticness_Abs {
        Iir* Choice_Range;
    };

    class Iir_Choice_By_Expression
            : public Iir_Choice_Abs,
              public Iir_Choice_Staticness_Abs {
        Iir* Choice_Expression;
    };

    class Iir_Kind_Choice_By_Others
            : public Iir_Choice_Abs {
    };

    class Iir_Choice_By_None
            : public Iir_Choice_Abs {
    };

    class Iir_Choice_By_Name
            : public Iir_Choice_Abs {
        Iir* Choice_Name;
    };

    class Iir_Entity_Name_Abs
            : public virtual Iir {
        Iir* Entity_Name;
    };

    class Iir_Entity_Aspect_Entity
            : public Iir_Entity_Name_Abs {
        Iir* Architecture;
    };

    class Iir_Entity_Aspect_Configuration
            : public virtual Iir {
        Iir* Configuration_Name;
    };

    class Iir_Entity_Aspect_Open
            : public virtual Iir {
    };

    class Iir_Declaration_Chain_Abs
            : public virtual Iir {
        std::vector<Iir*> Declaration_Chain;
    };

    class Iir_Block_Configuration
            : public Iir_Chain_Abs,
              public Iir_Declaration_Chain_Abs,
              public Iir_Parent_Abs {
        Iir* Prev_Block_Configuration;
        Iir* Block_Specification;
        Iir* Configuration_Item_Chain;
    };

    class Iir_Port_Chain_Abs
            : public virtual Iir {
        Iir* Port_Chain;
    };

    class Iir_Generic_Chain_Abs
            : public virtual Iir {
        Iir* Generic_Chain;
    };

    class Iir_Generic_Map_Aspect_Chain_Abs
            : public virtual Iir {
        Iir* Generic_Map_Aspect_Chain;
    };

    class Iir_Port_Map_Aspect_Chain_Abs
            : public virtual Iir {
        Iir* Port_Map_Aspect_Chain;
    };

    class Iir_Block_Header
            : public Iir_Port_Chain_Abs,
              public Iir_Generic_Chain_Abs,
              public Iir_Generic_Map_Aspect_Chain_Abs,
              public Iir_Port_Map_Aspect_Chain_Abs {
    };

    class Iir_Block_Configuration_Abs
            : public virtual Iir {
        Iir* Block_Configuration;
    };

    class Iir_Component_Name_Abs
            : public virtual Iir {
        std::vector<Iir*> Instantiation_List;
        Iir_Binding_Indication* Binding_Indication;
        Iir* Component_Name;
    };

    class Iir_Is_Ref_Abs
            : public virtual Iir {
        bool Is_Ref;
    };

    class Iir_Component_Configuration
            : public Iir_Block_Configuration_Abs,
              public Iir_Chain_Abs,
              public Iir_Parent_Abs,
              public Iir_Component_Name_Abs,
              public Iir_Is_Ref_Abs {
    };

    class Iir_Binding_Indication
            : public Iir_Generic_Map_Aspect_Chain_Abs,
              public Iir_Port_Map_Aspect_Chain_Abs {
        Iir* Entity_Aspect;
        Iir* Default_Entity_Aspect;
    };

    class Iir_Entity_Class_Abs
            : public virtual Iir {
        Token_Type Entity_Class;
    };

    class Iir_Entity_Class
            : public Iir_Entity_Class_Abs,
              public Iir_Chain_Abs {
    };

    class Iir_Base_Name_Abs
            : public virtual Iir {
        Iir* Base_Name;
    };

    class Iir_Name_Staticness_Abs
            : public virtual Iir {
        Iir_Staticness Name_Staticness;
    };

    class Iir_Attribute_Value
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs {
        Iir* Designated_Entity;
        Iir* Value_Chain;
        Iir* Attribute_Specification;
        Iir* Spec_Chain;
    };

    class Iir_Return_Type_Mark_Abs
            : public virtual Iir {
        Iir* Return_Type_Mark;
    };

    class Iir_Signature
            : public Iir_Return_Type_Mark_Abs {
        Iir* Signature_Prefix;
        std::vector<Iir*> Type_Marks_List;
    };

    class Iir_Aggregate_Info
            : public virtual Iir {
        bool Aggr_Named_Flag;
        Iir* Aggr_Low_Limit;
        bool Aggr_Others_Flag;
        Iir* Sub_Aggregate_Info;
        Iir* Aggr_High_Limit;
        bool Aggr_Dynamic_Flag;
        int Aggr_Min_Length;
    };

    class Iir_Prefix_Abs
            : public virtual Iir {
        Iir* Prefix;
    };

    class Iir_Implementation_Abs
            : public virtual Iir {
        Iir* Implementation;
    };

    class Iir_Parameter_Association_Chain_Abs
            : public virtual Iir {
        Iir* Parameter_Association_Chain;
        Iir* Method_Object;
    };

    class Iir_Procedure_Call
            : public Iir_Prefix_Abs,
              public Iir_Implementation_Abs,
              public Iir_Parameter_Association_Chain_Abs {
    };

    class Iir_Visible_Flag_Abs
            : public virtual Iir {
        bool Visible_Flag;
    };

    class Iir_Record_Element_Constraint
            : public Iir_Type_Abs,
              public Iir_Identifier_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Parent_Abs {
        Iir* Element_Declaration;
    };

    class Iir_Resolution_Indication_Abs
            : public virtual Iir {
        Iir* Resolution_Indication;
    };

    class Iir_Element_Subtype_Indication_Abs
            : public virtual Iir {
        Iir* Element_Subtype_Indication;
    };

    class Iir_Array_Element_Resolution
            : public Iir_Resolution_Indication_Abs,
              public Iir_Element_Subtype_Indication_Abs {
    };

    class Iir_Record_Element_Resolution
            : public Iir_Chain_Abs,
              public Iir_Identifier_Abs,
              public Iir_Resolution_Indication_Abs {
    };

    class Iir_Record_Resolution
            : public virtual Iir {
        std::vector<Iir_Record_Element_Resolution*> chain;
    };

    class Iir_Attribute_Specification
            : public Iir_Entity_Class_Abs,
              public Iir_Chain_Abs,
              public Iir_Expression_Abs,
              public Iir_Parent_Abs {
        Iir* Attribute_Value_Spec_Chain;
        Iir* Attribute_Specification_Chain;
        std::vector<Iir*> Entity_Name_List;
        Iir* Attribute_Designator;
    };

    class Iir_Type_Mark_Abs
            : public virtual Iir {
        Iir* Type_Mark;
    };

    class Iir_Disconnection_Specification
            : public Iir_Chain_Abs,
              public Iir_Expression_Abs,
              public Iir_Parent_Abs,
              public Iir_Type_Mark_Abs,
              public Iir_Is_Ref_Abs {
        std::vector<Iir*> Signal_List;
    };

    class Iir_Configuration_Specification
            : public Iir_Chain_Abs,
              public Iir_Parent_Abs,
              public Iir_Component_Name_Abs,
              public Iir_Is_Ref_Abs {
    };

    class Iir_Incomplete_Type_Ref_Chain_Abs
            : public virtual Iir {
        Iir* Incomplete_Type_Ref_Chain;
    };

    class Iir_Type_Staticness_Abs
            : public virtual Iir {
        Iir_Staticness Type_Staticness;
    };

    class Iir_Designated_Type_Abs
            : public virtual Iir {
        Iir* Designated_Type;
        Iir* Designated_Subtype_Indication;
    };

    class Iir_Access_Type_Definition
            : public Iir_Type_Declarator_Abs,
              public Iir_Incomplete_Type_Ref_Chain_Abs,
              public Iir_Type_Staticness_Abs,
              public Iir_Designated_Type_Abs {
    };

    class Iir_Incomplete_Type_Definition
            : public Iir_Type_Declarator_Abs,
              public Iir_Incomplete_Type_Ref_Chain_Abs,
              public Iir_Type_Staticness_Abs,
              public Iir_Has_Signal_Flag_Abs {
        Iir* Complete_Type_Definition;
    };

    class Iir_Interface_Type_Definition
            : public Iir_Type_Declarator_Abs,
              public Iir_Type_Staticness_Abs,
              public Iir_Has_Signal_Flag_Abs {
        Iir* Associated_Type;
    };

    class Iir_File_Type_Definition
            : public Iir_Type_Declarator_Abs,
              public Iir_Type_Staticness_Abs {
        Iir* File_Type_Mark;
        bool Text_File_Flag;
    };

    class Iir_End_Has_Reserved_Id_Abs
            : public virtual Iir {
        bool End_Has_Reserved_Id;
    };

    class Iir_End_Has_Identifier_Abs
            : public virtual Iir {
        bool End_Has_Identifier;
    };

    class Iir_Protected_Type_Declaration
            : public Iir_Declaration_Chain_Abs,
              public Iir_Type_Declarator_Abs,
              public Iir_Type_Staticness_Abs,
              public Iir_End_Has_Reserved_Id_Abs,
              public Iir_End_Has_Identifier_Abs {
        Iir* Protected_Type_Body;
    };

    class Iir_Constraint_State_Abs
            : public virtual Iir {
        Iir_Constraint Constraint_State;
    };

    class Iir_Elements_Declaration_List_Abs
            : public virtual Iir {
        std::vector<Iir*> Elements_Declaration_List;
    };

    class Iir_Record_Type_Definition
            : public Iir_Type_Declarator_Abs,
              public Iir_Type_Staticness_Abs,
              public Iir_Constraint_State_Abs,
              public Iir_Elements_Declaration_List_Abs,
              public Iir_Has_Signal_Flag_Abs,
              public Iir_End_Has_Reserved_Id_Abs,
              public Iir_End_Has_Identifier_Abs {
    };

    class Iir_Index_Subtype_List_Abs
            : public virtual Iir {
        std::vector<Iir*> Index_Subtype_List;
        Iir* Element_Subtype;
        bool Index_Constraint_Flag;
    };

    class Iir_Array_Type_Definition
            : public Iir_Type_Declarator_Abs,
              public Iir_Type_Staticness_Abs,
              public Iir_Constraint_State_Abs,
              public Iir_Index_Subtype_List_Abs,
              public Iir_Element_Subtype_Indication_Abs,
              public Iir_Has_Signal_Flag_Abs {
        std::vector<Iir*> Index_Subtype_Definition_List;
    };

    class Iir_Tolerance_Abs
            : public virtual Iir {
        Iir* Tolerance;
    };

    class Iir_Subtype_Type_Mark_Abs
            : public virtual Iir {
        Iir* Subtype_Type_Mark;
    };

    class Iir_Array_Subtype_Definition
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

    class Iir_Record_Subtype_Definition
            : public Iir_Type_Declarator_Abs,
              public Iir_Resolution_Indication_Abs,
              public Iir_Tolerance_Abs,
              public Iir_Type_Staticness_Abs,
              public Iir_Constraint_State_Abs,
              public Iir_Elements_Declaration_List_Abs,
              public Iir_Has_Signal_Flag_Abs,
              public Iir_Subtype_Type_Mark_Abs {
    };

    class Iir_Access_Subtype_Definition
            : public Iir_Type_Declarator_Abs,
              public Iir_Type_Staticness_Abs,
              public Iir_Designated_Type_Abs,
              public Iir_Subtype_Type_Mark_Abs {
    };

    class Iir_Range_Constraint_Abs
            : public virtual Iir {
        Iir* Range_Constraint;
    };

    class Iir_Physical_Subtype_Definition
            : public Iir_Type_Declarator_Abs,
              public Iir_Range_Constraint_Abs,
              public Iir_Resolution_Indication_Abs,
              public Iir_Type_Staticness_Abs,
              public Iir_Has_Signal_Flag_Abs,
              public Iir_Subtype_Type_Mark_Abs,
              public Iir_Is_Ref_Abs {
    };

    class Iir_Floating_Type_Definition
            : public Iir_Type_Declarator_Abs,
              public Iir_Range_Constraint_Abs,
              public Iir_Type_Staticness_Abs,
              public Iir_Has_Signal_Flag_Abs,
              public Iir_Is_Ref_Abs {
    };

    class Iir_Floating_Subtype_Definition
            : public Iir_Floating_Type_Definition,
              public Iir_Resolution_Indication_Abs,
              public Iir_Tolerance_Abs,
              public Iir_Subtype_Type_Mark_Abs {
    };

    class Iir_Integer_Type_Definition
            : public Iir_Type_Declarator_Abs,
              public Iir_Range_Constraint_Abs,
              public Iir_Type_Staticness_Abs,
              public Iir_Has_Signal_Flag_Abs,
              public Iir_Is_Ref_Abs {
    };

    class Iir_Integer_Subtype_Definition
            : public Iir_Integer_Type_Definition,
              public Iir_Resolution_Indication_Abs,
              public Iir_Subtype_Type_Mark_Abs {
    };

    class Iir_Enumeration_Subtype_Definition
            : public Iir_Type_Declarator_Abs,
              public Iir_Range_Constraint_Abs,
              public Iir_Resolution_Indication_Abs,
              public Iir_Type_Staticness_Abs,
              public Iir_Has_Signal_Flag_Abs,
              public Iir_Subtype_Type_Mark_Abs,
              public Iir_Is_Ref_Abs {
    };

    class Iir_Enumeration_Type_Definition
            : public Iir_Type_Declarator_Abs,
              public Iir_Range_Constraint_Abs,
              public Iir_Type_Staticness_Abs,
              public Iir_Has_Signal_Flag_Abs,
              public Iir_Is_Ref_Abs {
        std::vector<Iir_Enumeration_Literal*> Enumeration_Literal_List;
        bool Only_Characters_Flag;
    };

    class Iir_Physical_Type_Definition
            : public Iir_Type_Declarator_Abs,
              public Iir_Range_Constraint_Abs,
              public Iir_Type_Staticness_Abs,
              public Iir_Has_Signal_Flag_Abs,
              public Iir_End_Has_Reserved_Id_Abs,
              public Iir_End_Has_Identifier_Abs,
              public Iir_Is_Ref_Abs {
        std::vector<Iir*> Unit_Chain;
    };

    class Iir_Range_Expression
            : public Iir_Type_Abs,
              public Iir_Expr_Staticness_Abs {
        Iir_Direction Direction;
        Iir* Left_Limit;
        Iir* Right_Limit_Expr;
        Iir* Range_Origin;
        Iir* Left_Limit_Expr;
        Iir* Right_Limit;
    };

    class Iir_Protected_Type_Body
            : public Iir_Chain_Abs,
              public Iir_Declaration_Chain_Abs,
              public Iir_Identifier_Abs,
              public Iir_Parent_Abs,
              public Iir_End_Has_Reserved_Id_Abs,
              public Iir_End_Has_Identifier_Abs {
        Iir* Protected_Type_Declaration;
    };

    class Iir_Wildcard_Type_Definition
            : public Iir_Type_Declarator_Abs,
              public Iir_Type_Staticness_Abs {
    };

    class Iir_Subtype_Definition
            : public Iir_Range_Constraint_Abs,
              public Iir_Resolution_Indication_Abs,
              public Iir_Tolerance_Abs,
              public Iir_Subtype_Type_Mark_Abs,
              public Iir_Is_Ref_Abs {
    };

    class Iir_Scalar_Nature_Definition
            : public virtual Iir {
        Iir* Nature_Declarator;
        Iir* Through_Type;
        Iir* Across_Type;
        Iir* Reference;
    };

    class Iir_Overload_List
            : public virtual Iir {
        std::vector<Iir*> Overload_List;
    };

    class Iir_Type_Definition_Abs
            : public virtual Iir {
        Iir* Incomplete_Type_Declaration;
        Iir* Type_Definition;
    };

    class Iir_Use_Flag_Abs
            : public virtual Iir {
        bool Use_Flag;
    };

    class Iir_Type_Declaration
            : public Iir_Chain_Abs,
              public Iir_Type_Definition_Abs,
              public Iir_Identifier_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Parent_Abs,
              public Iir_Use_Flag_Abs {
    };

    class Iir_Anonymous_Type_Declaration
            : public Iir_Chain_Abs,
              public Iir_Type_Definition_Abs,
              public Iir_Identifier_Abs,
              public Iir_Parent_Abs {
        Iir* Subtype_Definition;
    };

    class Iir_Subtype_Indication_Abs
            : public virtual Iir {
        Iir* Subtype_Indication;
    };

    class Iir_Subtype_Declaration
            : public Iir_Chain_Abs,
              public Iir_Type_Abs,
              public Iir_Subtype_Indication_Abs,
              public Iir_Identifier_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Parent_Abs,
              public Iir_Use_Flag_Abs {
    };

    class Iir_Nature_Abs
            : public virtual Iir {
        Iir* Nature;
    };

    class Iir_Nature_Declaration
            : public Iir_Chain_Abs,
              public Iir_Nature_Abs,
              public Iir_Identifier_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Parent_Abs,
              public Iir_Use_Flag_Abs {
    };

    class Iir_Subnature_Declaration
            : public Iir_Chain_Abs,
              public Iir_Nature_Abs,
              public Iir_Identifier_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Parent_Abs,
              public Iir_Use_Flag_Abs {
    };

    class Iir_Attribute_Value_Chain_Abs
            : public virtual Iir {
        Iir* Attribute_Value_Chain;
    };

    class Iir_Package_Body_Abs
            : public virtual Iir {
        Iir* Package_Body;
    };

    class Iir_Package_Declaration
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

    class Iir_Uninstantiated_Package_Name_Abs
            : public virtual Iir {
        Iir* Uninstantiated_Package_Name;
        Iir* Uninstantiated_Package_Decl;
    };

    class Iir_Package_Instantiation_Declaration
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

    class Iir_Package_Body
            : public Iir_Attribute_Value_Chain_Abs,
              public Iir_Chain_Abs,
              public Iir_Declaration_Chain_Abs,
              public Iir_Identifier_Abs,
              public Iir_Parent_Design_Unit_Abs,
              public Iir_End_Has_Reserved_Id_Abs,
              public Iir_End_Has_Identifier_Abs {
        Iir* Package;
    };

    class Iir_Configuration_Declaration
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

    class Iir_Concurrent_Statement_Chain_Abs
            : public virtual Iir {
        Iir* Concurrent_Statement_Chain;
    };

    class Iir_Is_Within_Flag_Abs
            : public virtual Iir {
        bool Is_Within_Flag;
    };

    class Iir_Has_Begin_Abs
            : public virtual Iir {
        bool Has_Begin;
    };

    class Iir_Entity_Declaration
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

    class Iir_Foreign_Flag_Abs
            : public virtual Iir {
        bool Foreign_Flag;
    };

    class Iir_Architecture_Body
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

    class Iir_Context_Declaration
            : public Iir_Context_Items_Abs,
              public Iir_Identifier_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Parent_Design_Unit_Abs,
              public Iir_End_Has_Reserved_Id_Abs,
              public Iir_End_Has_Identifier_Abs {
    };

    class Iir_Package_Header
            : public Iir_Generic_Chain_Abs,
              public Iir_Generic_Map_Aspect_Chain_Abs {
    };

    class Iir_Unit_Declaration
            : public Iir_Chain_Abs,
              public Iir_Type_Abs,
              public Iir_Identifier_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Parent_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs {
        Iir* Physical_Literal;
    };

    class Iir_Library_Declaration
            : public Iir_Date_Abs,
              public Iir_Chain_Abs,
              public Iir_Identifier_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Elab_Flag_Abs {
        std::vector<Iir_Design_File*> Design_Files;
        std::filesystem::path Library_Directory;
    };

    class Iir_Has_Is_Abs
            : public virtual Iir {
        bool Has_Is;
    };

    class Iir_Component_Declaration
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

    class Iir_Attribute_Declaration
            : public Iir_Chain_Abs,
              public Iir_Type_Abs,
              public Iir_Identifier_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Parent_Abs,
              public Iir_Type_Mark_Abs,
              public Iir_Use_Flag_Abs {
    };

    class Iir_Group_Template_Declaration
            : public Iir_Chain_Abs,
              public Iir_Identifier_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Parent_Abs,
              public Iir_Use_Flag_Abs {
        Iir* Entity_Class_Entry_Chain;
    };

    class Iir_Group_Declaration
            : public Iir_Chain_Abs,
              public Iir_Identifier_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Parent_Abs,
              public Iir_Use_Flag_Abs {
        std::vector<Iir*> Group_Constituent_List;
        Iir* Group_Template_Name;
    };

    class Iir_Element_Declaration
            : public Iir_Type_Abs,
              public Iir_Subtype_Indication_Abs,
              public Iir_Identifier_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Parent_Abs,
              public Iir_Has_Identifier_List_Abs {
    };

    class Iir_Name_Abs
            : public virtual Iir {
        Iir* Name;
    };

    class Iir_Non_Object_Alias_Declaration
            : public Iir_Chain_Abs,
              public Iir_Identifier_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Parent_Abs,
              public Iir_Name_Abs,
              public Iir_Use_Flag_Abs {
        Iir* Alias_Signature;
        bool Implicit_Alias_Flag;
    };

    class Iir_Psl_Declaration_Abs
            : public virtual Iir {
        PSL_Node Psl_Declaration;
    };

    class Iir_PSL_Clock_Abs
            : public virtual Iir {
        PSL_NFA PSL_Nfa;
        PSL_Node PSL_Clock;
    };

    class Iir_Psl_Declaration
            : public Iir_Chain_Abs,
              public Iir_Identifier_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Parent_Abs,
              public Iir_Use_Flag_Abs,
              public Iir_Psl_Declaration_Abs,
              public Iir_PSL_Clock_Abs {
    };

    class Iir_PSL_Nbr_States_Abs
            : public virtual Iir {
        bool PSL_EOS_Flag;
        int PSL_Nbr_States;
        std::vector<Iir*> PSL_Clock_Sensitivity;
    };

    class Iir_Psl_Endpoint_Declaration
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

    class Iir_Terminal_Declaration
            : public Iir_Chain_Abs,
              public Iir_Nature_Abs,
              public Iir_Identifier_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Parent_Abs,
              public Iir_Use_Flag_Abs {
    };

    class Iir_Default_Value_Abs
            : public virtual Iir {
        Iir* Default_Value;
    };

    class Iir_Free_Quantity_Declaration
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

    class Iir_Plus_Terminal_Abs
            : public virtual Iir {
        Iir* Minus_Terminal;
        Iir* Plus_Terminal;
    };

    class Iir_Across_Quantity_Declaration
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

    class Iir_Through_Quantity_Declaration
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

    class Iir_Subprogram_Hash_Abs
            : public virtual Iir {
        int Subprogram_Hash;
    };

    class Iir_Seen_Flag_Abs
            : public virtual Iir {
        bool Seen_Flag;
    };

    class Iir_Enumeration_Literal
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

    class Iir_Interface_Declaration_Chain_Abs
            : public virtual Iir {
        int Subprogram_Depth;
        Iir* Interface_Declaration_Chain;
        Iir_All_Sensitized All_Sensitized_State;
        bool Has_Parameter;
    };

    class Iir_Subprogram_Body_Abs
            : public virtual Iir {
        Iir_Predefined_Functions Implicit_Definition;
        Iir* Subprogram_Body;
        bool Has_Body;
        int Overload_Number;
        bool Hide_Implicit_Flag;
    };

    class Iir_Return_Type_Abs
            : public virtual Iir {
        bool Has_Pure;
        Iir* Return_Type;
        bool Pure_Flag;
        bool Resolution_Function_Flag;
    };

    class Iir_Wait_State_Abs
            : public virtual Iir {
        Tri_State_Type Wait_State;
    };

    class Iir_Function_Declaration
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

    class Iir_Passive_Flag_Abs
            : public virtual Iir {
        bool Passive_Flag;
    };

    class Iir_Suspend_Flag_Abs
            : public virtual Iir {
        bool Suspend_Flag;
    };

    class Iir_Procedure_Declaration
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

    class Iir_Subprogram_Specification_Abs
            : public virtual Iir {
        Iir* Subprogram_Specification;
        int Impure_Depth;
    };

    class Iir_Sequential_Statement_Chain_Abs
            : public virtual Iir {
        Iir* Sequential_Statement_Chain;
    };

    class Iir_Callees_List_Abs
            : public virtual Iir {
        std::vector<Iir*> Callees_List;
    };

    class Iir_Function_Body
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

    class Iir_Procedure_Body
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

    class Iir_After_Drivers_Flag_Abs
            : public virtual Iir {
        bool After_Drivers_Flag;
    };

    class Iir_Object_Alias_Declaration
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

    class Iir_Mode_Abs
            : public virtual Iir {
        std::optional<Iir_Mode> Mode;
    };

    class Iir_File_Declaration
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

    class Iir_Guarded_Signal_Flag_Abs
            : public virtual Iir {
        bool Guarded_Signal_Flag;
        Iir_Signal_Kind Signal_Kind;
    };

    class Iir_Has_Active_Flag_Abs
            : public virtual Iir {
        bool Has_Active_Flag;
    };

    class Iir_Guard_Signal_Declaration
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

    class Iir_Has_Disconnect_Flag_Abs
            : public virtual Iir {
        bool Has_Disconnect_Flag;
    };

    class Iir_Signal_Declaration
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

    class Iir_Shared_Flag_Abs
            : public virtual Iir {
        bool Shared_Flag;
    };

    class Iir_Variable_Declaration
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

    class Iir_Constant_Declaration
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

    class Iir_Iterator_Declaration
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

    class Iir_Has_Class_Abs
            : public virtual Iir {
        bool Has_Class;
    };

    class Iir_Interface_Signal_Declaration_Extras
            : public Iir_Guarded_Signal_Flag_Abs,
              public Iir_Has_Disconnect_Flag_Abs,
              public Iir_Has_Active_Flag_Abs {
        bool Open_Flag;
    };

    class Iir_Interface_Object_Declaration
            : public Iir_Identifier_Abs,
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
        enum class type {constant, variable, signal, file} Interface_Object_Type;
        std::optional<Iir_Interface_Signal_Declaration_Extras> extras;
    };

    class Iir_Interface_Type_Declaration
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

    class Iir_Interface_Package_Declaration
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

    class Iir_Interface_Function_Declaration
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

    class Iir_Interface_Procedure_Declaration
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

    class Iir_Signal_Attribute_Declaration
            : public Iir_Chain_Abs,
              public Iir_Parent_Abs {
        Iir* Signal_Attribute_Chain;
    };

    class Iir_Operand_Abs
            : public virtual Iir {
        Iir* Operand;
    };

    class Iir_Unary_Operator
            : public Iir_Type_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Operand_Abs,
              public Iir_Implementation_Abs {
        Iir_Unary_Operator_Type Operator_Type;
    };

    class Iir_Binary_Operator
            : public Iir_Type_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Implementation_Abs {
        Iir* Left;
        Iir* Right;
        Iir_Binary_Operator_Type Operator_Type;
    };

    class Iir_Function_Call
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs,
              public Iir_Implementation_Abs,
              public Iir_Parameter_Association_Chain_Abs {
    };

    class Iir_Aggregate
            : public Iir_Literal_Origin_Abs,
              public Iir_Literal_Subtype_Abs,
              public Iir_Type_Abs,
              public Iir_Expr_Staticness_Abs {
        Iir* Association_Choices_Chain;
        bool Aggregate_Expand_Flag;
        Iir* Aggregate_Info;
    };

    class Iir_Parenthesis_Expression
            : public Iir_Type_Abs,
              public Iir_Expression_Abs,
              public Iir_Expr_Staticness_Abs {
    };

    class Iir_Qualified_Expression
            : public Iir_Type_Abs,
              public Iir_Expression_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Type_Mark_Abs {
    };

    class Iir_Type_Conversion
            : public Iir_Type_Abs,
              public Iir_Expression_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Type_Mark_Abs {
        Iir* Type_Conversion_Subtype;
    };

    class Iir_Allocator_Designated_Type_Abs
            : public virtual Iir {
        Iir* Allocator_Designated_Type;
    };

    class Iir_Allocator_By_Expression
            : public Iir_Type_Abs,
              public Iir_Expression_Abs,
              public Iir_Allocator_Designated_Type_Abs,
              public Iir_Expr_Staticness_Abs {
    };

    class Iir_Allocator_By_Subtype
            : public Iir_Type_Abs,
              public Iir_Subtype_Indication_Abs,
              public Iir_Allocator_Designated_Type_Abs,
              public Iir_Expr_Staticness_Abs {
        Iir* Allocator_Subtype;
    };

    class Iir_Selected_Element
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs {
        Iir* Selected_Element;
    };

    class Iir_Dereference
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs {
    };

    class Iir_Implicit_Dereference
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs {
    };

    class Iir_Slice_Name
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs {
        Iir* Suffix;
        Iir* Slice_Subtype;
    };

    class Iir_Indexed_Name
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs {
        std::vector<Iir*> Index_List;
    };

    class Iir_Psl_Expression
            : public Iir_Type_Abs {
        PSL_Node Psl_Expression;
    };

    class Iir_Label_Abs
            : public virtual Iir {
        std::string Label;
    };

    class Iir_Sensitivity_List_Abs
            : public virtual Iir {
        std::vector<Iir*> Sensitivity_List;
    };

    class Iir_Process_Origin_Abs
            : public virtual Iir {
        bool End_Has_Postponed;
        Iir* Process_Origin;
    };

    class Iir_Postponed_Flag_Abs
            : public virtual Iir {
        bool Postponed_Flag;
    };

    class Iir_Has_Label_Abs
            : public virtual Iir {
        bool Has_Label;
    };

    class Iir_Sensitized_Process_Statement
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

    class Iir_Process_Statement
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

    class Iir_Guarded_Target_State_Abs
            : public virtual Iir {
        Iir_Delay_Mechanism Delay_Mechanism;
        Tri_State_Type Guarded_Target_State;
        Iir* Reject_Time_Expression;
    };

    class Iir_Target_Abs
            : public virtual Iir {
        Iir* Target;
    };

    class Iir_Guard_Abs
            : public virtual Iir {
        Iir* Guard;
    };

    class Iir_Simple_Signal_Assignment_Statement
            : public Iir_Guarded_Target_State_Abs,
              public Iir_Chain_Abs,
              public Iir_Label_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Target_Abs,
              public Iir_Parent_Abs {
        Iir_Waveform waveform;
    };

    class Iir_Conditional_Signal_Assignment_Statement
            : public Iir_Guarded_Target_State_Abs,
              public Iir_Chain_Abs,
              public Iir_Label_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Target_Abs,
              public Iir_Parent_Abs {
        Iir_Conditional_Waveforms conditional_waveforms;
    };

    class Iir_Selected_Waveform_Assignment_Statement
            : public Iir_Guarded_Target_State_Abs,
              public Iir_Chain_Abs,
              public Iir_Label_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Target_Abs,
              public Iir_Expression_Abs,
              public Iir_Selected_Waveform_Chain_Abs,
              public Iir_Parent_Abs {
    };

    class Iir_Concurrent_Simple_Signal_Assignment
            : public Iir_Simple_Signal_Assignment_Statement,
              public Iir_Guard_Abs,
              public Iir_Postponed_Flag_Abs {
    };

    class Iir_Concurrent_Conditional_Signal_Assignment
            : public Iir_Conditional_Signal_Assignment_Statement,
              public Iir_Guard_Abs,
              public Iir_Postponed_Flag_Abs {
        Iir_Conditional_Waveforms conditional_waveforms;
    };

    class Iir_Selected_Waveform_Chain_Abs
            : public virtual Iir {
        Iir* Selected_Waveform_Chain;
    };

    class Iir_Concurrent_Selected_Signal_Assignment
            : public Iir_Selected_Waveform_Assignment_Statement,
              public Iir_Guard_Abs,
              public Iir_Postponed_Flag_Abs {
    };

    class Iir_Assertion_Condition_Abs
            : public virtual Iir {
        Iir* Assertion_Condition;
    };

    class Iir_Report_Expression_Abs
            : public virtual Iir {
        Iir* Report_Expression;
        Iir* Severity_Expression;
    };

    class Iir_Concurrent_Assertion_Statement
            : public Iir_Assertion_Statement,
              public Iir_Postponed_Flag_Abs {
    };

    class Iir_Procedure_Call_Abs
            : public virtual Iir {
        Iir* Procedure_Call;
    };

    class Iir_Concurrent_Procedure_Call_Statement
            : public Iir_Procedure_Call_Statement,
              public Iir_Postponed_Flag_Abs {
    };

    class Iir_Psl_Assert_Statement
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

    class Iir_Psl_Cover_Statement
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

    class Iir_Block_Statement
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

    class Iir_Generate_Statement_Body_Abs
            : public virtual Iir {
        Iir* Generate_Statement_Body;
    };

    class Iir_Generate_Else_Clause_Abs
            : public virtual Iir {
        Iir* Generate_Else_Clause;
    };

    class Iir_If_Generate_Statement
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

    class Iir_Case_Statement_Alternative_Chain_Abs
            : public virtual Iir {
        std::vector<Iir*> Case_Statement_Alternative;
    };

    class Iir_Case_Generate_Statement
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

    class Iir_Parameter_Specification_Abs
            : public virtual Iir {
        Iir* Parameter_Specification;
    };

    class Iir_For_Generate_Statement
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

    class Iir_Component_Instantiation_Statement
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

    class Iir_Psl_Default_Clock
            : public Iir_Chain_Abs,
              public Iir_Label_Abs,
              public Iir_Parent_Abs {
        PSL_Node Psl_Boolean;
    };

    class Iir_Simple_Simultaneous_Statement
            : public Iir_Chain_Abs,
              public Iir_Label_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Tolerance_Abs,
              public Iir_Parent_Abs {
        Iir* Simultaneous_Left;
        Iir* Simultaneous_Right;
    };

    class Iir_Generate_Statement_Body
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

    class Iir_If_Generate_Else_Clause
            : public Iir_Visible_Flag_Abs,
              public Iir_Generate_Statement_Body_Abs,
              public Iir_Generate_Else_Clause_Abs,
              public Iir_Condition_Abs,
              public Iir_Parent_Abs {
    };

    class Iir_Null_Statement
            : public Iir_Chain_Abs,
              public Iir_Label_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Parent_Abs {
    };

    class Iir_Assertion_Statement
            : public Iir_Chain_Abs,
              public Iir_Label_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Assertion_Condition_Abs,
              public Iir_Report_Expression_Abs,
              public Iir_Parent_Abs {
    };

    class Iir_Report_Statement
            : public Iir_Chain_Abs,
              public Iir_Label_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Report_Expression_Abs,
              public Iir_Parent_Abs {
    };

    class Iir_Wait_Statement
            : public Iir_Chain_Abs,
              public Iir_Label_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Sensitivity_List_Abs,
              public Iir_Parent_Abs,
              public Iir_Is_Ref_Abs {
        Iir* Condition_Clause;
        Iir* Timeout_Clause;
    };

    class Iir_Variable_Assignment_Statement
            : public Iir_Chain_Abs,
              public Iir_Label_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Target_Abs,
              public Iir_Expression_Abs,
              public Iir_Parent_Abs {
    };

    class Iir_Conditional_Variable_Assignment_Statement
            : public Iir_Chain_Abs,
              public Iir_Label_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Target_Abs,
              public Iir_Parent_Abs {
        Iir* Conditional_Expression;
    };

    class Iir_Return_Statement
            : public Iir_Chain_Abs,
              public Iir_Type_Abs,
              public Iir_Label_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Expression_Abs,
              public Iir_Parent_Abs {
    };

    class Iir_For_Loop_Statement
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

    class Iir_While_Loop_Statement
            : public Iir_Chain_Abs,
              public Iir_Sequential_Statement_Chain_Abs,
              public Iir_Label_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Condition_Abs,
              public Iir_Parent_Abs,
              public Iir_End_Has_Identifier_Abs,
              public Iir_Suspend_Flag_Abs {
    };

    class Iir_Loop_Label_Abs
            : public virtual Iir {
        Iir* Loop_Label;
    };

    class Iir_Next_Statement
            : public Iir_Chain_Abs,
              public Iir_Label_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Condition_Abs,
              public Iir_Parent_Abs,
              public Iir_Loop_Label_Abs {
    };

    class Iir_Exit_Statement
            : public Iir_Chain_Abs,
              public Iir_Label_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Condition_Abs,
              public Iir_Parent_Abs,
              public Iir_Loop_Label_Abs {
    };

    class Iir_Case_Statement
            : public Iir_Chain_Abs,
              public Iir_Label_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Expression_Abs,
              public Iir_Parent_Abs,
              public Iir_Case_Statement_Alternative_Chain_Abs,
              public Iir_End_Has_Identifier_Abs,
              public Iir_Suspend_Flag_Abs {
    };

    class Iir_Procedure_Call_Statement
            : public Iir_Chain_Abs,
              public Iir_Label_Abs,
              public Iir_Visible_Flag_Abs,
              public Iir_Parent_Abs,
              public Iir_Procedure_Call_Abs,
              public Iir_Suspend_Flag_Abs {
    };

    class Iir_Else_Clause_Abs
            : public virtual Iir {
        Iir* Else_Clause;
    };

    class Iir_If_Statement
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

    class Iir_Elsif
            : public Iir_Sequential_Statement_Chain_Abs,
              public Iir_Condition_Abs,
              public Iir_Else_Clause_Abs,
              public Iir_Parent_Abs,
              public Iir_End_Has_Identifier_Abs {
    };

    class Iir_Named_Entity_Abs
            : public virtual Iir {
        Iir* Named_Entity;
        bool Is_Forward_Ref;
    };

    class Iir_Alias_Declaration_Abs
            : public virtual Iir {
        Iir* Alias_Declaration;
    };

    class Iir_Character_Literal
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Identifier_Abs,
              public Iir_Named_Entity_Abs,
              public Iir_Alias_Declaration_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs {
    };

    class Iir_Simple_Name
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Identifier_Abs,
              public Iir_Named_Entity_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs {
    };

    class Iir_Selected_Name
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Identifier_Abs,
              public Iir_Named_Entity_Abs,
              public Iir_Alias_Declaration_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs {
    };

    class Iir_Operator_Symbol
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Identifier_Abs,
              public Iir_Named_Entity_Abs,
              public Iir_Alias_Declaration_Abs {
    };

    class Iir_Reference_Name
            : public Iir_Named_Entity_Abs {
        Iir* Referenced_Name;
    };

    class Iir_Selected_By_All_Name
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Named_Entity_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Prefix_Abs {
    };

    class Iir_Parenthesis_Name
            : public Iir_Type_Abs,
              public Iir_Named_Entity_Abs,
              public Iir_Prefix_Abs {
        Iir* Association_Chain;
    };

    class Iir_External_Pathname_Abs
            : public virtual Iir {
        Iir* External_Pathname;
    };

    class Iir_External_Name
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

    class Iir_Pathname_Suffix_Abs
            : public virtual Iir {
        Iir* Pathname_Suffix;
    };

    class Iir_Package_Pathname
            : public Iir_Identifier_Abs,
              public Iir_Named_Entity_Abs,
              public Iir_Pathname_Suffix_Abs {
    };

    class Iir_Absolute_Pathname
            : public Iir_Pathname_Suffix_Abs {
    };

    class Iir_Relative_Pathname
            : public Iir_Pathname_Suffix_Abs {
    };

    class Iir_Pathname_Element
            : public Iir_Identifier_Abs,
              public Iir_Named_Entity_Abs,
              public Iir_Pathname_Suffix_Abs {
        Iir* Pathname_Expression;
    };

    class Iir_Base_Attribute
            : public Iir_Type_Abs,
              public Iir_Prefix_Abs {
    };

    class Iir_Subtype_Attribute
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Type_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs {
    };

    class Iir_Element_Attribute
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Type_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs {
    };

    class Iir_Left_Type_Attribute
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs {
    };

    class Iir_Right_Type_Attribute
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs {
    };

    class Iir_High_Type_Attribute
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs {
    };

    class Iir_Low_Type_Attribute
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs {
    };

    class Iir_Ascending_Type_Attribute
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs {
    };

    class Iir_Parameter_Abs
            : public virtual Iir {
        Iir* Parameter;
    };

    class Iir_Image_Attribute
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs,
              public Iir_Parameter_Abs {
    };

    class Iir_Value_Attribute
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs,
              public Iir_Parameter_Abs {
    };

    class Iir_Pos_Attribute
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs,
              public Iir_Parameter_Abs {
    };

    class Iir_Val_Attribute
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs,
              public Iir_Parameter_Abs {
    };

    class Iir_Succ_Attribute
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs,
              public Iir_Parameter_Abs {
    };

    class Iir_Pred_Attribute
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs,
              public Iir_Parameter_Abs {
    };

    class Iir_Leftof_Attribute
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs,
              public Iir_Parameter_Abs {
    };

    class Iir_Rightof_Attribute
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs,
              public Iir_Parameter_Abs {
    };

    class Iir_Attr_Chain_Abs
            : public virtual Iir {
        Iir* Signal_Attribute_Declaration;
        Iir* Attr_Chain;
    };

    class Iir_Delayed_Attribute
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs,
              public Iir_Parameter_Abs,
              public Iir_Attr_Chain_Abs,
              public Iir_Has_Active_Flag_Abs {
    };

    class Iir_Stable_Attribute
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs,
              public Iir_Parameter_Abs,
              public Iir_Attr_Chain_Abs,
              public Iir_Has_Active_Flag_Abs {
    };

    class Iir_Quiet_Attribute
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs,
              public Iir_Parameter_Abs,
              public Iir_Attr_Chain_Abs,
              public Iir_Has_Active_Flag_Abs {
    };

    class Iir_Transaction_Attribute
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs,
              public Iir_Parameter_Abs,
              public Iir_Attr_Chain_Abs,
              public Iir_Has_Active_Flag_Abs {
    };

    class Iir_Attribute
            : public Iir_Type_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs {
        Iir_Attribute_Type Attribute_Type;
    };

    class Iir_Behavior_Attribute
            : public virtual Iir {
    };

    class Iir_Structure_Attribute
            : public virtual Iir {
    };

    class Iir_Simple_Name_Attribute
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs {
        std::string Simple_Name_Identifier;
        Iir* Simple_Name_Subtype;
    };

    class Iir_Instance_Name_Attribute
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs {
    };

    class Iir_Path_Name_Attribute
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs {
    };

    class Iir_Index_Subtype_Abs
            : public virtual Iir {
        Iir* Index_Subtype;
    };

    class Iir_Array_Attribute
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs,
              public Iir_Index_Subtype_Abs,
              public Iir_Parameter_Abs {
        Array_Attribute_Type Attribute_Type;
    };

    class Iir_Attribute_Name
            : public Iir_Type_Abs,
              public Iir_Base_Name_Abs,
              public Iir_Identifier_Abs,
              public Iir_Named_Entity_Abs,
              public Iir_Expr_Staticness_Abs,
              public Iir_Name_Staticness_Abs,
              public Iir_Prefix_Abs {
        Iir* Attribute_Signature;
    };

}
#endif // AVLC_IIR_H
