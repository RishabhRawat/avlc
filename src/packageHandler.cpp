//
// Created by scalpel on 12/3/17.
//

#include <CompilerExceptions.h>
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
//with Flags;
//with Std_Package;
//with Disp_Tree;
//with Disp_Vhdl;
//with Sem;
//with Post_Sems;
//with Canon;
//with Nodes_GC;
//

//  Chain of known libraries.  This is also the top node of all iir node.
Libraries_List : Iir_Library_Declaration := Null_Iir;
Libraries_List_Last : Iir_Library_Declaration := Null_Iir;

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


function Path_To_Id (Path : String) return Name_Id is
begin
if Path (Path'Last) /= GNAT.OS_Lib.Directory_Separator then
return Get_Identifier (Path & GNAT.OS_Lib.Directory_Separator);
else
return Get_Identifier (Path);
end if;
end Path_To_Id;

void packageHandler::Add_Library_Path(std::filesystem::path Path) {
    paths.push_back(Path);
}

// Transform a library identifier into a file name. Very simple mechanism: just add '-objVV.cf' extension,
// where VV is the version.

std::string packageHandler::Library_To_File_Name(Iir_Library_Declaration* Library) {
    switch (options.standard) {
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
    switch (options.standard) {
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
        Warning("Directory '" + Path + "' set by --workdir= does not exist", options.Warnid_Library);
    Work_Directory = Path;
}

//  Every design unit is put in this hash table to be quickly found by
//  its (primary) identifier.
Unit_Hash_Length : constant Name_Id := 127;
subtype Hash_Id is Name_Id range 0 .. Unit_Hash_Length - 1;
Unit_Hash_Table : array (Hash_Id) of Iir := (others => Null_Iir);

//  Get the hash value for DESIGN_UNIT.
//  Architectures use the entity name.
function Get_Hash_Id_For_Unit (Design_Unit : Iir_Design_Unit)
return Hash_Id
        is
Lib_Unit : Iir;
Id : Name_Id;
begin
        Lib_Unit := Get_Library_Unit (Design_Unit);
case Get_Kind (Lib_Unit) is
        when Iir_Kind_Entity_Declaration
| Iir_Kind_Configuration_Declaration
| Iir_Kind_Package_Declaration
| Iir_Kind_Package_Body
| Iir_Kind_Package_Instantiation_Declaration
| Iir_Kind_Context_Declaration =>
Id := Get_Identifier (Lib_Unit);
when Iir_Kind_Architecture_Body =>
//  Architectures are put with the entity identifier.
Id := Get_Entity_Identifier_Of_Architecture (Lib_Unit);
when others =>
Error_Kind ("get_Hash_Id_For_Unit", Lib_Unit);
end case;
return Id mod Unit_Hash_Length;
end Get_Hash_Id_For_Unit;

//  Put DESIGN_UNIT into the unit hash table.
procedure Add_Unit_Hash (Design_Unit : Iir)
is
        Id : Hash_Id;
begin
        Id := Get_Hash_Id_For_Unit (Design_Unit);
Set_Hash_Chain (Design_Unit, Unit_Hash_Table (Id));
Unit_Hash_Table (Id) := Design_Unit;
end Add_Unit_Hash;

//  Remove DESIGN_UNIT from the unit hash table.
procedure Remove_Unit_Hash (Design_Unit : Iir)
is
        Id : Hash_Id;
Unit, Prev, Next : Iir_Design_Unit;
begin
        Id := Get_Hash_Id_For_Unit (Design_Unit);
Unit := Unit_Hash_Table (Id);
Prev := Null_Iir;
while Unit /= Null_Iir loop
Next := Get_Hash_Chain (Unit);
if Unit = Design_Unit then
if Prev = Null_Iir then
        Unit_Hash_Table (Id) := Next;
else
Set_Hash_Chain (Prev, Next);
end if;
return;
end if;
Prev := Unit;
Unit := Next;
end loop;
//  Not found.
raise Internal_Error;
end Remove_Unit_Hash;

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
        Implicit_Source_File := Create_Virtual_Source_File
        (Get_Identifier ("*implicit*"));
Command_Source_File := Create_Virtual_Source_File
        (Get_Identifier ("*command line*"));
Command_Line_Location := Source_File_To_Location (Command_Source_File);
Implicit_Location := Source_File_To_Location (Implicit_Source_File);
end Create_Virtual_Locations;
*/
// Note: the scanner shouldn't be in use, since this procedure uses it.
void packageHandler::Load_Std_Library (bool Build_Standard = true) {
// NOTES:        use Std_Package;

    if (!Libraries_List.empty())
//  This procedure must not be called twice.
throw std::logic_error("cannot call twice");

//TODO: Flags.Create_Flag_String;
//Create_Virtual_Locations;

Std_Package.Create_First_Nodes;

//  Create the library.
Std_Library := Create_Iir (Iir_Kind_Library_Declaration);
Set_Identifier (Std_Library, Std_Names.Name_Std);
Set_Location (Std_Library, Implicit_Location);
Libraries_List := Std_Library;
Libraries_List_Last := Std_Library;

if Build_Standard then
Create_Std_Standard_Package (Std_Library);
Add_Unit_Hash (Std_Standard_Unit);
end if;

if Flags.Bootstrap
        and then Work_Library_Name = Std_Names.Name_Std
then
        Dir := Work_Directory;
else
Dir := Null_Identifier;
end if;
Set_Library_Directory (Std_Library, Dir);
if Load_Library (Std_Library) = False
        and then not Flags.Bootstrap
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

procedure Load_Work_Library (Empty : Boolean := False)
is
        use Std_Names;
begin
if Work_Library_Name = Name_Std then
if not Flags.Bootstrap then
Error_Msg_Option ("the WORK library cannot be STD");
return;
end if;
Work_Library := Std_Library;
else
Work_Library := Create_Iir (Iir_Kind_Library_Declaration);
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
Libraries_List_Last := Work_Library;
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
Library := Iirs_Utils.Find_Name_In_Chain (Libraries_List, Ident);
if Library /= Null_Iir then
return Library;
end if;

//  This is a new library.
if Ident = Std_Names.Name_Std then
//  Load_std_library must have been called before.
raise Internal_Error;
end if;

Library := Create_Iir (Iir_Kind_Library_Declaration);
Set_Location (Library, Loc);
Set_Library_Directory (Library, Null_Identifier);
Set_Identifier (Library, Ident);
if Load_Library (Library) = False then
        Error_Msg_Sem (+Loc, "cannot find resource library %i", +Ident);
end if;
Set_Visible_Flag (Library, True);

Set_Chain (Libraries_List_Last, Library);
Libraries_List_Last := Library;

return Library;
end Get_Library;

// Return TRUE if LIBRARY_UNIT and UNIT have identifiers for the same
// design unit identifier.
// eg: 'entity A' and 'package A' returns TRUE.
function Is_Same_Library_Unit (Library_Unit, Unit: Iir) return Boolean
        is
Entity_Name1, Entity_Name2: Name_Id;
Library_Unit_Kind, Unit_Kind : Iir_Kind;
begin
if Get_Identifier (Unit) /= Get_Identifier (Library_Unit) then
return False;
end if;

Library_Unit_Kind := Get_Kind (Library_Unit);
Unit_Kind := Get_Kind (Unit);

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
        Entity_Name1 := Get_Entity_Identifier_Of_Architecture (Unit);
Entity_Name2 := Get_Entity_Identifier_Of_Architecture (Library_Unit);
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

procedure Free_Dependence_List (Design : Iir_Design_Unit)
is
        List : Iir_List;
El : Iir;
begin
        List := Get_Dependence_List (Design);
if List /= Null_Iir_List then
for I in Natural loop
El := Get_Nth_Element (List, I);
exit when El = Null_Iir;
Iirs_Utils.Free_Recursive (El);
end loop;
Destroy_Iir_List (List);
end if;
end Free_Dependence_List;

//  This procedure is called when the DESIGN_UNIT (either the stub created
//  when a library is read or created from a previous unit in a source
//  file) has been replaced by a new unit.  Free everything but DESIGN_UNIT,
//  has it may be referenced in other units (dependence...)
//  FIXME: Isn't the library unit also referenced too ?
procedure Free_Design_Unit (Design_Unit : Iir_Design_Unit)
is
        Lib : Iir;
Unit : Iir_Design_Unit;
Dep_List : Iir_List;
begin
//  Free dependence list.
Dep_List := Get_Dependence_List (Design_Unit);
Destroy_Iir_List (Dep_List);
Set_Dependence_List (Design_Unit, Null_Iir_List);

//  Free default configuration of architecture (if any).
Lib := Get_Library_Unit (Design_Unit);
if Lib /= Null_Iir
        and then Get_Kind (Lib) = Iir_Kind_Architecture_Body
then
        Free_Iir (Get_Entity_Name (Lib));
Unit := Get_Default_Configuration_Declaration (Lib);
if Unit /= Null_Iir then
Free_Design_Unit (Unit);
end if;
end if;

//  Free library unit.
Free_Iir (Lib);
Set_Library_Unit (Design_Unit, Null_Iir);
end Free_Design_Unit;

procedure Remove_Unit_From_File
        (Unit_Ref : Iir_Design_Unit; File : Iir_Design_File)
is
        Prev : Iir_Design_Unit;
Unit, Next : Iir_Design_Unit;
begin
        Prev := Null_Iir;
Unit := Get_First_Design_Unit (File);
while Unit /= Null_Iir loop
Next := Get_Chain (Unit);
if Unit = Unit_Ref then
if Prev = Null_Iir then
        Set_First_Design_Unit (File, Next);
else
Set_Chain (Prev, Next);
end if;
if Next = Null_Iir then
        Set_Last_Design_Unit (File, Prev);
end if;
return;
end if;
Prev := Unit;
Unit := Next;
end loop;
//  Not found.
raise Internal_Error;
end Remove_Unit_From_File;

//  Last design_file used.  Kept to speed-up operations.
Last_Design_File : Iir_Design_File := Null_Iir;

// Add or replace a design unit in the working library.
procedure Add_Design_Unit_Into_Library
        (Unit : in Iir_Design_Unit; Keep_Obsolete : Boolean := False)
is
        Design_File: Iir_Design_File;
Design_Unit, Prev_Design_Unit : Iir_Design_Unit;
Last_Unit : Iir_Design_Unit;
Library_Unit: Iir;
New_Library_Unit: Iir;
Unit_Id : Name_Id;
Date: Date_Type;
New_Lib_Checksum : File_Checksum_Id;
Id : Hash_Id;

//  File name and dir name of DECL.
File_Name : Name_Id;
Dir_Name : Name_Id;
begin
//  As specified, the Chain must be not set.
pragma Assert (Get_Chain (Unit) = Null_Iir);

//  The unit must not be in the library.
pragma Assert (Get_Date_State (Unit) = Date_Extern);

//  Mark this design unit as being loaded.
New_Library_Unit := Get_Library_Unit (Unit);
Unit_Id := Get_Identifier (New_Library_Unit);

//  Set the date of the design unit as the most recently analyzed
//  design unit.
case Get_Date (Unit) is
        when Date_Parsed =>
Set_Date_State (Unit, Date_Parse);
when Date_Analyzed =>
Date := Get_Date (Work_Library) + 1;
Set_Date (Unit, Date);
Set_Date (Work_Library, Date);
Set_Date_State (Unit, Date_Analyze);
when Date_Valid =>
raise Internal_Error;
when others =>
raise Internal_Error;
end case;

//  Set file time stamp.
declare
        File : Source_File_Entry;
Pos : Source_Ptr;
begin
        Files_Map.Location_To_File_Pos (Get_Location (New_Library_Unit),
        File, Pos);
New_Lib_Checksum := Files_Map.Get_File_Checksum (File);
File_Name := Files_Map.Get_File_Name (File);
Image (File_Name);
if GNAT.OS_Lib.Is_Absolute_Path (Nam_Buffer (1 .. Nam_Length)) then
        Dir_Name := Null_Identifier;
else
Dir_Name := Files_Map.Get_Home_Directory;
end if;
end;

//  Try to find a design unit with the same name in the work library.
Id := Get_Hash_Id_For_Unit (Unit);
Design_Unit := Unit_Hash_Table (Id);
Prev_Design_Unit := Null_Iir;
while Design_Unit /= Null_Iir loop
Design_File := Get_Design_File (Design_Unit);
Library_Unit := Get_Library_Unit (Design_Unit);
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
Next_Design := Get_Hash_Chain (Design_Unit);
if Prev_Design_Unit = Null_Iir then
        Unit_Hash_Table (Id) := Next_Design;
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
Obsoleted_Design_Units := Design_Unit;
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
Prev_Design_Unit := Design_Unit;
Design_Unit := Get_Hash_Chain (Design_Unit);
end if;
end loop;

//  Try to find the design file in the library.
//  First try the last one found.
if Last_Design_File /= Null_Iir
        and then Get_Library (Last_Design_File) = Work_Library
        and then Get_Design_File_Filename (Last_Design_File) = File_Name
        and then Get_Design_File_Directory (Last_Design_File) = Dir_Name
then
        Design_File := Last_Design_File;
else
//  Search.
Design_File := Get_Design_File_Chain (Work_Library);
while Design_File /= Null_Iir loop
if Get_Design_File_Filename (Design_File) = File_Name
        and then Get_Design_File_Directory (Design_File) = Dir_Name
then
        exit;
end if;
Design_File := Get_Chain (Design_File);
end loop;
Last_Design_File := Design_File;
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
Design_Unit := Get_First_Design_Unit (Design_File);
while Design_Unit /= Null_Iir loop
if Design_Unit /= Unit then
//  Mark other design unit as obsolete.
Set_Date (Design_Unit, Date_Obsolete);
Remove_Unit_Hash (Design_Unit);
else
raise Internal_Error;
end if;
Prev_Design_Unit := Design_Unit;
Design_Unit := Get_Chain (Design_Unit);

Set_Chain (Prev_Design_Unit, Obsoleted_Design_Units);
Obsoleted_Design_Units := Prev_Design_Unit;
end loop;
Set_First_Design_Unit (Design_File, Null_Iir);
Set_Last_Design_Unit (Design_File, Null_Iir);
end if;

if Design_File = Null_Iir then
// This is the first apparition of the design file.
Design_File := Create_Iir (Iir_Kind_Design_File);
Location_Copy (Design_File, Unit);

Set_Design_File_Filename (Design_File, File_Name);
Set_Design_File_Directory (Design_File, Dir_Name);

Set_File_Checksum (Design_File, New_Lib_Checksum);
Set_Parent (Design_File, Work_Library);
Set_Chain (Design_File, Get_Design_File_Chain (Work_Library));
Set_Design_File_Chain (Work_Library, Design_File);
end if;

//  Add DECL to DESIGN_FILE.
Last_Unit := Get_Last_Design_Unit (Design_File);
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
Set_Hash_Chain (Unit, Unit_Hash_Table (Id));
Unit_Hash_Table (Id) := Unit;

//  Update the analyzed time stamp.
Set_Analysis_Time_Stamp (Design_File, Files_Map.Get_Os_Time_Stamp);
end Add_Design_Unit_Into_Library;

procedure Add_Design_File_Into_Library (File : in out Iir_Design_File)
is
        Unit : Iir_Design_Unit;
Next_Unit : Iir_Design_Unit;
First_Unit : Iir_Design_Unit;
begin
        Unit := Get_First_Design_Unit (File);
First_Unit := Unit;
Set_First_Design_Unit (File, Null_Iir);
Set_Last_Design_Unit (File, Null_Iir);
while Unit /= Null_Iir loop
Next_Unit := Get_Chain (Unit);
Set_Chain (Unit, Null_Iir);
Libraries.Add_Design_Unit_Into_Library (Unit, True);
Unit := Next_Unit;
end loop;
if First_Unit /= Null_Iir then
File := Get_Design_File (First_Unit);
end if;
end Add_Design_File_Into_Library;

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
        Entity_Id := Get_Identifier (Entity);
Lib := Get_Library (Get_Design_File (Get_Design_Unit (Entity)));
Design_File := Get_Design_File_Chain (Lib);
Res := Null_Iir;
while Design_File /= Null_Iir loop
Design_Unit := Get_First_Design_Unit (Design_File);
while Design_Unit /= Null_Iir loop
Library_Unit := Get_Library_Unit (Design_Unit);

if Get_Kind (Library_Unit) = Iir_Kind_Architecture_Body
        and then
Get_Entity_Identifier_Of_Architecture (Library_Unit) = Entity_Id
then
if Res = Null_Iir then
        Res := Design_Unit;
elsif Get_Date (Design_Unit) > Get_Date (Res) then
        Res := Design_Unit;
end if;
end if;
Design_Unit := Get_Chain (Design_Unit);
end loop;
Design_File := Get_Chain (Design_File);
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
Res := Null_Iir;
else
Res := Parse.Parse_Design_File;
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
        Fe := Files_Map.Load_Source_File (Local_Directory, File_Name);
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
        Lib := Get_Library (Get_Identifier (Get_Prefix (Unit)),
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
if not Flags.Flag_Elaborate_With_Outdated then
if Loc = Null_Iir then
        Error_Msg_Sem (Command_Line_Location, Msg, Arg1);
else
Error_Msg_Sem (+Loc, Msg, Arg1);
end if;
end if;
end Error_Obsolete;

procedure Error_Obsolete (Msg : String; Args : Earg_Arr) is
        begin
if not Flags.Flag_Elaborate_With_Outdated then
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
List := Get_Dependence_List (Design_Unit);
if List = Null_Iir_List then
return False;
end if;
Du_Ts := Get_Analysis_Time_Stamp (Get_Design_File (Design_Unit));
for I in Natural loop
El := Get_Nth_Element (List, I);
exit when El = Null_Iir;
Unit := Find_Design_Unit (El);
if Unit /= Null_Iir then
U_Ts := Get_Analysis_Time_Stamp (Get_Design_File (Unit));
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

procedure Finish_Compilation
        (Unit : Iir_Design_Unit; Main : Boolean := False)
is
        Lib_Unit : Iir;
begin
        Lib_Unit := Get_Library_Unit (Unit);
if (Main or Flags.Dump_All) and then Flags.Dump_Parse then
Disp_Tree.Disp_Tree (Unit);
end if;

if Flags.Check_Ast_Level > 0 then
        Nodes_GC.Check_Tree (Unit);
end if;

if Flags.Verbose then
Report_Msg (Msgid_Note, Semantic, +Lib_Unit,
"analyze %n", (1 => +Lib_Unit));
end if;

Sem.Semantic (Unit);

if (Main or Flags.Dump_All) and then Flags.Dump_Sem then
Disp_Tree.Disp_Tree (Unit);
end if;

if Errorout.Nbr_Errors > 0 then
        raise Compilation_Error;
end if;

if (Main or Flags.List_All) and then Flags.List_Sem then
Disp_Vhdl.Disp_Vhdl (Unit);
end if;

if Flags.Check_Ast_Level > 0 then
        Nodes_GC.Check_Tree (Unit);
end if;

//  Post checks
//--------------

Post_Sems.Post_Sem_Checks (Unit);

if Errorout.Nbr_Errors > 0 then
        raise Compilation_Error;
end if;

//  Canonalisation.
//----------------

if Flags.Verbose then
Report_Msg (Msgid_Note, Semantic, +Lib_Unit,
"canonicalize %n", (1 => +Lib_Unit));
end if;

Canon.Canonicalize (Unit);

if (Main or Flags.Dump_All) and then Flags.Dump_Canon then
Disp_Tree.Disp_Tree (Unit);
end if;

if Errorout.Nbr_Errors > 0 then
        raise Compilation_Error;
end if;

if (Main or Flags.List_All) and then Flags.List_Canon then
Disp_Vhdl.Disp_Vhdl (Unit);
end if;

if Flags.Check_Ast_Level > 0 then
        Nodes_GC.Check_Tree (Unit);
end if;
end Finish_Compilation;

procedure Load_Parse_Design_Unit (Design_Unit: Iir_Design_Unit; Loc : Iir)
is
        use Scanner;
Line, Off: Natural;
Pos: Source_Ptr;
Res: Iir;
Design_File : Iir_Design_File;
Fe : Source_File_Entry;
begin
//  The unit must not be loaded.
pragma Assert (Get_Date_State (Design_Unit) = Date_Disk);

//  Load the file in memory.
Design_File := Get_Design_File (Design_Unit);
Fe := Files_Map.Load_Source_File
        (Get_Design_File_Directory (Design_File),
                Get_Design_File_Filename (Design_File));
if Fe = No_Source_File_Entry then
        Error_Lib_Msg ("cannot load %n", +Get_Library_Unit (Design_Unit));
raise Compilation_Error;
end if;
Set_File (Fe);

//  Check if the file has changed.
if not Files_Map.Is_Eq
(Files_Map.Get_File_Checksum (Get_Current_Source_File),
        Get_File_Checksum (Design_File))
then
        Error_Msg_Sem (+Loc, "file %i has changed and must be reanalysed",
        +Get_Design_File_Filename (Design_File));
raise Compilation_Error;
elsif Get_Date (Design_Unit) = Date_Obsolete then
        Error_Msg_Sem (+Design_Unit, "%n is not anymore its source file",
        +Get_Library_Unit (Design_Unit));
raise Compilation_Error;
end if;

//  Set the position of the lexer
Pos := Get_Design_Unit_Source_Pos (Design_Unit);
Line := Natural (Get_Design_Unit_Source_Line (Design_Unit));
Off := Natural (Get_Design_Unit_Source_Col (Design_Unit));
Files_Map.File_Add_Line_Number (Get_Current_Source_File, Line, Pos);
Set_Current_Position (Pos + Source_Ptr (Off));

//  Parse
        Res := Parse.Parse_Design_Unit;
Close_File;
if Res = Null_Iir then
        raise Compilation_Error;
end if;

Set_Date_State (Design_Unit, Date_Parse);

//  FIXME: check the library unit read is the one expected.

//  Move the unit in the library: keep the design_unit of the library,
//  but replace the library_unit by the one that has been parsed.  Do
//  not forget to relocate parents.
Iirs_Utils.Free_Recursive (Get_Library_Unit (Design_Unit));
Set_Library_Unit (Design_Unit, Get_Library_Unit (Res));
Set_Design_Unit (Get_Library_Unit (Res), Design_Unit);
Set_Parent (Get_Library_Unit (Res), Design_Unit);
declare
        Item : Iir;
begin
        Item := Get_Context_Items (Res);
Set_Context_Items (Design_Unit, Item);
while Is_Valid (Item) loop
        Set_Parent (Item, Design_Unit);
Item := Get_Chain (Item);
end loop;
end;
Location_Copy (Design_Unit, Res);
Free_Dependence_List (Design_Unit);
Set_Dependence_List (Design_Unit, Get_Dependence_List (Res));
Set_Dependence_List (Res, Null_Iir_List);
Free_Iir (Res);
end Load_Parse_Design_Unit;

// Load, parse, analyze, back-end a design_unit if necessary.
procedure Load_Design_Unit (Design_Unit: Iir_Design_Unit; Loc : Iir)
is
        Warnings : Warnings_Setting;
begin
if Get_Date_State (Design_Unit) = Date_Disk then
        Load_Parse_Design_Unit (Design_Unit, Loc);
end if;

if Get_Date_State (Design_Unit) = Date_Parse then
//  Analyze the design unit.

if Get_Date (Design_Unit) = Date_Analyzed then
//  Work-around for an internal check in sem.
//  FIXME: to be removed ?
Set_Date (Design_Unit, Date_Parsed);
end if;

//  Avoid infinite recursion, if the unit is self-referenced.
Set_Date_State (Design_Unit, Date_Analyze);

//  Disable all warnings.  Warnings are emitted only when the unit
//  is analyzed.
Save_Warnings_Setting (Warnings);
Disable_All_Warnings;

//  Analyze unit.
Finish_Compilation (Design_Unit);

//  Restore warnings.
Restore_Warnings_Setting (Warnings);
end if;

case Get_Date (Design_Unit) is
        when Date_Parsed =>
raise Internal_Error;
when Date_Analyzing =>
//  Self-referenced unit.
return;
when Date_Analyzed =>
//  FIXME: Accept it silently ?
//  Note: this is used when Flag_Elaborate_With_Outdated is set.
//  This is also used by anonymous configuration declaration.
null;
when Date_Uptodate =>
return;
when Date_Valid =>
null;
when Date_Obsolete =>
if not Flags.Flag_Elaborate_With_Outdated then
Error_Msg_Sem (+Loc, "%n is obsolete", +Design_Unit);
return;
end if;
when others =>
raise Internal_Error;
end case;

if not Flags.Flag_Elaborate_With_Outdated
        and then Is_Obsolete (Design_Unit, Loc)
then
        Set_Date (Design_Unit, Date_Obsolete);
end if;
end Load_Design_Unit;

//  Return the declaration of primary unit NAME of LIBRARY.
function Find_Primary_Unit
        (Library: Iir_Library_Declaration; Name: Name_Id)
return Iir_Design_Unit
        is
Unit : Iir_Design_Unit;
begin
        Unit := Unit_Hash_Table (Name mod Unit_Hash_Length);
while Unit /= Null_Iir loop
if Get_Identifier (Unit) = Name
        and then Get_Library (Get_Design_File (Unit)) = Library
then
case Get_Kind (Get_Library_Unit (Unit)) is
        when Iir_Kind_Package_Declaration
| Iir_Kind_Package_Instantiation_Declaration
| Iir_Kind_Entity_Declaration
| Iir_Kind_Configuration_Declaration
| Iir_Kind_Context_Declaration =>
//  Only return a primary unit.
return Unit;
when Iir_Kind_Package_Body
| Iir_Kind_Architecture_Body =>
null;
when others =>
raise Internal_Error;
end case;
end if;
Unit := Get_Hash_Chain (Unit);
end loop;

// The primary unit is not in the library, return null.
return Null_Iir;
end Find_Primary_Unit;

function Load_Primary_Unit
        (Library: Iir_Library_Declaration; Name: Name_Id; Loc : Iir)
return Iir_Design_Unit
        is
Design_Unit: Iir_Design_Unit;
begin
        Design_Unit := Find_Primary_Unit (Library, Name);
if Design_Unit /= Null_Iir then
Load_Design_Unit (Design_Unit, Loc);
end if;
return Design_Unit;
end Load_Primary_Unit;

// Return the declaration of secondary unit NAME for PRIMARY, or null if
// not found.
function Find_Secondary_Unit (Primary: Iir_Design_Unit; Name: Name_Id)
return Iir_Design_Unit
        is
Design_Unit: Iir_Design_Unit;
Library_Unit: Iir;
Primary_Ident: Name_Id;
Lib_Prim : Iir;
begin
        Lib_Prim := Get_Library (Get_Design_File (Primary));
Primary_Ident := Get_Identifier (Get_Library_Unit (Primary));
Design_Unit := Unit_Hash_Table (Primary_Ident mod Unit_Hash_Length);
while Design_Unit /= Null_Iir loop
Library_Unit := Get_Library_Unit (Design_Unit);

//  The secondary is always in the same library as the primary.
if Get_Library (Get_Design_File (Design_Unit)) = Lib_Prim then
// Set design_unit to null iff this is not the correct
// design unit.
case Get_Kind (Library_Unit) is
        when Iir_Kind_Architecture_Body =>
// The entity field can be either an identifier (if the
// library unit was not loaded) or an access to the entity
// unit.
if (Get_Entity_Identifier_Of_Architecture (Library_Unit)
        = Primary_Ident)
and then Get_Identifier (Library_Unit) = Name
then
return Design_Unit;
end if;
when Iir_Kind_Package_Body =>
if Name = Null_Identifier
        and then Get_Identifier (Library_Unit) = Primary_Ident
then
return Design_Unit;
end if;
when others =>
null;
end case;
end if;
Design_Unit := Get_Hash_Chain (Design_Unit);
end loop;

// The architecture or the body is not in the library, return null.
return Null_Iir;
end Find_Secondary_Unit;

// Load an secondary unit and analyse it.
function Load_Secondary_Unit
        (Primary: Iir_Design_Unit; Name: Name_Id; Loc : Iir)
return Iir_Design_Unit
        is
Design_Unit: Iir_Design_Unit;
begin
        Design_Unit := Find_Secondary_Unit (Primary, Name);
if Design_Unit /= Null_Iir then
Load_Design_Unit (Design_Unit, Loc);
end if;
return Design_Unit;
end Load_Secondary_Unit;

function Find_Entity_For_Component (Name: Name_Id) return Iir_Design_Unit
        is
Res : Iir_Design_Unit := Null_Iir;
Unit : Iir_Design_Unit;
begin
        Unit := Unit_Hash_Table (Name mod Unit_Hash_Length);
while Unit /= Null_Iir loop
if Get_Identifier (Unit) = Name
        and then (Get_Kind (Get_Library_Unit (Unit))
                = Iir_Kind_Entity_Declaration)
then
if Res = Null_Iir then
        Res := Unit;
else
//  Many entities.
return Null_Iir;
end if;
end if;
Unit := Get_Hash_Chain (Unit);
end loop;

return Res;
end Find_Entity_For_Component;

function Get_Libraries_List return Iir_Library_Declaration is
begin
return Libraries_List;
end Get_Libraries_List;
end Libraries;

