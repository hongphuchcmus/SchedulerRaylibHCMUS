document.getElementById('extractButton').addEventListener('click', async () => {
  const statusDiv = document.getElementById('status');
  statusDiv.textContent = 'Extracting...';

  try {
      const [tab] = await chrome.tabs.query({ active: true, currentWindow: true });
      
      const response = await chrome.tabs.sendMessage(tab.id, { action: "extract" });
      
      if (response.success) {
          // Create blob and download
          const blob = new Blob([response.content], { type: 'text/plain;charset=utf-8'});
          const url = URL.createObjectURL(blob);
          
          await chrome.downloads.download({
              url: url,
              filename: 'extracted_table.txt',
              saveAs: true
          });
          
          statusDiv.textContent = 'Table extracted successfully!';
      } else {
          statusDiv.textContent = response.message || 'Extraction failed';
      }
  } catch (error) {
      statusDiv.textContent = 'Error: ' + error.message;
  }
});