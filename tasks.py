import glob
from pathlib import Path
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
  build_dir_name = f'build_{experimental_targets.lower()}_{build_type.lower()}'
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
      help={"debug": "Whether to search for the clang binary in the release or debug folder"})
def run_tests(c, debug=True):
  """
    Compiles all C programs found in the `tests/c-patterns` directory and keeps the outputs in the `tests/c-patterns/bin` directory.
    This task uses pytest to run the tests, so that we will have a better overview of the results.
  """
  info("Running tests ...")
  # c_patterns_path = os.path.join(os.path.dirname(__file__), "tests/c-patterns/c_patterns_compilation_test.py")
  # c.run(f"pytest {c_patterns_path} --build_type=debug -rP -rx")

  build_type = "debug" if debug else "release"
  
  clang_path = os.path.join(os.path.dirname(__file__), f"build_tricore_{build_type}/bin/clang")
  c_files_path = os.path.join(os.path.dirname(__file__), "tests/c-patterns/src/")
  s_files_path = os.path.join(os.path.dirname(__file__), "tests/c-patterns/assembly/")

  if not os.path.exists(s_files_path):
    os.mkdir(s_files_path)

  clang_arguments = f"-S --target=tricore {" --debug" if debug else ""}"

  # Get a list of files with the .c extension from the c_files_path
  c_files = [os.path.basename(x) for x in glob.glob(c_files_path + "*.c")]
  file_names = [Path(x).stem for x in c_files]
  info(file_names)

  log_file_path = os.path.join(os.path.dirname(__file__), "tests/c-patterns/logs/")
  
  # Iterate over the c_files list
  for c_file in file_names:
      # Run clang compiler on all C files
      command = f"{clang_path} {clang_arguments} {os.path.join(c_files_path, f'{c_file}.c')} -o {os.path.join(s_files_path, f'{c_file}.s')}"
      # print(command)
      # result = subprocess.run([f"{clang_path} {clang_arguments} {os.path.join(c_files_path, c_file)} -o {os.path.join(s_files_path, f'{c_file}.s')}"], check=True)
      result = subprocess.run(shlex.split(command), env=os.environ, capture_output=True)

      if result.returncode != 0:
        error(result.stderr.decode())


