document.getElementById('compare-btn').addEventListener('click', async function() {
    const textTabActive = document.getElementById('text-tab').classList.contains('active');
    const resultDiv = document.getElementById('result');
    const previewDiv = document.getElementById('text-preview');
    
    resultDiv.textContent = "Comparing...";
    resultDiv.style.color = 'black';
    previewDiv.innerHTML = "";
    
    try {
        let formData = new FormData();
        
        if (textTabActive) {
            // Text comparison mode
            const text1 = document.getElementById('text1').value;
            const text2 = document.getElementById('text2').value;
            
            if (!text1.trim() || !text2.trim()) {
                throw new Error("Both text fields must contain text");
            }
            
            formData.append('text1', text1);
            formData.append('text2', text2);
        } else {
            // File comparison mode
            const file1 = document.getElementById('file1').files[0];
            const file2 = document.getElementById('file2').files[0];
            
            if (!file1 || !file2) {
                throw new Error("Please select two files");
            }
            
            formData.append('file1', file1);
            formData.append('file2', file2);
        }
        
        const response = await fetch('/compare', {
            method: 'POST',
            body: formData
        });
        
        const data = await response.json();
        
        if (!data.success) {
            throw new Error(data.error || "Comparison failed");
        }
        
        resultDiv.textContent = `Similarity: ${data.similarity.toFixed(2)}%`;
        
        // Color coding
        if (data.similarity > 70) {
            resultDiv.style.color = 'red';
        } else if (data.similarity > 30) {
            resultDiv.style.color = 'orange';
        } else {
            resultDiv.style.color = 'green';
        }
        
        // Show preview
        previewDiv.innerHTML = `
            <h3>Text Preview:</h3>
            <div class="preview-container">
                <div class="text-sample">
                    <h4>Text 1 (${data.text1.length > 1500 ? 'first 1500 chars' : 'full text'}):</h4>
                    <p>${data.text1 || "(empty)"}</p>
                </div>
                <div class="text-sample">
                    <h4>Text 2 (${data.text2.length > 1500 ? 'first 1500 chars' : 'full text'}):</h4>
                    <p>${data.text2 || "(empty)"}</p>
                </div>
            </div>
        `;
        
    } catch (error) {
        resultDiv.textContent = `Error: ${error.message}`;
        resultDiv.style.color = 'red';
    }
});

// Tab switch logic
document.querySelectorAll('.tab-btn').forEach(button => {
    button.addEventListener('click', () => {
        // Remove active class from all buttons and tabs
        document.querySelectorAll('.tab-btn').forEach(btn => btn.classList.remove('active'));
        document.querySelectorAll('.tab-content').forEach(tab => tab.classList.remove('active'));

        // Add active class to clicked button and its target tab
        button.classList.add('active');
        const tabId = button.getAttribute('data-tab');
        document.getElementById(tabId).classList.add('active');

        // Clear result and preview on tab switch
        document.getElementById('result').textContent = '';
        document.getElementById('text-preview').innerHTML = '';
    });
});
