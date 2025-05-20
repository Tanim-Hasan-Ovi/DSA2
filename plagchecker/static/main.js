// Tab Functionality
document.querySelectorAll('.tab-button').forEach(button => {
    button.addEventListener('click', () => {
        const tabContainer = button.closest('.tab');
        const contentId = button.dataset.target;
        
        // Deactivate all buttons in this tab group
        tabContainer.querySelectorAll('.tab-button').forEach(btn => {
            btn.classList.remove('active');
        });
        
        // Activate clicked button
        button.classList.add('active');
        
        // Hide all content in this section
        const section = button.closest('.input-section');
        section.querySelectorAll('.tab-content').forEach(content => {
            content.classList.remove('active');
        });
        
        // Show target content
        document.getElementById(contentId).classList.add('active');
    });
});

// Drag and Drop Setup
const setupDropZone = (dropZoneId, fileInputId, fileInfoId) => {
    const dropZone = document.getElementById(dropZoneId);
    const fileInput = document.getElementById(fileInputId);
    const fileInfo = document.getElementById(fileInfoId);

    // Click to browse
    dropZone.addEventListener('click', () => fileInput.click());
    
    // File selected via browse
    fileInput.addEventListener('change', () => {
        if (fileInput.files.length) {
            fileInfo.textContent = `Selected: ${fileInput.files[0].name}`;
        }
    });

    // Drag over highlight
    dropZone.addEventListener('dragover', (e) => {
        e.preventDefault();
        dropZone.classList.add('highlight');
    });

    // Remove highlight
    ['dragleave', 'dragend'].forEach(type => {
        dropZone.addEventListener(type, () => {
            dropZone.classList.remove('highlight');
        });
    });

    // Handle drop
    dropZone.addEventListener('drop', (e) => {
        e.preventDefault();
        dropZone.classList.remove('highlight');
        
        if (e.dataTransfer.files.length) {
            fileInput.files = e.dataTransfer.files;
            fileInfo.textContent = `Selected: ${e.dataTransfer.files[0].name}`;
        }
    });
};

// Initialize drop zones
setupDropZone('dropZone1', 'fileInput1', 'fileInfo1');
setupDropZone('dropZone2', 'fileInput2', 'fileInfo2');

// Form Submission
document.getElementById('plagiarismForm').addEventListener('submit', async (e) => {
    e.preventDefault();
    
    const formData = new FormData();
    const algorithm = document.getElementById('algorithm').value;
    let valid = true;

    // Process Document 1
    const text1Active = document.getElementById('text1').classList.contains('active');
    if (text1Active) {
        const text1 = document.getElementById('textInput1').value.trim();
        if (!text1) {
            alert('Please enter text or upload a file for Document 1');
            valid = false;
        } else {
            formData.append('text1', text1);
        }
    } else {
        const file1 = document.getElementById('fileInput1').files[0];
        if (!file1) {
            alert('Please upload a file for Document 1');
            valid = false;
        } else {
            formData.append('file1', file1);
        }
    }

    // Process Document 2
    const text2Active = document.getElementById('text2').classList.contains('active');
    if (text2Active) {
        const text2 = document.getElementById('textInput2').value.trim();
        if (!text2) {
            alert('Please enter text or upload a file for Document 2');
            valid = false;
        } else {
            formData.append('text2', text2);
        }
    } else {
        const file2 = document.getElementById('fileInput2').files[0];
        if (!file2) {
            alert('Please upload a file for Document 2');
            valid = false;
        } else {
            formData.append('file2', file2);
        }
    }

    if (!valid) return;

    formData.append('algorithm', algorithm);

    const resultDiv = document.getElementById('result');
    resultDiv.innerHTML = '<p class="processing">Processing...</p>';

    try {
        const response = await fetch('/compare', {
            method: 'POST',
            body: formData
        });
        
        if (!response.ok) {
            throw new Error(await response.text());
        }
        
        const data = await response.json();
        resultDiv.innerHTML = `
            <p><strong>Algorithm:</strong> ${data.algorithm}</p>
            <p><strong>Similarity:</strong> ${data.similarity}%</p>
        `;
    } catch (error) {
        console.error('Error:', error);
        resultDiv.innerHTML = `<p class="error">Error: ${error.message}</p>`;
    }
});