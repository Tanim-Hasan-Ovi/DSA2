from flask import Flask, request, jsonify, render_template
import subprocess
import os
import PyPDF2
from docx import Document
from werkzeug.utils import secure_filename

app = Flask(__name__)

# Allowed extensions
ALLOWED_EXTENSIONS = {'pdf', 'docx', 'txt'}

def allowed_file(filename):
    return '.' in filename and filename.rsplit('.', 1)[1].lower() in ALLOWED_EXTENSIONS

def extract_text(file_path, file_type):
    try:
        if file_type == 'pdf':
            text = ""
            with open(file_path, 'rb') as file:
                reader = PyPDF2.PdfReader(file)
                for page in reader.pages:
                    if page.extract_text():
                        text += page.extract_text()
            return text
        elif file_type == 'docx':
            doc = Document(file_path)
            return "\n".join([para.text for para in doc.paragraphs])
        elif file_type == 'txt':
            with open(file_path, 'r', encoding='utf-8') as file:
                return file.read()
        else:
            return ""
    except Exception as e:
        print(f"[Error extracting text from {file_path}]: {e}")
        return ""

@app.route("/")
def home():
    return render_template("index.html")

@app.route("/compare", methods=["POST"])
def compare():
    text1, text2 = "", ""

    # Document 1
    if 'file1' in request.files and request.files['file1'].filename:
        file1 = request.files['file1']
        if allowed_file(file1.filename):
            filename = secure_filename(file1.filename)
            file1_type = filename.rsplit('.', 1)[1].lower()
            file1_path = f"temp1.{file1_type}"
            file1.save(file1_path)
            text1 = extract_text(file1_path, file1_type)
    elif 'text1' in request.form:
        text1 = request.form['text1']

    # Document 2
    if 'file2' in request.files and request.files['file2'].filename:
        file2 = request.files['file2']
        if allowed_file(file2.filename):
            filename = secure_filename(file2.filename)
            file2_type = filename.rsplit('.', 1)[1].lower()
            file2_path = f"temp2.{file2_type}"
            file2.save(file2_path)
            text2 = extract_text(file2_path, file2_type)
    elif 'text2' in request.form:
        text2 = request.form['text2']

    # Validation
    if not text1 or not text2:
        return jsonify({"error": "Text extraction failed for one or both documents."}), 400

    try:
        with open("text1.txt", "w", encoding='utf-8') as f1:
            f1.write(text1)
        with open("text2.txt", "w", encoding='utf-8') as f2:
            f2.write(text2)
    except Exception as e:
        return jsonify({"error": f"Failed to write text files: {e}"}), 500

    # Run the C++ program
    try:
        result = subprocess.run(["./plag"], capture_output=True, text=True)
        if result.returncode != 0:
            raise Exception(result.stderr.strip())

        similarity = float(result.stdout.strip())
        return jsonify({
            "algorithm": "plag",
            "similarity": similarity
        })
    except Exception as e:
        return jsonify({"error": f"Plagiarism check failed: {e}"}), 500

if __name__ == "__main__":
    app.run(debug=True)
