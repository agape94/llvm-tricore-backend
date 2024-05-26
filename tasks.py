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

def check_arguments(target_name, path, build_type):
  if target_name is None or not target_name[0].isupper():
    raise ValueError(f"Please provide a valid target name (upper case and camel case when applicable)")
  
  if not os.path.exists(path) or len(os.listdir(path)) == 0:
    raise ValueError(f"{path} is empty or doesn't exist")
  
  if not build_type in {"Debug", "Release"}:
    raise ValueError(f"Please provide a valid build type (Debug or Release)")

# ==================================================================================================================

@task(aliases = ['b'], 
      help={"experimental_targets": "List of experimental targets to build, Strings separated by ';' (semicolon). Example: 'TriCore;RISCW;etc'. If not defined, the name of the target the environment was created for will be used.",
             "path": "Path to llvm source root. By default the current directory is considered",
             "build_type": "Possible values: Debug, Release. Default value: Release",
             "clang": "Whether to build clang or not. Default value: True",
             "upstream_targets": "List of upstream targets to build, Strings separated by ';' (semicolon). By default it is None. Example: Lanai;ARM;RISCV;etc",
             "parallel_link_jobs": "Number of parallel link jobs. By default it is set to 2. Note: A high number of link jobs may cause your system to run out of memory and crash."})
def build_target(c, experimental_targets="", path=".", upstream_targets="", build_type="Release", clang=True, parallel_link_jobs=2):
  """
    Starts the build process for the target specified in the target_name parameter. If the target_name is not provided, the name of the target the environment was created for will be used.
    By default, the build type is set to Release. If you want to build the target in Debug mode, you can specify the build type as Debug.
  """
  if experimental_targets == "":
    with open(os.path.join(os.getcwd(), ".experimental_targets_to_build"), "r") as f:
      experimental_targets = f.read().strip()
  
  check_arguments(experimental_targets, path, build_type)

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
  enable_clang = "-DLLVM_ENABLE_PROJECTS=\"clang\"" if clang else ""
  enable_assertions = "-DLLVM_ENABLE_ASSERTIONS=ON" if build_type == "Debug" else "-DLLVM_ENABLE_ASSERTIONS=OFF"
  cmake_command = f'cmake -S llvm -B {build_dir} -G Ninja {enable_clang} -DCMAKE_INSTALL_PREFIX={build_dir} {enable_assertions} -DCMAKE_BUILD_TYPE={build_type} -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD="{experimental_targets}" -DLLVM_TARGETS_TO_BUILD="{upstream_targets}" -DLLVM_PARALLEL_LINK_JOBS={parallel_link_jobs} -DLLVM_USE_LINKER=lld'
  
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
