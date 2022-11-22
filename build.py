from pathlib import Path
import subprocess
import json
import sys
import os


# Reads map otherwise returns passed value
def check_update_map(_map, key, value):
    if key in _map:
        value = _map[key]
    else:
        _map[key] = value
    return value


# Handles json reading
def load_json(path):
    try:
        with open(path, mode="r") as file:
            return json.load(file)
    except:
        pass
    return {}


# Handles json saving
def save_json(path, data):
    try:
        with open(path, mode="w") as file:
            json.dump(data, file, indent=2)
    except:
        pass


# Creates a new folder
def create_folder(path):
    if not os.path.exists(path):
        os.makedirs(path)


# Deletes a file
def delete_file(path):
    try:
        os.remove(path)
    except:
        pass


# Recusevly gets all folder files or returns self if it is a file
def get_files(path, extension):
    if os.path.isfile(path):
        return path
    extension_string = "*."
    for c in extension:
        extension_string += "[%c%c]" % (c.lower(), c.upper())
    return list(Path(path).rglob(extension_string))


# Returns array of files informations
def process_files(files):
    result = []
    for file in files:
        result.append([
            file,
            file.absolute().as_posix(),
            os.path.splitext(file)[1],
            os.path.getmtime(file),
        ])
    return result


# Runs the shell command
def run_command(arguments):
    command = ""
    for arg in arguments:
        command += arg + " "
    return subprocess.run(command, shell=True).returncode


# Compiles a single source file to an intermediate object
def compile_file(file, custom_args, include_folders, output_folder):
    print("Compiling source file [%s]" % file.name)

    output_file = "%s/%s.o" % (output_folder, file.stem)
    delete_file(output_file)

    arguments = ["g++", "-Wall"]
    arguments += custom_args
    for folder in include_folders:
        arguments.append("-I")
        arguments.append(folder)
    arguments += ["-c", file.as_posix()]
    arguments += ["-o", output_file]

    return run_command(arguments)


# Comiples an array of header/source files to intermediate objects
def compile_intermediate(files, custom_args, include_folders, build_folder):
    objects_folder = build_folder + "/objects"
    info_file_path = build_folder + "/files_info.json"
    files_info = load_json(info_file_path)
    files = process_files(files)

    # Header files
    print("Checking header files")
    build_all = False
    for file in files:
        if file[2] == ".h":
            if file[1] not in files_info or file[3] != files_info[file[1]]:
                build_all = True
                files_info[file[1]] = file[3]
                print("Header file changed [%s]" % file[0].name)
            else:
                print("Skipping header file [%s]" % file[0].name)

    # Source files
    print("Checking source files")
    compiled_counter = 0
    for file in files:
        if file[2] == ".c" or file[2] == ".cpp":
            if build_all or file[1] not in files_info or file[3] != files_info[file[1]]:
                if compile_file(file[0], custom_args, include_folders, objects_folder) != 0:
                    return -1
                files_info[file[1]] = file[3]
                compiled_counter += 1
            else:
                print("Skipping source file [%s]" % file[0].name)

    save_json(info_file_path, files_info)
    return compiled_counter


# Combines all intermediate objects to a single executable file
def build_executable(project_name, build_folder):
    print("Building project")

    arguments = ["g++"]
    for file in get_files(build_folder + "/objects", "o"):
        arguments.append(file.as_posix())
    arguments += [
        "-o",
        "%s/%s" % (build_folder, project_name)
    ]
    if run_command(arguments) == 0:
        print("Project built")


if __name__ == "__main__":
    # Processing args
    should_clear = False
    should_run = False
    for arg in sys.argv:
        arg = arg.lower()
        if arg == "-c":
            should_clear = True
        elif arg == "-r":
            should_run = True

    # Reading/saving build.json
    build_info_path = "build.json"
    build_info = load_json(build_info_path)
    project_name = check_update_map(build_info, "Name", "program")
    build_release = check_update_map(build_info, "Release", False)
    custom_args = check_update_map(build_info, "Args", [])
    include_folders = check_update_map(build_info, "Include folders", [])
    source_folders = check_update_map(build_info, "Source folders", [])
    build_folder = check_update_map(build_info, "Build folder", "build")
    save_json(build_info_path, build_info)

    # Getting/creating build folder
    if build_release:
        build_folder += "/release"
        custom_args += ["-O3"]
    else:
        build_folder += "/debug"

    create_folder(build_folder + "/objects")

    # Making commands
    clear_command = "clear"
    executable_file = "%s/%s" % (build_folder, project_name)

    if os.name == 'nt':
        clear_command = "cls"
        executable_file += ".exe"

    # Clearing the console
    if should_clear:
        run_command([clear_command])

    # Getting the header/source files
    header_files = []
    for folder in include_folders:
        header_files += get_files(folder, "h")

    source_files = []
    for folder in source_folders:
        source_files += get_files(folder, "c")
        source_files += get_files(folder, "cpp")

    # Compiling the source files
    compile_state = compile_intermediate(header_files + source_files, custom_args, include_folders, build_folder)

    # Checking compile status
    if compile_state > 0:
        build_executable(project_name, build_folder)
    elif compile_state < 0:
        delete_file(executable_file)
        should_run = False
    else:
        print("Skipping project build")

    # Running the app
    if should_run:
        print("Running the program")
        run_command([executable_file])
