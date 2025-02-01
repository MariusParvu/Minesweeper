### You will need the SDL2 and SDl2_image libraries to compile
## Linux ( Debian distros )
```sudo apt-get install SDL2 SDL2_image```
## Mac ( first install Brew ) 
```sudo brew install SDL2 SDL2_image```
## Windows
I use the Nuwen MinGW distro on Windows which comes packed with the SDL 2 libs. Get it here https://nuwen.net/mingw.html
Make is included somewhere in the bin folder of the Nuwen distro which will anyway be added to the PATH so to compile just run make.exe in the the project folder.

## You can make your own themes and animations ( see the sprites in the resources folder )

<img width="1440" alt="Screenshot 2023-02-26 at 14 12 04" src="https://user-images.githubusercontent.com/70338038/221413130-16bbb62b-def9-4824-ac81-950c560ab305.png">
<img width="1440" alt="Screenshot 2023-02-26 at 14 17 48" src="https://user-images.githubusercontent.com/70338038/221413141-74ec12b1-1d4a-4e4c-aa1e-e1f23db346f2.png">

![Untitled1](https://user-images.githubusercontent.com/70338038/221413155-f9cedb3e-f064-4009-885a-2eb9046c94fb.gif)


### There is no GUI and the mine count is displayed on the field after the first flag is set. 
### The difficulty is fine tuned by myself to my personal taste but if you would like to help build a GUI with a prompt and all, then I'd love to assist.
### The themes cycle through along with the difficulty and this was again a personal preference.
### There is also no timer which is trivial to add but there should be made a way to display it first.
### There are no bounds at the edges, instead the field extends itself in the opposite side of the screen. You could actually envision it as a toroidal field, unwrapped in a rectangular area.
