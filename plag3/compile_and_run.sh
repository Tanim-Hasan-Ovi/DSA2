#!/bin/bash

# Compile C++ code
echo "Compiling C++ code..."
g++ compare.cpp -o compare

# Install Python requirements
echo "Installing Python requirements..."
pip install -r requirements.txt

# Run the Flask app
echo "Starting the web server..."
python3 app.py