#!/usr/bin/env python3
import json
from collections import OrderedDict


def generatecpp(filename):
    jsonData = {}
    with open(filename + '.json') as data_file:
        jsonData = json.load(data_file, object_pairs_hook=OrderedDict)

    with open(filename + '.h', 'w') as cppSource:
        cppSource.write(jsonData["copyright"])
        cppSource.write('\n\n')
        cppSource.write('#ifndef ' + jsonData["guard"] + '\n')
        cppSource.write('#define ' + jsonData["guard"] + '\n')
        cppSource.write('\n\n')

        for include in jsonData['includes']:
            if include[-2:] == ".h":
                cppSource.write('#include "%s"\n' % (include))
            else:
                cppSource.write('#include <%s>\n' % (include))

        cppSource.write('\n\n')
        cppSource.write('\n'.join(jsonData["macros"]))
        cppSource.write('\n\n')

        for namespace in jsonData['namespaces']:
            cppSource.write('namespace %s {\n' % (namespace["name"]))
            cppSource.write('\n\n')

            count = 0
            for classId in namespace['elements']:
                if classId['type'] == 'class':
                    cppSource.write('    const int ' + classId["name"] + '_id = ' + str(count) + ';\n')
                    count += 1

            cppSource.write('\n')

            for element in namespace['elements']:
                if element["type"] == "enum class":
                    cppSource.write('    %s %s {\n' % (element["type"], element["name"]))
                    cppSource.write('        ' + ',\n        '.join(element["members"]))
                    cppSource.write('\n    };\n\n')
            cppSource.write('    struct ' + ';\n    struct '.join(
                [cls["name"] for cls in namespace['elements'] if cls["type"] == "class"]))
            cppSource.write(';\n\n')

            for element in namespace['elements']:
                if element["type"] == "variant" or element["type"] == "vector":
                    cppSource.write('    using %s = std::%s<' % (element["name"], element["type"]))
                    cppSource.write(', '.join(element["contents"]))
                    cppSource.write('>;\n\n')
                if element["type"] == "class":
                    cppSource.write('    %s %s' % ("struct", element["name"]))
                    for index, (inh_type, inh_class) in enumerate(element["inheritance"].items()):
                        if index is 0 and len(element['inheritance']) is 1:
                            cppSource.write('\n            : %s %s' % (inh_class, inh_type))
                        elif index is 0:
                            cppSource.write('\n            : %s %s,\n' % (inh_class, inh_type))
                        elif index is len(element['inheritance']) - 1:
                            cppSource.write('              %s %s' % (inh_class, inh_type))
                        else:
                            cppSource.write('              %s %s,\n' % (inh_class, inh_type))
                    cppSource.write(' {\n')
                    for mem_name, mem_type in element["members"].items():
                        cppSource.write('        %s %s;\n' % (mem_type, mem_name))
                    cppSource.write('    };\n\n')

        cppSource.write('}\n')
        cppSource.write('#endif // ' + jsonData["guard"] + '\n')


def findClass(jsonData, className):
    for _class in jsonData["namespaces"][0]['elements']:
        if _class["name"] == className:
            return _class
    raise LookupError('class not found')


def simplifyClass(jsonData, classjson):
    if len(classjson["inheritance"]) == 1 and "Iir" in classjson["inheritance"]:
        return
    elif len(classjson["inheritance"]) == 0:
        return

    for inh, inhType in classjson["inheritance"].items():
        if inh is not "Iir":
            classInh = findClass(jsonData, inh)
            simplifyClass(jsonData, classInh)
            for memkey, memval in classInh["members"].items():
                if not memkey in classjson["members"]:
                    classjson["members"][memkey] = memval
                elif classjson["members"][memkey] == memval:
                    continue
                else:
                    raise NameError("Invalid value already inserted")


def simplifyInheritance(filename):
    jsonData = {}
    with open(filename + '.json') as data_file:
        jsonData = json.load(data_file, object_pairs_hook=OrderedDict)

    for _class in jsonData["namespaces"][0]['elements']:
        if _class["type"] == "class":
            simplifyClass(jsonData, _class)
    for _class in jsonData["namespaces"][0]['elements']:
        if _class["name"][:4] == "Iir_":
            _class["name"] = _class["name"][4:]
        if _class["name"] == "Base":
            _class["members"]["classId"] = "int"
        elif _class["type"] == "class":
            _class["inheritance"] = {"Base": "public"}
        if _class["type"] == "class":
            for mem in _class["members"].keys():
                if _class["members"][mem] == "Iir*":
                    _class["members"][mem] = "Base*"

    elems = [elem for elem in jsonData["namespaces"][0]['elements'] if elem["name"][-4:] != "_Abs"]
    jsonData["namespaces"][0]['elements'] = elems
    with open(filename + '2.json', 'w') as jsonSource:
        json.dump(jsonData, jsonSource)


def containingClasses(member, filename):
    jsonData = {}
    with open(filename + '.json') as data_file:
        jsonData = json.load(data_file, object_pairs_hook=OrderedDict)
    mylist = [_class["name"] for _class in jsonData["namespaces"][0]['elements'] if
              _class["type"] == "class" and member in _class["members"]]
    print(', '.join(mylist))


def main():
    from pprint import pprint

    from optparse import OptionParser, OptionGroup

    global opts

    parser = OptionParser("usage: %prog [options] {filename}")
    # parser.add_option("", "--show-ids", dest="showIDs",
    #                   help="Compute cursor IDs (very slow)",
    #                   action="store_true", default=False)
    # parser.add_option("", "--max-depth", dest="maxDepth",
    #                   help="Limit cursor expansion to depth N",
    #                   metavar="N", type=int, default=None)
    parser.disable_interspersed_args()
    (opts, args) = parser.parse_args()

    if len(args) == 0:
        parser.error('invalid number arguments')

    for file in args:
        # simplifyInheritance(file)
        generatecpp(file + '2')


if __name__ == '__main__':
    main()
