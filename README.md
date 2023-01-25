<h1>Console3DGameEngine</h1>
<p>This project is a 3D game engine rendered in a console using C++.</p>

<h3>Key Features</h3>
 <li> <strong>Transformation Matrices:</strong> The engine uses different transformation matrices to transform and manipulate objects in 3D space, as well as to project them onto the camera.
 <li> <strong>Clipping Functionality:</strong> One of the key features of the engine is the clipping functionality, which is used to avoid rendering triangles that are out of frame. This is important for optimizing performance and ensuring that the game runs smoothly, as it eliminates the need to render unnecessary parts of the scene.
 <li> <strong>Importing .obj files:</strong> Another important feature of the engine is the ability to import .obj files, which allows users to load their own 3D objects into the engine. However, it is important to note that the faces of the object must be triangulated in order for it to be properly rendered in the game.
Rasterization Method: The engine uses a simple but powerful method of 3D rendering, known as the "rasterization" method. This method works by taking 3D coordinates and projecting them onto a 2D plane, known as the "screen space".
<h2>Camera Control</h2>
 <li> <strong>W:</strong> Move camera forward
 <li> <strong>S:</strong> Move camera backward
 <li> <strong>A:</strong> Strafe left
 <li> <strong>B:</strong> Strafe right
 <li> <strong>SPACE:</strong> Move up
 <li> <strong>LEFT_SHIFT:</strong> Move down
 <li> <strong>LEFT_ARROW:</strong> Yaw left
 <li> <strong>RIGHT_ARROW:</strong> Yaw right
 <li> <strong>UP_ARROW:</strong> Tilt up
 <li> <strong>DOWN_ARROW:</strong> Tilt down
 
<p>Overall, this engine provides a simple and way to create and render 3D scenes in the console. It is a great starting point for in learning more about 3D game development and the underlying concepts and techniques used in 3D game engines.</p>
