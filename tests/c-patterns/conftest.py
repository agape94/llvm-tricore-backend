def pytest_addoption(parser):
    parser.addoption("--build_type", action="store", default="release", help="Whether to search for the clang binery in the release or debug folder")