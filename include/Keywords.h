#ifndef AVLC_KEYWORDS_H
#define AVLC_KEYWORDS_H

#include <string>
#include <unordered_map>

// Important names
namespace Keywords {
const std::string Name_Mod = "mod";
const std::string Name_Rem = "rem";

const std::string Name_And = "and";
const std::string Name_Or = "or";
const std::string Name_Xor = "xor";
const std::string Name_Nand = "nand";
const std::string Name_Nor = "nor";

const std::string Name_Abs = "abs";
const std::string Name_Not = "not";

const std::string Name_Access = "access";
const std::string Name_After = "after";
const std::string Name_Alias = "alias";
const std::string Name_All = "all";
const std::string Name_Architecture = "architecture";
const std::string Name_Array = "array";
const std::string Name_Assert = "assert";
const std::string Name_Attribute = "attribute";

const std::string Name_Begin = "begin";
const std::string Name_Block = "block";
const std::string Name_Body = "body";
const std::string Name_Buffer = "buffer";
const std::string Name_Bus = "bus";

const std::string Name_Case = "case";
const std::string Name_Component = "component";
const std::string Name_Configuration = "configuration";
const std::string Name_Constant = "constant";

const std::string Name_Disconnect = "disconnect";
const std::string Name_Downto = "downto";

const std::string Name_Else = "else";
const std::string Name_Elsif = "elsif";
const std::string Name_End = "end";
const std::string Name_Entity = "entity";
const std::string Name_Exit = "exit";

const std::string Name_File = "file";
const std::string Name_For = "for";
const std::string Name_Function = "function";

const std::string Name_Generate = "generate";
const std::string Name_Generic = "generic";
const std::string Name_Guarded = "guarded";

const std::string Name_If = "if";
const std::string Name_In = "in";
const std::string Name_Inout = "inout";
const std::string Name_Is = "is";

const std::string Name_Label = "label";
const std::string Name_Library = "library";
const std::string Name_Linkage = "linkage";
const std::string Name_Loop = "loop";

const std::string Name_Map = "map";

const std::string Name_New = "new";
const std::string Name_Next = "next";
const std::string Name_Null = "null";

const std::string Name_Of = "of";
const std::string Name_On = "on";
const std::string Name_Open = "open";
const std::string Name_Others = "others";
const std::string Name_Out = "out";

const std::string Name_Package = "package";
const std::string Name_Port = "port";
const std::string Name_Procedure = "procedure";
const std::string Name_Process = "process";

const std::string Name_Range = "range";
const std::string Name_Record = "record";
const std::string Name_Register = "register";
const std::string Name_Report = "report";
const std::string Name_Return = "return";

const std::string Name_Select = "select";
const std::string Name_Severity = "severity";
const std::string Name_Signal = "signal";
const std::string Name_Subtype = "subtype";

const std::string Name_Then = "then";
const std::string Name_To = "to";
const std::string Name_Transport = "transport";
const std::string Name_Type = "type";

const std::string Name_Units = "units";
const std::string Name_Until = "until";
const std::string Name_Use = "use";

const std::string Name_Variable = "variable";

const std::string Name_Wait = "wait";
const std::string Name_When = "when";
const std::string Name_While = "while";
const std::string Name_With = "with";

// VHDL93 reserved words.
const std::string Name_Xnor = "xnor";
const std::string Name_Group = "group";
const std::string Name_Impure = "impure";
const std::string Name_Inertial = "inertial";
const std::string Name_Literal = "literal";
const std::string Name_Postponed = "postponed";
const std::string Name_Pure = "pure";
const std::string Name_Reject = "reject";
const std::string Name_Shared = "shared";
const std::string Name_Unaffected = "unaffected";

const std::string Name_Sll = "sll";
const std::string Name_Sla = "sla";
const std::string Name_Sra = "sra";
const std::string Name_Srl = "srl";
const std::string Name_Rol = "rol";
const std::string Name_Ror = "ror";

// VHDL00 keywords
const std::string Name_Protected = "protected";

// VHDL08 keywords
const std::string Name_Context = "context";
const std::string Name_Parameter = "parameter";

const std::string Name_Across = "across";
const std::string Name_Break = "break";
const std::string Name_Limit = "limit";
const std::string Name_Nature = "nature";
const std::string Name_Noise = "noise";
const std::string Name_Procedural = "procedural";
const std::string Name_Quantity = "quantity";
const std::string Name_Reference = "reference";
const std::string Name_Spectrum = "spectrum";
const std::string Name_Subnature = "subnature";
const std::string Name_Terminal = "terminal";
const std::string Name_Through = "through";
const std::string Name_Tolerance = "tolerance";

// Create operators.
const std::string Name_Op_Equality = "=";
const std::string Name_Op_Inequality = "/=";
const std::string Name_Op_Less = "<";
const std::string Name_Op_Less_Equal = "<=";
const std::string Name_Op_Greater = ">";
const std::string Name_Op_Greater_Equal = ">=";
const std::string Name_Op_Plus = "+";
const std::string Name_Op_Minus = "-";
const std::string Name_Op_Mul = "*";
const std::string Name_Op_Div = "/";
const std::string Name_Op_Exp = "**";
const std::string Name_Op_Concatenation = "&";
const std::string Name_Op_Condition = "??";
const std::string Name_Op_Match_Equality = "?=";
const std::string Name_Op_Match_Inequality = "?/=";
const std::string Name_Op_Match_Less = "?<";
const std::string Name_Op_Match_Less_Equal = "?<=";
const std::string Name_Op_Match_Greater = "?>";
const std::string Name_Op_Match_Greater_Equal = "?>=";

// Create Attributes.
const std::string Name_Base = "base";
const std::string Name_Left = "left";
const std::string Name_Right = "right";
const std::string Name_High = "high";
const std::string Name_Low = "low";
const std::string Name_Pos = "pos";
const std::string Name_Val = "val";
const std::string Name_Succ = "succ";
const std::string Name_Pred = "pred";
const std::string Name_Leftof = "leftof";
const std::string Name_Rightof = "rightof";
const std::string Name_Reverse_Range = "reverse_range";
const std::string Name_Length = "length";
const std::string Name_Delayed = "delayed";
const std::string Name_Stable = "stable";
const std::string Name_Quiet = "quiet";
const std::string Name_Transaction = "transaction";
const std::string Name_Event = "event";
const std::string Name_Active = "active";
const std::string Name_Last_Event = "last_event";
const std::string Name_Last_Active = "last_active";
const std::string Name_Last_Value = "last_value";

const std::string Name_Behavior = "behavior";
const std::string Name_Structure = "structure";

const std::string Name_Ascending = "ascending";
const std::string Name_Image = "image";
const std::string Name_Value = "value";
const std::string Name_Driving = "driving";
const std::string Name_Driving_Value = "driving_value";
const std::string Name_Simple_Name = "simple_name";
const std::string Name_Instance_Name = "instance_name";
const std::string Name_Path_Name = "path_name";

const std::string Name_Element = "element";

const std::string Name_Contribution = "contribution";
const std::string Name_Dot = "dot";
const std::string Name_Integ = "integ";
const std::string Name_Above = "above";
const std::string Name_ZOH = "zoh";
const std::string Name_LTF = "ltf";
const std::string Name_ZTF = "ztf";
const std::string Name_Ramp = "ramp";
const std::string Name_Slew = "slew";

//  Create standard.
const std::string Name_Std = "std";
const std::string Name_Standard = "standard";
const std::string Name_Boolean = "boolean";
const std::string Name_False = "false";
const std::string Name_True = "true";
const std::string Name_Bit = "bit";
const std::string Name_Character = "character";
const std::string Name_Severity_Level = "severity_level";
const std::string Name_Note = "note";
const std::string Name_Warning = "warning";
const std::string Name_Error = "error";
const std::string Name_Failure = "failure";
const std::string Name_Universal_Integer = "UNIVERSAL_INTEGER";
const std::string Name_Universal_Real = "UNIVERSAL_REAL";
const std::string Name_Convertible_Integer = "CONVERTIBLE_INTEGER";
const std::string Name_Convertible_Real = "CONVERTIBLE_REAL";
const std::string Name_Integer = "integer";
const std::string Name_Real = "real";
const std::string Name_Time = "time";
const std::string Name_Fs = "fs";
const std::string Name_Ps = "ps";
const std::string Name_Ns = "ns";
const std::string Name_Us = "us";
const std::string Name_Ms = "ms";
const std::string Name_Sec = "sec";
const std::string Name_Min = "min";
const std::string Name_Hr = "hr";
const std::string Name_Delay_Length = "delay_length";
const std::string Name_Now = "now";
const std::string Name_Natural = "natural";
const std::string Name_Positive = "positive";
const std::string Name_String = "string";
const std::string Name_Bit_Vector = "bit_vector";
const std::string Name_File_Open_Kind = "file_open_kind";
const std::string Name_Read_Mode = "read_mode";
const std::string Name_Write_Mode = "write_mode";
const std::string Name_Append_Mode = "append_mode";
const std::string Name_File_Open_Status = "file_open_status";
const std::string Name_Open_Ok = "open_ok";
const std::string Name_Status_Error = "status_error";
const std::string Name_Name_Error = "name_error";
const std::string Name_Mode_Error = "mode_error";
const std::string Name_Foreign = "foreign";

const std::string Name_Boolean_Vector = "boolean_vector";
const std::string Name_To_Bstring = "to_bstring";
const std::string Name_To_Binary_String = "to_binary_string";
const std::string Name_To_Ostring = "to_ostring";
const std::string Name_To_Octal_String = "to_octal_string";
const std::string Name_To_Hstring = "to_hstring";
const std::string Name_To_Hex_String = "to_hex_string";
const std::string Name_Integer_Vector = "integer_vector";
const std::string Name_Real_Vector = "real_vector";
const std::string Name_Time_Vector = "time_vector";
const std::string Name_Digits = "digits";
const std::string Name_Format = "format";
const std::string Name_Unit = "unit";

const std::string Name_Domain_Type = "domain_type";
const std::string Name_Quiescent_Domain = "quiescent_domain";
const std::string Name_Time_Domain = "time_domain";
const std::string Name_Frequency_Domain = "frequency_domain";
const std::string Name_Domain = "domain";
const std::string Name_Frequency = "frequency";
//const std::string Name_Real_Vector = "real_vector";

const std::string Name_Nul = "nul";
const std::string Name_Soh = "soh";
const std::string Name_Stx = "stx";
const std::string Name_Etx = "etx";
const std::string Name_Eot = "eot";
const std::string Name_Enq = "enq";
const std::string Name_Ack = "ack";
const std::string Name_Bel = "bel";
const std::string Name_Bs = "bs";
const std::string Name_Ht = "ht";
const std::string Name_Lf = "lf";
const std::string Name_Vt = "vt";
const std::string Name_Ff = "ff";
const std::string Name_Cr = "cr";
const std::string Name_So = "so";
const std::string Name_Si = "si";
const std::string Name_Dle = "dle";
const std::string Name_Dc1 = "dc1";
const std::string Name_Dc2 = "dc2";
const std::string Name_Dc3 = "dc3";
const std::string Name_Dc4 = "dc4";
const std::string Name_Nak = "nak";
const std::string Name_Syn = "syn";
const std::string Name_Etb = "etb";
const std::string Name_Can = "can";
const std::string Name_Em = "em";
const std::string Name_Sub = "sub";
const std::string Name_Esc = "esc";
const std::string Name_Fsp = "fsp";
const std::string Name_Gsp = "gsp";
const std::string Name_Rsp = "rsp";
const std::string Name_Usp = "usp";
const std::string Name_Del = "del";

const std::string Name_C128 = "c128";
const std::string Name_C129 = "c129";
const std::string Name_C130 = "c130";
const std::string Name_C131 = "c131";
const std::string Name_C132 = "c132";
const std::string Name_C133 = "c133";
const std::string Name_C134 = "c134";
const std::string Name_C135 = "c135";
const std::string Name_C136 = "c136";
const std::string Name_C137 = "c137";
const std::string Name_C138 = "c138";
const std::string Name_C139 = "c139";
const std::string Name_C140 = "c140";
const std::string Name_C141 = "c141";
const std::string Name_C142 = "c142";
const std::string Name_C143 = "c143";
const std::string Name_C144 = "c144";
const std::string Name_C145 = "c145";
const std::string Name_C146 = "c146";
const std::string Name_C147 = "c147";
const std::string Name_C148 = "c148";
const std::string Name_C149 = "c149";
const std::string Name_C150 = "c150";
const std::string Name_C151 = "c151";
const std::string Name_C152 = "c152";
const std::string Name_C153 = "c153";
const std::string Name_C154 = "c154";
const std::string Name_C155 = "c155";
const std::string Name_C156 = "c156";
const std::string Name_C157 = "c157";
const std::string Name_C158 = "c158";
const std::string Name_C159 = "c159";

// Create misc.
const std::string Name_Guard = "guard";
const std::string Name_Deallocate = "deallocate";
const std::string Name_File_Open = "file_open";
const std::string Name_File_Close = "file_close";
const std::string Name_Read = "read";
const std::string Name_Write = "write";
const std::string Name_Flush = "flush";
const std::string Name_Endfile = "endfile";
const std::string Name_P = "p";
const std::string Name_F = "f";
const std::string Name_L = "l";
const std::string Name_R = "r";
const std::string Name_S = "s";
const std::string Name_External_Name = "external_name";
const std::string Name_Open_Kind = "open_kind";
const std::string Name_Status = "status";
const std::string Name_First = "first";
const std::string Name_Last = "last";
const std::string Name_Textio = "textio";
const std::string Name_Work = "work";
const std::string Name_Text = "text";
const std::string Name_To_String = "to_string";
const std::string Name_Minimum = "minimum";
const std::string Name_Maximum = "maximum";
const std::string Name_Untruncated_Text_Read = "untruncated_text_read";
const std::string Name_Get_Resolution_Limit = "get_resolution_limit";
const std::string Name_Control_Simulation = "control_simulation";

const std::string Name_Ieee = "ieee";
const std::string Name_Std_Logic_1164 = "std_logic_1164";
const std::string Name_Std_Ulogic = "std_ulogic";
const std::string Name_Std_Ulogic_Vector = "std_ulogic_vector";
const std::string Name_Std_Logic = "std_logic";
const std::string Name_Std_Logic_Vector = "std_logic_vector";
const std::string Name_Rising_Edge = "rising_edge";
const std::string Name_Falling_Edge = "falling_edge";
const std::string Name_VITAL_Timing = "vital_timing";
const std::string Name_VITAL_Level0 = "vital_level0";
const std::string Name_VITAL_Level1 = "vital_level1";
const std::string Name_Numeric_Std = "numeric_std";
const std::string Name_Numeric_Bit = "numeric_bit";
const std::string Name_Unsigned = "unsigned";
const std::string Name_Signed = "signed";
const std::string Name_Unresolved_Unsigned = "unresolved_unsigned";
const std::string Name_Unresolved_Signed = "unresolved_signed";
const std::string Name_Std_Logic_Arith = "std_logic_arith";
const std::string Name_Std_Logic_Signed = "std_logic_signed";
const std::string Name_Std_Logic_Textio = "std_logic_textio";
const std::string Name_Std_Logic_Unsigned = "std_logic_unsigned";

//  Verilog keywords
const std::string Name_Always = "always";
const std::string Name_Assign = "assign";
const std::string Name_Buf = "buf";
const std::string Name_Bufif0 = "bufif0";
const std::string Name_Bufif1 = "bufif1";
const std::string Name_Casex = "casex";
const std::string Name_Casez = "casez";
const std::string Name_Cmos = "cmos";
const std::string Name_Deassign = "deassign";
const std::string Name_Default = "default";
const std::string Name_Defparam = "defparam";
const std::string Name_Disable = "disable";
const std::string Name_Endcase = "endcase";
const std::string Name_Endfunction = "endfunction";
const std::string Name_Endmodule = "endmodule";
const std::string Name_Endprimitive = "endprimitive";
const std::string Name_Endspecify = "endspecify";
const std::string Name_Endtable = "endtable";
const std::string Name_Endtask = "endtask";
const std::string Name_Forever = "forever";
const std::string Name_Fork = "fork";
const std::string Name_Highz0 = "highz0";
const std::string Name_Highz1 = "highz1";
const std::string Name_Initial = "initial";
const std::string Name_Input = "input";
const std::string Name_Join = "join";
const std::string Name_Large = "large";
const std::string Name_Medium = "medium";
const std::string Name_Module = "module";
const std::string Name_Negedge = "negedge";
const std::string Name_Nmos = "nmos";
const std::string Name_Notif0 = "notif0";
const std::string Name_Notif1 = "notif1";
const std::string Name_Output = "output";
const std::string Name_Pmos = "pmos";
const std::string Name_Posedge = "posedge";
const std::string Name_Primitive = "primitive";
const std::string Name_Pull0 = "pull0";
const std::string Name_Pull1 = "pull1";
const std::string Name_Pulldown = "pulldown";
const std::string Name_Pullup = "pullup";
const std::string Name_Reg = "reg";
const std::string Name_Repeat = "repeat";
const std::string Name_Rcmos = "rcmos";
const std::string Name_Rnmos = "rnmos";
const std::string Name_Rpmos = "rpmos";
const std::string Name_Rtran = "rtran";
const std::string Name_Rtranif0 = "rtranif0";
const std::string Name_Rtranif1 = "rtranif1";
const std::string Name_Small = "small";
const std::string Name_Specify = "specify";
const std::string Name_Specparam = "specparam";
const std::string Name_Strong0 = "strong0";
const std::string Name_Strong1 = "strong1";
const std::string Name_Supply0 = "supply0";
const std::string Name_Supply1 = "supply1";
const std::string Name_Tablex = "table";
const std::string Name_Task = "task";
const std::string Name_Tran = "tran";
const std::string Name_Tranif0 = "tranif0";
const std::string Name_Tranif1 = "tranif1";
const std::string Name_Tri = "tri";
const std::string Name_Tri0 = "tri0";
const std::string Name_Tri1 = "tri1";
const std::string Name_Trireg = "trireg";
const std::string Name_Wand = "wand";
const std::string Name_Weak0 = "weak0";
const std::string Name_Weak1 = "weak1";
const std::string Name_Wire = "wire";
const std::string Name_Wor = "wor";

const std::string Name_Define = "define";
const std::string Name_Endif = "endif";
const std::string Name_Ifdef = "ifdef";
const std::string Name_Include = "include";
const std::string Name_Timescale = "timescale";
const std::string Name_Undef = "undef";

//  Verilog system tasks
const std::string Name_Display = "display";
const std::string Name_Finish = "finish";

//  BSV keywords
const std::string Name_uAction = "Action";
const std::string Name_uActionValue = "ActionValue";
const std::string Name_BVI = "BVI";
const std::string Name_uC = "C";
const std::string Name_uCF = "CF";
const std::string Name_uE = "E";
const std::string Name_uSB = "SB";
const std::string Name_uSBR = "SBR";
const std::string Name_Action = "action";
const std::string Name_Endaction = "endaction";
const std::string Name_Actionvalue = "actionvalue";
const std::string Name_Endactionvalue = "endactionvalue";
const std::string Name_Ancestor = "ancestor";
const std::string Name_Clocked_By = "clocked_by";
const std::string Name_Continue = "continue";
const std::string Name_Default_Clock = "default_clock";
const std::string Name_Default_Reset = "default_reset";
const std::string Name_Dependencies = "dependencies";
const std::string Name_Deriving = "deriving";
const std::string Name_Determines = "determines";
const std::string Name_Enable = "enable";
const std::string Name_Enum = "enum";
const std::string Name_Export = "export";
const std::string Name_Ifc_Inout = "ifc_inout";
const std::string Name_Import = "import";
const std::string Name_Input_Clock = "input_clock";
const std::string Name_Input_Reset = "input_reset";
const std::string Name_Instance = "instance";
const std::string Name_Endinstance = "endinstance";
const std::string Name_Interface = "interface";
const std::string Name_Endinterface = "endinterface";
const std::string Name_Let = "let";
const std::string Name_Match = "match";
const std::string Name_Matches = "matches";
const std::string Name_Method = "method";
const std::string Name_Endmethod = "endmethod";
const std::string Name_Numeric = "numeric";
const std::string Name_Output_Clock = "output_clock";
const std::string Name_Output_Reset = "output_reset";
const std::string Name_Endpackage = "endpackage";
const std::string Name_Par = "par";
const std::string Name_Endpar = "endpar";
const std::string Name_Path = "path";
const std::string Name_Provisos = "provisos";
const std::string Name_Ready = "ready";
const std::string Name_Reset_By = "reset_by";
const std::string Name_Rule = "rule";
const std::string Name_Endrule = "endrule";
const std::string Name_Rules = "rules";
const std::string Name_Endrules = "endrules";
const std::string Name_Same_Family = "same_family";
const std::string Name_Schedule = "schedule";
const std::string Name_Seq = "seq";
const std::string Name_Endseq = "endseq";
const std::string Name_Struct = "struct";
const std::string Name_Tagged = "tagged";
const std::string Name_Typeclass = "typeclass";
const std::string Name_Endtypeclass = "endtypeclass";
const std::string Name_Typedef = "typedef";
const std::string Name_Union = "union";
const std::string Name_Valueof = "valueof";
const std::string Name_uValueof = "valueOf";
const std::string Name_Void = "void";

//  VHDL special comments
const std::string Name_Psl = "psl";
const std::string Name_Pragma = "pragma";

//  PSL keywords
const std::string Name_A = "a";
const std::string Name_Af = "af";
const std::string Name_Ag = "ag";
const std::string Name_Ax = "ax";
const std::string Name_Abort = "abort";
const std::string Name_Assume = "assume";
const std::string Name_Assume_Guarantee = "assume_guarantee";
const std::string Name_Before = "before";
const std::string Name_Clock = "clock";
const std::string Name_Const = "const";
const std::string Name_Cover = "cover";
const std::string Name_E = "e";
const std::string Name_Ef = "ef";
const std::string Name_Eg = "eg";
const std::string Name_Ex = "ex";
const std::string Name_Endpoint = "endpoint";
const std::string Name_Eventually = "eventually";
const std::string Name_Fairness = "fairness";
const std::string Name_Fell = "fell ";
const std::string Name_Forall = "forall";
const std::string Name_G = "g";
const std::string Name_Inf = "inf";
const std::string Name_Inherit = "inherit";
const std::string Name_Never = "never";
const std::string Name_Next_A = "next_a";
const std::string Name_Next_E = "next_e";
const std::string Name_Next_Event = "next_event";
const std::string Name_Next_Event_A = "next_event_a";
const std::string Name_Next_Event_E = "next_event_e";
const std::string Name_Property = "property";
const std::string Name_Prev = "prev";
const std::string Name_Restrict = "restrict";
const std::string Name_Restrict_Guarantee = "restrict_guarantee";
const std::string Name_Rose = "rose";
const std::string Name_Sequence = "sequence";
const std::string Name_Strong = "strong";
//const std::string Name_Union = "union";
const std::string Name_Vmode = "vmode";
const std::string Name_Vprop = "vprop";
const std::string Name_Vunit = "vunit";
const std::string Name_W = "w";
const std::string Name_Whilenot = "whilenot";
const std::string Name_Within = "within";
const std::string Name_X = "x";

const unsigned int Name_First_Keyword = 0;
const unsigned int Name_Last_Keyword = Name_First_Keyword + 99;

const unsigned int Name_First_AMS_Keyword = Name_Last_Keyword + 1;
const unsigned int Name_Last_AMS_Keyword = Name_First_AMS_Keyword + 12;

const unsigned int Name_First_Operator = Name_Last_AMS_Keyword + 1;
const unsigned int Name_Last_Operator = Name_First_Operator + 18;

const unsigned int Name_First_Attribute = Name_Last_Operator + 1;
const unsigned int Name_Last_Attribute = Name_First_Attribute + 31;

const unsigned int Name_First_Vhdl08_Attribute = Name_Last_Attribute + 1;
const unsigned int Name_Last_Vhdl08_Attribute = Name_First_Vhdl08_Attribute + 0;

const unsigned int Name_First_AMS_Attribute = Name_Last_Vhdl08_Attribute + 1;
const unsigned int Name_Last_AMS_Attribute = Name_First_AMS_Attribute + 8;

const unsigned int Name_First_Standard = Name_Last_AMS_Attribute + 1;
const unsigned int Name_Last_Standard = Name_First_Standard + 61;

const unsigned int Name_First_Charname = Name_Last_Standard + 1;
const unsigned int Name_Last_Charname = Name_First_Charname + 64;

const unsigned int Name_First_Misc = Name_Last_Charname + 1;
const unsigned int Name_Last_Misc = Name_First_Misc + 26;

const unsigned int Name_First_Ieee = Name_Last_Misc + 1;
const unsigned int Name_Last_Ieee = Name_First_Ieee + 20;

const unsigned int Name_First_Verilog = Name_Last_Ieee + 1;
const unsigned int Name_Last_Verilog = Name_First_Verilog + 69;

const unsigned int Name_First_Directive = Name_Last_Verilog + 1;
const unsigned int Name_Last_Directive = Name_First_Directive + 5;

const unsigned int Name_First_Systask = Name_Last_Directive + 1;
const unsigned int Name_Second_Systask = Name_First_Systask + 1;

const unsigned int Name_First_BSV = Name_Second_Systask + 1;
const unsigned int Name_Last_BSV = Name_First_BSV + 62;

const unsigned int Name_First_Comment = Name_Last_BSV + 1;
const unsigned int Name_Last_Comment = Name_First_Comment + 1;

const unsigned int Name_First_PSL = Name_Last_Comment + 1;
const unsigned int Name_Last_PSL = Name_First_PSL + 42;

const std::unordered_map<std::string, unsigned int> KeywordTable{
		{Name_Mod, Name_First_Keyword + 0},
		{Name_Rem, Name_First_Keyword + 1},

		{Name_And, Name_First_Keyword + 2},
		{Name_Or, Name_First_Keyword + 3},
		{Name_Xor, Name_First_Keyword + 4},
		{Name_Nand, Name_First_Keyword + 5},
		{Name_Nor, Name_First_Keyword + 6},

		{Name_Abs, Name_First_Keyword + 7},
		{Name_Not, Name_First_Keyword + 8},

		{Name_Access, Name_First_Keyword + 9},
		{Name_After, Name_First_Keyword + 10},
		{Name_Alias, Name_First_Keyword + 11},
		{Name_All, Name_First_Keyword + 12},
		{Name_Architecture, Name_First_Keyword + 13},
		{Name_Array, Name_First_Keyword + 14},
		{Name_Assert, Name_First_Keyword + 15},
		{Name_Attribute, Name_First_Keyword + 16},

		{Name_Begin, Name_First_Keyword + 17},
		{Name_Block, Name_First_Keyword + 18},
		{Name_Body, Name_First_Keyword + 19},
		{Name_Buffer, Name_First_Keyword + 20},
		{Name_Bus, Name_First_Keyword + 21},

		{Name_Case, Name_First_Keyword + 22},
		{Name_Component, Name_First_Keyword + 23},
		{Name_Configuration, Name_First_Keyword + 24},
		{Name_Constant, Name_First_Keyword + 25},

		{Name_Disconnect, Name_First_Keyword + 26},
		{Name_Downto, Name_First_Keyword + 27},

		{Name_Else, Name_First_Keyword + 28},
		{Name_Elsif, Name_First_Keyword + 29},
		{Name_End, Name_First_Keyword + 30},
		{Name_Entity, Name_First_Keyword + 31},
		{Name_Exit, Name_First_Keyword + 32},

		{Name_File, Name_First_Keyword + 33},
		{Name_For, Name_First_Keyword + 34},
		{Name_Function, Name_First_Keyword + 35},

		{Name_Generate, Name_First_Keyword + 36},
		{Name_Generic, Name_First_Keyword + 37},
		{Name_Guarded, Name_First_Keyword + 38},

		{Name_If, Name_First_Keyword + 39},
		{Name_In, Name_First_Keyword + 40},
		{Name_Inout, Name_First_Keyword + 41},
		{Name_Is, Name_First_Keyword + 42},

		{Name_Label, Name_First_Keyword + 43},
		{Name_Library, Name_First_Keyword + 44},
		{Name_Linkage, Name_First_Keyword + 45},
		{Name_Loop, Name_First_Keyword + 46},

		{Name_Map, Name_First_Keyword + 47},

		{Name_New, Name_First_Keyword + 48},
		{Name_Next, Name_First_Keyword + 49},
		{Name_Null, Name_First_Keyword + 50},

		{Name_Of, Name_First_Keyword + 51},
		{Name_On, Name_First_Keyword + 52},
		{Name_Open, Name_First_Keyword + 53},
		{Name_Others, Name_First_Keyword + 54},
		{Name_Out, Name_First_Keyword + 55},

		{Name_Package, Name_First_Keyword + 56},
		{Name_Port, Name_First_Keyword + 57},
		{Name_Procedure, Name_First_Keyword + 58},
		{Name_Process, Name_First_Keyword + 59},

		{Name_Range, Name_First_Keyword + 60},
		{Name_Record, Name_First_Keyword + 61},
		{Name_Register, Name_First_Keyword + 62},
		{Name_Report, Name_First_Keyword + 63},
		{Name_Return, Name_First_Keyword + 64},

		{Name_Select, Name_First_Keyword + 65},
		{Name_Severity, Name_First_Keyword + 66},
		{Name_Signal, Name_First_Keyword + 67},
		{Name_Subtype, Name_First_Keyword + 68},

		{Name_Then, Name_First_Keyword + 69},
		{Name_To, Name_First_Keyword + 70},
		{Name_Transport, Name_First_Keyword + 71},
		{Name_Type, Name_First_Keyword + 72},

		{Name_Units, Name_First_Keyword + 73},
		{Name_Until, Name_First_Keyword + 74},
		{Name_Use, Name_First_Keyword + 75},

		{Name_Variable, Name_First_Keyword + 76},

		{Name_Wait, Name_First_Keyword + 77},
		{Name_When, Name_First_Keyword + 78},
		{Name_While, Name_First_Keyword + 79},
		{Name_With, Name_First_Keyword + 80},

		// VHDL 93 reserved words.
		{Name_Xnor, Name_First_Keyword + 81},
		{Name_Group, Name_First_Keyword + 82},
		{Name_Impure, Name_First_Keyword + 83},
		{Name_Inertial, Name_First_Keyword + 84},
		{Name_Literal, Name_First_Keyword + 85},
		{Name_Postponed, Name_First_Keyword + 86},
		{Name_Pure, Name_First_Keyword + 87},
		{Name_Reject, Name_First_Keyword + 88},
		{Name_Shared, Name_First_Keyword + 89},
		{Name_Unaffected, Name_First_Keyword + 90},

		{Name_Sll, Name_First_Keyword + 91},
		{Name_Sla, Name_First_Keyword + 92},
		{Name_Sra, Name_First_Keyword + 93},
		{Name_Srl, Name_First_Keyword + 94},
		{Name_Rol, Name_First_Keyword + 95},
		{Name_Ror, Name_First_Keyword + 96},

		// VHDL 00 reserved words.
		{Name_Protected, Name_First_Keyword + 97},

		// VHDL 08 Keywords
		{Name_Context, Name_First_Keyword + 98},
		{Name_Parameter, Name_First_Keyword + 99},

		{Name_Across, Name_First_AMS_Keyword + 0},
		{Name_Break, Name_First_AMS_Keyword + 1},
		{Name_Limit, Name_First_AMS_Keyword + 2},
		{Name_Nature, Name_First_AMS_Keyword + 3},
		{Name_Noise, Name_First_AMS_Keyword + 4},
		{Name_Procedural, Name_First_AMS_Keyword + 5},
		{Name_Quantity, Name_First_AMS_Keyword + 6},
		{Name_Reference, Name_First_AMS_Keyword + 7},
		{Name_Spectrum, Name_First_AMS_Keyword + 8},
		{Name_Subnature, Name_First_AMS_Keyword + 9},
		{Name_Terminal, Name_First_AMS_Keyword + 10},
		{Name_Through, Name_First_AMS_Keyword + 11},
		{Name_Tolerance, Name_First_AMS_Keyword + 12},

		// Create operators
		{Name_Op_Equality, Name_First_Operator + 0},
		{Name_Op_Inequality, Name_First_Operator + 1},
		{Name_Op_Less, Name_First_Operator + 2},
		{Name_Op_Less_Equal, Name_First_Operator + 3},
		{Name_Op_Greater, Name_First_Operator + 4},
		{Name_Op_Greater_Equal, Name_First_Operator + 5},
		{Name_Op_Plus, Name_First_Operator + 6},
		{Name_Op_Minus, Name_First_Operator + 7},
		{Name_Op_Mul, Name_First_Operator + 8},
		{Name_Op_Div, Name_First_Operator + 9},
		{Name_Op_Exp, Name_First_Operator + 10},
		{Name_Op_Concatenation, Name_First_Operator + 11},
		{Name_Op_Condition, Name_First_Operator + 12},
		{Name_Op_Match_Equality, Name_First_Operator + 13},
		{Name_Op_Match_Inequality, Name_First_Operator + 14},
		{Name_Op_Match_Less, Name_First_Operator + 15},
		{Name_Op_Match_Less_Equal, Name_First_Operator + 16},
		{Name_Op_Match_Greater, Name_First_Operator + 17},
		{Name_Op_Match_Greater_Equal, Name_First_Operator + 18},

		// Create Attributes.
		{Name_Base, Name_First_Attribute + 0},
		{Name_Left, Name_First_Attribute + 1},
		{Name_Right, Name_First_Attribute + 2},
		{Name_High, Name_First_Attribute + 3},
		{Name_Low, Name_First_Attribute + 4},
		{Name_Pos, Name_First_Attribute + 5},
		{Name_Val, Name_First_Attribute + 6},
		{Name_Succ, Name_First_Attribute + 7},
		{Name_Pred, Name_First_Attribute + 8},
		{Name_Leftof, Name_First_Attribute + 9},
		{Name_Rightof, Name_First_Attribute + 10},
		{Name_Reverse_Range, Name_First_Attribute + 11},
		{Name_Length, Name_First_Attribute + 12},
		{Name_Delayed, Name_First_Attribute + 13},
		{Name_Stable, Name_First_Attribute + 14},
		{Name_Quiet, Name_First_Attribute + 15},
		{Name_Transaction, Name_First_Attribute + 16},
		{Name_Event, Name_First_Attribute + 17},
		{Name_Active, Name_First_Attribute + 18},
		{Name_Last_Event, Name_First_Attribute + 19},
		{Name_Last_Active, Name_First_Attribute + 20},
		{Name_Last_Value, Name_First_Attribute + 21},

		{Name_Behavior, Name_First_Attribute + 22},
		{Name_Structure, Name_First_Attribute + 23},

		{Name_Ascending, Name_First_Attribute + 24},
		{Name_Image, Name_First_Attribute + 25},
		{Name_Value, Name_First_Attribute + 26},
		{Name_Driving, Name_First_Attribute + 27},
		{Name_Driving_Value, Name_First_Attribute + 28},
		{Name_Simple_Name, Name_First_Attribute + 29},
		{Name_Instance_Name, Name_First_Attribute + 30},
		{Name_Path_Name, Name_First_Attribute + 31},

		{Name_Element, Name_First_Vhdl08_Attribute + 0},

		{Name_Contribution, Name_First_AMS_Attribute + 0},
		{Name_Dot, Name_First_AMS_Attribute + 1},
		{Name_Integ, Name_First_AMS_Attribute + 2},
		{Name_Above, Name_First_AMS_Attribute + 3},
		{Name_ZOH, Name_First_AMS_Attribute + 4},
		{Name_LTF, Name_First_AMS_Attribute + 5},
		{Name_ZTF, Name_First_AMS_Attribute + 6},
		{Name_Ramp, Name_First_AMS_Attribute + 7},
		{Name_Slew, Name_First_AMS_Attribute + 8},

//  Create standard.
		{Name_Std, Name_First_Standard + 0},
		{Name_Standard, Name_First_Standard + 1},
		{Name_Boolean, Name_First_Standard + 2},
		{Name_False, Name_First_Standard + 3},
		{Name_True, Name_First_Standard + 4},
		{Name_Bit, Name_First_Standard + 5},
		{Name_Character, Name_First_Standard + 6},
		{Name_Severity_Level, Name_First_Standard + 7},
		{Name_Note, Name_First_Standard + 8},
		{Name_Warning, Name_First_Standard + 9},
		{Name_Error, Name_First_Standard + 10},
		{Name_Failure, Name_First_Standard + 11},
		{Name_Universal_Integer, Name_First_Standard + 12},
		{Name_Universal_Real, Name_First_Standard + 13},
		{Name_Convertible_Integer, Name_First_Standard + 14},
		{Name_Convertible_Real, Name_First_Standard + 15},
		{Name_Integer, Name_First_Standard + 16},
		{Name_Real, Name_First_Standard + 17},
		{Name_Time, Name_First_Standard + 18},
		{Name_Fs, Name_First_Standard + 19},
		{Name_Ps, Name_First_Standard + 20},
		{Name_Ns, Name_First_Standard + 21},
		{Name_Us, Name_First_Standard + 22},
		{Name_Ms, Name_First_Standard + 23},
		{Name_Sec, Name_First_Standard + 24},
		{Name_Min, Name_First_Standard + 25},
		{Name_Hr, Name_First_Standard + 26},
		{Name_Delay_Length, Name_First_Standard + 27},
		{Name_Now, Name_First_Standard + 28},
		{Name_Natural, Name_First_Standard + 29},
		{Name_Positive, Name_First_Standard + 30},
		{Name_String, Name_First_Standard + 31},
		{Name_Bit_Vector, Name_First_Standard + 32},
		{Name_File_Open_Kind, Name_First_Standard + 33},
		{Name_Read_Mode, Name_First_Standard + 34},
		{Name_Write_Mode, Name_First_Standard + 35},
		{Name_Append_Mode, Name_First_Standard + 36},
		{Name_File_Open_Status, Name_First_Standard + 37},
		{Name_Open_Ok, Name_First_Standard + 38},
		{Name_Status_Error, Name_First_Standard + 39},
		{Name_Name_Error, Name_First_Standard + 40},
		{Name_Mode_Error, Name_First_Standard + 41},
		{Name_Foreign, Name_First_Standard + 42},

		{Name_Boolean_Vector, Name_First_Standard + 43},
		{Name_To_Bstring, Name_First_Standard + 44},
		{Name_To_Binary_String, Name_First_Standard + 45},
		{Name_To_Ostring, Name_First_Standard + 46},
		{Name_To_Octal_String, Name_First_Standard + 47},
		{Name_To_Hstring, Name_First_Standard + 48},
		{Name_To_Hex_String, Name_First_Standard + 49},
		{Name_Integer_Vector, Name_First_Standard + 50},
		{Name_Real_Vector, Name_First_Standard + 51},
		{Name_Time_Vector, Name_First_Standard + 52},
		{Name_Digits, Name_First_Standard + 53},
		{Name_Format, Name_First_Standard + 54},
		{Name_Unit, Name_First_Standard + 55},

		{Name_Domain_Type, Name_First_Standard + 56},
		{Name_Quiescent_Domain, Name_First_Standard + 57},
		{Name_Time_Domain, Name_First_Standard + 58},
		{Name_Frequency_Domain, Name_First_Standard + 59},
		{Name_Domain, Name_First_Standard + 60},
		{Name_Frequency, Name_First_Standard + 61},

		{Name_Nul, Name_First_Charname + 0},
		{Name_Soh, Name_First_Charname + 1},
		{Name_Stx, Name_First_Charname + 2},
		{Name_Etx, Name_First_Charname + 3},
		{Name_Eot, Name_First_Charname + 4},
		{Name_Enq, Name_First_Charname + 5},
		{Name_Ack, Name_First_Charname + 6},
		{Name_Bel, Name_First_Charname + 7},
		{Name_Bs, Name_First_Charname + 8},
		{Name_Ht, Name_First_Charname + 9},
		{Name_Lf, Name_First_Charname + 10},
		{Name_Vt, Name_First_Charname + 11},
		{Name_Ff, Name_First_Charname + 12},
		{Name_Cr, Name_First_Charname + 13},
		{Name_So, Name_First_Charname + 14},
		{Name_Si, Name_First_Charname + 15},
		{Name_Dle, Name_First_Charname + 16},
		{Name_Dc1, Name_First_Charname + 17},
		{Name_Dc2, Name_First_Charname + 18},
		{Name_Dc3, Name_First_Charname + 19},
		{Name_Dc4, Name_First_Charname + 20},
		{Name_Nak, Name_First_Charname + 21},
		{Name_Syn, Name_First_Charname + 22},
		{Name_Etb, Name_First_Charname + 23},
		{Name_Can, Name_First_Charname + 24},
		{Name_Em, Name_First_Charname + 25},
		{Name_Sub, Name_First_Charname + 26},
		{Name_Esc, Name_First_Charname + 27},
		{Name_Fsp, Name_First_Charname + 28},
		{Name_Gsp, Name_First_Charname + 29},
		{Name_Rsp, Name_First_Charname + 30},
		{Name_Usp, Name_First_Charname + 31},
		{Name_Del, Name_First_Charname + 32},

		{Name_C128, Name_First_Charname + 33},
		{Name_C129, Name_First_Charname + 34},
		{Name_C130, Name_First_Charname + 35},
		{Name_C131, Name_First_Charname + 36},
		{Name_C132, Name_First_Charname + 37},
		{Name_C133, Name_First_Charname + 38},
		{Name_C134, Name_First_Charname + 39},
		{Name_C135, Name_First_Charname + 40},
		{Name_C136, Name_First_Charname + 41},
		{Name_C137, Name_First_Charname + 42},
		{Name_C138, Name_First_Charname + 43},
		{Name_C139, Name_First_Charname + 44},
		{Name_C140, Name_First_Charname + 45},
		{Name_C141, Name_First_Charname + 46},
		{Name_C142, Name_First_Charname + 47},
		{Name_C143, Name_First_Charname + 48},
		{Name_C144, Name_First_Charname + 49},
		{Name_C145, Name_First_Charname + 50},
		{Name_C146, Name_First_Charname + 51},
		{Name_C147, Name_First_Charname + 52},
		{Name_C148, Name_First_Charname + 53},
		{Name_C149, Name_First_Charname + 54},
		{Name_C150, Name_First_Charname + 55},
		{Name_C151, Name_First_Charname + 56},
		{Name_C152, Name_First_Charname + 57},
		{Name_C153, Name_First_Charname + 58},
		{Name_C154, Name_First_Charname + 59},
		{Name_C155, Name_First_Charname + 60},
		{Name_C156, Name_First_Charname + 61},
		{Name_C157, Name_First_Charname + 62},
		{Name_C158, Name_First_Charname + 63},
		{Name_C159, Name_First_Charname + 64},

// Create misc.
		{Name_Guard, Name_First_Misc + 0},
		{Name_Deallocate, Name_First_Misc + 1},
		{Name_File_Open, Name_First_Misc + 2},
		{Name_File_Close, Name_First_Misc + 3},
		{Name_Read, Name_First_Misc + 4},
		{Name_Write, Name_First_Misc + 5},
		{Name_Flush, Name_First_Misc + 6},
		{Name_Endfile, Name_First_Misc + 7},
		{Name_P, Name_First_Misc + 8},
		{Name_F, Name_First_Misc + 9},
		{Name_L, Name_First_Misc + 10},
		{Name_R, Name_First_Misc + 11},
		{Name_S, Name_First_Misc + 12},
		{Name_External_Name, Name_First_Misc + 13},
		{Name_Open_Kind, Name_First_Misc + 14},
		{Name_Status, Name_First_Misc + 15},
		{Name_First, Name_First_Misc + 16},
		{Name_Last, Name_First_Misc + 17},
		{Name_Textio, Name_First_Misc + 18},
		{Name_Work, Name_First_Misc + 19},
		{Name_Text, Name_First_Misc + 20},
		{Name_To_String, Name_First_Misc + 21},
		{Name_Minimum, Name_First_Misc + 22},
		{Name_Maximum, Name_First_Misc + 23},
		{Name_Untruncated_Text_Read, Name_First_Misc + 24},
		{Name_Get_Resolution_Limit, Name_First_Misc + 25},
		{Name_Control_Simulation, Name_First_Misc + 26},

		{Name_Ieee, Name_First_Ieee + 0},
		{Name_Std_Logic_1164, Name_First_Ieee + 1},
		{Name_Std_Ulogic, Name_First_Ieee + 2},
		{Name_Std_Ulogic_Vector, Name_First_Ieee + 3},
		{Name_Std_Logic, Name_First_Ieee + 4},
		{Name_Std_Logic_Vector, Name_First_Ieee + 5},
		{Name_Rising_Edge, Name_First_Ieee + 6},
		{Name_Falling_Edge, Name_First_Ieee + 7},
		{Name_VITAL_Timing, Name_First_Ieee + 8},
		{Name_VITAL_Level0, Name_First_Ieee + 9},
		{Name_VITAL_Level1, Name_First_Ieee + 10},
		{Name_Numeric_Std, Name_First_Ieee + 11},
		{Name_Numeric_Bit, Name_First_Ieee + 12},
		{Name_Unsigned, Name_First_Ieee + 13},
		{Name_Signed, Name_First_Ieee + 14},
		{Name_Unresolved_Unsigned, Name_First_Ieee + 15},
		{Name_Unresolved_Signed, Name_First_Ieee + 16},
		{Name_Std_Logic_Arith, Name_First_Ieee + 17},
		{Name_Std_Logic_Signed, Name_First_Ieee + 18},
		{Name_Std_Logic_Textio, Name_First_Ieee + 19},
		{Name_Std_Logic_Unsigned, Name_First_Ieee + 20},

//  Verilog keywords
		{Name_Always, Name_First_Verilog + 0},
		{Name_Assign, Name_First_Verilog + 1},
		{Name_Buf, Name_First_Verilog + 2},
		{Name_Bufif0, Name_First_Verilog + 3},
		{Name_Bufif1, Name_First_Verilog + 4},
		{Name_Casex, Name_First_Verilog + 5},
		{Name_Casez, Name_First_Verilog + 6},
		{Name_Cmos, Name_First_Verilog + 7},
		{Name_Deassign, Name_First_Verilog + 8},
		{Name_Default, Name_First_Verilog + 9},
		{Name_Defparam, Name_First_Verilog + 10},
		{Name_Disable, Name_First_Verilog + 11},
		{Name_Endcase, Name_First_Verilog + 12},
		{Name_Endfunction, Name_First_Verilog + 13},
		{Name_Endmodule, Name_First_Verilog + 14},
		{Name_Endprimitive, Name_First_Verilog + 15},
		{Name_Endspecify, Name_First_Verilog + 16},
		{Name_Endtable, Name_First_Verilog + 17},
		{Name_Endtask, Name_First_Verilog + 18},
		{Name_Forever, Name_First_Verilog + 19},
		{Name_Fork, Name_First_Verilog + 20},
		{Name_Highz0, Name_First_Verilog + 21},
		{Name_Highz1, Name_First_Verilog + 22},
		{Name_Initial, Name_First_Verilog + 23},
		{Name_Input, Name_First_Verilog + 24},
		{Name_Join, Name_First_Verilog + 25},
		{Name_Large, Name_First_Verilog + 26},
		{Name_Medium, Name_First_Verilog + 27},
		{Name_Module, Name_First_Verilog + 28},
		{Name_Negedge, Name_First_Verilog + 29},
		{Name_Nmos, Name_First_Verilog + 30},
		{Name_Notif0, Name_First_Verilog + 31},
		{Name_Notif1, Name_First_Verilog + 32},
		{Name_Output, Name_First_Verilog + 33},
		{Name_Pmos, Name_First_Verilog + 34},
		{Name_Posedge, Name_First_Verilog + 35},
		{Name_Primitive, Name_First_Verilog + 36},
		{Name_Pull0, Name_First_Verilog + 37},
		{Name_Pull1, Name_First_Verilog + 38},
		{Name_Pulldown, Name_First_Verilog + 39},
		{Name_Pullup, Name_First_Verilog + 40},
		{Name_Reg, Name_First_Verilog + 41},
		{Name_Repeat, Name_First_Verilog + 42},
		{Name_Rcmos, Name_First_Verilog + 43},
		{Name_Rnmos, Name_First_Verilog + 44},
		{Name_Rpmos, Name_First_Verilog + 45},
		{Name_Rtran, Name_First_Verilog + 46},
		{Name_Rtranif0, Name_First_Verilog + 47},
		{Name_Rtranif1, Name_First_Verilog + 48},
		{Name_Small, Name_First_Verilog + 49},
		{Name_Specify, Name_First_Verilog + 50},
		{Name_Specparam, Name_First_Verilog + 51},
		{Name_Strong0, Name_First_Verilog + 52},
		{Name_Strong1, Name_First_Verilog + 53},
		{Name_Supply0, Name_First_Verilog + 54},
		{Name_Supply1, Name_First_Verilog + 55},
		{Name_Tablex, Name_First_Verilog + 56},
		{Name_Task, Name_First_Verilog + 57},
		{Name_Tran, Name_First_Verilog + 58},
		{Name_Tranif0, Name_First_Verilog + 59},
		{Name_Tranif1, Name_First_Verilog + 60},
		{Name_Tri, Name_First_Verilog + 61},
		{Name_Tri0, Name_First_Verilog + 62},
		{Name_Tri1, Name_First_Verilog + 63},
		{Name_Trireg, Name_First_Verilog + 64},
		{Name_Wand, Name_First_Verilog + 65},
		{Name_Weak0, Name_First_Verilog + 66},
		{Name_Weak1, Name_First_Verilog + 67},
		{Name_Wire, Name_First_Verilog + 68},
		{Name_Wor, Name_First_Verilog + 69},

		{Name_Define, Name_First_Directive + 0},
		{Name_Endif, Name_First_Directive + 1},
		{Name_Ifdef, Name_First_Directive + 2},
		{Name_Include, Name_First_Directive + 3},
		{Name_Timescale, Name_First_Directive + 4},
		{Name_Undef, Name_First_Directive + 5},

//  Verilog system tasks
		{Name_Display, Name_First_Systask + 0},
		{Name_Finish, Name_First_Systask + 1},

//  BSV keywords
		{Name_uAction, Name_First_BSV + 0},
		{Name_uActionValue, Name_First_BSV + 1},
		{Name_BVI, Name_First_BSV + 2},
		{Name_uC, Name_First_BSV + 3},
		{Name_uCF, Name_First_BSV + 4},
		{Name_uE, Name_First_BSV + 5},
		{Name_uSB, Name_First_BSV + 6},
		{Name_uSBR, Name_First_BSV + 7},
		{Name_Action, Name_First_BSV + 8},
		{Name_Endaction, Name_First_BSV + 9},
		{Name_Actionvalue, Name_First_BSV + 10},
		{Name_Endactionvalue, Name_First_BSV + 11},
		{Name_Ancestor, Name_First_BSV + 12},
		{Name_Clocked_By, Name_First_BSV + 13},
		{Name_Continue, Name_First_BSV + 14},
		{Name_Default_Clock, Name_First_BSV + 15},
		{Name_Default_Reset, Name_First_BSV + 16},
		{Name_Dependencies, Name_First_BSV + 17},
		{Name_Deriving, Name_First_BSV + 18},
		{Name_Determines, Name_First_BSV + 19},
		{Name_Enable, Name_First_BSV + 20},
		{Name_Enum, Name_First_BSV + 21},
		{Name_Export, Name_First_BSV + 22},
		{Name_Ifc_Inout, Name_First_BSV + 23},
		{Name_Import, Name_First_BSV + 24},
		{Name_Input_Clock, Name_First_BSV + 25},
		{Name_Input_Reset, Name_First_BSV + 26},
		{Name_Instance, Name_First_BSV + 27},
		{Name_Endinstance, Name_First_BSV + 28},
		{Name_Interface, Name_First_BSV + 29},
		{Name_Endinterface, Name_First_BSV + 30},
		{Name_Let, Name_First_BSV + 31},
		{Name_Match, Name_First_BSV + 32},
		{Name_Matches, Name_First_BSV + 33},
		{Name_Method, Name_First_BSV + 34},
		{Name_Endmethod, Name_First_BSV + 35},
		{Name_Numeric, Name_First_BSV + 36},
		{Name_Output_Clock, Name_First_BSV + 37},
		{Name_Output_Reset, Name_First_BSV + 38},
		{Name_Endpackage, Name_First_BSV + 39},
		{Name_Par, Name_First_BSV + 40},
		{Name_Endpar, Name_First_BSV + 41},
		{Name_Path, Name_First_BSV + 42},
		{Name_Provisos, Name_First_BSV + 43},
		{Name_Ready, Name_First_BSV + 44},
		{Name_Reset_By, Name_First_BSV + 45},
		{Name_Rule, Name_First_BSV + 46},
		{Name_Endrule, Name_First_BSV + 47},
		{Name_Rules, Name_First_BSV + 48},
		{Name_Endrules, Name_First_BSV + 49},
		{Name_Same_Family, Name_First_BSV + 50},
		{Name_Schedule, Name_First_BSV + 51},
		{Name_Seq, Name_First_BSV + 52},
		{Name_Endseq, Name_First_BSV + 53},
		{Name_Struct, Name_First_BSV + 54},
		{Name_Tagged, Name_First_BSV + 55},
		{Name_Typeclass, Name_First_BSV + 56},
		{Name_Endtypeclass, Name_First_BSV + 57},
		{Name_Typedef, Name_First_BSV + 58},
		{Name_Union, Name_First_BSV + 59},
		{Name_Valueof, Name_First_BSV + 60},
		{Name_uValueof, Name_First_BSV + 61},
		{Name_Void, Name_First_BSV + 62},

//  VHDL special comments
		{Name_Psl, Name_First_Comment + 0},
		{Name_Pragma, Name_First_Comment + 1},

//  PSL keywords
		{Name_A, Name_First_PSL + 0},
		{Name_Af, Name_First_PSL + 1},
		{Name_Ag, Name_First_PSL + 2},
		{Name_Ax, Name_First_PSL + 3},
		{Name_Abort, Name_First_PSL + 4},
		{Name_Assume, Name_First_PSL + 5},
		{Name_Assume_Guarantee, Name_First_PSL + 6},
		{Name_Before, Name_First_PSL + 7},
		{Name_Clock, Name_First_PSL + 8},
		{Name_Const,},
		{Name_Cover, Name_First_PSL + 10},
		{Name_E, Name_First_PSL + 11},
		{Name_Ef, Name_First_PSL + 12},
		{Name_Eg, Name_First_PSL + 13},
		{Name_Ex, Name_First_PSL + 14},
		{Name_Endpoint, Name_First_PSL + 15},
		{Name_Eventually, Name_First_PSL + 16},
		{Name_Fairness, Name_First_PSL + 17},
		{Name_Fell, Name_First_PSL + 18},
		{Name_Forall, Name_First_PSL + 19},
		{Name_G, Name_First_PSL + 20},
		{Name_Inf, Name_First_PSL + 21},
		{Name_Inherit, Name_First_PSL + 22},
		{Name_Never, Name_First_PSL + 23},
		{Name_Next_A, Name_First_PSL + 24},
		{Name_Next_E, Name_First_PSL + 25},
		{Name_Next_Event, Name_First_PSL + 26},
		{Name_Next_Event_A, Name_First_PSL + 27},
		{Name_Next_Event_E, Name_First_PSL + 28},
		{Name_Property, Name_First_PSL + 29},
		{Name_Prev, Name_First_PSL + 30},
		{Name_Restrict, Name_First_PSL + 31},
		{Name_Restrict_Guarantee, Name_First_PSL + 32},
		{Name_Rose, Name_First_PSL + 33},
		{Name_Sequence, Name_First_PSL + 34},
		{Name_Strong, Name_First_PSL + 35},
		{Name_Vmode, Name_First_PSL + 36},
		{Name_Vprop, Name_First_PSL + 37},
		{Name_Vunit, Name_First_PSL + 38},
		{Name_W, Name_First_PSL + 39},
		{Name_Whilenot, Name_First_PSL + 40},
		{Name_Within, Name_First_PSL + 41},
		{Name_X, Name_First_PSL + 42}
};

inline bool isKeyword(std::string id) {
	auto i = KeywordTable.find(id);
	return (i != KeywordTable.end() && i->second >= Name_First_Keyword && i->second <= Name_Last_AMS_Keyword);
}

inline bool isAMSReservedWord(std::string id) {
	auto i = KeywordTable.find(id);
	return (i != KeywordTable.end() && i->second >= Name_First_AMS_Keyword && i->second <= Name_Last_AMS_Keyword);
}

inline bool isVHDL08ReservedWord(std::string id) {
	auto i = KeywordTable.find(id);
	return (i != KeywordTable.end() && i->second >= (Name_First_Keyword + 98)
			&& i->second <= (Name_First_Keyword + 99));
}

inline bool isVHDL00ReservedWord(std::string id) {
	auto i = KeywordTable.find(id);
	return (i != KeywordTable.end() && i->second == (Name_First_Keyword + 97));
}

inline bool isVHDL93ReservedWord(std::string id) {
	auto i = KeywordTable.find(id);
	return (i != KeywordTable.end() && i->second >= (Name_First_Keyword + 81)
			&& i->second <= (Name_First_Keyword + 96));
}

inline bool isVHDL87ReservedWord(std::string id) {
	auto i = KeywordTable.find(id);
	return (i != KeywordTable.end() && i->second >= Name_First_Keyword && i->second <= (Name_First_Keyword + 80));
}

}
#endif //AVLC_KEYWORDS_H
