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

-------------------------------------------------------------------

Tiene un método (LoadMesh) para cargar los datos desde un obj y almacenar estos en los
búfferes. Y otros dos para cargar los shaders y las texturas. En este caso está pensado para
que estas mallas funcionen con los shaders vertex.glsl y fragment.glsl, y que carguen las 3
texturas de Diffuse, Specular y Normal.
El método Draw se encarga de pintar en el frameBuffer activo la malla (puede ser el original u
otro que hayamos creado), y se encarga de pasar todos los datos de luces, texturas y atributos
al shader.
Hay otros métodos privados, que símplemente son partes de los anteriores mencionados. Esto
se ha hecho así para una mejor lectura del código.
PointLight
Clase que representa una luz en la escena (y por ello hereda de Actor). Este tipo de luz es solo
una luz que está en un punto y que afecta a todas direcciones por igual. Tiene propiedades
adicionales como el color o la intensidad, además de métodos para encender, apagar o cambiar
de un estado a otro (Switch).
ShaderManager
Clase que se encarga de la gestión de shaders. Carga archivos de código de shader (.glsl) y los
lee y compila. Además tiene un método para recoger los Uniforms del shader (que es muy
sencillo), y un método que devuelve el ID del shaderProgram por si es necesario hacer alguna
operación adicional.
Postprocess
Clase que implementa un efecto de postproceso en la escena. Puede usarse para heredar de
ella a un efecto más complejo o usar como un postproceso sencillo. Básicamente consiste encambiar el frameBuffer por defecto (el 0) por un nuestro, y así, cuando se pinte una malla y el
fragment shader saqué los parámetro de color, nosotros los recogemos de este frameBuffer y
los pintamos en una textura. Después pintamos una malla sencilla que es un plano en la
pantalla, haciendo coincidir los bordes de este plano con los de la pantalla, y pintar la textura en
este plano. Esto nos permite modificar la textura con otro shader, dando así distintos efectos.
Esta clase tiene un método Initialize que inicializa todo lo necesario para el postproceso (su
plano, una textura para pintar en ella, los shaders, los búffers, etc). Después, antes de pintar,
hay que llamar a un método en cada Frame de “Preprocess”, que simplemente cambia el
frameBuffer actual por el de este efecto. Después se pintaría normal (todas las mallas, las
luces, etc) y finalmente llamar al método Draw del efecto, que ya pinta en el frameBuffer 0.
DizzyProcess
Este es un efecto sencillo que duplica la pantalla y la mueve. Hereda de Postprocess para
obtener lo básico, y adicionalmente tenemos 3 uniforms, que son el tiempo, el radio y la altura
de la pantalla, para pasárselos al shader.
MotionBlur
Hereda de Postprocess, y es un efecto algo más complejo, ya que requiere de 2 texturas,
aunque es un Motion Blur sencillo de cámara sólo (no de objetos), siguiendo el método de la
textura de velocidad (hay otro que es con la textura de profundidad). La idea básicamente es
que en los shaders de las mallas, calculemos una velocidad de cada pixel, y esto se lo
pasemos como textura al shader del postproceso. Por ello necesitamos una segunda textura.
Esta hay que cargarla con GL_COLOR_ATTACHMENT1, en vez de el 0, por ser la segunda, y
además se añade al búffer de pintado. Después simplemente aplicamos la textura a la hora de
pintar como se hace normalmente. La clase Postprocess ya cuenta con un método para cargar
texturas, por lo que sólo tenemos que llamar a InitializeTexture() para cargar nuestra textura.
View
Clase que representa la escena en sí. Contiene todos sus elementos (mallas, luces, cámara, y
postprocesos).
Tiene un método Initialize que se encarga de Inicializar todo lo de arriba. Al final del .cpp están
los métodos que se encargan de cargar cada cosa, para tenerlo todo mejor organizado.
También tenemos los métodos Update y Draw, que se llaman desde fuera y se encargan de
mover los objetos, llamar a los Update de cada elemento, y de pintar en la escena. El método
Draw se divide en varias partes, primero llama al método de Preproceso de un efecto si es
necesario, luego rellenamos el búffer de luces con los datos necesarios para pasárselo a las
mallas y que puedan pintar las luces. Luego pinta las mallas, si va a haber reflexión hay que
llamar a unos métodos adicionales, por eso se han englobado estos dos pintados, y por último
se llama al Draw del postproceso si es necesario.El pintado de reflexión (ReflectionDraw) primero pinta las mallas normalmente que no tendrán
reflejos, luego llama a PrepareReflectiveDraws(), que prepara el Stencil Buffer, para decir que lo
siguiente que se pinte servirá también de máscara para los siguientes pintados. Después
pintamos las mallas que tendrán reflejos, y llamamos a PrepareReflectionDraws(), que hace
que se aplique la máscara del Stencil Búffer, lo que quede fuera de esta máscara no se pintará,
además habilitamos la transparencia para los siguientes pintados y así parezca que el reflejo se
mezcla con el suelo. Después pintamos los reflejos (invirtiendo la Y), y por último llamamos a
EndReflection(), que deshabilita la transparencia y el uso del Stencil Búffer.
Esta clase también tiene el método de Inputs, que recoge los eventos del teclado y aplica cada
uno su efecto.
Main
Inicia el programa, se encarga de inicializar la ventana de SFML, Glew, y View. Antes de
inicializar View pinta una pantalla de loading (el método está al final del código, es muy sencillo,
se basa principalmente en el código de Postproceso, pinta un Quad en pantalla con una
textura). También se encarga de los eventos de la ventana, como el Cerrar y el Resize.
Shaders
vertex y fragment
Estos son los shaders de las mallas. Básicamente recogen los datos de posición, las luces,
hacen la matriz TBN para las normales, y luego aplican las luces y las texturas (diffuse,
specular y normals). Adicionalmente recogen la MVP (model view projection) actual y anterior, y
las resta para conseguir la textura de velocidad para el Motion Blur, y se le pasa como segundo
parámetro desde el fragment (si no se recoge esta segunda textura no pasa nada).
basicPostProcess
Como su nombre indica, un shader sencillo, que pinta por pantalla la textura recogida. No hace
nada más, se puede usar como base para más postprocesos.
blur
Efecto de postproceso de Blur (desenfoque), simplemente en el fragment recogemos los
píxeles de alrededor del nuestro y los mezclamos.
dizzy
Un efecto normalillo, duplicamos la textura y las movemos en función del seno y coseno, el
radio, y el alto de la pantalla. Esto mueve las 2 texturas de forma diferente, teniendo 2 colores en
un mismo píxel, por lo que mezclamos ambos (con un mix un 50% cada color).loading
Un shader muy básico parecido a basicPostProcess, simplemente pinta una textura.
motionBlur
Recoge la textura de color y de velocidad, y se recorre un bucle (con un nº de iteraciones ya
establecido). En este bucle se calcula un offset distinto para cada iteración, usando una
dirección ya calculada. Esto recorre varios pixeles (si se han movido) y se mezclan entre ellos.
Si no se han movido, el píxel recorrido será siempre el mismo, por lo que no habrá desenfoque.
Librerias
Adicionalmente, se han usado algunas librerías adicionales a las de los repositorios de clase.
Estas son:
● tinyobjloader: Librería sencilla que se encarga de leer obj. Se puede descargar de aquí:
https://github.com/syoyo/tinyobjloader
● stb_image: Una librería muy conocida para la carga de imágenes. Puede cargar
distintos formatos. https://github.com/nothings/stb
● vboindexer: No es una librería exáctamente, solo es un método extraído de un tutorial
para ordenar los índices cargados de un obj (y por ello algo lento, pero eficaz).
http://www.opengl­tutorial.org/download/ El archivo está en la carpeta common.
