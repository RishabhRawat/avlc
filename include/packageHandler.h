//
// Created by scalpel on 12/3/17.
//

#ifndef AVLC_PACKAGEHANDLER_H
#define AVLC_PACKAGEHANDLER_H

#include <experimental/filesystem>
#include <vector>
#include "iir.h"
#include "State.h"
#include "Keywords.h"

class packageHandler {
private:
    std::vector<Iir_Library_Declaration*> Libraries_List;
    std::vector<std::filesystem::path> paths;
    State state;

    std::string Library_To_File_Name(Iir_Library_Declaration* Library);
    bool Search_Library_In_Path(Iir_Library_Declaration* Library);
public:
    // This package defines the library manager.
    // The purpose of the library manager is to associate library logical names
    // with host-dependent library.
    //
    // In this implementation a host-dependent library is a file, whose name  is logical name of the library
    // with the extension '.cf'.  This file contains the name and the position (filename, line, column and offset)
    // of all library unit of the library.
    //
    // The working library WORK can be aliased with a resource library, they share the same host-dependenet library
    // whose name is the name of the resource library.  This is done by load_work_library.

    // Location for a command line.
//    Location_Type Command_Line_Location;

    // Library declaration for the std library.
    // This is also the first library of the libraries chain.
    Iir_Library_Declaration* Std_Library = nullptr;

    // Library declaration for the work library.
    // Note: the identifier of the work_library is work_library_name, which
    // may be different from 'WORK'.
    Iir_Library_Declaration* Work_Library;

    // Name of the WORK library.
    std::string Work_Library_Name = Keywords::Name_Work;

    // Directory of the work library.
    // Set by default by INIT_PATHES to the local directory.
    std::filesystem::path Work_Directory;

    // Local (current) directory.
    std::filesystem::path Local_Directory;

    // Chain of obsoleted design units.
    Iir* Obsoleted_Design_Units = nullptr;

    // Add PATH in the search path.
    void Add_Library_Path(std::filesystem::path Path);

    // Get path N.
    std::string Get_Path(unsigned int N);

    // Set PATH as the path of the work library.
    void Set_Work_Library_Path(std::filesystem::path Path);

    // Transform a library identifier into a file name.
    // Very simple mechanism: just add '-objVV.cf' extension, where VV is the version.
    std::string Library_To_File_Name(Iir_Library_Declaration Library);

    // Set the name of the work library, load the work library.
    // Note: the scanner shouldn't be in use, since this function uses it.
    // If EMPTY is set, the work library is just created and not loaded.
    void Load_Work_Library(bool Empty = false);

    // Initialize the library manager and load the STD library.
    // If BUILD_STANDARD is false, the std.standard library is not created.
    void Load_Std_Library(bool Build_Standard = true);

    // Save the work library as a host-dependent library.
    void Save_Work_Library();

    // Start the analyse a file (ie load and parse it).
    // The file is read from the current directory (unless FILE_NAME is an absolute path).
    // Emit an error if the file cannot be opened.
    // Return NULL_IIR in case of parse error.
    Iir_Design_File* Load_File(std::filesystem::path file);

    // Load, parse, analyze, back-end a design_unit if necessary.
    // Check Design_Unit is not obsolete.
    // LOC is the location where the design unit was needed, in case of error.
    void Load_Design_Unit(Iir_Design_Unit* Design_Unit, Iir* Loc);

    // Load and parse DESIGN_UNIT.
    // Contrary to Load_Design_Unit, the design_unit is not analyzed.
    // Also, the design_unit must not have been already loaded.
    // Used almost only by Load_Design_Unit.
    void Load_Parse_Design_Unit(Iir_Design_Unit* Design_Unit, Iir* Loc);

    // Remove the same file as DESIGN_FILE from work library and all of its units.
    void Purge_Design_File(Iir_Design_File Design_File);

    // Just return the design_unit for NAME, or NULL if not found.
    Iir_Design_Unit* Find_Primary_Unit(Iir_Library_Declaration* Library, std::string Name);

    // Load an already analyzed primary unit NAME from library LIBRARY
    // and compile it.
    // Return NULL_IIR if not found (ie, NAME does not correspond to a library unit identifier).
    Iir_Design_Unit* Load_Primary_Unit(Iir_Library_Declaration* Library, std::string Name, Iir* Loc);

    // Find the secondary unit of PRIMARY.
    // If PRIMARY is a package declaration, returns the package body,
    // If PRIMARY is an entity declaration, returns the architecture NAME.
    // Return NULL_IIR if not found.
    Iir_Design_Unit* Find_Secondary_Unit(Iir_Design_Unit* Primary, std::string Name);

    // Load an secondary unit of primary unit PRIMARY and analyse it.
    // NAME must be set only for an architecture.
    Iir_Design_Unit* Load_Secondary_Unit(Iir_Design_Unit* Primary, std::string Name, Iir* Loc);

    // Analyze UNIT.
    void Finish_Compilation(Iir_Design_Unit* Unit, bool Main = false);

    // Get or create a library from an identifier.
    // LOC is used only to report errors.
    Iir_Library_Declaration* Get_Library(std::string Ident, Location_Type Loc);

    // Add or replace an design unit in the work library.
    // DECL must not have a chain (because it may be modified).
    //
    // If the design_file of UNIT is not already in the library, a new one
    // is created.
    //
    // Units are always appended to the design_file.  Therefore, the order is
    // kept.
    //
    // If KEEP_OBSOLETE is True, obsoleted units are kept in the library.
    // This is used when a whole design file has to be added in the library and
    // then processed (without that feature, redefined units would disappear).
    void Add_Design_Unit_Into_Library(Iir_Design_Unit* Unit, bool Keep_Obsolete = false);

    // Put all design_units of FILE into the work library, by calling Add_Design_Unit_Into_Library.
    // FILE is updated since it may changed (FILE is never put in the library, a new one is created).
    void Add_Design_File_Into_Library(Iir_Design_File* File);

    // Return the design unit (stubbed if not loaded) from UNIT.
    // UNIT may be either a design unit, in this case UNIT is returned, or a selected name, in this case the prefix
    // is a library name and the suffix a primary design unit name, or an entity_aspect_entity to designate an architecture.
    // Return null_iir if the design unit is not found.
    Iir_Design_Unit* Find_Design_Unit(Iir_Selected_Name* Unit);
    Iir_Design_Unit* Find_Design_Unit(Iir_Entity_Aspect_Entity* Unit);

    // Find an entity whose name is NAME in any library.
    // If there is no such entity, return nullptr. (Cannot have multiple, thanks to hash table)
    Iir_Design_Unit* Find_Entity_For_Component(std::string Name);

    // Get the chain of libraries.  Can be used only to read (it musn't be modified).
    std::vector<Iir_Library_Declaration*> Get_Libraries_List();

    void Purge_Design_File(Iir_Design_File* Design_File);

    void Save_Library(Iir_Library_Declaration* Library);

    bool Load_Library(Iir_Library_Declaration* Library);
};

#endif //AVLC_PACKAGEHANDLER_H
