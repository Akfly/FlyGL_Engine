FlyGL Engine
============

Description
-----------
This is a university project that consisted on making an engine using OpenGl. This engine shows a scene that is composed by multiple meshes loaded from obj files. The scene might take some time to load, while it shows a loading screen. There are two light in this scene, one attached to the camera (that can be switched on and off and change its intensity), and a second one which switch intensity automatically (the code is prepared to have more than one light). The meshes load three textures, diffuse, specular and normal, and there are different postprocessing effects, like Motion Blur.

Controls
--------
- W,S                           Move Camera on the Z axis.
- A,D                           Move Camera on the X axis.
- R,F                           Move Camera on the Y axis.
- I,K                           Rotate Camera on the X axis.
- J,L                           Rotate Camera on the Y axis.
- Space Bar                     Switch Camera Light On and Off.
- Left and Right Arrows (←, →)  Rotate the Character.
- Up and Down Arrows (↑, ↓)     Increase/Decrease Camera Light Intensity.
- 0                             Remove every postprocess effect.
- 1-4                           Switch between postprocess effects.

Effects
-------
The effects can be switched with numbers from 1 to 4. The number 0 removes all effects.
- 0    None.
- 1    Motion Blur.
- 2    Reflection (The character reflects on the floor, using the Stencil Buffer).
- 3    Blur.
- 4    Dizzy Effect (duplicates the screen, move them and blend them).

Executable
----------
You can download the executable version from here:
https://www.dropbox.com/s/rwazhqi5urf5w3r/FlyEngine.rar

Classes
-------
**Actor**
Class that represents an element in the world. This class has the basic transformations and its requiered matrices (position, rotation, scale).

**Camera**
Represents the world camera. Its parent class is Actor, and has additionally camera properties, like the Field of View, and Near and Far Planes. The "modelMatrix" of the camera is used as the "viewMatrix" on the scene. It also has a method that returns the "projectionMatrix".

**Mesh**
Represents a mesh in the world (inherits from Actor). This class contains the different buffers needed such as vertices, uvs, normals, tangents and bitangents (the last two needed for correct normal mapping, named as that because they are tangents to the normal vector).

It has a method called LoadMesh, that loads the data from an obj file and fills the buffers with it. It uses another two methods to load the shaders and the textures. The actual working shaders are vertex.glsl and fragment.glsl, and they load three types of texture: Diffuse, Specular and Normal (no less, no more).

The Draw method draws the mesh in the actual frameBuffer (the original or our custom), and pass every data to shaders (lights, textures and attributes).

**PointLight**
This class represents a light in the scene. This kind of light is just one that is in a point and affects every light equally. It has every additionally properties like color and intensity, and it also has methods to turn it on/off and switch between it.

**ShaderManager**
This class manages the shaders. It loads the .glsl files, read them and compile them. It also gets the Uniforms and returns the program ID (it could be neccessary to perform additional operations).

**Postprocess**
This class implements a postprocess effect on the scene. It could be used as a base class to inherit from it to get a more complex effect or use it to get a simple effect. Basically, it changes the default frameBuffer (0) to our frameBuffer, so when the fragment shader outputs the data it is send to our frameBuffer and draws it on a texture. Afterwards we draw a simple square mesh that has the same size and position as the screen and render the texture on this mesh. This way, we can modify the texture with another shader, giving it different effects.

This class has an Initialize method that loads everything necessary for creating a postprocess (a plane, a texture, shaders, buffers, etc). Then, before drawing, the method "Preprocess" has to be called, so we switch the actual frameBuffer to our own. Then we draw as always (meshes, lights, etc) and finally call the "Draw" method of our effect, that will draw on the frameBuffer 0.

**DizzyProcess**
This is a simple effect that duplicates the screen and move it. Inherits from Postprocess to get the basics, and additionally we have 3 uniforms, which are Time, Radius and Screen Height.

**MotionBlur**
Inherits from Postprocess, and it is a little more complex effect, because it requires 2 textures instead of one. It is a simple Motion Blur (Camera only, not Object-MotionBlur), and this one is performed with the speed texture (there is another one with depth texture instead). The basic idea is that in the mesh shaders we calculate the speed in each fragment and then we pass it to the second texture. To implement this second texture we load it with GL_COLOR_ATTACHMENT1 (instead of 0), and add a drawing buffer. Then we just apply the texture when drawing like usual. The Postprocess class already has a method to load textures, so we just have to call "InitializeTexture" to load this new one.

**View**
This class represents the scene itself and has every element on it (meshes, lights, camera and postprocesses).

It has an Initialize method that loads everything. At the end of the .cpp file are every method that load those different elements to keep it organized. We also have an Update and a Draw methods. The Draw method is divided in different steps, the first one calls the Preprocess method of an effect if necessary, then we fill the light buffer with the data to send it to the different meshes, and if we are drawing with reflection we need to call additional methods, and then call the Postprocess Draw if necessary. ReflectionDraw draws first the meshes that won't have reflections on it a usual, then it calls PrepareReflectiveDraws, that sets the Stencil Buffer, that specifies that the next to be drawn is also used as a mask for next drawings (if what we are drawing is inside this mask then draw it, else we don't). Then we draw the meshes that will recieve this reflection, in our case the floor, and call PrepareReflectionDraws, that will apply the mask to the Stencil Buffer, so what is not inside this mask will not be drawn, and apply transparency for the next elements. Then we draw the reflections (the same meshes with an inverted Y), and finally we call EndReflection, that disables transparency and the Stencil Buffer.

This class also takes care of inputs, and the different effects that this will have on the scene (change positions, lights, etc).

**Main**
Initialize the program (the SFML window, Glew and View). Before initializing View we draw a loading screen (the mothod is at the end of the file, and it is really simple, based on the Postprocess code, we draw a quad on the screen).It also handles window events like Resize or Close.

Shaders
-------

**Vertex and Fragment**
This are the mesh shaders. They gather the position data, lights, create the TBN matrix for normal mapping and apply textures (diffuse, normal, secular). Additionally they get the actual and previous MVP matrices (Model View Projection) and subtract them to get the Speed Texture for the Motion Blur effect, and send it as a second parameter from the fragment shader.

**BasicPostProcess**
A simple shader that takes a textures and draws it. I used it just to copy it and then make more shaders.

**Blur**
A simple blur effect. We take the near pixels on the fragment and mix them.

**Dizzy**
A normal effect, we duplicate our texture and move it with sin and cosin, a specified radius and the height of the screen. Then we mix both colors (50%).

**Loading**
A basic shader similar to BasicPostProcess, just draws a texture.

**MotionBlur**
Gets both color and speed textures, and in a loop (with a specified number of iterations) we calculate the offset using an already calculated direction. This gives a result of different pixels, so we blend them, giving the motion blur effect. If nothing has been moved, the pixel on the loop will always be the same, so there will be no blur.

**Libraries**
I've used some additional libraries:

- tinyobjloader: Simple library to read obj files. It can be downloaded from: https://github.com/syoyo/tinyobjloader

- stb_image: To load different images and textures, it can load different formats. https://github.com/nothings/stb

- vboindexer: This is just a method extracted from a tutorial that I've used to learn OpenGL. This method orders the indices loaded from an obj. http://www.opengl­tutorial.org/download/The file is in the common folder.
