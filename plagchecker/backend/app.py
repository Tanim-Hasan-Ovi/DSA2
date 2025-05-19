from flask import Flask, request, jsonify, render_template
import subprocess
import os
import PyPDF2
from docx import Document
from werkzeug.utils import secure_filename

app = Flask(__name__, template_folder="../frontend")

# Allowed file extensions
ALLOWED_EXTENSIONS = {'pdf', 'docx'}

def allowed_file(filename):
    return '.' in filename and filename.rsplit('.', 1)[1].lower() in ALLOWED_EXTENSIONS

def extract_text(file_path, file_type):
    """Extract text from PDF, DOCX, or raw text file"""
    try:
        if file_type == 'pdf':
            text = ""
            with open(file_path, 'rb') as file:
                reader = PyPDF2.PdfReader(file)
                for page in reader.pages:
                    text += page.extract_text()
            return text
        elif file_type == 'docx':
            doc = Document(file_path)
            return "\n".join([para.text for para in doc.paragraphs])
        else:  # Raw text
            with open(file_path, 'r') as file:
                return file.read()
    except Exception as e:
        print(f"Error extracting text: {e}")
        return ""

@app.route("/")
def home():
    return render_template("index.html")

@app.route("/compare", methods=["POST"])
def compare():
    # Initialize variables
    text1, text2 = "", ""
    
    # Process first input (file or text)
    if 'file1' in request.files and request.files['file1'].filename:
        file1 = request.files['file1']
        if file1 and allowed_file(file1.filename):
            filename = secure_filename(file1.filename)
            file1_type = filename.split('.')[-1]
            file1_path = f"temp1.{file1_type}"
            file1.save(file1_path)
            text1 = extract_text(file1_path, file1_type)
    elif 'text1' in request.form:
        text1 = request.form['text1']
    
    # Process second input (file or text)
    if 'file2' in request.files and request.files['file2'].filename:
        file2 = request.files['file2']
        if file2 and allowed_file(file2.filename):
            filename = secure_filename(file2.filename)
            file2_type = filename.split('.')[-1]
            file2_path = f"temp2.{file2_type}"
            file2.save(file2_path)
            text2 = extract_text(file2_path, file2_type)
    elif 'text2' in request.form:
        text2 = request.form['text2']
    
    # Save texts for C++ processing
    with open("text1.txt", "w") as f:
        f.write(text1)
    with open("text2.txt", "w") as f:
        f.write(text2)
    
    # Get selected algorithm
    algorithm = request.form.get('algorithm', 'rabin_karp')
    
    try:
        # Call C++ executable
        result = subprocess.run(
            [f"./{algorithm}"],
            capture_output=True,
            text=True
        )
        similarity = float(result.stdout.strip())
        return jsonify({
            "algorithm": algorithm,
            "similarity": similarity
        })
    except Exception as e:
        return jsonify({"error": str(e)}), 500

if __name__ == "__main__":
    app.run(debug=True)