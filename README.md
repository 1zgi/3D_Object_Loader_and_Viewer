# ViewMe - 3D Model Viewer Desktop App

## 🎯 **About**
ViewMe is a lightweight, standalone 3D model viewer that can load and display Wavefront (.obj) 3D models with textures, lighting, and dynamic shadows. Perfect for previewing 3D models quickly and easily.


![ViewMe_intro](https://github.com/user-attachments/assets/fbbd3a19-2602-46c4-9f1b-98436cd4c8aa)


## 🚀 **Quick Start**

### **Method 1: File Association (RECOMMENDED)**
- Right-click any .obj file -> "Open with" -> Choose ViewMe.exe
- Future .obj files can be opened by double-clicking

![ViewMe_right_click_open_gif___2](https://github.com/user-attachments/assets/1c576483-0fb2-4f2a-a7c4-59f41f7b38c5)

### **Method 2: Command Line**
```bash
ViewMe.exe "path/to/your/model.obj"
```

### **Method 3: Browse in App**
```bash
ViewMe.exe
```
- Application starts clean
- Click "Browse Models..." to select any .obj file
- Navigate to any folder on your computer

![ViewMe_browser](https://github.com/user-attachments/assets/3818f697-33af-44cf-8e48-14cda3b1f52c)

## 🎮 **Controls**

### **Camera Controls:**
- **Mouse Wheel** - Zoom in/out
- **Left Click + Drag** - Rotate camera around model

### **Interface Panel:**
- **Ambient Light** - Adjust scene brightness (0-100%)
- **Auto Rotate Model** - Enable/disable automatic rotation
- **Rotation Speed** - Control rotation speed (5-120°/s)
- **Enable Shadows** - Toggle realistic shadows on/off
- **Browse Models...** - Load new 3D models

## **Supported Files**
- **Models**: .obj files (Wavefront format)
- **Textures**: .jpg, .png, .bmp (referenced in .mtl files)
- **Materials**: .mtl files (should be in same directory as .obj file)

## **Features**
- **Instant Loading** - Fast 3D model loading
- **Dynamic Shadows** - Realistic shadow mapping with soft edges
- **Multiple Lighting** - Advanced lighting system
- **Auto-Rotation** - Showcase models from all angles
- **File Association** - Right-click on .obj files and "select ViewMe as open with" to view the .obj file
- **Texture Support** - Full material and texture rendering
- **No Installation** - Portable executable
- **Crash-Free** - Robust file handling

## **Technical Requirements**

### **Windows**
- **OS**: Windows 10/11 (64-bit)
- **Graphics**: OpenGL 3.3+ compatible graphics card
- **Dependencies**: All required libraries included (no installation needed)

## 🫨 **Troubleshooting**

### **ViewMe won't start:**
- Ensure all .dll files are in the same directory as ViewMe.exe
- Update your graphics drivers
- Check if your graphics card supports OpenGL 3.+

### **Model appears too dark:**
- Increase the "Ambient Light" slider
- Check if the model has proper materials (.mtl file)

### **Model not visible:**
- Use mouse wheel to zoom out
- Try the auto-rotate feature to see the model from different angles
- Verify the .obj file is valid

### **Missing textures:**
- Place texture files (.jpg, .png, .bmp) in the same directory as the .obj file
- Ensure the .mtl file is in the same directory as the .obj file
- Check texture file paths in the .mtl file

## 💡 **Tips**
- For best results, keep .obj, .mtl, and texture files in the same folder
- ViewMe automatically scales models to fit the viewport
- Use the ground plane as a reference for model size
- Enable shadows for more realistic viewing
- File association works great for quick model previewing
- If you don't have any prepared .obj files, you can visit the "models" folder on top. Download & View

## 📧 **Support**
ViewMe is designed to be a simple, fast 3D model viewer. For issues with specific models, check that they follow standard Wavefront OBJ format.

## 📎 **Download**
[ViewMe v1.0](https://github.com/1zgi/3D_Object_Loader_and_Viewer/releases/tag/vwm1.0)

---
**ViewMe** - Fast, Simple 3D Model Viewing  
**Version**: 1.0   
**Build**: Release (August 2025)
