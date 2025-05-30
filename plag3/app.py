from flask import Flask, render_template, request, jsonify
import os
import subprocess
import PyPDF2
from docx import Document
from io import BytesIO

app = Flask(__name__)

def extract_text(input_data):
    # Handle both file uploads and direct text
    if hasattr(input_data, 'filename'):  # It's a file upload
        if input_data.filename.endswith('.pdf'):
            pdf = PyPDF2.PdfReader(input_data)
            return " ".join(page.extract_text() for page in pdf.pages if page.extract_text())
        elif input_data.filename.endswith('.docx'):
            doc = Document(BytesIO(input_data.read()))
            return " ".join(para.text for para in doc.paragraphs)
        else:  # Plain text file
            return input_data.read().decode('utf-8')
    else:  # Direct text input
        return input_data

@app.route('/')
def home():
    return render_template('index.html')

@app.route('/compare', methods=['POST'])
def compare():
    # Get both text inputs (either from files or textareas)
    text1 = request.files.get('file1') or request.form.get('text1', "")
    text2 = request.files.get('file2') or request.form.get('text2', "")
    
    try:
        # Process both inputs
        processed1 = extract_text(text1)
        processed2 = extract_text(text2)
        
        # Save to temp files for C++ program
        with open('temp1.txt', 'w', encoding='utf-8') as f:
            f.write(processed1)
        with open('temp2.txt', 'w', encoding='utf-8') as f:
            f.write(processed2)
        
        # Run comparison
        result = subprocess.run(
            ['./compare', 'temp1.txt', 'temp2.txt'],
            capture_output=True,
            text=True,
            encoding='utf-8'
        )
        
        similarity = float(result.stdout.strip())
        return jsonify({
            'success': True,
            'similarity': similarity,
            'text1': processed1[:500] + ("..." if len(processed1) > 500 else ""),
            'text2': processed2[:500] + ("..." if len(processed2) > 500 else "")
        })
    
    except Exception as e:
        return jsonify({'success': False, 'error': str(e)})

if __name__ == '__main__':
    # Ensure compare executable exists
    if not os.path.exists('compare'):
        subprocess.run(['g++', 'compare.cpp', '-o', 'compare'])
    app.run(debug=True)