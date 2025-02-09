# SchedulerRaylibHCMUS
An simple app to help you visualize your class schedule.
# Usage

# How to build
1. **Install Raylib**  
Go to the [Raylib's website](https://www.raylib.com/) and download Raylib version 5.5

2. **Configure your Raylib path**  
By default, Raylib will be installed at `C:\raylib` and there's no way to change it during set up.  I changed my raylib directory to `D:\raylib` and configured related build files to use this path. In VSCode, You can try `Ctrl + Shift + F`, search for `D:\raylib` in those files and replace it with your own path.

3. **Build the project**  
In VSCode, press `F5` 

# Problems
The VSCode template is from [Andrew Hamel Codes](https://youtu.be/xWWqhQ1JnvE?si=nqmY1581xXtpsxsh). The author said this workflow is outdated but to me it's a very easy way to get started with Raylib using VSCode. This template helps you seperate your logic into mutiple .c files and .h files.  

Note that with this setup, changes made in header files **won't** be recompiled sometimes. If you encounter some weird errors, try clear the build with `make clean` and press `F5` again.  
