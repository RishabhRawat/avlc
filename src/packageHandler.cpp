//
// Created by scalpel on 12/3/17.
//

#include <CompilerExceptions.h>
#include <Parser.h>
#include "packageHandler.h"
#include "json.hpp"

namespace fs = std::filesystem;
//
//with Ada.Text_IO; use Ada.Text_IO;
//with Tables;
//with GNAT.OS_Lib;
//with Interfaces.C_Streams;
//with System;
//with Errorout; use Errorout;
//with Scanner;
//with Iirs_Utils; use Iirs_Utils;
//with Parse;
//with Name_Table; use Name_Table;
//with Str_Table;
//with Tokens;
//with Files_Map;
//with state.options;
//with Std_Package;
//with Disp_Tree;
//with Disp_Vhdl;
//with Sem;
//with Post_Sems;
//with Canon;
//with Nodes_GC;
//

//  A location for any implicit declarations (such as library WORK).
Implicit_Location: Location_Type;

//  Report an error message.
procedure Error_Lib_Msg (Msg : String) is
        begin
Report_Msg (Msgid_Error, Library, No_Location, Msg);
end Error_Lib_Msg;

procedure Error_Lib_Msg (Msg : String; Arg1 : Earg_Type) is
        begin
Report_Msg (Msgid_Error, Library, No_Location, Msg, (1 => Arg1));
end Error_Lib_Msg;

void packageHandler::Add_Library_Path(fs::path Path) {
    paths.push_back(Path);
}

// Transform a library identifier into a file name. Very simple mechanism: just add '-objVV.cf' extension,
// where VV is the version.

std::string packageHandler::Library_To_File_Name(Iir_Library_Declaration* Library) {
    switch (state.options.standard) {
    case Vhdl_Std::Vhdl_87:
        return Library->Identifier + "-obj87.cf";
    case Vhdl_Std::Vhdl_93c:
    case Vhdl_Std::Vhdl_93:
    case Vhdl_Std::Vhdl_00:
    case Vhdl_Std::Vhdl_02:
        return Library->Identifier + "-obj93.cf";
    case Vhdl_Std::Vhdl_08:
        return Library->Identifier + "-obj08.cf";
    }
}

//  Search LIBRARY in the library path.
bool packageHandler::Search_Library_In_Path(Iir_Library_Declaration* Library) {
    auto libraryName = Library_To_File_Name(Library);

    std::string str;
    switch (state.options.standard) {
    case Vhdl_Std::Vhdl_87:
        str = "v87";
        break;
    case Vhdl_Std::Vhdl_93c:
    case Vhdl_Std::Vhdl_93:
    case Vhdl_Std::Vhdl_00:
    case Vhdl_Std::Vhdl_02:
        str = "v93";
        break;
    case Vhdl_Std::Vhdl_08:
        str = "v08";
        break;
    }

    for (auto &&path: paths) {
        auto p = paths + fs::path(libraryName);
        if (fs::is_regular_file(paths + fs::path(libraryName)))
            Library->Library_Directory = p;
        else if (fs::is_regular_file(paths + fs::path(str) + fs::path(libraryName)))
                Library->Library_Directory = paths + fs::path(str);
        else
            continue;
        return true;
    }
    return false;
}

void packageHandler::Set_Work_Library_Path(fs::path Path) {
    //  This is a warning, since 'clean' action should not fail in this cases.
    if (!fs::is_directory(Path))
        Warning("Directory '" + Path + "' set by --workdir= does not exist", state.options.Warnid_Library);
    Work_Directory = Path;
}

//  Every design unit is put in this hash table to be quickly found by its (primary) identifier.
std::unordered_map<std::string, Iir_Design_Unit*> Primary_Units;
std::unordered_map<std::string, Iir_Design_Unit*> Secondary_Units;

// NOTE: Architectures are put with the entity identifier. (How to handle both??)

void packageHandler::Purge_Design_File(Iir_Design_File* Design_File) {
    auto it = std::find(Work_Library.Design_Files.begin(), Work_Library.Design_Files.end(), Design_File);
    if (it == Work_Library.Design_Files.end())
        Warning("Look this up bro, your design file aint in work library");
    else
        Work_Library.Design_Files.erase(it);

    //FIXME: Remove_Unit_Hash (Unit); for all containing units
}

// Load the contents of a library from a map file.
// The format of this file, used by save_library and load_library is
// as follow:
//
// file_format ::= header { design_file_format }
// header ::= v 3
// design_file_format ::=
//      filename_format { design_unit_format  }
// filename_format ::=
//      FILE directory "filename" "file_time_stamp" "analyze_time_stamp":
// design_unit_format ::= entity_format
//                        | architecture_format
//                        | package_format
//                        | package_body_format
//                        | configuration_format
//                        | context_format
// position_format ::= LINE(POS) + OFF on DATE
// entity_format ::=
//      ENTITY identifier AT position_format ;
// architecture_format ::=
//      ARCHITECTURE identifier of name AT position_format ;
// package_format ::=
//      PACKAGE identifier AT position_format [BODY] ;
// package_body_format ::=
//      PACKAGE BODY identifier AT position_format ;
// configuration_format ::=
//      CONFIGURATION identifier AT position_format ;
// context_format ::=
//      CONTEXT identifier AT position_format ;
//
// The position_format meaning is:
//       LINE is the line number (first line is number 1),
//       POS is the offset of this line number, as a source_ptr value,
//       OFF is the offset in the line, starting with 0.
//       DATE is the symbolic date of analysis (order).
//
// Return TRUE if the library was found.
struct Save_Design_Unit {

    nlohmann::json operator()(Iir_Entity_Declaration* unit) {
        nlohmann::json result;
        result["type"] = "entity";
        result["name"] = unit->Identifier;
        result["Location"]["Line"] = unit->Location.Line;
        result["Location"]["Line_Pos"] = unit->Location.Line_Pos;
        result["Location"]["Pos"] = unit->Location.Pos;
        return result;

    }

    nlohmann::json operator()(Iir_Architecture_Body* unit) {
        nlohmann::json result;
        result["type"] = "architecture";
        result["name"] = unit->Identifier;
        result["entity"] = unit->Entity_Name;
        result["Location"]["Line"] = unit->Location.Line;
        result["Location"]["Line_Pos"] = unit->Location.Line_Pos;
        result["Location"]["Pos"] = unit->Location.Pos;
        return result;

    }

    nlohmann::json operator()(Iir_Package_Declaration* unit) {
        nlohmann::json result;
        result["type"] = "package";
        result["name"] = unit->Identifier;
        result["body"] = unit->Need_Body;
        result["Location"]["Line"] = unit->Location.Line;
        result["Location"]["Line_Pos"] = unit->Location.Line_Pos;
        result["Location"]["Pos"] = unit->Location.Pos;
        return result;

    }

    nlohmann::json operator()(Iir_Package_Instantiation_Declaration* unit) {
        nlohmann::json result;
        result["type"] = "package_body";
        result["name"] = unit->Identifier;
        result["Location"]["Line"] = unit->Location.Line;
        result["Location"]["Line_Pos"] = unit->Location.Line_Pos;
        result["Location"]["Pos"] = unit->Location.Pos;
        return result;

    }

    nlohmann::json operator()(Iir_Package_Body* unit) {
        nlohmann::json result;
        result["type"] = "package_body";
        result["name"] = unit->Identifier;
        result["Location"]["Line"] = unit->Location.Line;
        result["Location"]["Line_Pos"] = unit->Location.Line_Pos;
        result["Location"]["Pos"] = unit->Location.Pos;
        return result;

    }

    nlohmann::json operator()(Iir_Configuration_Declaration* unit) {
        nlohmann::json result;
        result["type"] = "configuration";
        result["name"] = unit->Identifier;
        result["Location"]["Line"] = unit->Location.Line;
        result["Location"]["Line_Pos"] = unit->Location.Line_Pos;
        result["Location"]["Pos"] = unit->Location.Pos;
        return result;

    }

    nlohmann::json operator()(Iir_Context_Declaration* unit) {
        nlohmann::json result;
        result["type"] = "context";
        result["name"] = unit->Identifier;
        result["Location"]["Line"] = unit->Location.Line;
        result["Location"]["Line_Pos"] = unit->Location.Line_Pos;
        result["Location"]["Pos"] = unit->Location.Pos;
        return result;

    }
};

// Save the file map of library LIBRARY.
bool packageHandler::Load_Library(Iir_Library_Declaration* Library) {
    //TODO: Assert Library is not already loaded
    assert(Library->Design_Files.empty());;
    if (!Library->Library_Directory && Search_Library_In_Path(Library))
        Library->Date = 10;


    std::ifstream ifs(Library->Library_Directory + Library_To_File_Name(Library));
    if(ifs.fail()) {
        Library->Date = 10;
        return false;
    }
    nlohmann::json data = nlohmann::json::parse(ifs);

    if(data["version"] != 1)
        throw std::runtime_error("wrong library format");

    for (auto &&file_item : data["files"]) {
        Iir_Design_File* f = new Iir_Design_File;
        f->Design_File_Filename = file_item.first;
        f->Design_File_Directory = file_item.second["directory"];
        f->File_Checksum = file_item.second["crc32"];
        for (auto &&unit_item  : file_item.second["design_units"]) {
            Iir_Design_Unit_n var;
            if(unit_item["type"] == "entity") {
                auto unit = new Iir_Entity_Declaration;
                unit->Location.Pos = unit_item["Location"]["Pos"];
                unit->Location.Line = unit_item["Location"]["Line"];
                unit->Location.Line_Pos = unit_item["Location"]["Line_Pos"];
                unit->Identifier = unit_item["name"];
                var = unit;
            }
            else if(unit_item["type"] == "architecture") {
                auto unit = new Iir_Architecture_Body;
                unit->Location.Pos = unit_item["Location"]["Pos"];
                unit->Location.Line = unit_item["Location"]["Line"];
                unit->Location.Line_Pos = unit_item["Location"]["Line_Pos"];
                unit->Identifier = unit_item["name"];
                var = unit;
            }
            else if(unit_item["type"] == "package") {
                auto unit = new Iir_Package_Declaration;
                unit->Location.Pos = unit_item["Location"]["Pos"];
                unit->Location.Line = unit_item["Location"]["Line"];
                unit->Location.Line_Pos = unit_item["Location"]["Line_Pos"];
                unit->Identifier = unit_item["name"];
                var = unit;
            }
            else if(unit_item["type"] == "package_body") {
                auto unit = new Iir_Package_Body;
                unit->Location.Pos = unit_item["Location"]["Pos"];
                unit->Location.Line = unit_item["Location"]["Line"];
                unit->Location.Line_Pos = unit_item["Location"]["Line_Pos"];
                unit->Identifier = unit_item["name"];
                var = unit;
            }
            else if(unit_item["type"] == "configuration") {
                auto unit = new Iir_Configuration_Declaration;
                unit->Location.Pos = unit_item["Location"]["Pos"];
                unit->Location.Line = unit_item["Location"]["Line"];
                unit->Location.Line_Pos = unit_item["Location"]["Line_Pos"];
                unit->Identifier = unit_item["name"];
                var = unit;
            }
            else if(unit_item["type"] == "context") {
                auto unit = new Iir_Context_Declaration;
                unit->Location.Pos = unit_item["Location"]["Pos"];
                unit->Location.Line = unit_item["Location"]["Line"];
                unit->Location.Line_Pos = unit_item["Location"]["Line_Pos"];
                unit->Identifier = unit_item["name"];
                var = unit;
            }
        }
        //TODO: dFile["timestamp"] =
        //TODO: Check file modification and also check if file name is unique;
        f->Library = Library;
        Library->Design_Files.push_back(f);
    }
/*
    for (auto &&designFile : Library->Design_Files) {
        //  Ignore std.standard as there is no corresponding file.
        if(!designFile->Design_Units.empty()){
            nlohmann::json dFile;
            dFile["directory"] = designFile->Design_File_Directory;
            //TODO: dFile["crc32"] =
            //TODO: dFile["timestamp"] =
            dFile["design_units"] = nlohmann::json::array();
            for (auto &&designUnit  : designFile->Design_Units) {
                dFile["design_units"].push_back(std::visit(Save_Design_Unit{}, designUnit));
            }
            data[designFile->Design_File_Filename] = dFile;
        }
    }*/
}
/*
procedure Create_Virtual_Locations
is
        use Files_Map;
Implicit_Source_File : Source_File_Entry;
Command_Source_File : Source_File_Entry;
begin
        Implicit_Source_File = Create_Virtual_Source_File("*implicit*");
Command_Source_File = Create_Virtual_Source_File("*command line*");
Command_Line_Location = Source_File_To_Location (Command_Source_File);
Implicit_Location = Source_File_To_Location (Implicit_Source_File);
end Create_Virtual_Locations;
*/
/*
// Note: the scanner shouldn't be in use, since this procedure uses it.
void packageHandler::Load_Std_Library (bool Build_Standard = true) {
// NOTES:        use Std_Package;

    if (!Libraries_List.empty())
//  This procedure must not be called twice.
throw std::logic_error("cannot call twice");

//TODO: state.options.Create_Flag_String;
//Create_Virtual_Locations;

Std_Package.Create_First_Nodes;

//  Create the library.
Std_Library = Create_Iir (Iir_Kind_Library_Declaration);
Set_Identifier (Std_Library, Std_Names.Name_Std);
Set_Location (Std_Library, Implicit_Location);
Libraries_List = Std_Library;
Libraries_List_Last = Std_Library;

if Build_Standard then
Create_Std_Standard_Package (Std_Library);
Add_Unit_Hash (Std_Standard_Unit);
end if;

if state.options.Bootstrap
        and then Work_Library_Name = Std_Names.Name_Std
then
        Dir = Work_Directory;
else
Dir = Null_Identifier;
end if;
Set_Library_Directory (Std_Library, Dir);
if Load_Library (Std_Library) = False
        and then not state.options.Bootstrap
        then
Error_Msg_Option ("cannot find ""std"" library");
end if;

if Build_Standard then
//  Add the standard_file into the library.
//  This is done after Load_Library, because it checks there is no
//  previous files in the library.
Set_Parent (Std_Standard_File, Std_Library);
Set_Chain (Std_Standard_File, Get_Design_File_Chain (Std_Library));
Set_Design_File_Chain (Std_Library, Std_Standard_File);
end if;

Set_Visible_Flag (Std_Library, True);
end Load_Std_Library;

procedure Load_Work_Library (Empty : Boolean = False)
is
        use Std_Names;
begin
if Work_Library_Name = Name_Std then
if not state.options.Bootstrap then
Error_Msg_Option ("the WORK library cannot be STD");
return;
end if;
Work_Library = Std_Library;
else
Work_Library = Create_Iir (Iir_Kind_Library_Declaration);
Set_Location (Work_Library, Implicit_Location);
//Set_Visible_Flag (Work_Library, True);
Set_Library_Directory (Work_Library, Work_Directory);

Set_Identifier (Work_Library, Work_Library_Name);

if not Empty then
if Load_Library (Work_Library) = False then
        null;
end if;
end if;

//  Add it to the list of libraries.
Set_Chain (Libraries_List_Last, Work_Library);
Libraries_List_Last = Work_Library;
end if;
Set_Visible_Flag (Work_Library, True);
end Load_Work_Library;

// Get or create a library from an identifier.
function Get_Library (Ident: Name_Id; Loc : Location_Type)
return Iir_Library_Declaration
        is
Library: Iir_Library_Declaration;
begin
//  The library work is a little bit special.
if Ident = Std_Names.Name_Work or else Ident = Work_Library_Name then
//  load_work_library must have been called before.
pragma Assert (Work_Library /= Null_Iir);
return Work_Library;
end if;

//  Check if the library has already been loaded.
Library = Iirs_Utils.Find_Name_In_Chain (Libraries_List, Ident);
if Library /= Null_Iir then
return Library;
end if;

//  This is a new library.
if Ident = Std_Names.Name_Std then
//  Load_std_library must have been called before.
raise Internal_Error;
end if;

Library = Create_Iir (Iir_Kind_Library_Declaration);
Set_Location (Library, Loc);
Set_Library_Directory (Library, Null_Identifier);
Set_Identifier (Library, Ident);
if Load_Library (Library) = False then
        Error_Msg_Sem (+Loc, "cannot find resource library %i", +Ident);
end if;
Set_Visible_Flag (Library, True);

Set_Chain (Libraries_List_Last, Library);
Libraries_List_Last = Library;

return Library;
end Get_Library;


// Return TRUE if LIBRARY_UNIT and UNIT have identifiers for the same
// design unit identifier.
// eg: 'entity A' and 'package A' returns TRUE.
bool Is_Same_Library_Unit (Iir* Library_Unit, Iir* Unit) return Boolean
        is
Entity_Name1, Entity_Name2: Name_Id;
Library_Unit_Kind, Unit_Kind : Iir_Kind;
begin
if Get_Identifier (Unit) /= Get_Identifier (Library_Unit) then
return False;
end if;

Library_Unit_Kind = Get_Kind (Library_Unit);
Unit_Kind = Get_Kind (Unit);

//  Package and package body are never the same library unit.
if Library_Unit_Kind = Iir_Kind_Package_Declaration
        and then Unit_Kind = Iir_Kind_Package_Body
then
return False;
end if;
if Unit_Kind = Iir_Kind_Package_Declaration
        and then Library_Unit_Kind = Iir_Kind_Package_Body
then
return False;
end if;

//  Two architecture declarations are identical only if they also have
//  the same entity name.
if Unit_Kind = Iir_Kind_Architecture_Body
        and then Library_Unit_Kind = Iir_Kind_Architecture_Body
then
        Entity_Name1 = Get_Entity_Identifier_Of_Architecture (Unit);
Entity_Name2 = Get_Entity_Identifier_Of_Architecture (Library_Unit);
if Entity_Name1 /= Entity_Name2 then
return False;
end if;
end if;

//  An architecture declaration never conflits with a library unit that
//  is not an architecture declaration.
if (Unit_Kind = Iir_Kind_Architecture_Body
        and then Library_Unit_Kind /= Iir_Kind_Architecture_Body)
or else
(Unit_Kind /= Iir_Kind_Architecture_Body
        and then Library_Unit_Kind = Iir_Kind_Architecture_Body)
then
return False;
end if;

return True;
end Is_Same_Library_Unit;

//  This procedure is called when the DESIGN_UNIT (either the stub created when a library is read or created
// from a previous unit in a source file) has been replaced by a new unit.  Free everything but DESIGN_UNIT, has
// it may be referenced in other units (dependence...)
//  FIXME: Isn't the library unit also referenced too ?
procedure Free_Design_Unit (Design_Unit : Iir_Design_Unit)
is
        Lib : Iir;
Unit : Iir_Design_Unit;
Dep_List : Iir_List;
begin
//  Free dependence list.
Dep_List = Get_Dependence_List (Design_Unit);
Destroy_Iir_List (Dep_List);
Set_Dependence_List (Design_Unit, Null_Iir_List);

//  Free default configuration of architecture (if any).
Lib = Get_Library_Unit (Design_Unit);
if Lib /= Null_Iir
        and then Get_Kind (Lib) = Iir_Kind_Architecture_Body
then
        Free_Iir (Get_Entity_Name (Lib));
Unit = Get_Default_Configuration_Declaration (Lib);
if Unit /= Null_Iir then
Free_Design_Unit (Unit);
end if;
end if;

//  Free library unit.
Free_Iir (Lib);
Set_Library_Unit (Design_Unit, Null_Iir);
end Free_Design_Unit;
*/
void Remove_Unit_From_File(Iir_Design_Unit* Unit_Ref,Iir_Design_File* File) {
    auto iter = std::find(File->Design_Units.begin(), File->Design_Units.end(), Unit_Ref);
    assert(iter != File->Design_Units.end());
    File->Design_Units.erase(iter);
}
/*
// Add or replace a design unit in the working library.
void packageHandler::Add_Design_Unit_Into_Library(Iir_Design_Unit* Unit, bool Keep_Obsolete = false) {
//        Design_File: Iir_Design_File;
//Design_Unit, Prev_Design_Unit : Iir_Design_Unit;
//Last_Unit : Iir_Design_Unit;
//Library_Unit: Iir;
//New_Library_Unit: Iir;
//Unit_Id : Name_Id;
//Date: Date_Type;
//New_Lib_Checksum : File_Checksum_Id;
//Id : Hash_Id;
//
////  File name and dir name of DECL.
//File_Name : Name_Id;
//Dir_Name : Name_Id;

//  As specified, the Chain must be not set.
    assert(Unit->Chain == nullptr);

//  The unit must not be in the library.
    assert(Unit->Date_State == Date_Extern);

//  Mark this design unit as being loaded.
auto New_Library_Unit = Unit->Library_Unit;

//Unit_Id = Get_Identifier (New_Library_Unit);

//  Set the date of the design unit as the most recently analyzed
//  design unit.

if (Unit->Date == 4) {
    Unit->Date_State = Date_Parse;
}
    else if (Unit->Date == 6) {
    auto Date = ++(Work_Library->Date);
    Unit->Date = Date;
    Unit->Date_State = Date_Analyze;
}
    else if (Unit->Date > 10) {
    throw std::logic_error("this 10");
}
    else
    throw std::logic_error("this bad");

//  Set file time stamp.
    /*
declare
        File : Source_File_Entry;
Pos : Source_Ptr;
begin
    std::filesystem::path = New_Library_Unit->Location.filePath;
        Files_Map.Location_To_File_Pos (Get_Location (New_Library_Unit),
        File, Pos);
New_Lib_Checksum = Files_Map.Get_File_Checksum (File);
File_Name = Files_Map.Get_File_Name (File);
Image (File_Name);
if GNAT.OS_Lib.Is_Absolute_Path (Nam_Buffer (1 .. Nam_Length)) then
        Dir_Name = Null_Identifier;
else
Dir_Name = Files_Map.Get_Home_Directory;
end if;
end;

//  Try to find a design unit with the same name in the work library.

    Design_Unit = Primary_Units.find(Unit->Identifier);
Prev_Design_Unit = Null_Iir;
while Design_Unit /= Null_Iir loop
Design_File = Get_Design_File (Design_Unit);
Library_Unit = Get_Library_Unit (Design_Unit);
if Get_Identifier (Design_Unit) = Unit_Id
        and then Get_Library (Design_File) = Work_Library
        and then Is_Same_Library_Unit (New_Library_Unit, Library_Unit)
then
//  LIBRARY_UNIT and UNIT designate the same design unit.
//  Remove the old one.
Set_Date (Design_Unit, Date_Obsolete);
declare
        Next_Design : Iir;
begin
//  Remove DESIGN_UNIT from the unit_hash.
Next_Design = Get_Hash_Chain (Design_Unit);
if Prev_Design_Unit = Null_Iir then
        Primary_Units (Id) = Next_Design;
else
Set_Hash_Chain (Prev_Design_Unit, Next_Design);
end if;

//  Remove DESIGN_UNIT from the design_file.
//  If KEEP_OBSOLETE is True, units that are obsoleted by units
//  in the same design file are kept.  This allows to process
//  (pretty print, xrefs, ...) all units of a design file.
//  But still remove units that are replaced (if a file was
//  already in the library).
if not Keep_Obsolete
or else Get_Date_State (Design_Unit) = Date_Disk
then
        Remove_Unit_From_File (Design_Unit, Design_File);

Set_Chain (Design_Unit, Obsoleted_Design_Units);
Obsoleted_Design_Units = Design_Unit;
end if;
end;

//  UNIT *must* replace library_unit if they don't belong
//  to the same file.
if Get_Design_File_Filename (Design_File) = File_Name
        and then Get_Design_File_Directory (Design_File) = Dir_Name
then
//  In the same file.
if Get_Date_State (Design_Unit) = Date_Analyze then
//  Warns only if we are not re-analyzing the file.
if Is_Warning_Enabled (Warnid_Library) then
        Warning_Msg_Sem
        (Warnid_Library, +Unit,
                "redefinition of a library unit in "
                        & "same design file:");
Warning_Msg_Sem
(Warnid_Library, +Unit, "%n defined at %l is now %n",
(+Library_Unit, +Library_Unit, +New_Library_Unit));
end if;
else
//  Free the stub.
if not Keep_Obsolete then
Free_Design_Unit (Design_Unit);
end if;
end if;

//  Note: the current design unit should not be freed if
//  in use; unfortunatly, this is not obvious to check.
else
if Is_Warning_Enabled (Warnid_Library) then
if Get_Kind (Library_Unit) /= Get_Kind (New_Library_Unit)
then
        Warning_Msg_Sem
        (Warnid_Library, +Unit,
                "changing definition of a library unit:");
Warning_Msg_Sem
(Warnid_Library, +Unit,
"%n is now %n", (+Library_Unit, +New_Library_Unit));
end if;
Warning_Msg_Sem
(Warnid_Library, +Unit,
"library unit %i was also defined in file %i",
(+Library_Unit, +Get_Design_File_Filename (Design_File)));
end if;
end if;
exit;
else
Prev_Design_Unit = Design_Unit;
Design_Unit = Get_Hash_Chain (Design_Unit);
end if;
end loop;

//  Try to find the design file in the library.
//  First try the last one found.
if Last_Design_File /= Null_Iir
        and then Get_Library (Last_Design_File) = Work_Library
        and then Get_Design_File_Filename (Last_Design_File) = File_Name
        and then Get_Design_File_Directory (Last_Design_File) = Dir_Name
then
        Design_File = Last_Design_File;
else
//  Search.
Design_File = Get_Design_File_Chain (Work_Library);
while Design_File /= Null_Iir loop
if Get_Design_File_Filename (Design_File) = File_Name
        and then Get_Design_File_Directory (Design_File) = Dir_Name
then
        exit;
end if;
Design_File = Get_Chain (Design_File);
end loop;
Last_Design_File = Design_File;
end if;

if Design_File /= Null_Iir
        and then not Files_Map.Is_Eq (New_Lib_Checksum,
        Get_File_Checksum (Design_File))
then
// FIXME: this test is not enough: what about reanalyzing
//  unmodified files (this works only because the order is not
//  changed).
// Design file is updated.
// Outdate all other units, overwrite the design_file.
Set_File_Checksum (Design_File, New_Lib_Checksum);
Design_Unit = Get_First_Design_Unit (Design_File);
while Design_Unit /= Null_Iir loop
if Design_Unit /= Unit then
//  Mark other design unit as obsolete.
Set_Date (Design_Unit, Date_Obsolete);
Remove_Unit_Hash (Design_Unit);
else
raise Internal_Error;
end if;
Prev_Design_Unit = Design_Unit;
Design_Unit = Get_Chain (Design_Unit);

Set_Chain (Prev_Design_Unit, Obsoleted_Design_Units);
Obsoleted_Design_Units = Prev_Design_Unit;
end loop;
Set_First_Design_Unit (Design_File, Null_Iir);
Set_Last_Design_Unit (Design_File, Null_Iir);
end if;

if Design_File = Null_Iir then
// This is the first apparition of the design file.
Design_File = Create_Iir (Iir_Kind_Design_File);
Location_Copy (Design_File, Unit);

Set_Design_File_Filename (Design_File, File_Name);
Set_Design_File_Directory (Design_File, Dir_Name);

Set_File_Checksum (Design_File, New_Lib_Checksum);
Set_Parent (Design_File, Work_Library);
Set_Chain (Design_File, Get_Design_File_Chain (Work_Library));
Set_Design_File_Chain (Work_Library, Design_File);
end if;

//  Add DECL to DESIGN_FILE.
Last_Unit = Get_Last_Design_Unit (Design_File);
if Last_Unit = Null_Iir then
if Get_First_Design_Unit (Design_File) /= Null_Iir then
raise Internal_Error;
end if;
Set_First_Design_Unit (Design_File, Unit);
else
if Get_First_Design_Unit (Design_File) = Null_Iir then
        raise Internal_Error;
end if;
Set_Chain (Last_Unit, Unit);
end if;
Set_Last_Design_Unit (Design_File, Unit);
Set_Design_File (Unit, Design_File);

//  Add DECL in unit hash table.
Set_Hash_Chain (Unit, Primary_Units (Id));
Primary_Units (Id) = Unit;

//  Update the analyzed time stamp.
Set_Analysis_Time_Stamp (Design_File, Files_Map.Get_Os_Time_Stamp);
end Add_Design_Unit_Into_Library;
*/

void packageHandler::Add_Design_File_Into_Library (Iir_Design_File* File) {
    for (auto&& unit : File->Design_Units) {
        Add_Design_Unit_Into_Library(unit, true);
    }
}

struct Save_Design_Unit {

    nlohmann::json operator()(Iir_Entity_Declaration* unit) {
        nlohmann::json result;
        result["type"] = "entity";
        result["name"] = unit->Identifier;
        result["Location"]["Line"] = unit->Location.Line;
        result["Location"]["Line_Pos"] = unit->Location.Line_Pos;
        result["Location"]["Pos"] = unit->Location.Pos;
        return result;

    }

    nlohmann::json operator()(Iir_Architecture_Body* unit) {
        nlohmann::json result;
        result["type"] = "architecture";
        result["name"] = unit->Identifier;
        result["entity"] = unit->Entity_Name;
        result["Location"]["Line"] = unit->Location.Line;
        result["Location"]["Line_Pos"] = unit->Location.Line_Pos;
        result["Location"]["Pos"] = unit->Location.Pos;
        return result;

    }

    nlohmann::json operator()(Iir_Package_Declaration* unit) {
        nlohmann::json result;
        result["type"] = "package";
        result["name"] = unit->Identifier;
        result["body"] = unit->Need_Body;
        result["Location"]["Line"] = unit->Location.Line;
        result["Location"]["Line_Pos"] = unit->Location.Line_Pos;
        result["Location"]["Pos"] = unit->Location.Pos;
        return result;

    }

    nlohmann::json operator()(Iir_Package_Instantiation_Declaration* unit) {
        nlohmann::json result;
        result["type"] = "package_body";
        result["name"] = unit->Identifier;
        result["Location"]["Line"] = unit->Location.Line;
        result["Location"]["Line_Pos"] = unit->Location.Line_Pos;
        result["Location"]["Pos"] = unit->Location.Pos;
        return result;

    }

    nlohmann::json operator()(Iir_Package_Body* unit) {
        nlohmann::json result;
        result["type"] = "package_body";
        result["name"] = unit->Identifier;
        result["Location"]["Line"] = unit->Location.Line;
        result["Location"]["Line_Pos"] = unit->Location.Line_Pos;
        result["Location"]["Pos"] = unit->Location.Pos;
        return result;

    }

    nlohmann::json operator()(Iir_Configuration_Declaration* unit) {
        nlohmann::json result;
        result["type"] = "configuration";
        result["name"] = unit->Identifier;
        result["Location"]["Line"] = unit->Location.Line;
        result["Location"]["Line_Pos"] = unit->Location.Line_Pos;
        result["Location"]["Pos"] = unit->Location.Pos;
        return result;

    }

    nlohmann::json operator()(Iir_Context_Declaration* unit) {
        nlohmann::json result;
        result["type"] = "context";
        result["name"] = unit->Identifier;
        result["Location"]["Line"] = unit->Location.Line;
        result["Location"]["Line_Pos"] = unit->Location.Line_Pos;
        result["Location"]["Pos"] = unit->Location.Pos;
        return result;

    }
};
/*
// Save the file map of library LIBRARY.
void packageHandler::Save_Library(Iir_Library_Declaration* Library) {
    nlohmann::json data;
    data["version"] = 1;
    for (auto &&designFile : Library->Design_Files) {
        //  Ignore std.standard as there is no corresponding file.
        if(!designFile->Design_Units.empty()){
            nlohmann::json dFile;
            dFile["directory"] = designFile->Design_File_Directory;
            //TODO: dFile["crc32"] =
            //TODO: dFile["timestamp"] =
            dFile["design_units"] = nlohmann::json::array();
            for (auto &&designUnit  : designFile->Design_Units) {
                dFile["design_units"].push_back(std::visit(Save_Design_Unit{}, designUnit));
            }
            data[designFile->Design_File_Filename] = dFile;
        }
    }
    std::ofstream o(Library_To_File_Name(Library));
    o << std::setw(4) << data << std::endl;
}

// Save the map of the work library.
inline void packageHandler::Save_Work_Library() {
    Save_Library(Work_Library);
}

// Return the name of the latest architecture analysed for an entity.
function Get_Latest_Architecture (Entity: Iir_Entity_Declaration)
return Iir_Architecture_Body
        is
Entity_Id : Name_Id;
Lib : Iir_Library_Declaration;
Design_File: Iir_Design_File;
Design_Unit: Iir_Design_Unit;
Library_Unit: Iir;
Res: Iir_Design_Unit;
begin
//  FIXME: use hash
        Entity_Id = Get_Identifier (Entity);
Lib = Get_Library (Get_Design_File (Get_Design_Unit (Entity)));
Design_File = Get_Design_File_Chain (Lib);
Res = Null_Iir;
while Design_File /= Null_Iir loop
Design_Unit = Get_First_Design_Unit (Design_File);
while Design_Unit /= Null_Iir loop
Library_Unit = Get_Library_Unit (Design_Unit);

if Get_Kind (Library_Unit) = Iir_Kind_Architecture_Body
        and then
Get_Entity_Identifier_Of_Architecture (Library_Unit) = Entity_Id
then
if Res = Null_Iir then
        Res = Design_Unit;
elsif Get_Date (Design_Unit) > Get_Date (Res) then
        Res = Design_Unit;
end if;
end if;
Design_Unit = Get_Chain (Design_Unit);
end loop;
Design_File = Get_Chain (Design_File);
end loop;
if Res = Null_Iir then
return Null_Iir;
else
return Get_Library_Unit (Res);
end if;
end Get_Latest_Architecture;

function Load_File (File : Source_File_Entry) return Iir_Design_File
        is
Res : Iir_Design_File;
begin
        Scanner.Set_File (File);
if Scanner.Detect_Encoding_Errors then
//  Don't even try to parse such a file.  The BOM will be interpreted
//  as an identifier, which is not valid at the beginning of a file.
Res = Null_Iir;
else
Res = Parse.Parse_Design_File;
end if;
Scanner.Close_File;

if Res /= Null_Iir then
Set_Parent (Res, Work_Library);
Set_Design_File_Filename (Res, Files_Map.Get_File_Name (File));
end if;
return Res;
end Load_File;

// parse a file.
// Return a design_file without putting it into the library
// (because it was not analyzed).
function Load_File (File_Name: Name_Id) return Iir_Design_File
        is
Fe : Source_File_Entry;
begin
        Fe = Files_Map.Load_Source_File (Local_Directory, File_Name);
if Fe = No_Source_File_Entry then
        Error_Msg_Option ("cannot open " & Image (File_Name));
return Null_Iir;
end if;
return Load_File (Fe);
end Load_File;

function Find_Design_Unit (Unit : Iir) return Iir_Design_Unit is
begin
case Get_Kind (Unit) is
        when Iir_Kind_Design_Unit =>
return Unit;
when Iir_Kind_Selected_Name =>
declare
        Lib : Iir_Library_Declaration;
begin
        Lib = Get_Library (Get_Identifier (Get_Prefix (Unit)),
        Get_Location (Unit));
return Find_Primary_Unit (Lib, Get_Identifier (Unit));
end;
when Iir_Kind_Entity_Aspect_Entity =>
return Find_Secondary_Unit
(Get_Design_Unit (Get_Entity (Unit)),
        Get_Identifier (Get_Architecture (Unit)));
when others =>
Error_Kind ("find_design_unit", Unit);
end case;
end Find_Design_Unit;

function Is_Obsolete (Design_Unit : Iir_Design_Unit; Loc : Iir)
return Boolean
        is
procedure Error_Obsolete (Msg : String; Arg1 : Earg_Type) is
        begin
if not state.options.Flag_Elaborate_With_Outdated then
if Loc = Null_Iir then
        Error_Msg_Sem (Command_Line_Location, Msg, Arg1);
else
Error_Msg_Sem (+Loc, Msg, Arg1);
end if;
end if;
end Error_Obsolete;

procedure Error_Obsolete (Msg : String; Args : Earg_Arr) is
        begin
if not state.options.Flag_Elaborate_With_Outdated then
if Loc = Null_Iir then
        Error_Msg_Sem (Command_Line_Location, Msg, Args);
else
Error_Msg_Sem (+Loc, Msg, Args);
end if;
end if;
end Error_Obsolete;

List : Iir_List;
El : Iir;
Unit : Iir_Design_Unit;
U_Ts : Time_Stamp_Id;
Du_Ts : Time_Stamp_Id;
begin
if Get_Date (Design_Unit) = Date_Obsolete then
        Error_Obsolete ("%n is obsolete", +Design_Unit);
return True;
end if;
List = Get_Dependence_List (Design_Unit);
if List = Null_Iir_List then
return False;
end if;
Du_Ts = Get_Analysis_Time_Stamp (Get_Design_File (Design_Unit));
for I in Natural loop
El = Get_Nth_Element (List, I);
exit when El = Null_Iir;
Unit = Find_Design_Unit (El);
if Unit /= Null_Iir then
U_Ts = Get_Analysis_Time_Stamp (Get_Design_File (Unit));
if Files_Map.Is_Gt (U_Ts, Du_Ts) then
        Error_Obsolete ("%n is obsoleted by %n", (+Design_Unit, +Unit));
return True;
elsif Is_Obsolete (Unit, Loc) then
        Error_Obsolete ("%n depends on obsolete unit", +Design_Unit);
return True;
end if;
end if;
end loop;
return False;
end Is_Obsolete;
*/
void packageHandler::Finish_Compilation(Iir_Design_Unit* Unit, bool Main = false) {
    auto Lib_Unit = Unit->Library_Unit;

    if (Main || state.options.Dump_All || state.options.Dump_Parse) {
//    Disp_Tree.Disp_Tree(Unit);
    }

    if (state.options.Check_Ast_Level > 0) {
//    Nodes_GC.Check_Tree(Unit);
    }

//TODO:
//if (state.options.Verbose) then
//Report_Msg (Msgid_Note, Semantic, +Lib_Unit,
//"analyze %n", (1 => +Lib_Unit));
//end if;

//FIXME: Sem.Semantic (Unit);

    if ((Main or state.options.Dump_All) && state.options.Dump_Sem) {
//    Disp_Tree.Disp_Tree(Unit);
    }


//FIXME:
//if Errorout.Nbr_Errors > 0 then
//        raise Compilation_Error;
//end if;

    if ((Main or state.options.List_All) && state.options.List_Sem) {
//        Disp_Vhdl.Disp_Vhdl(Unit);
    }

    if (state.options.Check_Ast_Level > 0) {
//    Nodes_GC.Check_Tree(Unit);
    }

//  Post checks
//--------------

//FIXME: Post_Sems.Post_Sem_Checks (Unit);

//  Canonalisation.
//----------------

//FIXME:
//if state.options.Verbose then
//Report_Msg (Msgid_Note, Semantic, +Lib_Unit,
//"canonicalize %n", (1 => +Lib_Unit));
//}

//FIXME: Canon.Canonicalize (Unit);

    if ((Main or state.options.Dump_All) && state.options.Dump_Canon) {
//Disp_Tree.Disp_Tree (Unit);
    }
//TODO:
//if (Errorout.Nbr_Errors > 0 then
//        raise Compilation_Error;
//}

    if ((Main or state.options.List_All) && state.options.List_Canon) {
//Disp_Vhdl.Disp_Vhdl (Unit);
    }

    if (state.options.Check_Ast_Level > 0) {
//        Nodes_GC.Check_Tree (Unit);
    }
}


void packageHandler::Load_Parse_Design_Unit(Iir_Design_Unit* Design_Unit, Iir* Loc) {
    assert (Design_Unit->Date_State == Date_Disk);

//  Load the file in memory.
    auto Design_File = Design_Unit->Design_File;
    std::ifstream ifs(Design_File->Design_File_Directory / Design_File->Design_File_Filename);
    if (ifs.fail())
        CompilationError("cannot load " + Design_Unit->Library_Unit->Identifier);

    if (Design_File->File_Checksum != Calculate_Checksum(ifs))
        CompilationError("file " + Design_File->Design_File_Filename + "has changed and must be reanalysed");
    else if (Design_Unit->Date == 0)
        SemanticError(Design_Unit->Design_File->Design_File_Filename + "is not anymore its source file");
    ifs.close();

//  Set the position of the lexer
    Parser parser(state, Design_Unit->Source_Pos);
    auto result = parser.Parse_Design_Unit(Design_Unit->Source_Pos);

    if (!result)
        throw CompilationError("");

    Design_Unit->Date_State = Date_Parse;

//  FIXME: check the library unit read is the one expected.

//  Move the unit in the library: keep the design_unit of the library,
//  but replace the library_unit by the one that has been parsed.  Do
//  not forget to relocate parents.
//FIXME: Iirs_Utils.Free_Recursive (Get_Library_Unit (Design_Unit));
    Design_Unit->Library_Unit = result->Library_Unit;
    //FIXME:
    result->Library_Unit->Parent_Design_Unit = Design_Unit;
//Set_Design_Unit (Get_Library_Unit (result), Design_Unit);
//Set_Parent (Get_Library_Unit (result), Design_Unit);

    for (auto&& contextItem : result->Context_Items) {
        Design_Unit->Context_Items.push_back(contextItem);
    }

    Design_Unit->Location = result->Location;

    for (auto&& item :Design_Unit->Dependence_List) {
        delete item;
    }

    Design_Unit->Dependence_List = result->Dependence_List;

    delete result;
}

// Load, parse, analyze, back-end a design_unit if necessary.

void packageHandler::Load_Design_Unit (Iir_Design_Unit* Design_Unit, Iir* Loc) {

    if (Design_Unit->Date_State == Date_Disk)
        Load_Parse_Design_Unit(Design_Unit, Loc);

    if (Design_Unit->Date_State == Date_Parse) {
//  Analyze the design unit.

        if (Design_Unit->Date = Date_Analyzed) {
//  Work-around for an internal check in sem.
//  FIXME: to be removed ?
            Design_Unit->Date = 4;
        }

//  Avoid infinite recursion, if the unit is self-referenced.
        Design_Unit->Date_State = Date_Analyze;

//  Disable all warnings.  Warnings are emitted only when the unit
//  is analyzed.
//FIXME: Save_Warnings_Setting (Warnings);
        Disable_All_Warnings;

//  Analyze unit.
        Finish_Compilation(Design_Unit);

//  Restore warnings.
//FIXME: Restore_Warnings_Setting (Warnings);
    }

    switch (Design_Unit->Date) {
    case 4:
        throw std::logic_error("");
    case 5:
//  Self-referenced unit.
        return;
    case 6:
//  FIXME: Accept it silently ?
//  Note: this is used when Flag_Elaborate_With_Outdated is set.
//  This is also used by anonymous configuration declaration.
        break;
    case 7:
        return;
    case 0:
        if (!state.options.Flag_Elaborate_With_Outdated)
            SemanticError(Design_Unit->Identifier + " is obsolete");
        return;
    default:
        if (Design_Unit->Date > 10)
            break;
        else
            throw std::logic_error("");
    }

    if (!state.options.Flag_Elaborate_With_Outdated && Is_Obsolete(Design_Unit, Loc))
        Design_Unit->Date = 0;
}

//  Return the declaration of primary unit NAME of LIBRARY.
Iir_Design_Unit* packageHandler::Find_Primary_Unit(Iir_Library_Declaration Library, std::string Name) {
    auto unit = Primary_Units.find(Name);

    if(unit == Primary_Units.end())
        return nullptr;
    else return unit->second;
}

Iir_Design_Unit* packageHandler::Load_Primary_Unit(Iir_Library_Declaration Library, std::string Name, Iir* Loc) {
    auto Design_Unit = Find_Primary_Unit(Library, Name);
    if (Design_Unit != nullptr)
        Load_Design_Unit(Design_Unit, Loc);
    return Design_Unit;
}


// Return the declaration of secondary unit NAME for PRIMARY, or null if
// not found.

Iir_Design_Unit* packageHandler::Find_Secondary_Unit(Iir_Design_Unit* Primary, std::string Name) {
    auto unit = Secondary_Units.find(Primary->Identifier);
    assert(unit->Design_File->Library == Primary->Design_File->Library);
    assert(unit->Library_Unit->Identifier == Name);
    return unit->second;
}
;

// Load an secondary unit and analyse it.
Iir_Design_Unit* packageHandler::Load_Secondary_Unit(Iir_Design_Unit* Primary, std::string Name, Iir* Loc) {
    auto Design_Unit = Find_Secondary_Unit (Primary, Name);
    if (Design_Unit)
            Load_Design_Unit (Design_Unit, Loc);
    return Design_Unit;
}

Iir_Design_Unit Find_Entity_For_Component(std::string Name) {
    if(auto a = Primary_Units.find(Name); a != Primary_Units.end()) {
        return a->second;
    }
    else
        return nullptr;
}

std::vector<Iir_Library_Declaration*> packageHandler::Get_Libraries_List () {
    return Libraries_List;
}

