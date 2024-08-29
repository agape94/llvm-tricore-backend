import pytest
import subprocess
import sys
import glob

build_type = "release"

# Add your test functions here
def test_c_patterns(request):
    build_type = request.config.getoption("--build_type")
    
    clang_path = f"build_tricore_{build_type}/bin/clang"
    c_files_path = "c-patterns/src/"
    s_files_path = "c-patterns/src/assembly/"
    clang_arguments = f"-S --target=tricore --debug"

    # Get a list of files with the .c extension from the c_files_path
    c_files = glob.glob(c_files_path + "*.c")

    # Iterate over the c_files list
    for c_file in c_files:
        # Run clang compiler on all C files
        result = subprocess.run([clang_path, c_file, "-o", f"{c_file}.s" , clang_arguments], capture_output=True)

        # Check if the clang compiler returned 0 (no errors)
        assert result.returncode == 0

if __name__ == "__main__":
    build_type = sys.argv[1] if len(sys.argv) > 1 else "release"
    pytest.main()