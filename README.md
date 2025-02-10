# SchedulerRaylibHCMUS
An simple app to help you visualize your class schedule.
# Usage
To start scheduling you have to provide the data of available classes in a Tab-delimited text file in the Release's `resources` folder. You can use the file `resources/extracted_table.txt` that already came with the Release, or use a Google Chrome extension to extract the data directly from "List of Open Class" page on your HCMUS Portal.  

# Table Extraction
You can download the .txt file containing your avaible classes using a chrome extension included in this repository.
+ Download this repository
+ Open Chrome, navigate to `chrome://extensions`
+ Turn on **Developer Mode** if you haven't yet
+ Click on **Load Unpacked Extension**, a folder browser window opens up
+ Choose the folder `chrome_extension` from your downloaded repository. The extension will then be installed on your browser

Go to "List of Open Class" page on your HCMUS Portal, enable the extension. Click on `Extract Table`, then select the `/resources` directory of your Release, and save the file as `extracted_table.txt`.  
**Note** The name file must be exactly `extracted_table.txt` for the excutable to work.

# How to build
1. **Install Raylib**  
Go to the [Raylib's website](https://www.raylib.com/) and download Raylib version 5.5

2. **Configure your Raylib path**  
By default, Raylib will be installed at `C:\raylib` and there's no way to change it during set up.  I changed my raylib directory to `D:\raylib` and configured related build files to use this path. In VSCode, You can try `Ctrl + Shift + F`, search for `D:\raylib` in those files and replace it with your own path.

3. **Setting CMake/Make**  
First, press `Window` button and search `Edit the system environment variables`  
Then, press `Environment variables`. In `User variables for Admin`, click `Path` and click `Edit` to open `Edit environment variables` tab  
After seeing that tab, click `New` and add the path `D:\raylib\w64devkit\bin` (this is my own path)  
Finally, press `OK` to turn off the tab, continue to press `OK` 2 more times to apply the changes  

4. **Build the project**  
In VSCode, press `F5` 



# Problems
The VSCode template is from [Andrew Hamel Codes](https://youtu.be/xWWqhQ1JnvE?si=nqmY1581xXtpsxsh). The author said this workflow is outdated but to me it's a very easy way to get started with Raylib using VSCode. This template helps you seperate your logic into mutiple .c files and .h files.  

Note that with this setup, changes made in header files **won't** be recompiled sometimes. If you encounter some weird errors, try clear the build with `make clean` and press `F5` again.  
