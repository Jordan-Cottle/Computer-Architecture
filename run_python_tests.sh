# Make sure main project files can be accessed the same as from within the project
export PYTHONPATH=${PYTHONPATH}:$(pwd)/python_solution

cd tests/python_tests

python -m unittest discover

cd --