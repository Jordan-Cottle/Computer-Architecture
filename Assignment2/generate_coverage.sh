# Make sure main project files can be accessed the same as from within the project
main_dir=$(pwd)/python_solution
export PYTHONPATH=${PYTHONPATH}:$main_dir

cd tests/python_tests

coverage run

coverage report

cd --