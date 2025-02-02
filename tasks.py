import glob
from pathlib import Path
import re
import shlex
from invoke import task
import os
import multiprocessing
import subprocess
import shutil
from datetime import datetime

# ==================================================================================================================

def info (msg):
  print(f"INFO: {msg}")

def error (msg):
  print(f"ERROR: {msg}")

class MockCompletedProcess:
    def __init__(self, returncode=0):
        self.returncode = returncode
        self.stderr = b''

# ==================================================================================================================

def check_arguments(target_name, path, debug, release):
  if target_name is None or not target_name[0].isupper():
    raise ValueError(f"Please provide a valid target name (upper case and camel case when applicable)")
  
  if not os.path.exists(path) or len(os.listdir(path)) == 0:
    raise ValueError(f"{path} is empty or doesn't exist")

# ==================================================================================================================

@task(aliases = ['b'], 
      help={"experimental_targets": "List of experimental targets to build, Strings separated by ';' (semicolon). Example: 'TriCore;RISCW;etc'. If not defined, the name of the target the environment was created for will be used.",
             "path": "Path to llvm source root. By default the current directory is considered",
             "debug": "Build Debug version of the target. Default value: False",
             "release": "Build Release version of the target. Default value: True",
             "llc_only": "Whether to only build llc or not. Default value: False",
             "upstream_targets": "List of upstream targets to build, Strings separated by ';' (semicolon). By default it is None. Example: Lanai;ARM;RISCV;etc",
             "parallel_link_jobs": "Number of parallel link jobs. By default it is set to 2. Note: A high number of link jobs may cause your system to run out of memory and crash."})
def build_target(c, experimental_targets="", path=".", upstream_targets="", debug=False, release=True, llc_only=False, parallel_link_jobs=2):
  """
    Starts the build process for the target specified in the target_name parameter. If the target_name is not provided, the name of the target the environment was created for will be used.
    By default, the build type is set to Release. If you want to build the target in Debug mode, you can specify the build type as Debug.
  """
  if experimental_targets == "":
    with open(os.path.join(os.getcwd(), ".experimental_targets_to_build"), "r") as f:
      experimental_targets = f.read().strip()
  
  if debug:
    release = False
  else:
    release = True

  check_arguments(experimental_targets, path, debug, release)

  build_type = "Debug" if debug else "Release"
  build_dir_name = f'build_tricore_{build_type.lower()}'
  build_dir = os.path.join(path, build_dir_name)
  
  if os.getenv('CI') is not None:
    # I decided to implement my own caching solution because the official one is way too slow when the build folder is very large (like it is for LLVM)
    print("We are in CI mode. Try to copy the cache build folder if it exists...")
    ci_build_path = os.getenv('CI_BUILDS_DIR')
    if os.path.exists(os.path.join(ci_build_path, build_dir_name)):
      shutil.move(os.path.join(ci_build_path, build_dir_name), os.path.join(ci_build_path, f"llvm-{experimental_targets.lower()}-backend/{build_dir_name}"))

  if not os.path.exists(f'{build_dir}'):
    os.mkdir(f'{build_dir}')

  os.chdir(path)
  enable_clang_lld = "-DLLVM_ENABLE_PROJECTS=\"clang;lld\"" if not llc_only else ""
  enable_assertions = "-DLLVM_ENABLE_ASSERTIONS=ON" # if build_type == "Debug" else "-DLLVM_ENABLE_ASSERTIONS=OFF"
  cmake_command = f'cmake -S llvm -B {build_dir} -G Ninja {enable_clang_lld} -DCMAKE_INSTALL_PREFIX={build_dir} {enable_assertions} -DCMAKE_BUILD_TYPE={build_type} -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD="{experimental_targets}" -DLLVM_TARGETS_TO_BUILD="{upstream_targets}" -DLLVM_PARALLEL_LINK_JOBS={parallel_link_jobs} -DLLVM_USE_LINKER=lld'
  
  info(f"Configuring {experimental_targets} ... ")
  info(f"Cmake command: {cmake_command}")

  process = subprocess.Popen(cmake_command, shell=True)
  # Wait for the subprocess to complete
  process.wait()

  # Check if the command was successful (return code 0)
  if process.returncode == 0:
    info("Configured succesfully.")
  else:
    raise Exception("Configure step failed.")

  os.chdir(build_dir)

  cores = multiprocessing.cpu_count()
  build_command = f'ninja -j {cores - 2 if cores > 4 else cores - 1}'
    
  info(f"Build command: {build_command}")
  info(f"Building target {experimental_targets} ...")
  process = subprocess.Popen(build_command, shell=True)
  # Wait for the subprocess to complete
  process.wait()

  # Check if the command was successful (return code 0)
  if process.returncode == 0:
    info("Built succesfully!")
  else:
    raise Exception("The build has failed.")
  
  if os.getenv('CI') is not None:
    # Copy the build folder back
    print("CI: Copy the build folder into the cache location")
    ci_build_path = os.getenv('CI_BUILDS_DIR')
    if os.path.exists(os.path.join(ci_build_path, build_dir_name)):
      shutil.move(os.path.join(ci_build_path, f"llvm-{experimental_targets.lower()}-backend/{build_dir_name}"), os.path.join(ci_build_path, build_dir_name))

  os.chdir(path)

@task(aliases = ['t'],
      help={"filter": "Glob pattern to filter the C files to be compiled. Example: 'test_*' will compile only the files that start with 'test_'",
            "debug": "Whether to compile the C files in debug mode or not. Default value: True",
            "c_to_llvmir": "Whether to compile the C files to LLVM IR or not. Default value: False",
            "c_to_assembly": "Whether to compile the C files to assembly or not. Default value: False",
            "llvmir_to_assembly": "Whether to compile the LLVM IR files to assembly or not. Default value: False",
            "optimization_level": "Optimization level for the compilation. Default value: 1",
            "list_tests": "Whether to list the tests or not. This parameter will stop the test run after printing the tests. Default value: False"
      })
def run_c_patterns_tests(c, filter="", debug=False, c_to_assembly=False, c_to_llvmir=True, llvmir_to_assembly=True, optimization_level=0, list_tests=False):
  """
    Compiles all C programs found in the `tests/c-patterns` directory and keeps the outputs in the `tests/c-patterns/bin` directory.
    This task uses pytest to run the tests, so that we will have a better overview of the results.
  """
  info("Running tests ...") if not list_tests else info("Listing tests ...")
  build_type = "debug" if debug else "release"
  
  # define relevant paths for this test
  c_files_path = os.path.join(os.path.dirname(__file__), "tests/c-patterns/src/")
  test_output_path = os.path.join(os.path.dirname(__file__), "tests/c-patterns/test-output/")
  assembly_files_path = os.path.join(test_output_path, "assembly")
  llvmir_files_path = os.path.join(test_output_path, "llvmir")
  log_files_path = os.path.join(test_output_path, "logs")

  if not list_tests:
    # cleanup from previous test run
    if os.path.exists(test_output_path):
      shutil.rmtree(test_output_path)
    
    # create test output folders
    os.mkdir(test_output_path)
    os.mkdir(assembly_files_path)
    os.mkdir(llvmir_files_path)
    os.mkdir(log_files_path)

  # Get a list of files with the .c extension from the c_files_path
  # Check if the filter already includes the .s extension
  if filter.endswith(".c"):
      file_pattern = os.path.join(c_files_path, filter)
  else:
      file_pattern = os.path.join(c_files_path, f"{filter}*.c" if filter != "" else "*.c")
  
  c_files = [os.path.basename(x) for x in glob.glob(file_pattern)]
  file_names = [Path(x).stem for x in c_files]

  # initialize statistical data
  passed = 0
  failed = 0
  total = len(file_names)

  run_clang = False
  run_llc = False
  
  if c_to_assembly:
    clang_arguments = f"-S --target=tricore {' --debug' if debug else ''} {f' -O{optimization_level}' if optimization_level > 0 else ''}"
    run_clang = True
  elif c_to_llvmir: 
    clang_arguments = f"-S --target=tricore {' --debug' if debug else ''} -emit-llvm {f' -O{optimization_level}' if optimization_level > 0 else ''}"
    run_clang = True

  if llvmir_to_assembly:
    llc_arguments = f"--march=tricore {' --debug' if debug else ''} -print-after-all {f' -O{optimization_level}' if optimization_level > 0 else ''}"
    run_llc = True
  
  clang_path = os.path.join(os.path.dirname(__file__), f"build_tricore_{build_type}/bin/clang")
  llc_path = os.path.join(os.path.dirname(__file__), f"build_tricore_{build_type}/bin/llc")

  index = 1
  # iterate over the c_files list
  for c_file in file_names:
      if list_tests:
        print(f"- {index:2}/{total}: {c_file}.c")
        index += 1
        continue

      print(f"{index:2}/{total}: {c_file}.c {'.' * (40 - len(c_file) - 1)}", end="")
      if c_to_assembly:
        clang_command = f"{clang_path} {clang_arguments} {os.path.join(c_files_path, f'{c_file}.c')} -o {os.path.join(assembly_files_path, f'{c_file}.s')}"
      elif c_to_llvmir:
        clang_command = f"{clang_path} {clang_arguments} {os.path.join(c_files_path, f'{c_file}.c')} -o {os.path.join(llvmir_files_path, f'{c_file}.ll')}"
      
      if llvmir_to_assembly:
        llc_command = f"{llc_path} {llc_arguments} {os.path.join(llvmir_files_path, f'{c_file}.ll')} -o {os.path.join(assembly_files_path, f'{c_file}.s')}"

      result_clang = MockCompletedProcess()
      result_llc = MockCompletedProcess()
      
      if run_clang:
        result_clang = subprocess.run(shlex.split(clang_command), env=os.environ, capture_output=True)
      if run_llc:
        result_llc = subprocess.run(shlex.split(llc_command), env=os.environ, capture_output=True)

      if result_clang.returncode != 0 or result_llc.returncode != 0:
        with open(os.path.join(log_files_path, f"{c_file}.log"), "w") as f:
          if run_clang:
            f.write(">>>>>>>>>>>>> CLANG ERROR OUTPUT\n")
            f.write(result_clang.stderr.decode())
          if run_llc:
            f.write("\n>>>>>>>>>>>>> LLC ERROR OUTPUT\n")
            f.write(result_llc.stderr.decode())
        failed += 1
        print(" Failed")
      else:
        passed += 1
        print(" Passed")
      
      index += 1

  if not list_tests:
    info(f"Passed: {passed}/{total} - {round(passed/total*100)}%")
    info(f"Failed: {failed}/{total} - {round(failed/total*100)}%")


@task(aliases = ['c'],
      help={})
def cleanup_test_output(c):
  """
    Cleans up the test output folder of the LLVM project.
  """
  shutil.rmtree(os.path.join(os.path.dirname(__file__), "tests/c-patterns/test-output/"))

@task(aliases = ['ct'],
      help={
        "filter": "Glob pattern to filter the assembly files to be converted to the Tasking compiler format. Example: 'test_' will convert only the files that start with 'test_'",
        "list_files": "Whether to list the files or not. This parameter will stop the conversion after printing the files. Default value: False"
      })
def convert_tests_to_tasking_format(c, filter="", list_files=False):
  """
    Converts the tests in the `tests/c-patterns` directory to the Tasking format.
  """
  # define relevant paths for this test
  test_output_path = os.path.join(os.path.dirname(__file__), "tests/c-patterns/test-output/")
  assembly_files_path = os.path.join(test_output_path, "assembly")
  converted_assembly_files_path = os.path.join(test_output_path, "tasking")

  # cleanup from previous test run
  if os.path.exists(converted_assembly_files_path):
    shutil.rmtree(converted_assembly_files_path)
  
  # create test output folders
  os.mkdir(converted_assembly_files_path)

  # Check if the filter already includes the .s extension
  if filter.endswith(".s"):
      file_pattern = os.path.join(assembly_files_path, filter)
  else:
      file_pattern = os.path.join(assembly_files_path, f"{filter}*.s" if filter != "" else "*.s")

  print(file_pattern)


  # Get a list of files with the .s extension from the assembly_files_path
  assembly_files = [os.path.basename(x) for x in glob.glob(file_pattern)]
  assembly_files = [Path(x).stem for x in assembly_files]

  index = 1
  total = len(assembly_files)
  print("Converting files to Tasking format ...") if not list_files else print("Listing files ...")
  for assembly_file in assembly_files:
    if list_files:
      print(f"- {index:2}/{total}: {assembly_file}.s")
      index += 1
      continue
    
    print(f"{index:2}/{total}: {assembly_file}.s {'.' * (40 - len(assembly_file) - 1)}", end="")
    
    with open(os.path.join(assembly_files_path, f"{assembly_file}.s"), "r") as original_file:
      lines = original_file.readlines()
      with open(os.path.join(converted_assembly_files_path, f"{assembly_file}.src"), "w") as tasking_file:
        header = \
          '.sdecl ".text", CODE                ; Declare a section with name, type and attributes\n' + \
          '.sect ".text"                       ; Activate a declared section\n' + \
          '.align 4                            ; Align the location counter on 4 bytes\n\n'
        tasking_file.write(header)

        for line in lines:
          if  ".text" in line or ".type" in line or ".size" in line or ".ident" in line or ".section" in line or ".globl" in line or ".file" in line or ".Lfunc_end" in line or re.search(".L.*\$local:", line):
            continue
          elif "main:" in line:
            main_function_name = f"{assembly_file.split('.')[0]}_main"
            tasking_file.write(f"\t.global {main_function_name}\n\n")
            tasking_file.write(f"{main_function_name}:\n\n")
            continue
          # Remove everything after and including '#'
          line = re.sub(r'#.*', '', line)

          # Replace all the registers with the Tasking compiler format
          line = line.replace("%a", "a")
          line = line.replace("%d", "d")
          
          # Remove whitespaces between the address register and the offset
          line = line.replace("] ", "]")

          # Replace all the registers with the Tasking compiler format
          line = re.sub(r', (\d+)', r', #\1', line)

          # Remove the trailing whitespaces
          line = line.rstrip() + "\n"

          if line == "\n":
            continue
          
          # Finally, write the line to the file
          tasking_file.write(line)
    
    print(" Done")

    index += 1